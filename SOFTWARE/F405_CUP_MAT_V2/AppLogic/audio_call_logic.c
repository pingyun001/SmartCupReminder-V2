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
    .is_first_power_up = true,
};

void auido_call_add_new_music(audio_type_e audio_type, bool is_cut_in_line)
{
    if(is_cut_in_line)
    {
        /* cut in line, stop current music */
        Lime_audio_play_stop();

        /* clean play list */
        memset(audio_call_logic.audio_play_book, audio_type_null, sizeof(audio_call_logic.audio_play_book));

        /* add new music to play list */
        audio_call_logic.audio_play_book_num = 1;
        audio_call_logic.audio_play_book[0] = audio_type;
    }
    else
    {
        /* check audio_call_logic.audio_play_book_num */
        if(audio_call_logic.audio_play_book_num >= AUDIO_PLAY_BOOK_MAX_NUM)
        {
            AUDIO_LOGIC_DEBUG_LOG("audio_call_logic.audio_play_book_num >= (AUDIO_PLAY_BOOK_MAX_NUM - 1)");
            return;
        }

        /* add new music to play list */
        audio_call_logic.audio_play_book[audio_call_logic.audio_play_book_num] = audio_type;
        audio_call_logic.audio_play_book_num++;
    }
}


void audio_call_logic_handle(void)
{
    /* if first power up, play default music */
    if((audio_call_logic.is_first_power_up) && (HAL_GetTick() > 3000))
    {
        audio_call_logic.is_first_power_up = false;
        auido_call_add_new_music(audio_type_hello, false);
    }


	/* get low level audio status */
    if(Lime_audio_play_get_status() != audiopy_status_idle)
    {
        return;
    }

    /* check audio_call_logic.audio_play_book_num */
    if(audio_call_logic.audio_play_book_num == 0)
    {
        return;
    }

    /* play audio */
    char path[24] = {0};
    uint8_t index = audio_call_logic.audio_play_book[0];
	snprintf(path, sizeof(path), "D:voice/%d.wav", index);
	
	/* last audio out fifo */
	audio_call_logic.audio_play_book_num --;
	for(uint8_t i = 0; i < AUDIO_PLAY_BOOK_MAX_NUM; i++)
	{
		audio_call_logic.audio_play_book[i] = audio_call_logic.audio_play_book[i + 1];
	}

    /* play new music */
	Lime_audio_play_music(path);
}
