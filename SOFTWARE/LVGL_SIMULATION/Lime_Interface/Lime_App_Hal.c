#include "Lime_App_Hal.h"
#include <stdio.h>
#include <string.h>

#define DEFAULT_COUNT_SECONDS 10
#define SIMULATION_COUNT_GAP 240

LimeHal_Info_t LimeHal_Info =
{
    .workingInfo.totalCountSeconds = DEFAULT_COUNT_SECONDS,
    .workingInfo.remainCountSeconds = DEFAULT_COUNT_SECONDS,
#if USING_LIME_HARDWARE
    .senserInfo.isWifiConnected = false,
#else
    .senserInfo.isWifiConnected = true,
    .senserInfo.ipAddress = {192, 168, 127, 100},
    // .workingInfo.isFileSystemError = true,
#endif
    .senserInfo.isWeatherDataValid = false,
    .senserInfo.homeTemper = 23,

    .senserInfo.cityName = "北京",
    .senserInfo.nowTemper = 23,
    .senserInfo.nowHumi = 50,
    .senserInfo.todayWeather.day = 1,
    .senserInfo.todayWeather.month = 1,
    .senserInfo.todayWeather.weatherLogoID = 1,
    .senserInfo.todayWeather.weatherChinese = "晴",
    .senserInfo.todayWeather.temperaLow = 10,
    .senserInfo.todayWeather.temperaHigh = 20,
    .senserInfo.todayWeather.humidity = 50,
    .senserInfo.tomorrowWeather.day = 2,
    .senserInfo.tomorrowWeather.month = 1,
    .senserInfo.tomorrowWeather.weatherLogoID = 2,
    .senserInfo.tomorrowWeather.weatherChinese = "多云",
    .senserInfo.tomorrowWeather.temperaLow = 15,
    .senserInfo.tomorrowWeather.temperaHigh = 25,
    .senserInfo.tomorrowWeather.humidity = 60,
    .senserInfo.dayAfterTomorrowWeather.day = 3,
    .senserInfo.dayAfterTomorrowWeather.month = 1,
    .senserInfo.dayAfterTomorrowWeather.weatherLogoID = 3,
    .senserInfo.dayAfterTomorrowWeather.weatherChinese = "阴",
    .senserInfo.dayAfterTomorrowWeather.temperaLow = 5,
    .senserInfo.dayAfterTomorrowWeather.temperaHigh = 15,
    .senserInfo.dayAfterTomorrowWeather.humidity = 40,

    .settingInfo.volume = 2,
    .settingInfo.lumen = 2,
    .settingInfo.lightMode = 0,

    .settingInfo.countDownTimeIndex = 2,
};

/*common API*/
LimeHal_Info_t *LimeHAL_GetInfoPin(void)
{
    return &LimeHal_Info;
}

