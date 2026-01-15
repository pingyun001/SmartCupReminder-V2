#ifndef __LIME_WEATHERFACE_H__
#define __LIME_WEATHERFACE_H__

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

static lv_font_t *lime_weather_make_big_font(void);

static lv_obj_t *lime_weather_widget_create(lv_obj_t* parent);
static void lime_weather_widget_set_date(lv_obj_t* weather_widget, uint8_t month, uint8_t day);
static void lime_weather_widget_set_icon(lv_obj_t* weather_widget, uint8_t icon_id);
static void lime_weather_widget_set_weather_chinese(lv_obj_t* weather_widget, const char* weather_chinese);
static void lime_weather_widget_set_temperature_range(lv_obj_t* weather_widget, int16_t min_temp, int16_t max_temp);
static void lime_weather_widget_set_humidity(lv_obj_t* weather_widget, uint8_t humidity);

lv_obj_t *lime_weatherface_create(lv_obj_t* parent);

#endif /* __LIME_WEATHERFACE_H__ */
