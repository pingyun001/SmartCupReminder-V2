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
static lv_anim_t *charactor_change_anim = NULL;
static bool charactor_change_anim_is_out = false;
static bool charactor_change_anim_running = false;
static lv_img_dsc_t *tar_charactor_img_dsc = NULL;

LV_FONT_DECLARE(lime_font_main_time);
LV_FONT_DECLARE(lime_font_main_remain);
LV_IMG_DECLARE(lime_img_counting);
LV_IMG_DECLARE(lime_img_idle);
LV_IMG_DECLARE(lime_img_timeout);

static void scan_timer_cb(lv_timer_t * timer);
static void anim_finish_cb(lv_anim_t * anim);
static void anim_run_xcb(void *obj, int32_t value);

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
    lv_obj_set_size(charactor_img, 160, 140);
    lv_obj_align(charactor_img, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    /* add animation */
    charactor_change_anim = lv_malloc(sizeof(lv_anim_t));
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(charactor_change_anim);
    lv_anim_init(charactor_change_anim);
    lv_anim_set_var(charactor_change_anim, charactor_img);
    lv_anim_set_values(charactor_change_anim, 0, 160);
    lv_anim_set_time(charactor_change_anim, 500);
    lv_anim_set_path_cb(charactor_change_anim, lv_anim_path_bounce);
    lv_anim_set_exec_cb(charactor_change_anim, anim_run_xcb);
    lv_anim_set_ready_cb(charactor_change_anim, anim_finish_cb);

    /* create timer to scan hal info */
    lv_timer_t* scan_timer = lv_timer_create(scan_timer_cb, 100, NULL);
    scan_timer_cb(scan_timer);

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

static void lime_countface_update_charactor_img(LimeHal_WorkingStatus_e status)
{
    MLV_BASE_OBJ_NULL_CHECK(bj_obj);

    static LimeHal_WorkingStatus_e last_status = LimeHal_WoringStatus_Idle;
    if(status == last_status)
        return;
    last_status = status;

    switch(status)
    {
        case LimeHal_WoringStatus_Init:
        case LimeHal_WoringStatus_Idle:
            tar_charactor_img_dsc = (lv_img_dsc_t *)&lime_img_idle;
            break;
        case LimeHal_WoringStatus_Countdown:
            tar_charactor_img_dsc = (lv_img_dsc_t *)&lime_img_counting;
            break;
        case LimeHal_WoringStatus_CountFinish:
            tar_charactor_img_dsc = (lv_img_dsc_t *)&lime_img_timeout;
            break;
        default:
            LV_LOG_ERROR("Invalid work status: %d", status);
            break;
    }

    /* start animation */
    lv_obj_update_layout(charactor_img);
    int32_t pic_width = lv_obj_get_width(charactor_img);
    int32_t start_x = lv_obj_get_x(charactor_img) + pic_width - 428;
    int32_t distance = 160 - start_x;
    int32_t elaps_time = fmapWithLimit(distance, 0, 160, 1, 500);
    // LV_LOG_USER("x:%d, start_x:%d, elaps_time:%d", lv_obj_get_x(charactor_img), start_x, elaps_time);
    lv_anim_set_values(charactor_change_anim, start_x, pic_width);
    lv_anim_set_time(charactor_change_anim, elaps_time);
    charactor_change_anim_is_out = true;
    charactor_change_anim_running = true;
    lv_anim_start(charactor_change_anim);
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

static void anim_finish_cb(lv_anim_t * anim)
{
    // LV_LOG_USER("anim_finish_cb");

    /* half of animation is done, change charactor image */
    if(charactor_change_anim_is_out)
    {
        charactor_change_anim_is_out = false;

        /* change img */
        MLV_BASE_OBJ_NULL_CHECK(tar_charactor_img_dsc);
        lv_img_set_src(charactor_img, tar_charactor_img_dsc);

        /* start reverse animation */
        lv_anim_set_values(charactor_change_anim, 160, 0);
        lv_anim_set_time(charactor_change_anim, 500);
        lv_anim_start(charactor_change_anim);
    }

    /* animation is done */
    else
    {
        charactor_change_anim_running = false;
    }
}
static void anim_run_xcb(void *obj, int32_t value)
{
    // LV_LOG_USER("anim_run_xcb: %d", value);

    lv_obj_align(charactor_img, LV_ALIGN_BOTTOM_RIGHT, value, 0);
}
