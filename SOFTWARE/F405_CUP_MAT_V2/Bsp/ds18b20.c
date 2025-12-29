#include "ds18b20.h"
#include "tim.h"

#define MODE_OUT 1
#define MODE_IN  0

static void ds18b20_delay_us(uint16_t us);
static void DS18B20_Mode(uint8_t mode);
static void DS18B20_Start(void);
static uint8_t DS18B20_Read_Byte(void);
static uint8_t DS18B20_Read_Bit(void);
static void DS18B20_Write_Byte(uint8_t dat);
static HAL_StatusTypeDef ds18b20_check(void);

static void ds18b20_delay_us(uint16_t us)
{
	TIM14->CNT = 0;
	
	while(TIM14->CNT < us * 84)
		;
}
 
void ds18b20_rst(void)
{                 
	DS18B20_Mode(MODE_OUT);
	DS18B20_LOW();
	ds18b20_delay_us(750);
	DS18B20_HIGH();
	ds18b20_delay_us(15);
}

static HAL_StatusTypeDef ds18b20_check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_Mode(MODE_IN);
  while ((HAL_GPIO_ReadPin(DS18B20_GPIO_Port,DS18B20_Pin)) && (retry<200))
	{
		retry++;
		ds18b20_delay_us(1);
	};
	if(retry>=200)
		return HAL_ERROR;
	else 
		retry=0;
	
  while (( !HAL_GPIO_ReadPin(DS18B20_GPIO_Port,DS18B20_Pin)) && (retry<240))
	{
		retry++;
		ds18b20_delay_us(1);
	};
	if(retry>=240)return HAL_ERROR;
	
	return HAL_OK;
}

static uint8_t DS18B20_Read_Bit(void) 	 
{
	uint8_t data;
	DS18B20_Mode(MODE_OUT);	//SET OUTPUT
	DS18B20_LOW();
	ds18b20_delay_us(2);
	DS18B20_HIGH(); 
	DS18B20_Mode(MODE_IN);	//SET INPUT
	ds18b20_delay_us(12);
	
	if(HAL_GPIO_ReadPin(DS18B20_GPIO_Port,DS18B20_Pin))
		data=1;
	else 
		data=0;
	
	ds18b20_delay_us(50);          
	
	return data;
}

static uint8_t DS18B20_Read_Byte(void)     
{        
	uint8_t i,j,dat;
	dat=0;
	for (i=1;i<=8;i++) 
	{
		j=DS18B20_Read_Bit();
		dat=(j<<7)|(dat>>1);
	}						    
	return dat;
}

static void DS18B20_Write_Byte(uint8_t dat)     
 {             
	uint8_t j;
	uint8_t testb;
	DS18B20_Mode(MODE_OUT);	//SET OUTPUT;
	for (j=1;j<=8;j++) 
	{
		testb=dat&0x01;
		dat=dat>>1;
		if (testb) 
		{
			DS18B20_LOW();	// Write 1
			ds18b20_delay_us(2);                            
			DS18B20_HIGH();
			ds18b20_delay_us(60);             
		}
		else 
		{
			DS18B20_LOW();	// Write 0
			ds18b20_delay_us(60);             
			DS18B20_HIGH();
			ds18b20_delay_us(2);                          
		}
	}
}

static void DS18B20_Start(void) 
{   						               
	ds18b20_rst();	   
	ds18b20_check();	 
	DS18B20_Write_Byte(0xcc);	// skip rom
	DS18B20_Write_Byte(0x44);	// convert
} 
 
//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在
HAL_StatusTypeDef ds18b20_Init(void)
{
	HAL_TIM_Base_Start(&htim14);
	
 	DS18B20_HIGH();    //输出1
 
	ds18b20_rst();
 
	return ds18b20_check();
}  
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
int16_t ds18b20_get_temp(void)
{
	uint8_t temp;
	uint8_t TL,TH;
	short tem;
	
	DS18B20_Start();
	ds18b20_rst();
	ds18b20_check();	 
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0xbe); 
	TL = DS18B20_Read_Byte();
	TH = DS18B20_Read_Byte();
				
	if(TH>7)
	{
			TH = ~TH;
			TL = ~TL; 
			temp = 0;
	}
	else 
		temp=1; 
	
	tem = TH;
	tem <<= 8;    
	tem += TL;
	tem = (float)tem * 0.625; 
	if(temp)
		return tem;
	
	return -tem;    
}
 
static void DS18B20_Mode(uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	
	if(mode)
	{
		GPIO_InitStructure.Pin = DS18B20_Pin;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		GPIO_InitStructure.Pin =  DS18B20_Pin;
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	}
	
	HAL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStructure);
}