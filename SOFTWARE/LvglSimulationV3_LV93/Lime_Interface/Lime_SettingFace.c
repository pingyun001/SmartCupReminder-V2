#include "Lime_SettingFace.h"
#include "Lime_App_Base.h"

static lv_obj_t *bj_obj = NULL;
static lv_obj_t *title_countdown_obj = NULL;
static lv_obj_t *title_switch_obj = NULL;
static lv_obj_t *title_voice_obj = NULL;
static lv_obj_t *title_about_obj = NULL;

LV_FONT_DECLARE(lime_font_setting_title);
LV_IMG_DECLARE(lime_setting_time);
LV_IMG_DECLARE(lime_setting_screen);
LV_IMG_DECLARE(lime_setting_voice);
LV_IMG_DECLARE(lime_setting_about);

static lv_obj_t *lime_setting_create_title(lv_obj_t * parent, const char * title, const lv_img_dsc_t * img);

lv_obj_t *lime_setting_widget_create(lv_obj_t* parent)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    bj_obj = lv_obj_create(parent);
    lv_obj_set_size(bj_obj, 428, 142);
    lv_obj_set_style_bg_opa(bj_obj, LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bj_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(bj_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(bj_obj);

    title_countdown_obj = lime_setting_create_title(bj_obj, "计时", &lime_setting_time);
    lv_obj_set_pos(title_countdown_obj, 16, 25);

    title_switch_obj = lime_setting_create_title(bj_obj, "交互", &lime_setting_screen);
    lv_obj_align_to(title_switch_obj, title_countdown_obj, LV_ALIGN_OUT_BOTTOM_MID, 0, -2);

    title_voice_obj = lime_setting_create_title(bj_obj, "预览", &lime_setting_voice);
    lv_obj_align_to(title_voice_obj, title_switch_obj, LV_ALIGN_OUT_BOTTOM_MID, 0, -2);

    title_about_obj = lime_setting_create_title(bj_obj, "关于", &lime_setting_about);
    lv_obj_align_to(title_about_obj, title_voice_obj, LV_ALIGN_OUT_BOTTOM_MID, 0, -2);

    lv_obj_t *gap_line_obj = lv_obj_create(bj_obj);
    lv_obj_set_size(gap_line_obj, 2, 90);
    lv_obj_set_pos(gap_line_obj, 83, 36);
    lv_obj_set_style_bg_opa(gap_line_obj, LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(gap_line_obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(gap_line_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(gap_line_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(gap_line_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(gap_line_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(gap_line_obj, LV_OBJ_FLAG_SCROLLABLE);

    return bj_obj;
}

static lv_obj_t *lime_setting_create_title(lv_obj_t * parent, const char * title, const lv_img_dsc_t * img)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    lv_obj_t *sub_bj_obj = lv_obj_create(parent);
    lv_obj_set_size(sub_bj_obj, 65, 28);
    lv_obj_set_style_bg_opa(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sub_bj_obj, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(sub_bj_obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(sub_bj_obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *img_obj = lv_img_create(sub_bj_obj);
    lv_img_set_src(img_obj, img);
    // lv_obj_set_size(img_obj, 14, 14);
    lv_obj_align(img_obj, LV_ALIGN_LEFT_MID, 4, 0);

    lv_obj_t *title_obj = lv_label_create(sub_bj_obj);
    lv_obj_set_size(title_obj, 37, 19);
    lv_label_set_text(title_obj, title);
    lv_obj_set_style_text_font(title_obj, &lime_font_setting_title, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(title_obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(title_obj, LV_ALIGN_RIGHT_MID, -4, 0);

    return sub_bj_obj;
}
