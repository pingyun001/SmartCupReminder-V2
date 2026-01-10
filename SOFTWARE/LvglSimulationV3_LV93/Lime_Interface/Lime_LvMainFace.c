#include "Lime_LvMainFace.h"
#include "lvgl.h"
#include "Lime_SimFiveKey.h"

#include "Lime_App_Hal.h"

#include "Lime_MessageBox.h"

#if USING_LIME_HARDWARE

#endif

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

    /* start simulator hardware timer */
#if !USING_LIME_HARDWARE
    LimeHAL_SoftSimHardwareTimer_Init();
#endif
}
