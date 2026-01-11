#ifndef __LIME_COUNTFACE_H__
#define __LIME_COUNTFACE_H__

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif




lv_obj_t *lime_countface_create(lv_obj_t* parent);



#endif // __LIME_COUNTFACE_H__
