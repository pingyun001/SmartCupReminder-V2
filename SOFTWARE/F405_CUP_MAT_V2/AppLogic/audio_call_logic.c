#include "audio_call_logic.h"
#include "audio_player.h"
#include <string.h>

#if 1
#define AUDIO_LOGIC_DEBUG_LOG DEBUG_LOG
#else
#define AUDIO_LOGIC_DEBUG_LOG(...)
#endif

audio_call_logic_t audio_call_logic = 
{
    .special_audio_u.flag.sp_hello = true,
    .is_cut_in_line = true,
};

static void audio_add_special_audio_to_list(void);
static bool audio_sync_weather_code_to_logic(void);
static void audio_sync_time_code_to_logic(void);
static uint8_t audio_get_time_zons(uint8_t hour);
static uint8_t audio_get_one_type_from_list(void);

void auido_call_add_new_music_to_list(audio_type_e audio_type, bool is_cut_in_line)
{
    if(is_cut_in_line)
    {
        /* add new music to play list <head> */
        for(int8_t i = AUDIO_PLAY_BOOK_MAX_NUM - 1; i >= 1; i--)
        {
            audio_call_logic.audio_play_book[i] = audio_call_logic.audio_play_book[i - 1];
        }

        audio_call_logic.audio_play_book_num ++;
        audio_call_logic.audio_play_book[0] = audio_type;
        audio_call_logic.is_cut_in_line = true;
    }
    else
    {
        /* check audio_call_logic.audio_play_book_num */
        if(audio_call_logic.audio_play_book_num >= AUDIO_PLAY_BOOK_MAX_NUM)
        {
            AUDIO_LOGIC_DEBUG_LOG("audio_call_logic.audio_play_book_num >= (AUDIO_PLAY_BOOK_MAX_NUM)");
            return;
        }

        /* add new music to play list <tali> */
        audio_call_logic.audio_play_book[audio_call_logic.audio_play_book_num] = audio_type;
        audio_call_logic.audio_play_book_num++;
    }
}

void audio_call_logic_sync_server(bool is_connected)
{
    audio_call_logic.is_server_connected = is_connected;
}
void audio_call_logic_sync_time(uint8_t hour)
{
    audio_call_logic.hour = hour;
}
void audio_call_logic_sync_weather(uint8_t weather_code)
{
    audio_call_logic.weather_code = weather_code;
}

static void audio_add_special_audio_to_list(void)
{
    /* if first power up, play default music */
    if(audio_call_logic.special_audio_u.flag.sp_hello)
    {
        audio_call_logic.special_audio_u.flag.sp_hello = false;
        auido_call_add_new_music_to_list(audio_type_hello, false);
    }
    
    /* check other special audio */
    if(audio_call_logic.special_audio_u.flag.sp_sunny)
    {
        audio_call_logic.special_audio_u.flag.sp_sunny = false;
        auido_call_add_new_music_to_list(audio_type_weather_sunny, false);
    }
    else if(audio_call_logic.special_audio_u.flag.sp_rainy)
    {
        audio_call_logic.special_audio_u.flag.sp_rainy = false;
        auido_call_add_new_music_to_list(audio_type_weather_rainy, false);
    }
    else if(audio_call_logic.special_audio_u.flag.sp_snowy)
    {
        audio_call_logic.special_audio_u.flag.sp_snowy = false;
        auido_call_add_new_music_to_list(audio_type_weather_snowy, false);
    }
    else if(audio_call_logic.special_audio_u.flag.sp_morning)
    {
        audio_call_logic.special_audio_u.flag.sp_morning = false;
        auido_call_add_new_music_to_list(audio_type_morning, false);
    }
    else if(audio_call_logic.special_audio_u.flag.sp_noon)
    {
        audio_call_logic.special_audio_u.flag.sp_noon = false;
        auido_call_add_new_music_to_list(audio_type_noon, false);
    }
    else if(audio_call_logic.special_audio_u.flag.sp_night)
    {
        audio_call_logic.special_audio_u.flag.sp_night = false;
        auido_call_add_new_music_to_list(audio_type_night, false);
    }
}

