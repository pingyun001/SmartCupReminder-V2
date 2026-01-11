#include "Lime_CountFace.h"
#include "Lime_App_Base.h"
#include "Lime_App_Hal.h"

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

static void scan_timer_cb(lv_timer_t * timer);

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
    lv_obj_align(count_shadow_label, LV_ALIGN_TOP_LEFT, 25 + 5, 42 + 5);
    lv_obj_set_style_text_font(count_shadow_label, &lime_font_main_time, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(count_shadow_label, lv_color_hex(0x3d3d3d), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(count_shadow_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    count_label = lv_label_create(bj_obj);
    lv_label_set_text(count_label, "12:03");
    lv_obj_align(count_label, LV_ALIGN_TOP_LEFT, 25, 42);
    lv_obj_set_style_text_font(count_label, &lime_font_main_time, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(count_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(count_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *remain_shadow_label = lv_label_create(bj_obj);
    lv_label_set_text(remain_shadow_label, "剩\n余");
    lv_obj_align(remain_shadow_label, LV_ALIGN_TOP_LEFT, 220 + 2, 61 + 2);
    lv_obj_set_style_text_font(remain_shadow_label, &lime_font_main_remain, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(remain_shadow_label, lv_color_hex(0x3d3d3d), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(remain_shadow_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *remain_label = lv_label_create(bj_obj);
    lv_label_set_text(remain_label, "剩\n余");
    lv_obj_align(remain_label, LV_ALIGN_TOP_LEFT, 220, 61);
    lv_obj_set_style_text_font(remain_label, &lime_font_main_remain, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(remain_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(remain_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    remain_slider_bj = lv_obj_create(bj_obj);
    lv_obj_set_size(remain_slider_bj, 210, 8);
    lv_obj_set_pos(remain_slider_bj, 25, 124);
    lv_obj_set_style_bg_opa(remain_slider_bj, LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(remain_slider_bj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(remain_slider_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(remain_slider_bj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(remain_slider_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(remain_slider_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(remain_slider_bj, LV_OBJ_FLAG_SCROLLABLE);

    remain_slider_minute_bj = lv_obj_create(remain_slider_bj);
    lv_obj_set_size(remain_slider_minute_bj, 170, 8);
    lv_obj_align(remain_slider_minute_bj, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_opa(remain_slider_minute_bj, LV_OPA_80, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(remain_slider_minute_bj, lv_color_hex(0xffbb79), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(remain_slider_minute_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(remain_slider_minute_bj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(remain_slider_minute_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(remain_slider_minute_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(remain_slider_minute_bj, LV_OBJ_FLAG_SCROLLABLE);

    remain_slider_second_bj = lv_obj_create(remain_slider_bj);
    lv_obj_set_size(remain_slider_second_bj, 120, 8);
    lv_obj_align(remain_slider_second_bj, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_opa(remain_slider_second_bj, LV_OPA_100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(remain_slider_second_bj, lv_color_hex(0xfc952d), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(remain_slider_second_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(remain_slider_second_bj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(remain_slider_second_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(remain_slider_second_bj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(remain_slider_second_bj, LV_OBJ_FLAG_SCROLLABLE);

    charactor_img = lv_img_create(bj_obj);
    lv_img_set_src(charactor_img, &lime_img_timeout);
    lv_obj_align(charactor_img, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    lv_timer_create(scan_timer_cb, 100, NULL);

    return bj_obj;
}

static void lime_countface_update_bar(uint32_t total_second, uint32_t remain_second)
{
    MLV_BASE_OBJ_NULL_CHECK(bj_obj);

    /* calculate time */
    float minute = remain_second / 60.0f;
    float minute_max = total_second / 60.0f;
    uint16_t second = remain_second % 60;

    /* calculate width of bar */
    uint16_t base = lv_obj_get_width(remain_slider_bj);
    uint16_t minute_width = (uint16_t)fmapWithLimit(minute, 0, minute_max, 8, base);
    uint16_t second_width = (uint16_t)fmapWithLimit(second, 0, 59, 8, base);

    /* fill full bar when not start countdown */
    second_width = (total_second == remain_second) ? base : second_width;

    /* sync UI */
    lv_obj_set_width(remain_slider_minute_bj, minute_width);
    lv_obj_set_width(remain_slider_second_bj, second_width);
}

static void lime_countface_update_main_time(uint32_t total_second, uint32_t remain_second)
{
    MLV_BASE_OBJ_NULL_CHECK(bj_obj);

    /* update main time */
    uint16_t minute = remain_second / 60;
    uint16_t second = remain_second % 60;

    /* limit minute and second to 2 digits */
    minute = minute > 99 ? 99 : minute;

    /* update UI */
    lime_base_set_label_string(count_shadow_label, "%02d:%02d", minute, second);
    lime_base_set_label_string(count_label, "%02d:%02d", minute, second);
}

static void lime_countface_update_charactor_img(LimeHal_WoringStatus_e status)
{
    MLV_BASE_OBJ_NULL_CHECK(bj_obj);

    switch(status)
    {
        case LimeHal_WoringStatus_Init:
        case LimeHal_WoringStatus_Idle:
            lv_img_set_src(charactor_img, &lime_img_idle);
            break;
        case LimeHal_WoringStatus_Countdown:
            lv_img_set_src(charactor_img, &lime_img_counting);
            break;
        case LimeHal_WoringStatus_CountFinish:
            lv_img_set_src(charactor_img, &lime_img_timeout);
            break;
        default:
            LV_LOG_ERROR("Invalid work status: %d", status);
            break;
    }
}

static void scan_timer_cb(lv_timer_t * timer)
{
    MLV_BASE_OBJ_NULL_CHECK(bj_obj);

    /* get hal info */
    const LimeHal_Info_t *info = LimeHAL_GetInfoPin();
    MLV_BASE_OBJ_NULL_CHECK(info);

    /* get countdown info */
    uint32_t total_second = info->workingInfo.totalCountSeconds;
    uint32_t remain_second = info->workingInfo.remainCountSeconds;

    /* update UI */
    lime_countface_update_bar(total_second, remain_second);
    lime_countface_update_main_time(total_second, remain_second);
    lime_countface_update_charactor_img(info->workingInfo.workingStatus);
}
