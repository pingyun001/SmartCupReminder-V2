#include "Lime_HeadBar.h"
#include "Lime_App_Base.h"


static lv_obj_t *bj_obj = NULL;
static lv_obj_t *tim_label = NULL;
static lv_obj_t *moon_img = NULL;
static lv_obj_t *weatherlogo_img = NULL;
static lv_obj_t *temper_label = NULL;
static lv_obj_t *wifistatus_img = NULL;

LV_FONT_DECLARE(lime_font_headbar);
LV_IMG_DECLARE(lime_img_moon);
LV_IMG_DECLARE(lime_img_weatherlogo);
LV_IMG_DECLARE(lime_img_wificonn);
LV_IMG_DECLARE(lime_img_wifidisconn);

lv_obj_t * lime_headbar_create(lv_obj_t *parent)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    /* left side */
    bj_obj = lv_obj_create(parent);
    lv_obj_set_size(bj_obj, 428, 23);
    lv_obj_align(bj_obj, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_opa(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(bj_obj, LV_OBJ_FLAG_SCROLLABLE);

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
    lv_obj_align(weatherlogo_img, LV_ALIGN_LEFT_MID, 159, LV_PART_MAIN | LV_STATE_DEFAULT);

    temper_label = lv_label_create(bj_obj);
    lv_label_set_text(temper_label, "25℃");
    lv_obj_set_size(temper_label, 30, 12);
    lv_obj_align_to(temper_label, weatherlogo_img, LV_ALIGN_OUT_RIGHT_MID, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(temper_label, &lime_font_headbar, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(temper_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(temper_label, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    wifistatus_img = lv_img_create(bj_obj);
    lv_img_set_src(wifistatus_img, &lime_img_wificonn);
    lv_obj_align_to(wifistatus_img, temper_label, LV_ALIGN_OUT_RIGHT_MID, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    return bj_obj;
}
