#include "Lime_App_Hal.h"
#include <stdio.h>
#include <string.h>

LimeHal_Info_t LimeHal_Info =
{


};

/*common API*/
LimeHal_Info_t *LimeHAL_GetInfoPin(void)
{
    return &LimeHal_Info;
}

int32_t absX(int32_t x)
{
    return x < 0? -x : x;
}

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float fmapWithLimit(float x, float in_min, float in_max, float out_min, float out_max)
{
    float returnVal = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

    if(out_min > out_max)
    {
        float temp = out_min;
        out_min = out_max;
        out_max = temp;
    }

    returnVal = (returnVal < out_min)? out_min : returnVal;
    returnVal = (returnVal > out_max)? out_max : returnVal;

    return returnVal;
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
    lv_timer_t * timer = lv_timer_create(timer_cb, 500, NULL);
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
