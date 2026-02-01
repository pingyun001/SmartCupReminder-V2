#ifndef __Lime_SettingFace_H__
#define __Lime_SettingFace_H__

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

#include "Lime_App_Hal.h"

lv_obj_t *lime_setting_widget_create(lv_obj_t* parent);

static lv_obj_t *lime_sub_setting_countdown_create(lv_obj_t* parent);
static lv_obj_t *lime_sub_setting_switch_create(lv_obj_t* parent);
static lv_obj_t *lime_sub_setting_preview_create(lv_obj_t* parent);
static lv_obj_t *lime_sub_setting_about_create(lv_obj_t* parent);

typedef enum
{
    setting_in_out_dir_none = 0,
    setting_in_out_dir_up = 1,
    setting_in_out_dir_down = 2,
}setting_in_out_dir_e;

setting_in_out_dir_e lime_setting_run_handler(const LimeHal_KeyInfo_t *keyInfo, setting_in_out_dir_e in_dir);


#endif /* __Lime_SettingFace_H__ */
