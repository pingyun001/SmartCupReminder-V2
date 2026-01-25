#include "sensor_main.h"
#include "cmsis_os.h"

#include "ws2812.h"
#include "ds18b20.h"
#include "key.h"
#include "ff.h"
#include "esp8266.h"

/* just test */
#include "file_system_logic.h"

#include "audio_player.h"

void sensor_main(void const * argument)
{
	DEBUG_LOG("Task %s,start\n", __FUNCTION__);
	
	/* mount fatfs */
	if(file_system_Init() != HAL_OK)
	{
		DEBUG_LOG("file system init failed\n");
	}
	
	if(file_system_confirm() != HAL_OK)
	{
		DEBUG_LOG("resources confirm failed\n");
	}
	
	/* Just test audio player */
//	while(1)
//	{
////		if(Lime_audio_play_get_status() == audiopy_status_idle)
////		{
////			osDelay(1000);

////			static uint8_t i = 1;
////			char file_name[50] = {0};
////			sprintf(file_name, "D:voice/%d.wav", i);
////			DEBUG_LOG("play file:%s\n", file_name);

////			Lime_audio_play_start(file_name);
////			
////			if(i >= 7)
////				while(1)
////					;

////			i = (i >= 7) ? 1 : i + 1;
////		}
//		

//		Lime_audio_run_handle();
//		osDelay(10);
//	}
	
	/* init ws2812 */
	ws2812_Init();
	
	/* init ds18b20 */
	while(ds18b20_Init())
	{
		printf("ds18b20 init err\n");
		HAL_Delay(300);
	}
	
	/* init ESP8266 */
	esp8266_Init(1000);
	esp8266_set_position("Beijing");
	esp8266_set_wifi_info("ziroom1802", "4001001111");
	
	while(1)
	{
		// printf("usb Vol:%.2f V\n", key_get_usb_vol());
		
		static bool is_cup_deteched_last = false;
		bool is_cup_deteched = IS_CUP_DETECHED();
		if(is_cup_deteched && !is_cup_deteched_last)
		{
			Lime_audio_play_stop();
			osDelay(10);
			Lime_audio_play_start("D:voice/3.wav");
		}
		if(!is_cup_deteched && is_cup_deteched_last)
		{
			Lime_audio_play_stop();
			osDelay(10);
			Lime_audio_play_start("D:voice/4.wav");
		}
		is_cup_deteched_last = is_cup_deteched;

		esp8266_sync_handle();
		
//		Lime_audio_run_handle();
		
		osDelay(10);
	}
}








