#ifndef __AUDIO_MAIN_H
#define __AUDIO_MAIN_H

#include "main.h"

void audio_main_task_yleld_exHook(bool isISR);

void audio_main_task_suspend(void);

#endif 		//__AUDIO_MAIN_H
