#include "Lime_SettingFace.h"
#include "Lime_App_Base.h"

static lv_obj_t *bj_obj = NULL;
static lv_obj_t *title_countdown_obj = NULL;
static lv_obj_t *title_switch_obj = NULL;
static lv_obj_t *title_voice_obj = NULL;
static lv_obj_t *title_about_obj = NULL;
static lv_obj_t *indicator_obj = NULL;
static lv_obj_t *mainTabview = NULL;
static lv_obj_t *sub_tab1;
static lv_obj_t *sub_tab2;
static lv_obj_t *sub_tab3;
static lv_obj_t *sub_tab4;

LV_FONT_DECLARE(lime_font_setting_title);
LV_FONT_DECLARE(lime_font_setting_sub_text);
LV_IMG_DECLARE(lime_setting_time);
LV_IMG_DECLARE(lime_setting_screen);
LV_IMG_DECLARE(lime_setting_voice);
LV_IMG_DECLARE(lime_setting_about);
LV_IMG_DECLARE(lime_setting_checked);

static lv_obj_t *lime_setting_create_title(lv_obj_t * parent, const char * title, const lv_img_dsc_t * img);
static void main_indicator_change_pos(uint8_t index, bool is_sub_menu_loop);

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

    indicator_obj = lv_obj_create(bj_obj);
    lv_obj_set_size(indicator_obj, 65, 28);
    lv_obj_set_style_bg_opa(indicator_obj, LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(indicator_obj, lv_color_hex(0x0367fd), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(indicator_obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(indicator_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(indicator_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(indicator_obj, LV_OBJ_FLAG_SCROLLABLE);

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

    lv_obj_t *main_descript_bg_obj = lv_obj_create(bj_obj);
    lv_obj_set_size(main_descript_bg_obj, 158, 114);
    lv_obj_set_pos(main_descript_bg_obj, 297, 22);
    lv_obj_set_style_bg_opa(main_descript_bg_obj, LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(main_descript_bg_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(main_descript_bg_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(main_descript_bg_obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(main_descript_bg_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(main_descript_bg_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(main_descript_bg_obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *main_descript_gap_line_obj = lv_obj_create(main_descript_bg_obj);
    lv_obj_set_size(main_descript_gap_line_obj, 74, 2);
    lv_obj_align(main_descript_gap_line_obj, LV_ALIGN_TOP_MID, -10, 34);
    lv_obj_set_style_bg_opa(main_descript_gap_line_obj, LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(main_descript_gap_line_obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(main_descript_gap_line_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(main_descript_gap_line_obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(main_descript_gap_line_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(main_descript_gap_line_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(main_descript_gap_line_obj, LV_OBJ_FLAG_SCROLLABLE);

    mainTabview = lv_tabview_create(bj_obj);
    lv_obj_set_size(mainTabview, 343, 142);
    lv_tabview_set_tab_bar_position(mainTabview, LV_DIR_RIGHT);
    lv_tabview_set_tab_bar_size(mainTabview, 0);
    lv_obj_set_style_bg_opa(mainTabview, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(mainTabview, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_t *tab1 = lv_tabview_add_tab(mainTabview, "Tab 1");
    lv_obj_set_style_bg_opa(tab1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tab1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t *tab2 = lv_tabview_add_tab(mainTabview, "Tab 2");
    lv_obj_set_style_bg_opa(tab2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tab2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t *tab3 = lv_tabview_add_tab(mainTabview, "Tab 3");
    lv_obj_set_style_bg_opa(tab3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tab3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t *tab4 = lv_tabview_add_tab(mainTabview, "Tab 4");
    lv_obj_set_style_bg_opa(tab4, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tab4, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_tabview_set_active(mainTabview, 0, LV_ANIM_OFF);

    sub_tab1 = lime_sub_setting_countdown_create(tab1);
    sub_tab2 = lime_sub_setting_switch_create(tab2);
    sub_tab3 = lime_sub_setting_preview_create(tab3);
    sub_tab4 = lime_sub_setting_about_create(tab4);


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
    lv_obj_align(img_obj, LV_ALIGN_LEFT_MID, 4, 0);

    lv_obj_t *title_obj = lv_label_create(sub_bj_obj);
    lv_obj_set_size(title_obj, 37, 19);
    lv_label_set_text(title_obj, title);
    lv_obj_set_style_text_font(title_obj, &lime_font_setting_title, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(title_obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(title_obj, LV_ALIGN_RIGHT_MID, -4, 0);

    return sub_bj_obj;
}

static void main_indicator_change_pos(uint8_t index, bool is_sub_menu_loop)
{
    switch(index)
    {
        case 1:
            lv_obj_set_pos(indicator_obj, 16, 25);
            break;
        case 2:
            lv_obj_set_pos(indicator_obj, 16, 51);
            break;
        case 3:
            lv_obj_set_pos(indicator_obj, 16, 77);
            break;
        case 4:
            lv_obj_set_pos(indicator_obj, 16, 103);
            break;
        default:
            break;
    }

    if( !is_sub_menu_loop)
        lv_obj_set_style_bg_color(indicator_obj, lv_color_hex(0x0367fd), LV_PART_MAIN | LV_STATE_DEFAULT);
    else
        lv_obj_set_style_bg_color(indicator_obj, lv_color_hex(0x3f3f3f), LV_PART_MAIN | LV_STATE_DEFAULT);
}

setting_key_option_e lime_sbu_setting_detech_key_option(const LimeHal_KeyInfo_t *keyInfo, const LimeHal_KeyInfo_t *keyInfo_last)
{
    if((keyInfo->sw_up != keyInfo_last->sw_up) && (keyInfo->sw_up % 2))
        return setting_key_option_up;

    if((keyInfo->sw_down != keyInfo_last->sw_down) && (keyInfo->sw_down % 2))
        return setting_key_option_down;

    if((keyInfo->sw_left != keyInfo_last->sw_left) && (keyInfo->sw_left % 2))
        return setting_key_option_left;

    if((keyInfo->sw_right != keyInfo_last->sw_right) && (keyInfo->sw_right % 2))
        return setting_key_option_right;

    if((keyInfo->sw_set != keyInfo_last->sw_set) && (keyInfo->sw_set % 2))
        return setting_key_option_set;

    return setting_key_option_none;
}


#define LIME_LV_CREATE_DEFAULT_MENU_LABEL(me, parent, text, x, y, w)     \
    lv_obj_t *me = lv_label_create(parent);                              \
    lv_obj_set_pos(me, x, y);                                            \
    lv_obj_set_size(me, w, 16);                                          \
    lv_label_set_text(me, text);                                         \
    lv_obj_set_style_text_color(me, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT); \
    lv_obj_set_style_text_font(me, &lime_font_setting_title, LV_PART_MAIN | LV_STATE_DEFAULT);

static lv_obj_t *lime_sub_setting_countdown_create(lv_obj_t* parent)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    lv_obj_t *sub_bj_obj = lv_obj_create(parent);
    lv_obj_set_size(sub_bj_obj, 343, 142);
    lv_obj_set_style_bg_opa(sub_bj_obj, LV_OPA_0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sub_bj_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(sub_bj_obj, LV_OBJ_FLAG_SCROLLABLE);

    /* create indicator */
    lv_obj_t *sub_indicator_obj = lv_obj_create(sub_bj_obj);
    lv_obj_set_size(sub_indicator_obj, 80, 26);
    lv_obj_set_style_bg_opa(sub_indicator_obj, LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sub_indicator_obj, lv_color_hex(0x0367fd), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(sub_indicator_obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(sub_indicator_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(sub_indicator_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(sub_indicator_obj, LV_OBJ_FLAG_SCROLLABLE);

    /* create checked pic */
    lv_obj_t *sub_checked_pic_obj = lv_img_create(sub_bj_obj);
    lv_obj_set_size(sub_checked_pic_obj, 12, 12);
    lv_img_set_src(sub_checked_pic_obj, &lime_setting_checked);


    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_1_label, sub_bj_obj, "1 分钟", 36, 37, 60);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_2_label, sub_bj_obj, "5 分钟", 36, 65, 60);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_3_label, sub_bj_obj, "10分钟", 36, 92, 60);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_4_label, sub_bj_obj, "25分钟", 120, 37, 60);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_5_label, sub_bj_obj, "45分钟", 120, 65, 60);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_6_label, sub_bj_obj, "60分钟", 120, 92, 60);

    /* test */
    lime_sub_setting_countdown_update(sub_bj_obj, 0, 1);

    /* create description title */
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(description_label, sub_bj_obj, "倒计时时间设置", 223, 32, 120);

    lv_obj_t *description_sub_label = lv_label_create(parent);
    lv_obj_set_size(description_sub_label, 110, 72);
    lv_obj_set_pos(description_sub_label, 221, 67);
    lv_label_set_text(description_sub_label, "放下水杯后的倒计时时间");
    lv_obj_set_style_text_color(description_sub_label, lv_color_hex(0x7F7F7F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(description_sub_label, &lime_font_setting_sub_text, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(description_sub_label, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_align(description_sub_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    return sub_bj_obj;
}
static void lime_sub_setting_countdown_update(lv_obj_t* obj, uint8_t indicator_index, uint8_t checked_index)
{
    /* check para */
    MLV_BASE_OBJ_NULL_CHECK(obj);

    /* get child obj */
    lv_obj_t *sub_indicator_obj = lv_obj_get_child(obj, 0);
    lv_obj_t *sub_checked_pic_obj = lv_obj_get_child(obj, 1);
    lv_obj_t *case_1_label = lv_obj_get_child(obj, 2);
    lv_obj_t *case_2_label = lv_obj_get_child(obj, 3);
    lv_obj_t *case_3_label = lv_obj_get_child(obj, 4);
    lv_obj_t *case_4_label = lv_obj_get_child(obj, 5);
    lv_obj_t *case_5_label = lv_obj_get_child(obj, 6);
    lv_obj_t *case_6_label = lv_obj_get_child(obj, 7);

    /* update indicator */
    if(indicator_index == 0)
    {
        lv_obj_add_flag(sub_indicator_obj, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_remove_flag(sub_indicator_obj, LV_OBJ_FLAG_HIDDEN);
    }
    switch(indicator_index)
    {
        case 1:
            lv_obj_align_to(sub_indicator_obj, case_1_label, LV_ALIGN_RIGHT_MID, -5, 2);
            break;
        case 2:
            lv_obj_align_to(sub_indicator_obj, case_2_label, LV_ALIGN_RIGHT_MID, -5, 2);
            break;
        case 3:
            lv_obj_align_to(sub_indicator_obj, case_3_label, LV_ALIGN_RIGHT_MID, -5, 2);
            break;
        case 4:
            lv_obj_align_to(sub_indicator_obj, case_4_label, LV_ALIGN_RIGHT_MID, -5, 2);
            break;
        case 5:
            lv_obj_align_to(sub_indicator_obj, case_5_label, LV_ALIGN_RIGHT_MID, -5, 2);
            break;
        case 6:
            lv_obj_align_to(sub_indicator_obj, case_6_label, LV_ALIGN_RIGHT_MID, -5, 2);
            break;
        default:
            break;
    }

    /* udpate checked */
    switch(checked_index)
    {
        case 1:
            lv_obj_align_to(sub_checked_pic_obj, case_1_label, LV_ALIGN_OUT_LEFT_MID, -5, 2);
            break;
        case 2:
            lv_obj_align_to(sub_checked_pic_obj, case_2_label, LV_ALIGN_OUT_LEFT_MID, -5, 2);
            break;
        case 3:
            lv_obj_align_to(sub_checked_pic_obj, case_3_label, LV_ALIGN_OUT_LEFT_MID, -5, 2);
            break;
        case 4:
            lv_obj_align_to(sub_checked_pic_obj, case_4_label, LV_ALIGN_OUT_LEFT_MID, -5, 2);
            break;
        case 5:
            lv_obj_align_to(sub_checked_pic_obj, case_5_label, LV_ALIGN_OUT_LEFT_MID, -5, 2);
            break;
        case 6:
            lv_obj_align_to(sub_checked_pic_obj, case_6_label, LV_ALIGN_OUT_LEFT_MID, -5, 2);
            break;
        default:
            LV_LOG_ERROR("checked_index error");
            break;
    }
}

static void lime_sub_setting_countdown_event_handler(lv_obj_t* obj, setting_key_option_e key_option, bool *is_sub_exit)
{
    MLV_BASE_OBJ_NULL_CHECK(obj);

    /* get hal data */
    LimeHal_Info_t *halInfo = LimeHAL_GetInfoPin();
    LimeHal_SettingInfo_t *settingInfo = &halInfo->settingInfo;
    uint8_t *count_down_index = &settingInfo->countDownTimeIndex;
    static int8_t now_selected;

    /* silently refresh */
    if(key_option == setting_key_option_none)
    {
        lime_sub_setting_countdown_update(obj, 0, *count_down_index);

        if((*count_down_index >= 1) && (*count_down_index <= 6))
            now_selected = *count_down_index;

        return;
    }

    /* first enter, show indicator */
    if(key_option == setting_key_option_first_enter)
    {
        lime_sub_setting_countdown_update(obj, now_selected, *count_down_index);

        return;
    }

    /* focusing on */
    switch(key_option)
    {
        /*
        1 , 4
        2 , 5
        3 , 6
        */
        case setting_key_option_up:
        {
            if(now_selected <= 3)
                now_selected = now_selected == 1 ? 3 : now_selected - 1;
            else
                now_selected = now_selected == 4 ? 6 : now_selected - 1;
            break;
        }
        case setting_key_option_down:
        {
            if(now_selected <= 3)
                now_selected = now_selected == 3 ? 1 : now_selected + 1;
            else
                now_selected = now_selected == 6 ? 4 : now_selected + 1;
            break;
        }
        case setting_key_option_left:
        {
            if(now_selected <= 3)
            {
                /* exti sub menu */
                if(is_sub_exit != NULL)
                    *is_sub_exit = true;

                /* hide sub indicator */
                lime_sub_setting_countdown_update(obj, 0, *count_down_index);

                return;
            }
            else
            {
                now_selected -= 3;
            }
            break;
        }
        case setting_key_option_right:
        {
            if(now_selected <= 3)
            {
                now_selected += 3;
            }
            break;
        }
        case setting_key_option_set:
        {
            *count_down_index = now_selected;
            break;
        }
        default:
            break;
    }

    LV_LOG_USER("key_option: %d, now_selected: %d", key_option, now_selected);

    /* update indicator UI */
    lime_sub_setting_countdown_update(obj, now_selected, *count_down_index);
}

static lv_obj_t *lime_sub_setting_switch_create(lv_obj_t* parent)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    lv_obj_t *sub_bj_obj = lv_obj_create(parent);
    lv_obj_set_size(sub_bj_obj, 343, 142);
    lv_obj_set_style_bg_opa(sub_bj_obj, LV_OPA_0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sub_bj_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(sub_bj_obj, LV_OBJ_FLAG_SCROLLABLE);

    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_1_title, sub_bj_obj, "音量", 9, 37, 60);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_2_title, sub_bj_obj, "光强", 9, 62, 60);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_3_title, sub_bj_obj, "光效", 9, 86, 60);

    /* create description title */
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(description_label, sub_bj_obj, "交互方式", 251, 32, 65);

    lv_obj_t *description_sub_label = lv_label_create(parent);
    lv_obj_set_size(description_sub_label, 110, 72);
    lv_obj_set_pos(description_sub_label, 221, 67);
    lv_label_set_text(description_sub_label, "音量：调整提示音大小\n光强：调整灯光亮度\n光效：选择灯光渐变类型");
    lv_obj_set_style_text_color(description_sub_label, lv_color_hex(0x7F7F7F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(description_sub_label, &lime_font_setting_sub_text, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(description_sub_label, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_align(description_sub_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    return sub_bj_obj;
}

static lv_obj_t *lime_sub_setting_preview_create(lv_obj_t* parent)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    lv_obj_t *sub_bj_obj = lv_obj_create(parent);
    lv_obj_set_size(sub_bj_obj, 343, 142);
    lv_obj_set_style_bg_opa(sub_bj_obj, LV_OPA_0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sub_bj_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(sub_bj_obj, LV_OBJ_FLAG_SCROLLABLE);

    /* create indicator */
    lv_obj_t *sub_indicator_obj = lv_obj_create(sub_bj_obj);
    lv_obj_set_size(sub_indicator_obj, 90, 20);
    lv_obj_set_style_bg_opa(sub_indicator_obj, LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sub_indicator_obj, lv_color_hex(0x0367fd), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(sub_indicator_obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(sub_indicator_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(sub_indicator_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(sub_indicator_obj, LV_OBJ_FLAG_SCROLLABLE);

    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_1_title, sub_bj_obj, "初次见面", 39, 29, 80);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_2_title, sub_bj_obj, "放下水杯", 39, 49, 80);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_3_title, sub_bj_obj, "时间到了", 39, 69, 80);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_4_title, sub_bj_obj, "取走水杯", 39, 89, 80);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_5_title, sub_bj_obj, "阳光很好", 39, 109, 80);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_6_title, sub_bj_obj, "下雨的时候", 120, 29, 80);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_7_title, sub_bj_obj, "下雪的时候", 120, 49, 80);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_8_title, sub_bj_obj, "早上好~", 120, 69, 80);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_9_title, sub_bj_obj, "中午好~", 120, 89, 80);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_10_title, sub_bj_obj, "晚上好~", 120, 109, 80);

    /* just test */
    lime_sub_setting_preview_update(sub_bj_obj, 7);

    /* create description title */
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(description_label, sub_bj_obj, "音频预览", 251, 32, 65);

    lv_obj_t *description_sub_label = lv_label_create(parent);
    lv_obj_set_size(description_sub_label, 110, 72);
    lv_obj_set_pos(description_sub_label, 221, 67);
    lv_label_set_text(description_sub_label, "预览已存入的音频文件\n若需修改相关文件 请通过U盘模式进行");
    lv_obj_set_style_text_color(description_sub_label, lv_color_hex(0x7F7F7F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(description_sub_label, &lime_font_setting_sub_text, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(description_sub_label, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_align(description_sub_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    return sub_bj_obj;
}
static void lime_sub_setting_preview_update(lv_obj_t* obj, uint8_t indicator_index)
{
    /* check para */
    MLV_BASE_OBJ_NULL_CHECK(obj);

    /* get child obj */
    lv_obj_t *sub_indicator_obj = lv_obj_get_child(obj, 0);
    lv_obj_t *case_1_label = lv_obj_get_child(obj, 1);
    lv_obj_t *case_2_label = lv_obj_get_child(obj, 2);
    lv_obj_t *case_3_label = lv_obj_get_child(obj, 3);
    lv_obj_t *case_4_label = lv_obj_get_child(obj, 4);
    lv_obj_t *case_5_label = lv_obj_get_child(obj, 5);
    lv_obj_t *case_6_label = lv_obj_get_child(obj, 6);
    lv_obj_t *case_7_label = lv_obj_get_child(obj, 7);
    lv_obj_t *case_8_label = lv_obj_get_child(obj, 8);
    lv_obj_t *case_9_label = lv_obj_get_child(obj, 9);
    lv_obj_t *case_10_label = lv_obj_get_child(obj, 10);

    /* update indicator */
    if(indicator_index == 0)
    {
        lv_obj_add_flag(sub_indicator_obj, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_remove_flag(sub_indicator_obj, LV_OBJ_FLAG_HIDDEN);
    }
    switch(indicator_index)
    {
        case 1:
            lv_obj_align_to(sub_indicator_obj, case_1_label, LV_ALIGN_CENTER, -10, 2);
            break;
        case 2:
            lv_obj_align_to(sub_indicator_obj, case_2_label, LV_ALIGN_CENTER, -10, 2);
            break;
        case 3:
            lv_obj_align_to(sub_indicator_obj, case_3_label, LV_ALIGN_CENTER, -10, 2);
            break;
        case 4:
            lv_obj_align_to(sub_indicator_obj, case_4_label, LV_ALIGN_CENTER, -10, 2);
            break;
        case 5:
            lv_obj_align_to(sub_indicator_obj, case_5_label, LV_ALIGN_CENTER, -10, 2);
            break;
        case 6:
            lv_obj_align_to(sub_indicator_obj, case_6_label, LV_ALIGN_CENTER, 0, 2);
            break;
        case 7:
            lv_obj_align_to(sub_indicator_obj, case_7_label, LV_ALIGN_CENTER, 0, 2);
            break;
        case 8:
            lv_obj_align_to(sub_indicator_obj, case_8_label, LV_ALIGN_CENTER, 0, 2);
            break;
        case 9:
            lv_obj_align_to(sub_indicator_obj, case_9_label, LV_ALIGN_CENTER, 0, 2);
            break;
        case 10:
            lv_obj_align_to(sub_indicator_obj, case_10_label, LV_ALIGN_CENTER, 0, 2);
            break;
        default:
            break;
    }
}
static void lime_sub_setting_preview_event_handler(lv_obj_t* obj, setting_key_option_e key_option, bool *is_sub_exit)
{
    MLV_BASE_OBJ_NULL_CHECK(obj);

    static int8_t now_selected;

    /* silently refresh */
    if(key_option == setting_key_option_none)
    {
        lime_sub_setting_preview_update(obj, 0);

        now_selected = 1;

        return;
    }

    /* first enter, show indicator */
    if(key_option == setting_key_option_first_enter)
    {
        lime_sub_setting_preview_update(obj, now_selected);

        return;
    }

    /* focusing on */
    switch(key_option)
    {
        /*
        1 , 6
        2 , 7
        3 , 8
        4 , 9
        5 , 10
        */
        case setting_key_option_up:
        {
            if(now_selected <= 5)
                now_selected = now_selected == 1 ? 5 : now_selected - 1;
            else
                now_selected = now_selected == 6 ? 10 : now_selected - 1;
            break;
        }
        case setting_key_option_down:
        {
            if(now_selected <= 5)
                now_selected = now_selected == 5 ? 1 : now_selected + 1;
            else
                now_selected = now_selected == 10 ? 6 : now_selected + 1;
            break;
        }
        case setting_key_option_left:
        {
            if(now_selected <= 5)
            {
                /* exti sub menu */
                if(is_sub_exit != NULL)
                    *is_sub_exit = true;

                /* hide sub indicator */
                lime_sub_setting_preview_update(obj, 0);

                return;
            }
            else
            {
                now_selected -= 5;
            }
            break;
        }
        case setting_key_option_right:
        {
            if(now_selected <= 5)
            {
                now_selected += 5;
            }
            break;
        }
        case setting_key_option_set:
        {
            LV_LOG_USER("play music, set selected: %d", now_selected);
            LimeHAL_SettingInfo_PlayMusicByIndex(now_selected);
            break;
        }
        default:
            break;
    }

    LV_LOG_USER("key_option: %d, now_selected: %d", key_option, now_selected);

    /* update indicator UI */
    lime_sub_setting_preview_update(obj, now_selected);
}


static lv_obj_t *lime_sub_setting_about_create(lv_obj_t* parent)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    lv_obj_t *sub_bj_obj = lv_obj_create(parent);
    lv_obj_set_size(sub_bj_obj, 343, 142);
    lv_obj_set_style_bg_opa(sub_bj_obj, LV_OPA_0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sub_bj_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(sub_bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(sub_bj_obj, LV_OBJ_FLAG_SCROLLABLE);

    /* create indicator */
    lv_obj_t *sub_indicator_obj = lv_obj_create(sub_bj_obj);
    lv_obj_set_size(sub_indicator_obj, 90, 28);
    lv_obj_set_style_bg_opa(sub_indicator_obj, LV_OPA_60, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sub_indicator_obj, lv_color_hex(0x0367fd), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(sub_indicator_obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(sub_indicator_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(sub_indicator_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(sub_indicator_obj, LV_OBJ_FLAG_SCROLLABLE);

    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_1_title, sub_bj_obj, "USB电压", 17, 38, 80);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_2_title, sub_bj_obj, "JEDEC ID", 17, 57, 80);
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(case_3_title, sub_bj_obj, "WIFI IP", 17, 75, 80);

    lv_obj_t *case_4_title = lv_label_create(sub_bj_obj);
    lv_obj_set_size(case_4_title, 100, 16);
    lv_obj_align(case_4_title, LV_ALIGN_TOP_RIGHT, -145, 38);
    lv_label_set_text(case_4_title, "4.88V");
    lv_obj_set_style_text_color(case_4_title, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(case_4_title, &lime_font_setting_title, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(case_4_title, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_set_style_text_align(case_4_title, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *case_5_title = lv_label_create(sub_bj_obj);
    lv_obj_set_size(case_5_title, 100, 16);
    lv_obj_align(case_5_title, LV_ALIGN_TOP_RIGHT, -145, 57);
    lv_label_set_text(case_5_title, "0x4017");
    lv_obj_set_style_text_color(case_5_title, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(case_5_title, &lime_font_setting_title, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(case_5_title, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_set_style_text_align(case_5_title, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *case_6_title = lv_label_create(sub_bj_obj);
    lv_obj_set_size(case_6_title, 100, 16);
    lv_obj_align(case_6_title, LV_ALIGN_TOP_RIGHT, -145, 75);
    lv_label_set_text(case_6_title, "192.168.199.100");
    lv_obj_set_style_text_color(case_6_title, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(case_6_title, &lime_font_setting_title, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(case_6_title, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_set_style_text_align(case_6_title, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *enter_u_disk_bg = lv_obj_create(sub_bj_obj);
    lv_obj_set_pos(enter_u_disk_bg, 15, 105);
    lv_obj_set_size(enter_u_disk_bg, 90, 28);
    lv_obj_set_style_bg_opa(enter_u_disk_bg, LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(enter_u_disk_bg, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(enter_u_disk_bg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(enter_u_disk_bg, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(enter_u_disk_bg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(enter_u_disk_bg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(enter_u_disk_bg, LV_OBJ_FLAG_SCROLLABLE);

    LIME_LV_CREATE_DEFAULT_MENU_LABEL(udisk_title, enter_u_disk_bg, "进入U盘", 0, 0, 65);
    lv_obj_set_style_text_align(udisk_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(udisk_title, LV_ALIGN_CENTER, 0, -2);

    lv_obj_t *restore_bg = lv_obj_create(sub_bj_obj);
    lv_obj_set_pos(restore_bg, 115, 105);
    lv_obj_set_size(restore_bg, 90, 28);
    lv_obj_set_style_bg_opa(restore_bg, LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(restore_bg, lv_color_hex(0xe60000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(restore_bg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(restore_bg, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(restore_bg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(restore_bg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(restore_bg, LV_OBJ_FLAG_SCROLLABLE);

    LIME_LV_CREATE_DEFAULT_MENU_LABEL(restore_title, restore_bg, "重置设备", 0, 0, 65);
    lv_obj_set_style_text_align(restore_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(restore_title, LV_ALIGN_CENTER, 0, -2);

    /* test */
    lime_sub_setting_about_update(sub_bj_obj, 2);
    // lv_obj_align_to(sub_indicator_obj, restore_bg, LV_ALIGN_CENTER, 0, 0);


    /* create description title */
    LIME_LV_CREATE_DEFAULT_MENU_LABEL(description_label, sub_bj_obj, "关于杯垫", 251, 32, 65);

    lv_obj_t *description_sub_label = lv_label_create(sub_bj_obj);
    lv_obj_set_size(description_sub_label, 110, 72);
    lv_obj_set_pos(description_sub_label, 221, 67);
    lv_label_set_text(description_sub_label, "软件版本：V2.0\n编译时间：Jan 24 2026\n\n\n由B站 平韵の小窝 开源");
    lv_obj_set_style_text_color(description_sub_label, lv_color_hex(0x7F7F7F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(description_sub_label, &lime_font_setting_sub_text, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(description_sub_label, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_align(description_sub_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    return sub_bj_obj;
}

static void lime_sub_setting_about_update(lv_obj_t* obj, uint8_t indicator_index)
{
    /* check para */
    MLV_BASE_OBJ_NULL_CHECK(obj);

    /* get child obj */
    lv_obj_t *sub_indicator_obj = lv_obj_get_child(obj, 0);
    lv_obj_t *enter_u_disk_bg = lv_obj_get_child(obj, 7);
    lv_obj_t *restore_bg = lv_obj_get_child(obj, 8);

    /* update indicator */
    if(indicator_index == 0)
    {
        lv_obj_add_flag(sub_indicator_obj, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_remove_flag(sub_indicator_obj, LV_OBJ_FLAG_HIDDEN);
    }
    switch(indicator_index)
    {
        case 1:
            lv_obj_align_to(sub_indicator_obj, enter_u_disk_bg, LV_ALIGN_CENTER, 0, 0);
            break;
        case 2:
            lv_obj_align_to(sub_indicator_obj, restore_bg, LV_ALIGN_CENTER, 0, 0);
            break;
        default:
            break;
    }
}
static void lime_sub_setting_about_event_handler(lv_obj_t* obj, setting_key_option_e key_option, bool *is_sub_exit)
{
    MLV_BASE_OBJ_NULL_CHECK(obj);

    static int8_t now_selected;

    /* silently refresh */
    if(key_option == setting_key_option_none)
    {
        lime_sub_setting_about_update(obj, 0);

        now_selected = 1;

        return;
    }

    /* first enter, show indicator */
    if(key_option == setting_key_option_first_enter)
    {
        lime_sub_setting_about_update(obj, now_selected);

        return;
    }

    /* focusing on */
    switch(key_option)
    {
        case setting_key_option_left:
        {
            if(now_selected == 2)
                now_selected = 1;
            else
            {
                /* exti sub menu */
                if(is_sub_exit != NULL)
                    *is_sub_exit = true;

                /* hide sub indicator */
                lime_sub_setting_about_update(obj, 0);

                return;
            }
            break;
        }
        case setting_key_option_right:
        {
            now_selected = 2;
            break;
        }
        case setting_key_option_set:
        {
            if(now_selected == 1)
            {
                /* enter u disk */
                LV_LOG_USER("enter u disk");
                LimeHAL_SettingInfo_EnterUdisk();
            }
            else
            {
                /* restore device */
                LV_LOG_USER("restore device");
                LimeHAL_SettingInfo_Restore();
            }
        }
    }

    /* update indicator UI */
    lime_sub_setting_about_update(obj, now_selected);
}

setting_in_out_dir_e lime_setting_run_handler(const LimeHal_KeyInfo_t *keyInfo, setting_in_out_dir_e in_dir)
{
    static uint8_t main_index = 0;
    static LimeHal_KeyInfo_t keyInfoLast = {0};
    static bool is_sub_menu_loop = false;
    static bool is_sub_menu_loop_last = false;
    bool is_press = false;

    if(indicator_obj == NULL)
    {
        LV_LOG_ERROR("indicator_obj is NULL");
        return setting_in_out_dir_none;
    }

    /* init */
    if(in_dir != setting_in_out_dir_none)
    {
        if(in_dir == setting_in_out_dir_up)
        {
            main_index = 1;
        }
        else
        {
            main_index = 4;
        }

        is_sub_menu_loop = false;
        is_sub_menu_loop_last = false;

        is_press = true;

        lv_tabview_set_active(mainTabview, main_index - 1, LV_ANIM_OFF);

        /* update now setting data to UI */
        lime_sub_setting_countdown_event_handler(sub_tab1, setting_key_option_none, NULL);
        lime_sub_setting_preview_event_handler(sub_tab3, setting_key_option_none, NULL);
        lime_sub_setting_about_event_handler(sub_tab4, setting_key_option_none, NULL);

        goto refresh_main_indicator;
    }

    /* sub menu loop */
    if(is_sub_menu_loop)
    {
        setting_key_option_e option = lime_sbu_setting_detech_key_option(keyInfo, &keyInfoLast);
        if(option == setting_key_option_none && is_sub_menu_loop_last)
        {
            goto refresh_main_indicator;
        }

        if( !is_sub_menu_loop_last)
        {
            is_sub_menu_loop_last = true;
            option = setting_key_option_first_enter;
        }

        /* call sub menu handler */
        bool is_sub_menu_return = false;
        switch(main_index)
        {
            case 1:
                lime_sub_setting_countdown_event_handler(sub_tab1, option, &is_sub_menu_return);
                break;
            case 3:
                lime_sub_setting_preview_event_handler(sub_tab3, option, &is_sub_menu_return);
                break;
            case 4:
                lime_sub_setting_about_event_handler(sub_tab4, option, &is_sub_menu_return);
                break;
            default:
                is_sub_menu_return = true;
                break;
        }

        if(is_sub_menu_return)
        {
            is_press = true;
            is_sub_menu_loop = false;
        }

        goto refresh_main_indicator;
    }

    /* judge key action */
    if((keyInfo->sw_up != keyInfoLast.sw_up) && (keyInfo->sw_up % 2))
    {
        is_press = true;

        if(main_index  == 1)
            return setting_in_out_dir_up;

        main_index--;
    }
    if((keyInfo->sw_down != keyInfoLast.sw_down) && (keyInfo->sw_down % 2))
    {
        is_press = true;

        if(main_index  == 4)
            return setting_in_out_dir_down;

        main_index++;
    }
    if((keyInfo->sw_right != keyInfoLast.sw_right) && (keyInfo->sw_right % 2))
    {
        is_sub_menu_loop = true;
        is_press = true;
    }
    if((keyInfo->sw_set != keyInfoLast.sw_set) && (keyInfo->sw_set % 2))
    {
        is_sub_menu_loop = true;
        is_press = true;
    }

refresh_main_indicator:
    if(is_press)
    {
        main_indicator_change_pos(main_index, is_sub_menu_loop);
        lv_tabview_set_active(mainTabview, main_index - 1, LV_ANIM_ON);
    }

    /* sync last key */
    memcpy(&keyInfoLast, keyInfo, sizeof(LimeHal_KeyInfo_t));
    is_sub_menu_loop_last = is_sub_menu_loop ? is_sub_menu_loop_last : false;

    return setting_in_out_dir_none;
}
