#include "Lime_LvMainFace.h"
#include "lvgl.h"
#include "Lime_SimFiveKey.h"

#include "Lime_App_Hal.h"
#include "Lime_CountFace.h"
#include "Lime_HeadBar.h"
#include "Lime_MessageBox.h"
#include "Lime_WeatherFace.h"
#include "Lime_App_Base.h"
#include "Lime_SettingFace.h"
#include "Lime_App_Hal.h"
#include "Lime_App_StartFace.h"

LV_IMG_DECLARE(lime_mainbg);

static lv_obj_t* mainFaceObj = NULL;
static lv_obj_t *mainTabview = NULL;
static lv_obj_t *tab1 = NULL;
static lv_obj_t *tab2 = NULL;
static lv_obj_t *tab3 = NULL;

static const uint8_t default_page = 1;

static void main_scan_timer_cb(lv_timer_t * timer);

void Lime_LvMainFace_Init(void)
{
    LV_LOG_USER("CompileTime:%s,%s", __DATE__, __TIME__);

    mainFaceObj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(mainFaceObj, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_opa(mainFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(mainFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(mainFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(mainFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(mainFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(mainFaceObj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(mainFaceObj);

    lv_obj_t* bg_img = lv_img_create(mainFaceObj);
    lv_img_set_src(bg_img, &lime_mainbg);
    lv_obj_align(bg_img, LV_ALIGN_CENTER, 0, 0);

    mainTabview = lv_tabview_create(mainFaceObj);
    lv_tabview_set_tab_bar_position(mainTabview, LV_DIR_RIGHT);
    lv_tabview_set_tab_bar_size(mainTabview, 0);
    lv_obj_set_style_bg_opa(mainTabview, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(mainTabview, LV_HOR_RES, LV_VER_RES);
    lv_obj_center(mainTabview);

    tab1 = lv_tabview_add_tab(mainTabview, "Tab 1");
    lv_obj_set_style_bg_opa(tab1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tab1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    tab2 = lv_tabview_add_tab(mainTabview, "Tab 2");
    lv_obj_set_style_bg_opa(tab2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tab2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_tabview_set_active(mainTabview, 1, LV_ANIM_OFF);
    tab3 = lv_tabview_add_tab(mainTabview, "Tab 3");
    lv_obj_set_style_bg_opa(tab3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tab3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lime_headbar_create(mainFaceObj);

    lime_countface_create(tab2);
    lime_setting_widget_create(tab3);

    /* just debug */
    lv_tabview_set_active(mainTabview, default_page, LV_ANIM_OFF);

    /* start simulator hardware timer */
#if !USING_LIME_HARDWARE
    Lime_SimFiveKey_Init(mainFaceObj);
    LimeHAL_SoftSimHardwareTimer_Init();
#else
    // LimeHAL_SoftSimHardwareTimer_Init();
#endif

    /* show start face */
    Lime_App_StartFace_Create(mainFaceObj, LV_HOR_RES, LV_VER_RES);
}

void Lime_App_StartFace_Finish_Hook(void)
{
    LV_LOG_USER("Lime_App_StartFace_Finish_Hook");

    /* create face 1(face 1 need fonts in fatfs!!!) */
    lime_weatherface_create(tab1);
    lime_weatherface_soft_start(false);

    /* start scan timer */
    lv_timer_create(main_scan_timer_cb, 10, NULL);
}

static void main_scan_timer_cb(lv_timer_t * timer)
{
    static LimeHal_KeyInfo_t keyInfoLast = {0};
    static int8_t now_page = default_page;
    bool is_press = false;
    const LimeHal_Info_t *info = LimeHAL_GetInfoPin();
    MLV_BASE_OBJ_NULL_CHECK(info);
    const LimeHal_KeyInfo_t *keyInfo = &info->keyInfo;
    static bool is_in_setting_loop = false;

    if(is_in_setting_loop)
    {
        setting_in_out_dir_e out_dir = lime_setting_run_handler(keyInfo, setting_in_out_dir_none);
        if(out_dir == setting_in_out_dir_none)
            goto sync_end;
        else
        {
            is_in_setting_loop = false;
            if(out_dir == setting_in_out_dir_up)
            {
                /* out by up direction, go back to main page */
                is_press = true;
                now_page = 1;
            }
            if(out_dir == setting_in_out_dir_down)
            {
                /* out by down direction, go back to weather page */
                is_press = true;
                now_page = 0;
            }

            goto sub_page_switch;
        }
    }

    if((keyInfo->sw_up != keyInfoLast.sw_up) && (keyInfo->sw_up % 2))
    {
        is_press = true;
        now_page = now_page <= 0 ? 2 : now_page - 1;
    }

    if((keyInfo->sw_down != keyInfoLast.sw_down) && (keyInfo->sw_down % 2))
    {
        is_press = true;
        now_page = now_page >= 2 ? 0 : now_page + 1;
    }


sub_page_switch:
    if(is_press)
    {
        switch(now_page)
        {
            case 0:
            {
                lime_weatherface_soft_start(true);
                lime_headbar_change_width(true);
                lv_tabview_set_active(mainTabview, 0, LV_ANIM_ON);

                break;
            }
            case 1:
            {
                lime_weatherface_soft_start(false);
                lime_headbar_change_width(false);
                lv_tabview_set_active(mainTabview, 1, LV_ANIM_ON);

                break;
            }
            case 2:
            {
                lime_weatherface_soft_start(false);
                lime_headbar_change_width(true);
                lv_tabview_set_active(mainTabview, 2, LV_ANIM_ON);

                /* start setting loop */
                setting_in_out_dir_e in_dir = (keyInfo->sw_down != keyInfoLast.sw_down) ? setting_in_out_dir_up : setting_in_out_dir_down;
                lime_setting_run_handler(keyInfo, in_dir);
                is_in_setting_loop = true;

                break;
            }
        }
    }


    /* sync last key info */
sync_end:
    keyInfoLast.sw_up = keyInfo->sw_up;
    keyInfoLast.sw_down = keyInfo->sw_down;
    keyInfoLast.sw_left = keyInfo->sw_left;
    keyInfoLast.sw_right = keyInfo->sw_right;
    keyInfoLast.sw_set = keyInfo->sw_set;
}
