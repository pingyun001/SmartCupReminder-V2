#include "audio_player.h"
#include <string.h>
#include "tim.h"
#include "dac.h"

#if 1
#define AUDIO_DEBUG_LOG DEBUG_LOG
#else
#define AUDIO_DEBUG_LOG(...)
#endif


audiopy_t audiopy = 
{
	.now_status = audiopy_status_idle,
};

static HAL_StatusTypeDef Lime_file_seek_relative(FIL* fp, FSIZE_t offset) 
{
    FRESULT fr;
    FSIZE_t cur_pos = f_tell(fp);
    
    if(cur_pos == (FSIZE_t)-1) 
	{
        return HAL_ERROR;
    }
    
    fr = f_lseek(fp, cur_pos + offset);
    if(fr != FR_OK) 
	{
        return HAL_ERROR;
    }
    
    return HAL_OK;
}

static void Lime_audio_raw_data_cal(uint16_t* data, uint32_t len_bytes)
{
	//dac_value = ( (sample + 32768) >> 4 ) & 0x0FFF;

	/* para check */
	if(data == NULL || len_bytes > AUDIO_BUFFER_SIZE / 2)
	{
		AUDIO_DEBUG_LOG("audio data error\n");
		return;
	}

	/* wave data */
	for(int i = 0; i < len_bytes / 2; i++)
	{
		data[i] = ((data[i] + 32768) >> 4 ) & 0x0FFF;
	}
	
	/* no used data, set to half of max value */
	for(int i = len_bytes / 2; i < AUDIO_BUFFER_SIZE / 4; i++)
	{
		data[i] = 0x7FF;
	}
}

static void Lime_audio_read_new_data(void)
{
	FRESULT fr;
	unsigned int bytesRead;

	if( !audiopy.is_file_opened)
	{
		return;
	}

	if((!audiopy.is_buffer_a_filled) && (!audiopy.is_file_read_finished))
	{
		fr = f_read(&audiopy.fil, audiopy.buffer_a, AUDIO_BUFFER_SIZE / 2, &bytesRead);
		if (fr != FR_OK)
		{
			AUDIO_DEBUG_LOG("Voice data read failed with error code: %d\n", fr);
			f_close(&audiopy.fil);
			audiopy.is_file_opened = false;
			return;
		}
		if(bytesRead < AUDIO_BUFFER_SIZE / 2)
		{
			AUDIO_DEBUG_LOG("Voice data end\n");
			f_close(&audiopy.fil);
			audiopy.is_file_opened = false;
			audiopy.is_file_read_finished = true;
			
			return;
		}

		Lime_audio_raw_data_cal(audiopy.buffer_a, bytesRead);

		audiopy.is_buffer_a_filled = 1;
	}

	if((!audiopy.is_buffer_b_filled) && (!audiopy.is_file_read_finished))
	{
		fr = f_read(&audiopy.fil, audiopy.buffer_b, AUDIO_BUFFER_SIZE / 2, &bytesRead);
		if (fr != FR_OK)
		{
			AUDIO_DEBUG_LOG("Voice data read failed with error code: %d\n", fr);
			f_close(&audiopy.fil);
			audiopy.is_file_opened = false;
			return;
		}
		if(bytesRead < AUDIO_BUFFER_SIZE / 2)
		{
			AUDIO_DEBUG_LOG("Voice data end\n");
			f_close(&audiopy.fil);
			audiopy.is_file_opened = false;
			audiopy.is_file_read_finished = true;

			return;
		}

		Lime_audio_raw_data_cal(audiopy.buffer_b, bytesRead);

		audiopy.is_buffer_b_filled = 1;
	}
}