static bool audio_sync_weather_code_to_logic(void)
{
    if(audio_call_logic.weather_code <= 3)
    {
        audio_call_logic.special_audio_u.flag.sp_sunny = true;
        return true;
    }
    else if((audio_call_logic.weather_code >= 10) && (audio_call_logic.weather_code <= 19))
    {
        audio_call_logic.special_audio_u.flag.sp_rainy = true;
        return true;
    }
    else if((audio_call_logic.weather_code >= 20) && (audio_call_logic.weather_code <= 25))
    {
        audio_call_logic.special_audio_u.flag.sp_snowy = true;
        return true;
    }

    return false;
}

static void audio_sync_time_code_to_logic(void)
{
    if((audio_call_logic.hour >= 7) && (audio_call_logic.hour <= 10))
    {
        audio_call_logic.special_audio_u.flag.sp_morning = true;
    }
    else if((audio_call_logic.hour >= 12) && (audio_call_logic.hour <= 17))
    {
        audio_call_logic.special_audio_u.flag.sp_noon = true;
    }
    else if((audio_call_logic.hour >= 18) && (audio_call_logic.hour <= 22))
    {
        audio_call_logic.special_audio_u.flag.sp_night = true;
    }
}

static uint8_t audio_get_time_zons(uint8_t hour)
{
    if((hour >= 7) && (hour <= 10))
        return 1;
    else if((hour >= 12) && (hour <= 17))
        return 2;
    else if((hour >= 18) && (hour <= 22))
        return 3;

    /* others(midnight, user is sleeping) */
    return 0;
}

static uint8_t audio_get_one_type_from_list(void)
{
    uint8_t index = audio_call_logic.audio_play_book[0];

    /* last audio out fifo */
	audio_call_logic.audio_play_book_num --;
	for(uint8_t i = 0; i < AUDIO_PLAY_BOOK_MAX_NUM - 1; i++)
	{
		audio_call_logic.audio_play_book[i] = audio_call_logic.audio_play_book[i + 1];
	}

    return index;
}

void audio_call_logic_handle(void)
{
	char path[24] = {0};
	uint8_t index;

    /* power up, add special audio to play list */
    if(audio_call_logic.special_audio_u.flag.sp_hello)
    {
        audio_add_special_audio_to_list();
    }
	
    /* check wifi status */
    if(audio_call_logic.is_server_connected)
    {
        /* first connect */
        if( !audio_call_logic.is_server_connected_last)
        {
            /* check weather */
            bool is_weather_has_audio = audio_sync_weather_code_to_logic();

            /* if no weather audio, add time audio */
            if( !is_weather_has_audio)
                audio_sync_time_code_to_logic();

            /* is cut in line */
            audio_call_logic.is_cut_in_line = true;
        }

        /* continue connect */
        else
        {
            /* across time zones && now time zons isn't midnight */
            if((audio_get_time_zons(audio_call_logic.hour) != audio_get_time_zons(audio_call_logic.hour_last)) &&
               (audio_get_time_zons(audio_call_logic.hour) != 0))
            {
                /* chean list */
                memset(audio_call_logic.audio_play_book, audio_type_null, sizeof(audio_call_logic.audio_play_book));
                audio_call_logic.audio_play_book_num = 0;

                /* check weather */
                audio_sync_weather_code_to_logic();

                /* check time */
                audio_sync_time_code_to_logic();
            }
        }

        /* add special audio to play list */
        while(audio_call_logic.special_audio_u.has_special_audio)
        {
            audio_add_special_audio_to_list();
        }
    }
    audio_call_logic.is_server_connected_last = audio_call_logic.is_server_connected;
    audio_call_logic.hour_last = audio_call_logic.hour;

    /* has cut in line, stop current music and play new music now! */
    if(audio_call_logic.is_cut_in_line)
    {
        audio_call_logic.is_cut_in_line = false;

        /* cut in line, stop current music */
        Lime_audio_play_stop();

        goto play_audio;
    }

    /* has another audio behind */
    if( (audio_call_logic.is_cut_in_line_last) && 
        (audio_call_logic.audio_play_book_num != 0) &&
        (Lime_audio_play_get_status() == audiopy_status_idle))
    {
        audio_call_logic.is_cut_in_line_last = false;

        goto play_audio;
    }


    return;

play_audio:
    /* get target audio path */
    index = audio_get_one_type_from_list();
	snprintf(path, sizeof(path), "D:voice/%d.wav", index);
	
	/* has another audio behind */
    audio_call_logic.is_cut_in_line_last = (audio_call_logic.audio_play_book_num != 0);

    /* play new music */
	Lime_audio_play_music(path);
}
