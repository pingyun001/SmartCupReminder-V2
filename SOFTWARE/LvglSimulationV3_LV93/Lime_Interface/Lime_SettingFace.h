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

typedef enum
{
    setting_key_option_none,
    setting_key_option_first_enter,
    setting_key_option_up,
    setting_key_option_down,
    setting_key_option_left,
    setting_key_option_right,
    setting_key_option_set,
}setting_key_option_e;

setting_key_option_e lime_sbu_setting_detech_key_option(const LimeHal_KeyInfo_t *keyInfo, const LimeHal_KeyInfo_t *keyInfo_last);

static lv_obj_t *lime_sub_setting_countdown_create(lv_obj_t* parent);
static void lime_sub_setting_countdown_update(lv_obj_t* obj, uint8_t indicator_index, uint8_t checked_index);
static void lime_sub_setting_countdown_event_handler(lv_obj_t* obj, setting_key_option_e key_option, bool *is_sub_exit);

static lv_obj_t *lime_sub_setting_switch_create(lv_obj_t* parent);
static void lime_sub_setting_switch_update(lv_obj_t* obj, uint8_t indicator_index, uint8_t volume, uint8_t lumen, uint8_t light_mode);
static void lime_sub_setting_switch_event_handler(lv_obj_t* obj, setting_key_option_e key_option, bool *is_sub_exit);

static lv_obj_t *lime_sub_setting_preview_create(lv_obj_t* parent);
static void lime_sub_setting_preview_update(lv_obj_t* obj, uint8_t indicator_index);
static void lime_sub_setting_preview_event_handler(lv_obj_t* obj, setting_key_option_e key_option, bool *is_sub_exit);

static lv_obj_t *lime_sub_setting_about_create(lv_obj_t* parent);
static void lime_sub_setting_about_update(lv_obj_t* obj, uint8_t indicator_index);
static void lime_sub_setting_about_event_handler(lv_obj_t* obj, setting_key_option_e key_option, bool *is_sub_exit);

typedef enum
{
    setting_in_out_dir_none = 0,
    setting_in_out_dir_up = 1,
    setting_in_out_dir_down = 2,
}setting_in_out_dir_e;

setting_in_out_dir_e lime_setting_run_handler(const LimeHal_KeyInfo_t *keyInfo, setting_in_out_dir_e in_dir);


#endif /* __Lime_SettingFace_H__ */
