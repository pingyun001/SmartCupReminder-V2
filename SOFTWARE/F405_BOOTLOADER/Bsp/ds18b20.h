#ifndef __DS18B20_H
#define __DS18B20_H 
#include "main.h"   

#define DS18B20_LOW()  HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin, GPIO_PIN_RESET)
#define DS18B20_HIGH() HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_SET)		
		
HAL_StatusTypeDef ds18b20_Init(void);
int16_t ds18b20_get_temp(void);
void ds18b20_rst(void);
 
#endif