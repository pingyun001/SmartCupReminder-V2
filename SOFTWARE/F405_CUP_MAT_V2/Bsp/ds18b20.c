#include "ds18b20.h"
#include "tim.h"

void ds18b20_delay_us(uint16_t us)
{
	TIM14->CNT = 0;
	
	while(TIM14->CNT < us * 84)
		;
}
 
void DS18B20_Rst(void)
{                 
		DS18B20_Mode(OUT); 	//SET OUTPUT
    DS18B20_Low; 				//拉低DQ
    ds18b20_delay_us(750);    	//拉低750us
    DS18B20_High; 			//DQ=1
		ds18b20_delay_us(15);     	//15US
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_Mode(IN);	//SET  INPUT	 
  while (HAL_GPIO_ReadPin(DS18B20_GPIO_Port,DS18B20_Pin)&&retry<200)
	{
		retry++;
		ds18b20_delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!HAL_GPIO_ReadPin(DS18B20_GPIO_Port,DS18B20_Pin)&&retry<240)
	{
		retry++;
		ds18b20_delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}
//从DS18B20读取一个位
//返回值：1/0
uint8_t DS18B20_Read_Bit(void) 	 
{
    uint8_t data;
	DS18B20_Mode(OUT);	//SET OUTPUT
    DS18B20_Low; 
	ds18b20_delay_us(2);
    DS18B20_High; 
	DS18B20_Mode(IN);	//SET INPUT
	ds18b20_delay_us(12);
	if(HAL_GPIO_ReadPin(DS18B20_GPIO_Port,DS18B20_Pin))data=1;
    else data=0;	 
    ds18b20_delay_us(50);           
    return data;
}
//从DS18B20读取一个字节
//返回值：读到的数据
uint8_t DS18B20_Read_Byte(void)     
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
//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(uint8_t dat)     
 {             
    uint8_t j;
    uint8_t testb;
	DS18B20_Mode(OUT);	//SET OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_Low;	// Write 1
            ds18b20_delay_us(2);                            
            DS18B20_High;
            ds18b20_delay_us(60);             
        }
        else 
        {
            DS18B20_Low;	// Write 0
            ds18b20_delay_us(60);             
            DS18B20_High;
            ds18b20_delay_us(2);                          
        }
    }
}
//开始温度转换
void DS18B20_Start(void) 
{   						               
    DS18B20_Rst();	   
		DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0x44);	// convert
} 
 
//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
uint8_t DS18B20_Init(void)
{
	HAL_TIM_Base_Start(&htim14);
	
 	DS18B20_High;    //输出1
 
	DS18B20_Rst();
 
	return DS18B20_Check();
}  
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp(void)
{
    uint8_t temp;
    uint8_t TL,TH;
	short tem;
    DS18B20_Start ();  			// ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0xbe);	// convert	    
    TL=DS18B20_Read_Byte(); 	// LSB   
    TH=DS18B20_Read_Byte(); 	// MSB  
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;					//温度为负  
    }else temp=1;				//温度为正	  	  
    tem=TH; 					//获得高八位
    tem<<=8;    
    tem+=TL;					//获得底八位
    tem=(float)tem*0.625;		//转换     
	if(temp)return tem; 		//返回温度值
	else return -tem;    
}
 
void DS18B20_Mode(uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
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