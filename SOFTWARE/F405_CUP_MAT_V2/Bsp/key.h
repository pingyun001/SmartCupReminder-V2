#ifndef __KEY_H
#define __KEY_H

#include "main.h"


#define IS_CUP_DETECHED()		(HAL_GPIO_ReadPin(CUP_DETECH_GPIO_Port, CUP_DETECH_Pin) == GPIO_PIN_RESET)

typedef enum
{
	MULT_KEY_NO = 0,
	MULT_KEY_UP,
	MULT_KEY_DOWN,
	MULT_KEY_LEFT,
	MULT_KEY_RIGHT,
	MULT_KEY_SET
}mult_key_e;

void key_Init(void);

uint16_t key_get_raw_value(void);

mult_key_e key_get_press(void);

mult_key_e key_get_press_with_filter(void);

float key_get_usb_vol(void);

#endif	//__KEY_H
