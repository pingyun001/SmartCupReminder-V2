#include "sensor_main.h"
#include "cmsis_os.h"

#include "ws2812.h"
#include "ds18b20.h"
#include "key.h"
#include "spi_flash.h"
#include "ff.h"
#include "esp8266.h"

/* just test */
#include "file_system_logic.h"

#include "audio_player.h"
#include "Lime_App_Hal.h"

static void senser_task_error_handle(void);
static void read_sync_setting_info(void);
static void ds18b20_scan_handle(void);
static void play_flash_music(void);

void sensor_main(void const * argument)
{
	DEBUG_LOG("Task %s,start\n", __FUNCTION__);
	
	/* mount fatfs */
	LimeHAL_SetInitStep(10, "flash");
	if(file_system_Init() != HAL_OK)
	{
		DEBUG_LOG("file system init failed\n");
		
		/* show UI */
		LimeHAL_SetInitStep(11, "flash - failed!");
		
		/* enter error handle */
		senser_task_error_handle();
	}
	LimeHAL_SetFlashID(spi_flash_get_jedec_id());
	
	osDelay(100);
	
	/* confirm necessery files */
	LimeHAL_SetInitStep(20, "file system");
	if(file_system_confirm() != HAL_OK)
	{
		DEBUG_LOG("resources confirm failed\n");
		
		/* show UI */
		LimeHAL_SetInitStep(21, "file system confirm failed!");
		
		/* enter error handle */
		senser_task_error_handle();
	}
	
	osDelay(100);
	
	/* read setting.txt */
	LimeHAL_SetInitStep(30, "file system");
	read_sync_setting_info();
	
	osDelay(100);
	
	/* init ws2812 */
	LimeHAL_SetInitStep(40, "rgb led");
	ws2812_Init();
	
	/* init ds18b20 */
	LimeHAL_SetInitStep(50, "temperature senser");
	while(ds18b20_Init())
	{
		static uint8_t err_cnt = 0;
		printf("ds18b20 init err\n");
		HAL_Delay(300);
		
		err_cnt ++;
		
		LimeHAL_SetInitStep(50 + err_cnt * 2, "temperature senser");
		
		if(err_cnt > 5)
		{
			LimeHAL_SetInitStep(59, "temperature senser failed!");
			osDelay(1000);
		}
	}
	
	osDelay(100);
	
	/* init ESP8266 */
	LimeHAL_SetInitStep(70, "ESP8266");
	if(esp8266_Init(1000) != HAL_OK)
	{
		DEBUG_LOG("ESP8266 init failed\n");
		
		LimeHAL_SetInitStep(71, "ESP8266 offline");
		
		osDelay(1000);
	}
	
	osDelay(100);
	
	LimeHAL_SetInitStep(100, "finish");
	
	osDelay(500);
	
	while(1)
	{
		// printf("usb Vol:%.2f V\n", key_get_usb_vol());
		
		static bool is_cup_deteched_last = false;
		bool is_cup_deteched = IS_CUP_DETECHED();
		if(is_cup_deteched && !is_cup_deteched_last)
		{
			LimeHAL_SettingInfo_PlayMusicByIndex(3);
			
//			Lime_audio_play_stop();
//			osDelay(10);
//			Lime_audio_play_start("D:voice/3.wav");
		}
		if(!is_cup_deteched && is_cup_deteched_last)
		{
//			Lime_audio_play_stop();
//			osDelay(10);
//			Lime_audio_play_start("D:voice/4.wav");
			LimeHAL_SettingInfo_PlayMusicByIndex(4);
		}
		is_cup_deteched_last = is_cup_deteched;

		esp8266_sync_handle();
		
		ds18b20_scan_handle();
		
		play_flash_music();
		
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
	DEBUG_LOG("readed setting.txt para:\n");
	DEBUG_LOG("wifi_name:%s\n", info->wifi_name);
	DEBUG_LOG("wifi_password:%s\n", info->wifi_password);
	DEBUG_LOG("city_name:%s\n", info->city_name);
	esp8266_set_wifi_info(info->wifi_name, info->wifi_password);
	esp8266_set_position(info->city_name);
	
	/* free buffer */
	vPortFree(info);
	
	return;
	
fill_default:
	DEBUG_LOG("fill default para:\n");
	DEBUG_LOG("wifi_name:%s\n", GLOBAL_DEFAULT_WIFI_NAME);
	DEBUG_LOG("wifi_password:%s\n", GLOBAL_DEFAULT_WIFI_PASSWORD);
	DEBUG_LOG("city_name:%s\n", GLOBAL_DEFAULT_CITY_NAME);
	esp8266_set_wifi_info(GLOBAL_DEFAULT_WIFI_NAME, GLOBAL_DEFAULT_WIFI_PASSWORD);
	esp8266_set_position(GLOBAL_DEFAULT_CITY_NAME);
}

static void play_flash_music(void)
{
	/* no need play music */
	if( !LimeHAL_IsNeed_PlayMusic())
		return;
	
	/* get music index */
	uint8_t index = LimeHAL_GetPlayMusicIndex();
	
	/* synthesis path */
	char path[24] = {0};
	snprintf(path, sizeof(path), "D:voice/%d.wav", index);
	DEBUG_LOG("%s(), play path:%s\n", __FUNCTION__, path);
	
	/* stop play */
	Lime_audio_play_stop();
	osDelay(10);
	
	/* suspend tasks, be sure file read not break by lvgl */
	vTaskSuspendAll();
	
	/* play new music */
	Lime_audio_play_start(path);
	
	/* resume tasks */
	if( !xTaskResumeAll())
	{
		taskYIELD();
	}
}

static void senser_task_error_handle(void)
{
	while(1)
	{
		osDelay(1000);
	}
}


