#include "audio_player.h"
#include <string.h>

audiopy_t audiopy = {0};

HAL_StatusTypeDef Lime_audio_play_start(const char* music_file_path)
{
	FRESULT fr;
	
	/* open file */
	fr = f_open(&audiopy.fil, music_file_path, FA_READ);  
	if (fr != FR_OK) 
	{
		DEBUG_LOG("music file opened failed.\n");
		
		return HAL_ERROR;
	}
	DEBUG_LOG("music file opened for reading.\n");

	/* read wav head */
	uint8_t temp_rd_buf[34] = {0};
	unsigned int bytesRead;
	fr = f_read(&audiopy.fil, temp_rd_buf, 12, &bytesRead);
	if (fr != FR_OK)
	{
		DEBUG_LOG("File read failed with error code: %d\n", fr);
		f_close(&audiopy.fil);
		
		return HAL_ERROR;
	}
	
	/* judge head1 */
	if(memcmp(temp_rd_buf + 0, "RIFF", 4) != 0)
	{
		DEBUG_LOG("wav head1 error\n");
		
		return HAL_ERROR;
	}
	
	/* judge head2 */
	if(memcmp(temp_rd_buf + 8, "WAVE", 4) != 0)
	{
		DEBUG_LOG("wav head2 error\n");
		
		return HAL_ERROR;
	}
		
	/* read data[0] -> chunk */
	uint8_t chunk_id[4];
	uint32_t chunk_size;
	fr = f_read(&audiopy.fil, temp_rd_buf, 8, &bytesRead);
	if (fr != FR_OK)
	{
		DEBUG_LOG("File read failed with error code: %d\n", fr);
		f_close(&audiopy.fil);
		
		return HAL_ERROR;
	}
	memcpy((uint8_t*)chunk_id, temp_rd_buf + 0, 4);
	memcpy((uint8_t*)&chunk_size, temp_rd_buf + 4, 4);
	DEBUG_LOG("chunk_id:%c,%c,%c,%c\n", chunk_id[0], chunk_id[1], chunk_id[2], chunk_id[3]);
	DEBUG_LOG("chunk_size:%d\n", chunk_size);
	if(chunk_size > sizeof(temp_rd_buf))
	{
		DEBUG_LOG("chunk_size is out of range!\n");
		
		return HAL_ERROR;
	}
	
	/* read data[0] -> fmt */
	fr = f_read(&audiopy.fil, temp_rd_buf, chunk_size, &bytesRead);
	if (fr != FR_OK)
	{
		DEBUG_LOG("File read failed with error code: %d\n", fr);
		f_close(&audiopy.fil);
		
		return HAL_ERROR;
	}
	memcpy((uint8_t*)&audiopy.formatTag, temp_rd_buf + 0, 2);
	memcpy((uint8_t*)&audiopy.channels, temp_rd_buf + 2, 2);
	memcpy((uint8_t*)&audiopy.samples_per_sec, temp_rd_buf + 4, 4);
	memcpy((uint8_t*)&audiopy.avg_bytes_per_sec, temp_rd_buf + 8, 4);
	
	/* read data[1] */
	
	
	return HAL_OK;
}

HAL_StatusTypeDef Lime_audio_play_stop(void);

HAL_StatusTypeDef Lime_audio_play_set_volume(uint8_t volume);

void Lime_audio_run_handle(void);
void Lime_audio_half_callback(void);







