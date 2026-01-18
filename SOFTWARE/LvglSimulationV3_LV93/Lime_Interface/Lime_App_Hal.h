#ifndef __Lime_App_Hal_H__
#define __Lime_App_Hal_H__


#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

typedef struct
{
	char hardwareName[32];
	uint8_t initPercent;

	/* if allHardwarePass==1, start interface will exit*/
	uint8_t allHardwarePass;

}LimeHAL_Hardware_Online_Info_t;

typedef struct
{
	uint8_t sw_up;
	uint8_t sw_set;
	uint8_t sw_left;
	uint8_t sw_right;
	uint8_t sw_down;
}LimeHal_KeyInfo_t;

/* working status */
//LimeHal_WoringStatus_Init -> LimeHal_WoringStatus_Idle -> LimeHal_WoringStatus_Countdown -> LimeHal_WoringStatus_CountFinish ->(back to LimeHal_WoringStatus_Idle)
typedef enum
{
	LimeHal_WoringStatus_Init = 0,
	LimeHal_WoringStatus_Idle,
	LimeHal_WoringStatus_Countdown,
	LimeHal_WoringStatus_CountFinish,
}LimeHal_WoringStatus_e;
typedef struct
{
	LimeHal_WoringStatus_e workingStatus;
	uint16_t remainCountSeconds;
	uint16_t totalCountSeconds;
	bool isSleepMode;

}LimeHal_WorkingInfo_t;


typedef struct
{
	uint8_t month, day;
	uint8_t weatherLogoID;
	char weatherChinese[16];
	float temperaLow;
	float temperaHigh;
	float humidity;

}dayWeather_t;

typedef struct
{
	/* from adc */
	float usbVolt;

	/* from ds18b20 */
	float homeTemper;

	/* from esp time api */
	uint8_t hour, minute;

	/* from esp weather api */
	bool isWifiConnected;
	bool isWeatherDataValid;
	char cityName[32];
	float nowTemper;
	float nowHumi;
	dayWeather_t todayWeather;
	dayWeather_t tomorrowWeather;
	dayWeather_t dayAfterTomorrowWeather;
}LimeHal_SenserInfo_t;
typedef struct
{
	/* hardware online info, used when LimeRC power on, show Hello interface */
    LimeHAL_Hardware_Online_Info_t onlineInfo;

	/* key info, as user input from hardware */
    LimeHal_KeyInfo_t keyInfo;

	/* working info */
	LimeHal_WorkingInfo_t workingInfo;

	/* weather info */
	LimeHal_SenserInfo_t senserInfo;

}LimeHal_Info_t;

/*common API*/
LimeHal_Info_t *LimeHAL_GetInfoPin(void);

/*VS simulator API*/
void LimeHAL_SoftSimHardwareTimer_Init(void);
void LimeHAL_KeyPressed_Hook(uint8_t keyID);
void LimeHAL_SetAllKeyToReleased(void);

/*LVGL Interface API*/
void LimeHAL_AppRunInfo_runTake(bool isTake);	//0: give, 1:take
bool LimeHAL_IsAppFirmwareScheLock(void);
void LimeHAL_SyncAppRunStatus(int16_t index);

#endif // __Lime_App_Hal_H__
