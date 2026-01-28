#ifndef __Lime_SettingFace_H__
#define __Lime_SettingFace_H__

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif


lv_obj_t *lime_setting_widget_create(lv_obj_t* parent);


#endif /* __Lime_SettingFace_H__ */
