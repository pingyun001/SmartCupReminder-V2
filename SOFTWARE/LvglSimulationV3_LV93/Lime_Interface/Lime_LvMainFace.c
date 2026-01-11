#include "Lime_LvMainFace.h"
#include "lvgl.h"
#include "Lime_SimFiveKey.h"

#include "Lime_App_Hal.h"
#include "Lime_CountFace.h"
#include "Lime_HeadBar.h"
#include "Lime_MessageBox.h"

LV_IMG_DECLARE(lime_mainbg);

static lv_obj_t* mainFaceObj = NULL;

uint8_t timerCount = 0;

static void timer_cb(lv_timer_t * timer)
{
    timerCount += 10;
}

void Lime_LvMainFace_Init(void)
{
    // LV_LOG_USER("CompileTime:%s,%s", __DATE__, __TIME__);

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

    lv_obj_t* header_shadow_obj = lv_obj_create(mainFaceObj);
    lv_obj_set_size(header_shadow_obj, 460, 142);
    lv_obj_set_pos(header_shadow_obj, -224, -118);
    lv_obj_set_style_bg_opa(header_shadow_obj, LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(header_shadow_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(header_shadow_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(header_shadow_obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(header_shadow_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(header_shadow_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(header_shadow_obj, LV_OBJ_FLAG_SCROLLABLE);

    lime_countface_create(mainFaceObj);
    lime_headbar_create(mainFaceObj);

    /* start simulator hardware timer */
#if !USING_LIME_HARDWARE
    LimeHAL_SoftSimHardwareTimer_Init();
#else
    LimeHAL_SoftSimHardwareTimer_Init();
#endif
}
