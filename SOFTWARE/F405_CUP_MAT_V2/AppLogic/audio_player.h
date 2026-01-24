#ifndef __AUDIO_PLAYER_H__
#define __AUDIO_PLAYER_H__

#include "main.h"
#include "ff.h"

typedef enum
{
	audiopy_status_idle,
	audiopy_status_decode_head,
	audiopy_status_running,
	audiopy_status_finish,
}audiopy_status_e;

typedef struct
{
	audiopy_status_e now_status;
	
	FIL fil;
	
	/* decode from file */
	uint16_t formatTag;
	uint16_t channels;
	uint32_t samples_per_sec;
	uint32_t avg_bytes_per_sec;
	
	uint32_t total_pcm_bytes;
	uint32_t total_time_ms;
	
	uint8_t user_setted_volume;
	
	bool is_buffer_a_filled;
	uint32_t buffer_a[4096 / 4];
	uint32_t buffer_a_data_len;
	
	bool is_buffer_b_filled;
	uint32_t buffer_b[4096 / 4];
	uint32_t buffer_b_data_len;
	
}audiopy_t;

HAL_StatusTypeDef Lime_audio_play_start(const char* music_file_path);

HAL_StatusTypeDef Lime_audio_play_stop(void);

HAL_StatusTypeDef Lime_audio_play_set_volume(uint8_t volume);

void Lime_audio_run_handle(void);
void Lime_audio_half_callback(void);

#endif //__AUDIO_PLAYER_H__
