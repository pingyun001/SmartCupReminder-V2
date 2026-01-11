#include "Lime_App_Hal.h"
#include <stdio.h>
#include <string.h>

#define DEFAULT_COUNT_SECONDS 10
#define SIMULATION_COUNT_GAP 240

LimeHal_Info_t LimeHal_Info =
{
    .workingInfo.totalCountSeconds = DEFAULT_COUNT_SECONDS,
    .workingInfo.remainCountSeconds = DEFAULT_COUNT_SECONDS,

    .senserInfo.isWifiConnected = true,
    .senserInfo.isWeatherDataValid = true,
    .senserInfo.homeTemper = 23,

};

/*common API*/
LimeHal_Info_t *LimeHAL_GetInfoPin(void)
{
    return &LimeHal_Info;
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

void LimeHAL_SoftSimHardwareTimer_Init(void)
{
    lv_timer_create(timer_cb, SIMULATION_COUNT_GAP, NULL);
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
    static uint8_t hour = 12, min = 10, sec = 0;
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
    LV_LOG_USER("delay_count:%d, remain_second:%d", delay_count, LimeHal_Info.workingInfo.remainCountSeconds);

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