HAL_StatusTypeDef Lime_audio_play_start(const char* music_file_path)
{
	FRESULT fr;

	/* prepare audio player */
	audiopy.buffer_a = (uint16_t*)audiopy.buffer;
	audiopy.buffer_b = (uint16_t*)(((uint32_t)audiopy.buffer) + AUDIO_BUFFER_SIZE / 2);
	AUDIO_DEBUG_LOG("buffer_a:0x%x\n", (uint32_t)audiopy.buffer_a);
	AUDIO_DEBUG_LOG("buffer_b:0x%x\n", (uint32_t)audiopy.buffer_b);
	audiopy.now_status = audiopy_status_decode_head;
	
	/* open file */
	fr = f_open(&audiopy.fil, music_file_path, FA_READ);  
	if (fr != FR_OK) 
	{
		AUDIO_DEBUG_LOG("music file opened failed.\n");
		audiopy.now_status = audiopy_status_idle;
		
		return HAL_ERROR;
	}
	AUDIO_DEBUG_LOG("music file opened for reading.\n");
	audiopy.is_file_opened = true;

	/* read wav head */
	uint8_t temp_rd_buf[34] = {0};
	unsigned int bytesRead;
	fr = f_read(&audiopy.fil, temp_rd_buf, 12, &bytesRead);
	if (fr != FR_OK)
	{
		AUDIO_DEBUG_LOG("File read failed with error code: %d\n", fr);
		f_close(&audiopy.fil);
		
		return HAL_ERROR;
	}
	
	/* judge head1 */
	if(memcmp(temp_rd_buf + 0, "RIFF", 4) != 0)
	{
		AUDIO_DEBUG_LOG("wav head1 error\n");
		
		return HAL_ERROR;
	}
	
	/* judge head2 */
	if(memcmp(temp_rd_buf + 8, "WAVE", 4) != 0)
	{
		AUDIO_DEBUG_LOG("wav head2 error\n");
		
		return HAL_ERROR;
	}
		
	/* read data[0] -> chunk */
	uint8_t chunk_id[4];
	uint32_t chunk_size;
	fr = f_read(&audiopy.fil, temp_rd_buf, 8, &bytesRead);
	if (fr != FR_OK)
	{
		AUDIO_DEBUG_LOG("File read failed with error code: %d\n", fr);
		f_close(&audiopy.fil);
		
		return HAL_ERROR;
	}
	memcpy((uint8_t*)chunk_id, temp_rd_buf + 0, 4);
	memcpy((uint8_t*)&chunk_size, temp_rd_buf + 4, 4);
	AUDIO_DEBUG_LOG("chunk_id:%c,%c,%c,%c\n", chunk_id[0], chunk_id[1], chunk_id[2], chunk_id[3]);
	AUDIO_DEBUG_LOG("chunk_size:%d\n", chunk_size);
	if(chunk_size > sizeof(temp_rd_buf))
	{
		AUDIO_DEBUG_LOG("chunk_size is out of range!\n");
		
		return HAL_ERROR;
	}
	
	/* read data[0] */
	fr = f_read(&audiopy.fil, temp_rd_buf, chunk_size, &bytesRead);
	if (fr != FR_OK)
	{
		AUDIO_DEBUG_LOG("File read failed with error code: %d\n", fr);
		f_close(&audiopy.fil);
		
		return HAL_ERROR;
	}
	memcpy((uint8_t*)&audiopy.formatTag, temp_rd_buf + 0, 2);
	memcpy((uint8_t*)&audiopy.channels, temp_rd_buf + 2, 2);
	memcpy((uint8_t*)&audiopy.samples_per_sec, temp_rd_buf + 4, 4);
	memcpy((uint8_t*)&audiopy.avg_bytes_per_sec, temp_rd_buf + 8, 4);
	AUDIO_DEBUG_LOG("formatTag:%d\n", audiopy.formatTag);
	AUDIO_DEBUG_LOG("channels:%d\n", audiopy.channels);
	AUDIO_DEBUG_LOG("samples_per_sec:%d\n", audiopy.samples_per_sec);
	AUDIO_DEBUG_LOG("avg_bytes_per_sec:%d\n", audiopy.avg_bytes_per_sec);
	
	/* read data[1] */
	fr = f_read(&audiopy.fil, temp_rd_buf, 8, &bytesRead);
	if (fr != FR_OK)
	{
		AUDIO_DEBUG_LOG("File read failed with error code: %d\n", fr);
		f_close(&audiopy.fil);
		
		return HAL_ERROR;
	}
	memcpy((uint8_t*)chunk_id, temp_rd_buf + 0, 4);
	memcpy((uint8_t*)&chunk_size, temp_rd_buf + 4, 4);
	AUDIO_DEBUG_LOG("chunk_id:%c,%c,%c,%c\n", chunk_id[0], chunk_id[1], chunk_id[2], chunk_id[3]);
	AUDIO_DEBUG_LOG("chunk_size:%d\n", chunk_size);
	
	/* (skip)read data[1] */
	Lime_file_seek_relative(&audiopy.fil, chunk_size);
	// fr = f_read(&audiopy.fil, temp_rd_buf, chunk_size, &bytesRead);
	// if (fr != FR_OK)
	// {
	// 	AUDIO_DEBUG_LOG("File read failed with error code: %d\n", fr);
	// 	f_close(&audiopy.fil);
		
	// 	return HAL_ERROR;
	// }

	/* read data[2] */
	fr = f_read(&audiopy.fil, temp_rd_buf, 8, &bytesRead);
	if (fr != FR_OK)
	{
		AUDIO_DEBUG_LOG("File read failed with error code: %d\n", fr);
		f_close(&audiopy.fil);
		
		return HAL_ERROR;
	}
	memcpy((uint8_t*)chunk_id, temp_rd_buf + 0, 4);
	memcpy((uint8_t*)&chunk_size, temp_rd_buf + 4, 4);
	AUDIO_DEBUG_LOG("chunk_id:%c,%c,%c,%c\n", chunk_id[0], chunk_id[1], chunk_id[2], chunk_id[3]);
	AUDIO_DEBUG_LOG("chunk_size:%d\n", chunk_size);
	
	if(memcmp(chunk_id, "data", 4) != 0)
	{
		AUDIO_DEBUG_LOG("data chunk error\n");
		
		return HAL_ERROR;
	}

	/* read audio wave data */
	audiopy.is_buffer_a_filled = false;
	audiopy.is_buffer_b_filled = false;
	audiopy.is_file_read_finished = false;
	Lime_audio_read_new_data();

	/* calculate sample rate */
	htim7.Init.Prescaler = 42 - 1;
	htim7.Init.Period = (2000000 / audiopy.samples_per_sec);
	if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
	{
		AUDIO_DEBUG_LOG("TIM7 init failed\n");

		return HAL_ERROR;
	}

	/* start audio player */
	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
	HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)audiopy.buffer, AUDIO_BUFFER_SIZE / 2, DAC_ALIGN_12B_R);
	HAL_TIM_Base_Start(&htim7);
	audiopy.now_status = audiopy_status_running;

	HAL_GPIO_WritePin(AUDIO_EN_GPIO_Port, AUDIO_EN_Pin, GPIO_PIN_RESET);
	
	return HAL_OK;
}