void LimeHAL_SyncKeyInfo(const LimeHal_KeyInfo_t *keyInfo)
{
    if(keyInfo == NULL)
        return;

    memcpy((uint8_t*)&LimeHal_Info.keyInfo, (uint8_t*)keyInfo, sizeof(LimeHal_KeyInfo_t));
}
void LimeHAL_SyncEspFirmwareVersion(const uint8_t *version)
{
    if(version == NULL)
        return;

    memcpy((uint8_t*)&LimeHal_Info.senserInfo.espFirmwareVersion, (uint8_t*)version, sizeof(LimeHal_Info.senserInfo.espFirmwareVersion));
}
void LimeHAL_SetWifiStatus(bool isConnected)
{
    LimeHal_Info.senserInfo.isWifiConnected = isConnected;
}
void LimeHAL_SetIpAddress(const uint8_t *ip)
{
    if(ip == NULL)
        return;

    memcpy((uint8_t*)&LimeHal_Info.senserInfo.ipAddress, (uint8_t*)ip, sizeof(LimeHal_Info.senserInfo.ipAddress));
}
void LimeHAL_SetTime(uint8_t hour, uint8_t minute)
{
    LimeHal_Info.senserInfo.hour = hour;
    LimeHal_Info.senserInfo.minute = minute;
}
void LimeHAL_SetWeatherDataValid(bool isValid)
{
    LimeHal_Info.senserInfo.isWeatherDataValid = isValid;
}
void LimeHAL_SetCityName(const char *cityName)
{
    if(cityName == NULL)
        return;

    if(strlen(cityName) >= sizeof(LimeHal_Info.senserInfo.cityName))
        return;

    memset(LimeHal_Info.senserInfo.cityName, 0, sizeof(LimeHal_Info.senserInfo.cityName));
    memcpy(LimeHal_Info.senserInfo.cityName, cityName, strlen(cityName));
}
void LimeHAL_SetNowTemper(float temper)
{
    LimeHal_Info.senserInfo.nowTemper = temper;
}
void LimeHAL_SetNowHumi(float humi)
{
    LimeHal_Info.senserInfo.nowHumi = humi;
}
void LimeHAL_SetTodayWeather(const dayWeather_t *weather)
{
    if(weather == NULL)
        return;

    LimeHAL_SetNowHumi(weather->humidity);
    memcpy((uint8_t*)&LimeHal_Info.senserInfo.todayWeather, (uint8_t*)weather, sizeof(dayWeather_t));
}
void LimeHAL_SetTomorrowWeather(const dayWeather_t *weather)
{
    if(weather == NULL)
        return;

    memcpy((uint8_t*)&LimeHal_Info.senserInfo.tomorrowWeather, (uint8_t*)weather, sizeof(dayWeather_t));
}
void LimeHAL_SetDayAfterTomorrowWeather(const dayWeather_t *weather)
{
    if(weather == NULL)
        return;

    memcpy((uint8_t*)&LimeHal_Info.senserInfo.dayAfterTomorrowWeather, (uint8_t*)weather, sizeof(dayWeather_t));
}
void LimeHAL_SetUsbLowVolErr(void)
{
	LimeHal_Info.workingInfo.isUsbLowPower = true;
}
void LimeHAL_SetFileSystemErr(void)
{
    LimeHal_Info.workingInfo.isFileSystemError = true;
}
void LimeHAL_SetUsbVoltage(float voltage)
{
	LimeHal_Info.senserInfo.usbVolt = voltage;
}
void LimeHAL_SetHomeTemerSensor(float homeTemper)
{
	LimeHal_Info.senserInfo.homeTemper = homeTemper;
}
void LimeHAL_SetFlashID(uint32_t flashID)
{
	LimeHal_Info.senserInfo.flashID = flashID;
}

uint8_t Lime_CalDayInMonth(uint32_t year, uint8_t month)
{
    const uint8_t days_per_month[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (month < 1 || month > 12)
        return 0;

    if (month == 2) {
        if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)) {
            return 29;
        }
        return 28;
    }

    return days_per_month[month - 1];
}

void LimeHAL_SetInitStep(uint8_t percent, const char* hardwareName)
{
    LimeHal_Info.onlineInfo.initPercent = percent;
    memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
    snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "%s", hardwareName);
    LimeHal_Info.onlineInfo.allHardwarePass = percent == 100 ? 1 : 0;
}

bool LimeHAL_IsAnyMenuKeyPressed(void)
{
    if(LimeHal_Info.keyInfo.sw_up % 2)
        return true;
    if(LimeHal_Info.keyInfo.sw_set % 2)
        return true;
    if(LimeHal_Info.keyInfo.sw_left % 2)
        return true;
    if(LimeHal_Info.keyInfo.sw_right % 2)
        return true;
    if(LimeHal_Info.keyInfo.sw_down % 2)
        return true;

    return false;
}

/*VS simulator API*/
static void timer_cb(lv_timer_t * timer);

