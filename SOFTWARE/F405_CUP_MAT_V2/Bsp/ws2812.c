#include "ws2812.h"
#include "tim.h"

#if 0
#define WS2812_DEBUG_LOG DEBUG_LOG
#else
#define WS2812_DEBUG_LOG(...)
#endif

static uint16_t ws2812RawBuf[24 * 4 + 8] __attribute__((aligned(32))) = {0};
#define WS2812TL 		126
#define WS2812TS 		42

void WS2812_SetRGB(uint8_t id, uint8_t r, uint8_t g, uint8_t b)
{
	WS2812_DEBUG_LOG("WS2812_SetRGB id:%d r:%d g:%d b:%d\n", id, r, g, b);

	uint32_t index = 0;
	for(index = 0; index < 8; index ++)
	{
		ws2812RawBuf[id * 24 + index   ] = (g & (0x01 << (7 - index))) ? WS2812TL : WS2812TS;
		ws2812RawBuf[id * 24 + index +8] = (r & (0x01 << (7 - index))) ? WS2812TL : WS2812TS;
		ws2812RawBuf[id * 24 + index+16] = (b & (0x01 << (7 - index))) ? WS2812TL : WS2812TS;
	}
}

void ws2812_Init(void)
{
	WS2812_SetRGB(0, 0x02, 0x00, 0x00);
	WS2812_SetRGB(1, 0x02, 0x00, 0x00);
	WS2812_SetRGB(2, 0x02, 0x00, 0x00);
	WS2812_SetRGB(3, 0x02, 0x00, 0x00);
}

void WS2812_SetBatteryPercent(uint8_t color[3], float batPercent)
{
	if(batPercent < 1.0f)
	{
		WS2812_SetRGB(0, 0x00, 0x00, 0x00);
		WS2812_SetRGB(1, 0x00, 0x00, 0x00);
		WS2812_SetRGB(2, 0x00, 0x00, 0x00);
		WS2812_SetRGB(3, 0x00, 0x00, 0x00);
	}
	else if(batPercent < 25.0f)
	{
		WS2812_SetRGB(0, color[0], color[1], color[2]);
		WS2812_SetRGB(1, 0x00, 0x00, 0x00);
		WS2812_SetRGB(2, 0x00, 0x00, 0x00);
		WS2812_SetRGB(3, 0x00, 0x00, 0x00);
	}
	else if(batPercent < 50.0f)
	{
		WS2812_SetRGB(0, color[0], color[1], color[2]);
		WS2812_SetRGB(1, color[0], color[1], color[2]);
		WS2812_SetRGB(2, 0x00, 0x00, 0x00);
		WS2812_SetRGB(3, 0x00, 0x00, 0x00);
	}
	else if(batPercent < 75.0f)
	{
		WS2812_SetRGB(0, color[0], color[1], color[2]);
		WS2812_SetRGB(1, color[0], color[1], color[2]);
		WS2812_SetRGB(2, color[0], color[1], color[2]);
		WS2812_SetRGB(3, 0x00, 0x00, 0x00);
	}
	else
	{
		WS2812_SetRGB(0, color[0], color[1], color[2]);
		WS2812_SetRGB(1, color[0], color[1], color[2]);
		WS2812_SetRGB(2, color[0], color[1], color[2]);
		WS2812_SetRGB(3, color[0], color[1], color[2]);
	}
}

void WS2812_Sync(void)
{	
	HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_1, (uint32_t*)ws2812RawBuf, sizeof(ws2812RawBuf) / sizeof(uint16_t));
}
