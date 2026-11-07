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

mult_key_e key_get_press_with_filter(void)
{
	static mult_key_e key_deteched[16] = {MULT_KEY_NO};
	
	/* move back & fill new key */
	for(uint8_t i = 0; i < 15; i++)
	{
		key_deteched[i + 1] = key_deteched[i];
	}
	key_deteched[0] = key_get_press();
	
	/* filter */
	mult_key_e return_key = key_deteched[0];
	for(uint8_t i = 1; i < 16; i++)
	{
		if(key_deteched[i] != return_key)
			return MULT_KEY_NO;
	}
	
	return return_key;
}

/* usb adc divider:
 * old board: 5k(top) / 10k(bottom), ratio 1.5, adc pin reaches 3.3V at usb 4.95V and saturates
 * new board: 10k / 10k,             ratio 2.0, adc pin is about 2.5V at usb 5.0V
 */
#define USB_DIV_RATIO_OLD	1.5f
#define USB_DIV_RATIO_NEW	2.0f
/* only old board can reach this raw value (usb >= 4.4V), new board needs usb >= 5.85V */
#define USB_ADC_SAT_RAW		3600

float key_get_usb_vol(void)
{
	/* default: new board, switch to old board once saturated and never go back */
	static float div_ratio = USB_DIV_RATIO_NEW;

	uint16_t raw_val = HAL_ADC_GetValue(&hadc1);

	if(raw_val >= USB_ADC_SAT_RAW)
	{
		div_ratio = USB_DIV_RATIO_OLD;
	}
	
	float res_val = raw_val / 4095.0f * 3.33f * div_ratio;
	
	if(res_val > 5.1f)
		res_val = 5.1f;
	
	return res_val;
}

