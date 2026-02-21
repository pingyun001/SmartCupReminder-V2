#ifndef __AUDIO_PLAYER_H__
#define __AUDIO_PLAYER_H__

#include "main.h"
#include "ff.h"

#define AUDIO_BUFFER_SIZE 8192

typedef enum
{
	audiopy_status_idle,
	audiopy_status_get_file_name,
	audiopy_status_decode_head,
	audiopy_status_running,
	audiopy_status_finish,
}audiopy_status_e;

typedef struct
{
	audiopy_status_e now_status;
	
	char file_name[63];
	bool new_file_name_need_play;
	FIL fil;
	bool is_file_opened;
	bool is_file_read_finished;
	
	/* decode from file */
	uint16_t formatTag;
	uint16_t channels;
	uint32_t samples_per_sec;
	uint32_t avg_bytes_per_sec;
	
	uint32_t total_pcm_bytes;
	uint32_t played_pcm_bytes;
	uint32_t total_time_ms;
	
	uint8_t user_setted_volume;
	
	bool is_buffer_a_filled;
	uint16_t *buffer_a;
	uint32_t buffer_a_data_len;
	
	bool is_buffer_b_filled;
	uint16_t *buffer_b;
	uint32_t buffer_b_data_len;

	uint32_t buffer[AUDIO_BUFFER_SIZE / 4];
	
}audiopy_t;

HAL_StatusTypeDef Lime_audio_play_start(const char* music_file_path);

HAL_StatusTypeDef Lime_audio_play_stop(void);

HAL_StatusTypeDef Lime_audio_play_set_volume(uint8_t volume);

audiopy_status_e Lime_audio_play_get_status(void);

void Lime_audio_play_music(const char* music_file_path);
void Lime_audio_run_handle(void);
void Lime_audio_dma_callback(bool is_half_callback);

#endif //__AUDIO_PLAYER_H__
