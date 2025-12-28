#ifndef __WS2812_H
#define __WS2812_H

#include "main.h"

void ws2812_Init(void);

void WS2812_SetRGB(uint8_t id, uint8_t r, uint8_t g, uint8_t b);

void WS2812_SetBatteryPercent(uint8_t color[3], float batPercent);

void WS2812_Sync(void);

#endif	//__WS2812_H