audiopy_status_e Lime_audio_play_get_status(void)
{
	return audiopy.now_status;
}

HAL_StatusTypeDef Lime_audio_play_stop(void)
{
	HAL_TIM_Base_Stop(&htim7);

	if(audiopy.is_file_opened)
	{
		f_close(&audiopy.fil);
		audiopy.is_file_opened = false;
	}

	audiopy.now_status = audiopy_status_idle;

	return HAL_OK;
}

HAL_StatusTypeDef Lime_audio_play_set_volume(uint8_t volume);

void Lime_audio_run_handle(void)
{
	Lime_audio_read_new_data();

	if(audiopy.now_status == audiopy_status_finish)
	{
		audiopy.now_status = audiopy_status_idle;

	}
}
void Lime_audio_dma_callback(bool is_half_callback)
{
	if(is_half_callback)
	{
		audiopy.is_buffer_a_filled = false;
	}
	else
	{
		audiopy.is_buffer_b_filled = false;
	}

	if((!audiopy.is_buffer_a_filled) && (!audiopy.is_buffer_b_filled))
	{
		HAL_TIM_Base_Stop(&htim7);

		HAL_GPIO_WritePin(AUDIO_EN_GPIO_Port, AUDIO_EN_Pin, GPIO_PIN_SET);

		audiopy.now_status = audiopy_status_finish;

		// AUDIO_DEBUG_LOG("v:c\n");
	}
}







