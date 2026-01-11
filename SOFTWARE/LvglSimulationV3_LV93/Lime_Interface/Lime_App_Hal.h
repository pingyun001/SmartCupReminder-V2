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

typedef struct
{
	char deletePhotoPath[128];
	bool isPhotoNeedDelete;
}LimeHal_PhotoFaceInfo_t;

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
}LimeHal_WorkingInfo_t;
typedef struct
{
	/* hardware online info, used when LimeRC power on, show Hello interface */
    LimeHAL_Hardware_Online_Info_t onlineInfo;

	/* key info, as user input from hardware */
    LimeHal_KeyInfo_t keyInfo;

	/* working info */


}LimeHal_Info_t;

extern LimeHal_Info_t LimeHal_Info;


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
