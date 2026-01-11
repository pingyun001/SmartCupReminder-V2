#ifndef __LIME_COUNTFACE_H__
#define __LIME_COUNTFACE_H__

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

#include "Lime_App_Hal.h"

static void lime_countface_update_bar(uint32_t total_second, uint32_t remain_second);
static void lime_countface_update_main_time(uint32_t total_second, uint32_t remain_second);
static void lime_countface_update_charactor_img(LimeHal_WoringStatus_e status);

lv_obj_t *lime_countface_create(lv_obj_t* parent);



#endif // __LIME_COUNTFACE_H__
