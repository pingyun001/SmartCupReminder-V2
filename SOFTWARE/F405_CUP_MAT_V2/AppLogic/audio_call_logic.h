#ifndef __AUDIO_CALL_LOGIC_H__
#define __AUDIO_CALL_LOGIC_H__

#include "main.h"

typedef enum
{
	audio_type_null = 0,
	audio_type_hello = 1,
	audio_type_cup_found = 2,
	audio_type_time_up = 3,
	audio_type_cup_lost = 4,
	audio_type_weather_sunny = 5,
	audio_type_weather_rainy = 6,
	audio_type_weather_snowy = 7,
	audio_type_morning = 8,
	audio_type_noon = 9,
	audio_type_night = 10,
}audio_type_e;

typedef struct
{
	bool sp_hello : 1;
	bool sp_sunny : 1;
	bool sp_rainy : 1;
	bool sp_snowy : 1;
	bool sp_morning : 1;
	bool sp_noon : 1;
	bool sp_night : 1;
}special_audio_t;

#define AUDIO_PLAY_BOOK_MAX_NUM 5

typedef struct
{
	audio_type_e audio_play_book[AUDIO_PLAY_BOOK_MAX_NUM];
	uint8_t audio_play_book_num;

	bool is_cut_in_line;
	bool is_cut_in_line_last;

	bool is_server_connected;
	bool is_server_connected_last;

	/* morning:  7:00 ~ 10:59
	   noon:    12:00 ~ 17:59
	   night:   18:00 ~ 22:59 */
	uint8_t hour;
	uint8_t hour_last;

	/* (https://www.seniverse.com/)
	0~3:sunny, 10~19:rainy, 20~25:snowy, 99:unknown */
	uint8_t weather_code;

	union
	{
		uint8_t has_special_audio;
		special_audio_t flag;
	}special_audio_u;
	
}audio_call_logic_t;

void auido_call_add_new_music_to_list(audio_type_e audio_type, bool is_cut_in_line);

void audio_call_logic_handle(void);

void audio_call_logic_sync_server(bool is_connected);
void audio_call_logic_sync_time(uint8_t hour);
void audio_call_logic_sync_weather(uint8_t weather_code);

#endif	//__AUDIO_CALL_LOGIC_H__
