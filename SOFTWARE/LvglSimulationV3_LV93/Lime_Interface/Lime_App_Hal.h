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
	LimeHal_WoringStatus_Idle = 0,
	LimeHal_WoringStatus_Countdown,
	LimeHal_WoringStatus_CountFinish,
}LimeHal_WorkingStatus_e;
typedef struct
{
	LimeHal_WorkingStatus_e workingStatus;
	uint16_t remainCountSeconds;
	uint16_t totalCountSeconds;
	bool isSleepMode;

	/* error check */
	bool isUsbLowPower;
	bool isFileSystemError;

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

	/* from flash */
	uint32_t flashID;

	/* from ds18b20 */
	float homeTemper;

	/* from esp time api */
	uint8_t hour, minute;

	/* from esp weather api */
	uint8_t espFirmwareVersion[3];
	bool isWifiConnected;
	uint8_t ipAddress[4];
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
	/* range: 1 ~ 6 */
	uint8_t countDownTimeIndex;

	/* range 0 ~ 3 */
	uint8_t volume;		//0:off, 1:low, 2:mid, 3:high
	uint8_t lumen;		//0:off, 1:low, 2:mid, 3:high
	uint8_t lightMode;	//0:虹,  1:烛,  2:星,  3:白

	/* need play music */
	bool isNeedPlayMusic;
	uint8_t playMusicIndex;

	/* need enter u-disk */
	bool isNeedEnterUdisk;

	/* need restore */
	bool isNeedRestore;

}LimeHal_SettingInfo_t;
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

	/* setting info */
	LimeHal_SettingInfo_t settingInfo;

}LimeHal_Info_t;

/*common API*/
LimeHal_Info_t *LimeHAL_GetInfoPin(void);
void LimeHAL_SetInitStep(uint8_t percent, const char* hardwareName);
void LimeHAL_SyncKeyInfo(const LimeHal_KeyInfo_t *keyInfo);
void LimeHAL_SyncEspFirmwareVersion(const uint8_t *version);
void LimeHAL_SetWifiStatus(bool isConnected);
void LimeHAL_SetIpAddress(const uint8_t *ip);
void LimeHAL_SetTime(uint8_t hour, uint8_t minute);
void LimeHAL_SetWeatherDataValid(bool isValid);
void LimeHAL_SetCityName(const char *cityName);
void LimeHAL_SetNowTemper(float temper);
void LimeHAL_SetNowHumi(float humi);
void LimeHAL_SetTodayWeather(const dayWeather_t *weather);
void LimeHAL_SetTomorrowWeather(const dayWeather_t *weather);
void LimeHAL_SetDayAfterTomorrowWeather(const dayWeather_t *weather);
void LimeHAL_SetUsbLowVolErr(void);
void LimeHAL_SetFileSystemErr(void);
void LimeHAL_SetUsbVoltage(float voltage);
void LimeHAL_SetHomeTemerSensor(float homeTemper);
void LimeHAL_SetFlashID(uint32_t flashID);
bool LimeHAL_IsSetted_UDiskMode(void);
bool LimeHAL_IsSetted_RestoreMode(void);
bool LimeHAL_IsNeed_PlayMusic(void);
uint8_t LimeHAL_GetPlayMusicIndex(void);
uint8_t LimeHAL_GetCountDownTimeIndex(void);
uint8_t LimeHAL_GetVolume(void);
uint8_t LimeHAL_GetLumen(void);
uint8_t LimeHAL_GetLightMode(void);
void LimeHAL_SetWorkingStatus(LimeHal_WorkingStatus_e status);
void LimeHAL_SetRemainCountSeconds(uint16_t remainSeconds);
void LimeHAL_SetTotalCountSeconds(uint16_t totalSeconds);
void LimeHAL_SetSleepMode(bool isSleepMode);
LimeHal_WorkingStatus_e LimeHAL_GetWorkingStatus(void);
bool LimeHAL_WorkingStatus_IsIdle(void);

/*VS simulator API*/
void LimeHAL_SoftSimHardwareTimer_Init(void);
void LimeHAL_KeyPressed_Hook(uint8_t keyID);
void LimeHAL_SetAllKeyToReleased(void);

/*LVGL Interface API*/
void LimeHAL_SettingInfo_PlayMusicByIndex(uint8_t index);
void LimeHAL_SettingInfo_EnterUdisk(void);
void LimeHAL_SettingInfo_Restore(void);
bool LimeHAL_WorkingInfo_IsUsbLowPower(void);
bool LimeHAL_WorkingInfo_IsFileSystemError(void);

#endif // __Lime_App_Hal_H__
