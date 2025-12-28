#ifndef __DS18B20_H
#define __DS18B20_H 
#include "main.h"   
 
 
#define OUT 1
#define IN  0
 
#define DS18B20_Low  HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin, GPIO_PIN_RESET)
#define DS18B20_High HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_SET)		
		
uint8_t DS18B20_Init(void);
short DS18B20_Get_Temp(void);
void DS18B20_Start(void);
void DS18B20_Write_Byte(uint8_t dat);
uint8_t DS18B20_Read_Byte(void);
uint8_t DS18B20_Read_Bit(void);
void DS18B20_Mode(uint8_t mode);
uint8_t DS18B20_Check(void);
void DS18B20_Rst(void);

void ds18b20_delay_us(uint16_t us);
 
#endif