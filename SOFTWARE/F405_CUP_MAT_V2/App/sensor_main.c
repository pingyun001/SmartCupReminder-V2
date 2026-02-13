#include "sensor_main.h"
#include "cmsis_os.h"

#include "ws2812.h"
#include "ds18b20.h"
#include "key.h"
#include "spi_flash.h"
#include "rtc_time.h"
#include "ff.h"
#include "esp8266.h"
#include "rgbled_logic.h"

/* just test */
#include "file_system_logic.h"

#include "audio_player.h"
#include "Lime_App_Hal.h"

static void senser_task_error_handle(void);
static void read_sync_setting_info(void);
static void ds18b20_scan_handle(void);
static void play_flash_music_handle(void);
static void time_logic_handle(void);
static void volume_sync_handle(void);
static void cup_scan_handle(void);
static void rgb_led_run_handle(void);

void sensor_main(void const * argument)
{
	DEBUG_LOG("Task %s,start\n", __FUNCTION__);
	
	osDelay(1000);
	LimeRtc_SetNowTime(12, 30, 0);
	
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
		
		osDelay(500);
		
		/* enter error handle */
		LimeHAL_SetFileSystemErr();
		while(1)
		{
			osDelay(100);
		}
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

		esp8266_sync_handle();
		
		ds18b20_scan_handle();
		
		play_flash_music_handle();
		
		time_logic_handle();
		
		volume_sync_handle();
		
		cup_scan_handle();
		
		rgb_led_run_handle();
		
		osDelay(10);
	}
}

static void ds18b20_scan_handle(void)
{
	/* limit frequence */
	static uint32_t last_run_time = 0;
	if(HAL_GetTick() - last_run_time < 200)
	{
		return;
	}
	last_run_time = HAL_GetTick();
	
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

static void play_flash_music_handle(void)
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
	
	/* play new music */
	Lime_audio_play_start(path);
	
}

static void time_logic_handle(void)
{
	static uint32_t last_run_time = 0;
	if(HAL_GetTick() - last_run_time < 1000)
		return;
	last_run_time = HAL_GetTick();
	
	/* get now time from RTC */
	volatile uint8_t hour = 0, minute = 0, second = 0;
	LimeRtc_GetNowTime((uint8_t*)&hour, (uint8_t*)&minute, (uint8_t*)&second);
	
	/* sync time to GUI */
	LimeHAL_SetTime(hour, minute);
}

static void cup_scan_handle(void)
{
	static bool is_countdown = false;
	static bool is_timeout = false;
	uint32_t timeout_second = 0;
	
	/* get setted info */
	uint8_t timeout_index = LimeHAL_GetCountDownTimeIndex();
	switch(timeout_index)
	{
		case 1:
			timeout_second = 1 * 60;
			break;
		case 2:
			timeout_second = 10;
			break;
		case 3:
			timeout_second = 10 * 60;
			break;
		case 4:
			timeout_second = 25 * 60;
			break;
		case 5:
			timeout_second = 45 * 60;
			break;
		case 6:
			timeout_second = 60 * 60;
			break;
		default:
			timeout_second = 120 * 60;
			DEBUG_LOG("timeout_index:%d,err\n", timeout_index);
			break;
	}
	
	/* cup status detech */
	static bool is_cup_deteched_last = false;
	bool is_cup_deteched = IS_CUP_DETECHED();
	
	/* cup senser is pressed */
	if(is_cup_deteched && !is_cup_deteched_last)
	{
		LimeHAL_SettingInfo_PlayMusicByIndex(2);
		LimeRtc_SetSoftAlarm(timeout_second);
		is_timeout = false;
	}
	
	/* cup senser is released */
	if(!is_cup_deteched && is_cup_deteched_last)
	{
		LimeHAL_SettingInfo_PlayMusicByIndex(4);
	}
	is_cup_deteched_last = is_cup_deteched;
	
	/* timeout check */
	int32_t elaps_time_seconds = LimeRtc_GetSoftAlarm_LastTime();
	if(is_cup_deteched)
	{
		/* no timeout */
		if(elaps_time_seconds >= 0)
		{
			LimeHAL_SetWorkingStatus(LimeHal_WoringStatus_Countdown);
			LimeHAL_SetRemainCountSeconds(elaps_time_seconds);
		}
		
		/* timeout */
		else
		{
			LimeHAL_SetWorkingStatus(LimeHal_WoringStatus_CountFinish);
			LimeHAL_SetRemainCountSeconds(0);
			if( !is_timeout)
			{
				is_timeout = true;
				LimeHAL_SettingInfo_PlayMusicByIndex(3);
			}
		}
	}
	else
	{
		LimeHAL_SetWorkingStatus(LimeHal_WoringStatus_Idle);
		LimeHAL_SetRemainCountSeconds(timeout_second);
	}
	
	LimeHAL_SetTotalCountSeconds(timeout_second);
	
	/* debug */
//	static uint32_t last_run_time = 0;
//	if(HAL_GetTick() - last_run_time >= 250)
//	{
//		last_run_time = HAL_GetTick();
//		DEBUG_LOG("elaps_time_seconds:%d\n", elaps_time_seconds);
//	}
}

void senser_main_set_now_time_hook(uint8_t hour, uint8_t minute, uint8_t second)
{
	LimeRtc_SetNowTime(hour, minute, second);
}

static void volume_sync_handle(void)
{
	uint8_t volume_setted = LimeHAL_GetVolume();
	
	/* 0:off, 1:low, 2:mid, 3:high */
	switch(volume_setted)
	{
		case 0:
			volume_setted = 0;
			break;
		case 1:
			volume_setted = 160;
			break;
		case 2:
			volume_setted = 220;
			break;
		case 3:
			volume_setted = 255;
			break;
		default:
			volume_setted = 255;
			break;
	}
	Lime_audio_play_set_volume(volume_setted);
}

static void rgb_led_run_handle(void)
{
	static uint32_t last_run_time = 0;
	if(HAL_GetTick() - last_run_time < 100)
		return;
	last_run_time = HAL_GetTick();
	
	/* get GUI setted value */
	uint8_t led_brightness = LimeHAL_GetLumen();
	uint8_t mode = LimeHAL_GetLightMode();
	
	/* get now led mode */
	rgbled_mode_e led_mode = rgbled_mode_off;
	switch(mode)
	{
		case 0:
			led_mode = rgbled_mode_rainbow;
			break;
		case 1:
			led_mode = rgbled_mode_candle;
			break;
		case 2:
			led_mode = rgbled_mode_starnight;
			break;
		case 3:
			led_mode = rgbled_mode_white;
			break;
		default:
			led_mode = rgbled_mode_off;
			break;
	}
	
	/* get now led brightness */
	switch(led_brightness)
	{
		case 0:
			led_brightness = 0;
			led_mode = rgbled_mode_off;
			break;
		case 1:
			led_brightness = 16;
			break;
		case 2:
			led_brightness = 32;
			break;
		case 3:
			led_brightness = 48;
			break;
		default:
			led_brightness = 4;
			break;
			
	}
	
	/* sync to low driver */
	rgbled_set_mode(led_mode);
	rgbled_set_brightness(led_brightness);
	
	/* led run handle */
	rgbled_run_handler();
}

static void senser_task_error_handle(void)
{
	while(1)
	{
		osDelay(1000);
	}
}