void LimeHAL_KeyPressed_Hook(uint8_t keyID)
{
    switch(keyID)
    {
        case 0:
            LimeHal_Info.keyInfo.sw_up ++;
            break;
        case 1:
            LimeHal_Info.keyInfo.sw_set ++;
            break;
        case 2:
            LimeHal_Info.keyInfo.sw_left ++;
            break;
        case 3:
            LimeHal_Info.keyInfo.sw_right ++;
            break;
        case 4:
            LimeHal_Info.keyInfo.sw_down ++;
            break;
        default:
            LV_LOG_WARN("Unknown keyID:%d", keyID);
    }

    // LV_LOG_USER("sw_up:%d, sw_set:%d, sw_left:%d, sw_right:%d, sw_down:%d", LimeHal_Info.keyInfo.sw_up, LimeHal_Info.keyInfo.sw_set, LimeHal_Info.keyInfo.sw_left, LimeHal_Info.keyInfo.sw_right, LimeHal_Info.keyInfo.sw_down);
}

void LimeHAL_SetAllKeyToReleased(void)
{
    LimeHal_Info.keyInfo.sw_up = 0;
    LimeHal_Info.keyInfo.sw_set = 0;
    LimeHal_Info.keyInfo.sw_left = 0;
    LimeHal_Info.keyInfo.sw_right = 0;
    LimeHal_Info.keyInfo.sw_down = 0;
}

void LimeHAL_SettingInfo_PlayMusicByIndex(uint8_t index)
{
    LimeHal_Info.settingInfo.isNeedPlayMusic = true;
    LimeHal_Info.settingInfo.playMusicIndex = index;
}
void LimeHAL_SettingInfo_EnterUdisk(void)
{
#if !USING_LIME_HARDWARE
    LV_LOG_USER("sim enter udisk");
#endif

    LimeHal_Info.settingInfo.isNeedEnterUdisk = true;
}
void LimeHAL_SettingInfo_Restore(void)
{
#if !USING_LIME_HARDWARE
    LV_LOG_USER("sim restore system");
#endif
    LimeHal_Info.settingInfo.isNeedRestore = true;
}
bool LimeHAL_WorkingInfo_IsUsbLowPower(void)
{
    bool vaule = LimeHal_Info.workingInfo.isUsbLowPower;

    LimeHal_Info.workingInfo.isUsbLowPower = false;

    return vaule;
}
bool LimeHAL_WorkingInfo_IsFileSystemError(void)
{
    bool vaule = LimeHal_Info.workingInfo.isFileSystemError;

    LimeHal_Info.workingInfo.isFileSystemError = false;

    return vaule;
}

void LimeHAL_SoftSimHardwareTimer_Init(void)
{
    lv_timer_create(timer_cb, SIMULATION_COUNT_GAP, NULL);
}

bool LimeHAL_IsSetted_UDiskMode(void)
{
	bool return_val = LimeHal_Info.settingInfo.isNeedEnterUdisk;

	LimeHal_Info.settingInfo.isNeedEnterUdisk = false;

	return return_val;
}
bool LimeHAL_IsSetted_RestoreMode(void)
{
	bool return_val = LimeHal_Info.settingInfo.isNeedRestore;

	LimeHal_Info.settingInfo.isNeedRestore = false;

	return return_val;
}

