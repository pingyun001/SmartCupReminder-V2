#include "key.h"
#include "adc.h"

void key_Init(void)
{
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);
}

uint16_t key_get_raw_value(void)
{
	return HAL_ADC_GetValue(&hadc2);
}


mult_key_e key_get_press(void)
{
	uint16_t key_raw_value = key_get_raw_value();
	
	if(key_raw_value < 1000)
		return MULT_KEY_NO;
	
	else if(key_raw_value < 2252)
		return MULT_KEY_UP;
	
	else if(key_raw_value < 2595)
		return MULT_KEY_SET;
	
	else if(key_raw_value < 2832)
		return MULT_KEY_LEFT;
	
	else if(key_raw_value < 3000)
		return MULT_KEY_RIGHT;
	
	return MULT_KEY_DOWN;
}

float key_get_usb_vol(void)
{
	const float register_up = 5.1f, register_down = 10.0f;
	
	float res_val = HAL_ADC_GetValue(&hadc1);
	
	res_val = res_val / 4095.0f * 3.3f * (register_down + register_up) / register_down;
	
	return res_val;
}

