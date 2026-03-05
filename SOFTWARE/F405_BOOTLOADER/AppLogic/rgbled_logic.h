#ifndef __RGB_LED_LOGIC_H__
#define __RGB_LED_LOGIC_H__ 

#include "main.h"

typedef enum
{
	rgbled_mode_off = 0,
    rgbled_mode_rainbow,
    rgbled_mode_candle,
    rgbled_mode_starnight,
    rgbled_mode_white,
}rgbled_mode_e;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}rgbled_unit_t;

typedef struct
{
    rgbled_mode_e now_mode;
    rgbled_mode_e last_mode;
    uint8_t glob_brightness;
	bool glob_night_mode;
    uint16_t angle;
    rgbled_unit_t led_unit[4];
}rgbled_t;

void rgbled_set_mode(rgbled_mode_e mode);
void rgbled_set_brightness(uint8_t brightness);
void rgbled_set_night_mode(bool night_mode);
void rgbled_run_handler(void);

#endif	//__RGB_LED_LOGIC_H__
