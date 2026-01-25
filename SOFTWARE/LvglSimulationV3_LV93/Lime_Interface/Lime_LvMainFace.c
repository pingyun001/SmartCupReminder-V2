#include "Lime_LvMainFace.h"
#include "lvgl.h"
#include "Lime_SimFiveKey.h"

#include "Lime_App_Hal.h"
#include "Lime_CountFace.h"
#include "Lime_HeadBar.h"
#include "Lime_MessageBox.h"
#include "Lime_WeatherFace.h"
#include "Lime_App_Base.h"
#include "Lime_App_Hal.h"

LV_IMG_DECLARE(lime_mainbg);

static lv_obj_t* mainFaceObj = NULL;
static lv_obj_t *mainTabview = NULL;

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

    lv_obj_t *tab1 = lv_tabview_add_tab(mainTabview, "Tab 1");
    lv_obj_set_style_bg_opa(tab1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tab1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t *tab2 = lv_tabview_add_tab(mainTabview, "Tab 2");
    lv_obj_set_style_bg_opa(tab2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tab2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_tabview_set_active(mainTabview, 1, LV_ANIM_OFF);

    lime_headbar_create(mainFaceObj);

    lime_weatherface_create(tab1);
    lime_weatherface_soft_start(false);
    lime_countface_create(tab2);

    /* start scan timer */
    lv_timer_create(main_scan_timer_cb, 30, NULL);

    /* start simulator hardware timer */
#if !USING_LIME_HARDWARE
    Lime_SimFiveKey_Init(mainFaceObj);
    LimeHAL_SoftSimHardwareTimer_Init();
#else
    LimeHAL_SoftSimHardwareTimer_Init();
#endif
}

static void main_scan_timer_cb(lv_timer_t * timer)
{
    static LimeHal_KeyInfo_t keyInfoLast = {0};
    const LimeHal_Info_t *info = LimeHAL_GetInfoPin();
    MLV_BASE_OBJ_NULL_CHECK(info);
    const LimeHal_KeyInfo_t *keyInfo = &info->keyInfo;

    if((keyInfo->sw_up != keyInfoLast.sw_up) && (keyInfo->sw_up % 2))
    {
        lime_weatherface_soft_start(true);
        lime_headbar_change_width(true);
        lv_tabview_set_active(mainTabview, 0, LV_ANIM_ON);
    }

    if((keyInfo->sw_down != keyInfoLast.sw_down) && (keyInfo->sw_down % 2))
    {
        lime_weatherface_soft_start(false);
        lime_headbar_change_width(false);
        lv_tabview_set_active(mainTabview, 1, LV_ANIM_ON);
    }


    /* sync last key info */
    keyInfoLast.sw_up = keyInfo->sw_up;
    keyInfoLast.sw_down = keyInfo->sw_down;
    keyInfoLast.sw_left = keyInfo->sw_left;
    keyInfoLast.sw_right = keyInfo->sw_right;
    keyInfoLast.sw_set = keyInfo->sw_set;
}
