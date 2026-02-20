#include "audio_main.h"
#include "cmsis_os.h"
#include "audio_player.h"

static TaskHandle_t audio_task_handler = NULL;

void audio_main(void const * argument)
{
	/* get the handle of task */
	audio_task_handler = xTaskGetHandle(pcTaskGetName(NULL));
	
	/* waiting FatFs mount */
	if(ulTaskNotifyTake(pdTRUE,pdMS_TO_TICKS(5000))  != pdPASS)
	{
		DEBUG_LOG(">>>Task:%s, ulTaskNotifyTake Timeout!\n", __FUNCTION__);
		
		while(1)
		{
			osDelay(1000);
		}
	}
	else
	{
		DEBUG_LOG(">>>Task:%s, start!\n", __FUNCTION__);
	}
	
	while(1)
	{
		/* audio run handle */
		Lime_audio_run_handle();
		
		/* wait 60ms(MAX 120ms, because of buffer size) */
		osDelay(60);
	}
}

void audio_main_task_yleld_exHook(bool isISR)
{
	if(audio_task_handler == NULL)
		return;
	
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		static BaseType_t xHigherPriorityTaskWoken;
		
		if(isISR)
		{
			vTaskNotifyGiveFromISR(audio_task_handler, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		else
		{
			xTaskNotifyGive(audio_task_handler);
			taskYIELD();
		}
	}
}
