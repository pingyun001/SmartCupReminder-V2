#include "ws2812.h"
#include "tim.h"

#if 0
#define WS2812_DEBUG_LOG DEBUG_LOG
#else
#define WS2812_DEBUG_LOG(...)
#endif

#define WS2812_LED_NUM	4
#define WS2812_BUF_LEN	(24 * WS2812_LED_NUM + 8)

/* buffer read by dma, only written in timer isr when dma is idle */
static uint16_t ws2812RawBuf[WS2812_BUF_LEN] __attribute__((aligned(4))) = {0};
/* buffer written by cpu task, never touched by dma */
static uint16_t ws2812BuildBuf[WS2812_BUF_LEN] __attribute__((aligned(4))) = {0};
/* dma transfer is running */
static volatile uint8_t ws2812TxBusy = 0;
/* new frame is ready to send */
static volatile uint8_t ws2812Dirty = 0;

#define WS2812TL 		126
#define WS2812TS 		42

void WS2812_SetRGB(uint8_t id, uint8_t r, uint8_t g, uint8_t b)
{
	WS2812_DEBUG_LOG("WS2812_SetRGB id:%d r:%d g:%d b:%d\n", id, r, g, b);

	if(id >= WS2812_LED_NUM)
		return;

	uint32_t index = 0;
	for(index = 0; index < 8; index ++)
	{
		ws2812BuildBuf[id * 24 + index     ] = (g & (0x01 << (7 - index))) ? WS2812TL : WS2812TS;
		ws2812BuildBuf[id * 24 + index +  8] = (r & (0x01 << (7 - index))) ? WS2812TL : WS2812TS;
		ws2812BuildBuf[id * 24 + index + 16] = (b & (0x01 << (7 - index))) ? WS2812TL : WS2812TS;
	}
}

void WS2812_Commit(void)
{
	/* make sure all color data is written before flag is set */
	__DMB();
	ws2812Dirty = 1;
}


void ws2812_Init(void)
{
	WS2812_SetRGB(0, 0x00, 0x00, 0x00);
	WS2812_SetRGB(1, 0x00, 0x00, 0x00);
	WS2812_SetRGB(2, 0x00, 0x00, 0x00);
	WS2812_SetRGB(3, 0x00, 0x00, 0x00);
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
	HAL_StatusTypeDef status = HAL_OK;
	uint32_t index = 0;

	/* last frame still on wire, wait for next call */
	if(ws2812TxBusy)
		return;

	/* no new frame */
	if(!ws2812Dirty)
		return;

	/* dma is idle now, copy build frame to dma buffer safely */
	for(index = 0; index < WS2812_BUF_LEN; index++)
	{
		ws2812RawBuf[index] = ws2812BuildBuf[index];
	}
	ws2812Dirty = 0;

	status = HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_1, (uint32_t*)ws2812RawBuf, WS2812_BUF_LEN);
	if(status == HAL_OK)
	{
		ws2812TxBusy = 1;
	}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM8)
	{
		ws2812TxBusy = 0;
	}
}

