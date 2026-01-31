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
#include "Lime_App_Hal.h"

static void read_sync_setting_info(void);

static void ds18b20_scan_handle(void);

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
	
	/* read setting.txt */
	read_sync_setting_info();
	
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
		
		ds18b20_scan_handle();
		
//		Lime_audio_run_handle();
		
		osDelay(10);
	}
}

static void ds18b20_scan_handle(void)
{
	/* suspend tasks */
	vTaskSuspendAll();
	
	/* get DS28B20 Temper */
	int16_t ds18b20_raw = ds18b20_get_temp();
	float home_temp = ds18b20_raw / 10.0f;
	
	/* sync to lvgl hal */
	LimeHAL_SetHomeTemerSensor(home_temp);
	
	/* resume tasks */
	if( !xTaskResumeAll())
	{
		taskYIELD();
	}
}

static void read_sync_setting_info(void)
{
	/* malloc buffer */
	setting_file_info_t *info = pvPortMalloc(sizeof(setting_file_info_t));
	if(info == NULL)
	{
		DEBUG_LOG("malloc Failed\n");
		
		goto fill_default;
	}
	
	/* read para from setting.txt */
	if(file_system_read_setting_file(info) != HAL_OK)
	{
		vPortFree(info);
		
		goto fill_default;
	}
	
	/* setting para */
	DEBUG_LOG("readed setting.txt para:");
	DEBUG_LOG("wifi_name:%s", info->wifi_name);
	DEBUG_LOG("wifi_password:%s", info->wifi_password);
	DEBUG_LOG("city_name:%s", info->city_name);
	esp8266_set_wifi_info(info->wifi_name, info->wifi_password);
	esp8266_set_position(info->city_name);
	
	/* free buffer */
	vPortFree(info);
	
	return;
	
fill_default:
	DEBUG_LOG("fill default para:\n");
	DEBUG_LOG("wifi_name:%s", GLOBAL_DEFAULT_WIFI_NAME);
	DEBUG_LOG("wifi_password:%s", GLOBAL_DEFAULT_WIFI_PASSWORD);
	DEBUG_LOG("city_name:%s", GLOBAL_DEFAULT_CITY_NAME);
	esp8266_set_wifi_info(GLOBAL_DEFAULT_WIFI_NAME, GLOBAL_DEFAULT_WIFI_PASSWORD);
	esp8266_set_position(GLOBAL_DEFAULT_CITY_NAME);
}




