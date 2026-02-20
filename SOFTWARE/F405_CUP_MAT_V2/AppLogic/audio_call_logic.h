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

#define AUDIO_PLAY_BOOK_MAX_NUM 5

typedef struct
{
	audio_type_e audio_play_book[AUDIO_PLAY_BOOK_MAX_NUM];
	uint8_t audio_play_book_num;

	bool is_first_power_up;
}audio_call_logic_t;

void auido_call_add_new_music(audio_type_e audio_type, bool is_cut_in_line);

void audio_call_logic_handle(void);

#endif	//__AUDIO_CALL_LOGIC_H__
