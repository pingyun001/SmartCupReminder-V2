#include "Lime_HeadBar.h"
#include "Lime_App_Base.h"
#include "Lime_App_Hal.h"

static lv_obj_t *bj_obj = NULL;
static lv_obj_t *header_shadow_obj = NULL;
static lv_obj_t *tim_label = NULL;
static lv_obj_t *moon_img = NULL;
static lv_obj_t *weatherlogo_img = NULL;
static lv_obj_t *temper_label = NULL;
static lv_obj_t *wifistatus_img = NULL;
static bool is_width_expanded = false;

LV_FONT_DECLARE(lime_font_headbar);
LV_IMG_DECLARE(lime_img_moon);
LV_IMG_DECLARE(lime_img_weatherlogo);
LV_IMG_DECLARE(lime_img_wificonn);
LV_IMG_DECLARE(lime_img_wifidisconn);

static void scan_timer_cb(lv_timer_t * timer);

lv_obj_t * lime_headbar_create(lv_obj_t *parent)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    /* bg */
    bj_obj = lv_obj_create(parent);
    lv_obj_set_size(bj_obj, 428, 23);
    lv_obj_align(bj_obj, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_opa(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(bj_obj, LV_OBJ_FLAG_SCROLLABLE);

    header_shadow_obj = lv_obj_create(bj_obj);
    lv_obj_set_size(header_shadow_obj, 460, 142);
    lv_obj_align(header_shadow_obj, LV_ALIGN_BOTTOM_RIGHT, -192, 0);
    lv_obj_set_style_bg_opa(header_shadow_obj, LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(header_shadow_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(header_shadow_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(header_shadow_obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(header_shadow_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(header_shadow_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(header_shadow_obj, LV_OBJ_FLAG_SCROLLABLE);

    /* left side */
    tim_label = lv_label_create(bj_obj);
    lv_label_set_text(tim_label, "12:30");
    lv_obj_set_size(tim_label, 38, 12);
    lv_obj_align(tim_label, LV_ALIGN_LEFT_MID, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(tim_label, &lime_font_headbar, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(tim_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(tim_label, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    moon_img = lv_img_create(bj_obj);
    lv_img_set_src(moon_img, &lime_img_moon);
    lv_obj_align(moon_img, LV_ALIGN_LEFT_MID, 46, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* right side */
    weatherlogo_img = lv_img_create(bj_obj);
    lv_img_set_src(weatherlogo_img, &lime_img_weatherlogo);
    lv_obj_align(weatherlogo_img, LV_ALIGN_LEFT_MID, 145, LV_PART_MAIN | LV_STATE_DEFAULT);

    temper_label = lv_label_create(bj_obj);
    lv_label_set_text(temper_label, "25.1℃");
    lv_obj_set_size(temper_label, 41, 12);
    lv_obj_align_to(temper_label, weatherlogo_img, LV_ALIGN_OUT_RIGHT_MID, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(temper_label, &lime_font_headbar, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(temper_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(temper_label, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    wifistatus_img = lv_img_create(bj_obj);
    lv_img_set_src(wifistatus_img, &lime_img_wificonn);
    lv_obj_align_to(wifistatus_img, temper_label, LV_ALIGN_OUT_RIGHT_MID, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_timer_create(scan_timer_cb, 10, NULL);

    return bj_obj;
}

static void scan_timer_cb(lv_timer_t * timer)
{
    /* animation to change width */
    static bool is_width_expanded_last = false;
    static int now_percent = 0;
    static bool anim_run = false;
    if(is_width_expanded != is_width_expanded_last)
    {
        anim_run = true;
    }
    if(anim_run)
    {
        if(is_width_expanded)
        {
            now_percent = now_percent > 100? 100 : now_percent + 5;
        }
        else
        {
            now_percent = now_percent < 0 ? 0 : now_percent - 5;
        }

        if(now_percent == 100 || now_percent == 0)
        {
            anim_run = false;
        }

        int32_t aligned_x1 = fmapWithLimit(now_percent, 0, 100, -192, 19);
        // lv_obj_align(header_shadow_obj, LV_ALIGN_BOTTOM_RIGHT, aligned_x1, 0);
        uint8_t bg_opa = fmapWithLimit(now_percent, 0, 60, LV_OPA_50, LV_OPA_0);
        lv_obj_set_style_bg_opa(header_shadow_obj, bg_opa, LV_PART_MAIN | LV_STATE_DEFAULT);

        int32_t aligned_x2 = fmapWithLimit(now_percent, 0, 100, 145, 331);
        lv_obj_align(weatherlogo_img, LV_ALIGN_LEFT_MID, aligned_x2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_align_to(temper_label, weatherlogo_img, LV_ALIGN_OUT_RIGHT_MID, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_align_to(wifistatus_img, temper_label, LV_ALIGN_OUT_RIGHT_MID, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    /* get sensor data */
    const LimeHal_Info_t *info = LimeHAL_GetInfoPin();
    MLV_BASE_OBJ_NULL_CHECK(info);
    const LimeHal_SenserInfo_t *senser = &info->senserInfo;

    /* update wifi ui */
    const lv_image_dsc_t *wifi_img_dsc = senser->isWifiConnected ? &lime_img_wificonn : &lime_img_wifidisconn;
    lv_img_set_src(wifistatus_img, wifi_img_dsc);

    /* update wifi-weather ui */
    if(senser->isWeatherDataValid)
    {
        lv_obj_remove_flag(weatherlogo_img, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(weatherlogo_img, LV_OBJ_FLAG_HIDDEN);
    }

    /* update time ui */
    lime_base_set_label_string(tim_label, "%02d:%02d", senser->hour, senser->minute);

	/* uypdate home temper ui */
	lime_base_set_label_string(temper_label, "%.1f℃", senser->homeTemper);

    is_width_expanded_last = is_width_expanded;
}

void lime_headbar_change_width(bool is_expanded)
{
    is_width_expanded = is_expanded;
}
