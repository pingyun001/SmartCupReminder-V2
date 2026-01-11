#include "Lime_CountFace.h"
#include "Lime_App_Base.h"

static lv_obj_t *bj_obj = NULL;
static lv_obj_t *count_label = NULL;
static lv_obj_t *count_shadow_label = NULL;
static lv_obj_t *remain_slider_bj = NULL;
static lv_obj_t *remain_slider_minute_bj = NULL;
static lv_obj_t *remain_slider_second_bj = NULL;
static lv_obj_t *charactor_img = NULL;

LV_FONT_DECLARE(lime_font_main_time);
LV_FONT_DECLARE(lime_font_main_remain);
LV_IMG_DECLARE(lime_img_counting);
LV_IMG_DECLARE(lime_img_idle);
LV_IMG_DECLARE(lime_img_timeout);

lv_obj_t *lime_countface_create(lv_obj_t* parent)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    bj_obj = lv_obj_create(parent);
    lv_obj_set_size(bj_obj, 428, 142);
    lv_obj_set_style_bg_opa(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(bj_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(bj_obj);

    count_shadow_label = lv_label_create(bj_obj);
    lv_label_set_text(count_shadow_label, "12:03");
    lv_obj_align(count_shadow_label, LV_ALIGN_TOP_LEFT, 25 + 5, 38 + 5);
    lv_obj_set_style_text_font(count_shadow_label, &lime_font_main_time, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(count_shadow_label, lv_color_hex(0x3d3d3d), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(count_shadow_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    count_label = lv_label_create(bj_obj);
    lv_label_set_text(count_label, "12:03");
    lv_obj_align(count_label, LV_ALIGN_TOP_LEFT, 25, 38);
    lv_obj_set_style_text_font(count_label, &lime_font_main_time, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(count_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(count_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *remain_shadow_label = lv_label_create(bj_obj);
    lv_label_set_text(remain_shadow_label, "剩\n余");
    lv_obj_align(remain_shadow_label, LV_ALIGN_TOP_LEFT, 220 + 2, 57 + 2);
    lv_obj_set_style_text_font(remain_shadow_label, &lime_font_main_remain, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(remain_shadow_label, lv_color_hex(0x3d3d3d), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(remain_shadow_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *remain_label = lv_label_create(bj_obj);
    lv_label_set_text(remain_label, "剩\n余");
    lv_obj_align(remain_label, LV_ALIGN_TOP_LEFT, 220, 57);
    lv_obj_set_style_text_font(remain_label, &lime_font_main_remain, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(remain_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(remain_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    remain_slider_bj = lv_obj_create(bj_obj);
    lv_obj_set_size(remain_slider_bj, 210, 8);
    lv_obj_set_pos(remain_slider_bj, 25, 124);
    lv_obj_set_style_bg_opa(remain_slider_bj, LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(remain_slider_bj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(remain_slider_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(remain_slider_bj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(remain_slider_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(remain_slider_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(remain_slider_bj, LV_OBJ_FLAG_SCROLLABLE);

    remain_slider_minute_bj = lv_obj_create(remain_slider_bj);
    lv_obj_set_size(remain_slider_minute_bj, 170, 8);
    lv_obj_align(remain_slider_minute_bj, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_opa(remain_slider_minute_bj, LV_OPA_80, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(remain_slider_minute_bj, lv_color_hex(0x639aed), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(remain_slider_minute_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(remain_slider_minute_bj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(remain_slider_minute_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(remain_slider_minute_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(remain_slider_minute_bj, LV_OBJ_FLAG_SCROLLABLE);

    remain_slider_second_bj = lv_obj_create(remain_slider_bj);
    lv_obj_set_size(remain_slider_second_bj, 120, 8);
    lv_obj_align(remain_slider_second_bj, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_opa(remain_slider_second_bj, LV_OPA_100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(remain_slider_second_bj, lv_color_hex(0x0367fd), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(remain_slider_second_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(remain_slider_second_bj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(remain_slider_second_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(remain_slider_second_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(remain_slider_second_bj, LV_OBJ_FLAG_SCROLLABLE);

    charactor_img = lv_img_create(bj_obj);
    lv_img_set_src(charactor_img, &lime_img_timeout);
    lv_obj_align(charactor_img, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
}
