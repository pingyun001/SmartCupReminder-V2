#include "rtc_time.h"
#include "rtc.h"
#include "stdio.h"
#include "string.h"

RTC_TimeTypeDef softAlarmTime = {0};
bool is_softAlarmTime_jumpDay = false;

void LimeRtc_PrintNowTime(void)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	
	DEBUG_LOG("%s():%d-%d-%d  %d-%d-%d\n", __FUNCTION__, (sDate.Year + 2000), sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);
}


void LimeRtc_SetNowTime(uint8_t hour, uint8_t minute, uint8_t second)
{
	/* get now time */
	uint8_t now_hour = 0, now_minutes = 0, now_seconds = 0;
	LimeRtc_GetNowTime(&now_hour, &now_minutes, &now_seconds);
	uint32_t now_abs_seconds = now_hour * 3600 + now_minutes * 60 + now_seconds;
	
	/* get setted time */
	uint32_t setted_abs_seconds = hour * 3600 + minute * 60 + second;
	
	/* get gap */
	int32_t gap_abs_seconds = setted_abs_seconds - now_abs_seconds;
	
	/* refresh alarm time */
	uint32_t alarm_abs_seconds = softAlarmTime.Hours * 3600 + softAlarmTime.Minutes * 60 + softAlarmTime.Seconds;
	alarm_abs_seconds += gap_abs_seconds;
	if(alarm_abs_seconds >= 24 * 60 * 60)
	{
		alarm_abs_seconds -= 24 * 60 * 60;
		is_softAlarmTime_jumpDay = true;
	}
	softAlarmTime.Hours = alarm_abs_seconds / 3600;
	softAlarmTime.Minutes = (alarm_abs_seconds / 60) % 60;
	softAlarmTime.Seconds = alarm_abs_seconds % 60;
	
	/* set time to RTC hardware */
	RTC_TimeTypeDef sTime = {0};
	sTime.Hours = hour;
	sTime.Minutes = minute;
	sTime.Seconds = second;
	
	if(HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
		DEBUG_LOG("HAL_RTC_SetTime Err!");
	
	RTC_DateTypeDef sDate = {0};
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	UNUSED(sDate);
}

void LimeRtc_SetNowDate(uint16_t year, uint8_t month, uint8_t day)
{
	RTC_DateTypeDef sDate = {0};
	
	if(year > 2000)
		year -= 2000;
	else 
		return ;
	
	sDate.Year = year;
	sDate.Month = month;
	sDate.Date = day;
	
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

uint8_t LimeRtc_CheckAndInitDefaultTime(void)
{
	char compileDate[20] = __DATE__;
	char compileTime[20] = __TIME__;
	
	char compileYearStr[5] = {0};
	
	memcpy(compileYearStr, compileDate + strlen(compileDate) - 4, 4);
	
	uint32_t compileYear = 0;
	sscanf(compileYearStr, "%d", &compileYear);
	
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	
	uint32_t rtcYear = sDate.Year + 2000;
	
	if(rtcYear != compileYear)
	{
		DEBUG_LOG("SystemWasLosted RTC Power!!!\n");
		
		LimeRtc_SetNowTime(18,30,00);
		LimeRtc_SetNowDate(2025, 8, 28);
		
		return 1;
	}
	
	return 0;
}

void LimeRtc_GetNowDate(uint16_t *year, uint8_t *month, uint8_t *day)
{
	RTC_DateTypeDef sDate = {0};
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	
	*year = sDate.Year + 2000;
	*month = sDate.Month;
	*day = sDate.Date;
}

void LimeRtc_GetNowTime(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	UNUSED(sDate);
	
	*hour = sTime.Hours;
	*minute = sTime.Minutes;
	*second = sTime.Seconds;
}

void LimeRtc_SetSoftAlarm(uint32_t gap_seconds)
{
	uint8_t nowHour, nowMinute, nowSecond;
	LimeRtc_GetNowTime(&nowHour, &nowMinute, &nowSecond);
	
	uint32_t now_abs_second = nowHour * 3600 + nowMinute * 60 + nowSecond;
	uint32_t target_second = now_abs_second + gap_seconds;
	
	if(target_second > 24 * 60 * 60)
	{
		is_softAlarmTime_jumpDay = true;
		target_second -= 24 * 60 * 60;
	}
	softAlarmTime.Hours = target_second / 3600;
	softAlarmTime.Minutes = (target_second / 60) % 60;
	softAlarmTime.Seconds = target_second % 60;
}
int32_t LimeRtc_GetSoftAlarm_LastTime(void)
{
	static uint8_t last_hour = 0, last_minutes = 0, last_seconds = 0;
	uint8_t now_hour = 0, now_minutes = 0, now_seconds = 0;
	
	/* get now time */
	LimeRtc_GetNowTime(&now_hour, &now_minutes, &now_seconds);
	uint32_t now_abs_second = now_hour * 3600 + now_minutes * 60 + now_seconds;
	
	/* now is new day */
	if((now_hour == 0) && (last_hour == 23))
	{
		if(is_softAlarmTime_jumpDay)
			is_softAlarmTime_jumpDay = false;
	}
	
	/* calculate target abs seconds */
	uint32_t target_abs_second = softAlarmTime.Hours * 3600 + softAlarmTime.Minutes * 60 + softAlarmTime.Seconds;
	if(is_softAlarmTime_jumpDay)
		target_abs_second += 24 * 60 * 60;
	
	/* sync last time */
	last_hour = now_hour;
	last_minutes = now_minutes;
	last_seconds = now_seconds;
	
	int32_t gap_seconds = target_abs_second - now_abs_second;
	
	return gap_seconds;
}