bool LimeHAL_IsNeed_PlayMusic(void)
{
	bool return_val = LimeHal_Info.settingInfo.isNeedPlayMusic;

	LimeHal_Info.settingInfo.isNeedPlayMusic = false;

	return return_val;
}
uint8_t LimeHAL_GetPlayMusicIndex(void)
{
	return LimeHal_Info.settingInfo.playMusicIndex;
}
uint8_t LimeHAL_GetCountDownTimeIndex(void)
{
	return LimeHal_Info.settingInfo.countDownTimeIndex;
}
uint8_t LimeHAL_GetVolume(void)
{
    return LimeHal_Info.settingInfo.volume;
}
uint8_t LimeHAL_GetLumen(void)
{
    return LimeHal_Info.settingInfo.lumen;
}
uint8_t LimeHAL_GetLightMode(void)
{
    return LimeHal_Info.settingInfo.lightMode;
}
void LimeHAL_SetWorkingStatus(LimeHal_WorkingStatus_e status)
{
	LimeHal_Info.workingInfo.workingStatus = status;
}
void LimeHAL_SetRemainCountSeconds(uint16_t remainSeconds)
{
	LimeHal_Info.workingInfo.remainCountSeconds = remainSeconds;
}
void LimeHAL_SetTotalCountSeconds(uint16_t totalSeconds)
{
	LimeHal_Info.workingInfo.totalCountSeconds = totalSeconds;
}
void LimeHAL_SetSleepMode(bool isSleepMode)
{
	LimeHal_Info.workingInfo.isSleepMode = isSleepMode;
}
LimeHal_WorkingStatus_e LimeHAL_GetWorkingStatus(void)
{
	return LimeHal_Info.workingInfo.workingStatus;
}
bool LimeHAL_WorkingStatus_IsIdle(void)
{
	return (LimeHal_Info.workingInfo.workingStatus == LimeHal_WoringStatus_Idle);
}

/*typedef struct {
    uint8_t keySubBoard;
    uint8_t eeprom;
    uint8_t qflash;
    uint8_t nrfVideo;
    uint8_t nrfMessage;

}LimeHAL_Hardware_Online_Info_t;*/

static void timer_cb(lv_timer_t * timer)
{
    static uint32_t totalRunCnt = 0;

    totalRunCnt++;

    /* sim time */
    static uint8_t hour = 12, min = 15, sec = 0;
    sec ++;
    if(sec == 60)
    {
        sec = 0;
        min ++;
        if(min == 60)
        {
            min = 0;
            hour ++;
            if(hour == 24)
            {
                hour = 0;
            }
        }
    }
    LimeHal_Info.senserInfo.hour = hour;
    LimeHal_Info.senserInfo.minute = min;

    /* sim countdown time */
    static uint32_t delay_count = 0;
    if(LimeHal_Info.workingInfo.remainCountSeconds != 0)
    {
        LimeHal_Info.workingInfo.workingStatus = LimeHal_WoringStatus_Countdown;
        LimeHal_Info.workingInfo.remainCountSeconds --;
    }
    else
    {
        if(delay_count == 0)
        {
            LimeHal_Info.workingInfo.workingStatus = LimeHal_WoringStatus_CountFinish;
        }

        if(delay_count ++ > 10)
        {
            delay_count = 0;
            LimeHal_Info.workingInfo.remainCountSeconds = LimeHal_Info.workingInfo.totalCountSeconds;
        }

        if(delay_count == 5)
        {
            LimeHal_Info.workingInfo.workingStatus = LimeHal_WoringStatus_Idle;
        }
    }
    // LV_LOG_USER("delay_count:%d, remain_second:%d", delay_count, LimeHal_Info.workingInfo.remainCountSeconds);

    /*sim Hardware Init sequence*/
    if(totalRunCnt == 1)
    {
        LimeHal_Info.onlineInfo.initPercent = 20;
        memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
        snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "KeySubBoard");
    }
    if(totalRunCnt == 2)
    {
        LimeHal_Info.onlineInfo.initPercent = 30;
        memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
        snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "EEPROM");
    }
    if(totalRunCnt == 3)
    {
        LimeHal_Info.onlineInfo.initPercent = 50;
        memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
        snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "QFlash");
    }
    if(totalRunCnt == 4)
    {
        LimeHal_Info.onlineInfo.initPercent = 75;
        memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
        snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "NRF Video");
    }
    if(totalRunCnt == 5)
    {
        LimeHal_Info.onlineInfo.initPercent = 90;
        memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
        snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "NRF Message");
    }
    if(totalRunCnt == 6)
    {
        LimeHal_Info.onlineInfo.initPercent = 100;
        memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
        snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "Self-Test Success");
    }
    if(totalRunCnt == 7)
    {
        LV_LOG_USER("All Hardware Online");
        LimeHal_Info.onlineInfo.allHardwarePass = 1;
    }
}
