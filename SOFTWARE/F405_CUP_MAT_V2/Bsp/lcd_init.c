#include "lcd_init.h"

#include "spi.h"
#include "tim.h"

//#include "bsp_dma.h"

extern SPI_HandleTypeDef SPI_InitStructure;
extern DMA_HandleTypeDef DMA_InitStructure;


void BSP_SPI_SendByte(uint8_t dat)
{
//    HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &dat, 1, 0xFF);
//    HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
}

/**
 * @brief       向液晶写寄存器命令
 * @param       reg: 要写的命令
 * @retval      无
 */
void LCD_WR_REG(uint8_t reg)
{
    LCD_DC_Clr();
    BSP_SPI_SendByte(reg);
    LCD_DC_Set();
}

/**
 * @brief       向液晶写一个字节数据
 * @param       dat: 要写的数据
 * @retval      无
 */
void LCD_WR_Byte(uint8_t dat)
{
    LCD_DC_Set();
    BSP_SPI_SendByte(dat);
    LCD_DC_Set();
}

/**
 * @brief       向液晶写一个半字数据
 * @param       dat: 要写的数据
 * @retval      无
 */
void LCD_WR_HalfWord(uint16_t dat)
{
    LCD_DC_Set();
    BSP_SPI_SendByte(dat >> 8);
    BSP_SPI_SendByte(dat & 0xFF);
    LCD_DC_Set();
}

/**
 * @brief       初始化LCD接口
 * @param       无
 * @retval      无
 */
void LCD_GPIOInit(void)
{
//    GPIO_InitTypeDef GPIO_InitStructure;
//    LCD_RES_CLK_Enable(); /* 使能端口时钟 */
//    LCD_DC_CLK_Enable();  /* 使能端口时钟 */
//    LCD_BLK_CLK_Enable(); /* 使能端口时钟 */

//    /* 配置GPIO端口 */
//    GPIO_InitStructure.Pin = LCD_RES_PIN;
//    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(LCD_RES_PORT, &GPIO_InitStructure);

//    GPIO_InitStructure.Pin = LCD_DC_PIN;
//    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(LCD_DC_PORT, &GPIO_InitStructure);

//    GPIO_InitStructure.Pin = LCD_BLK_PIN;
//    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(LCD_BLK_PORT, &GPIO_InitStructure);
}

/**
 * @brief       设置显示窗口
 * @param       xs:窗口列起始地址
 * @param       ys:坐标行起始地址
 * @param       xe:窗口列结束地址
 * @param       ye:坐标行结束地址
 * @retval      无
 */
void LCD_Address_Set(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye)
{
    #if USE_HORIZONTIAL==0
    LCD_WR_REG(0x2a); /* 列地址设置 */
    LCD_WR_HalfWord(xs+0x0C);
    LCD_WR_HalfWord(xe+0x0C);
    LCD_WR_REG(0x2b); /* 行地址设置 */
    LCD_WR_HalfWord(ys);
    LCD_WR_HalfWord(ye);
    LCD_WR_REG(0x2c); /* 储存器写 */
    #elif USE_HORIZONTIAL==1
    LCD_WR_REG(0x2a); /* 列地址设置 */
    LCD_WR_HalfWord(xs+0x0E);
    LCD_WR_HalfWord(xe+0x0E);
    LCD_WR_REG(0x2b); /* 行地址设置 */
    LCD_WR_HalfWord(ys);
    LCD_WR_HalfWord(ye);
    LCD_WR_REG(0x2c); /* 储存器写 */
    #elif USE_HORIZONTIAL==2
    LCD_WR_REG(0x2a); /* 列地址设置 */
    LCD_WR_HalfWord(xs);
    LCD_WR_HalfWord(xe);
    LCD_WR_REG(0x2b); /* 行地址设置 */
    LCD_WR_HalfWord(ys+0x0E);
    LCD_WR_HalfWord(ye+0x0E);
    LCD_WR_REG(0x2c); /* 储存器写 */
    #else
    LCD_WR_REG(0x2a); /* 列地址设置 */
    LCD_WR_HalfWord(xs);
    LCD_WR_HalfWord(xe);
    LCD_WR_REG(0x2b); /* 行地址设置 */
    LCD_WR_HalfWord(ys+0x0C);
    LCD_WR_HalfWord(ye+0x0C);
    LCD_WR_REG(0x2c); /* 储存器写 */
    #endif
}

/**
 * @brief       指定颜色填充区域
 * @param       xs:填充区域列起始地址
 * @param       ys:填充区域行起始地址
 * @param       xe:填充区域列结束地址
 * @param       ye:填充区域行结束地址
 * @param       color:填充颜色值
 * @retval      无
 */
uint16_t oneLineBuf[428] = {0};
void LCD_Fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color)
{
	while(__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_BSY));
	
	LCD_Address_Set(xs, ys, xe - 1, ye - 1);
	
	while(__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_BSY));
	
	color = ((color & 0x00ff) << 8) | ((color & 0xff00) >> 8);
	for(uint32_t i = 0; i < 428; i++)
	{
		oneLineBuf[i] = color;
	}
	
	LCD_DC_Set();
	
	for(uint32_t i = ys; i < ye; i++)
	{
		LCD_DC_Set();
//		HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET); 
		HAL_SPI_Transmit(&hspi1, (uint8_t*)oneLineBuf, (xe - xs)*2, 100);
//		HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET); 
	}
}

/**
 * @brief       初始化LCD
 * @param       无
 * @retval      无
 */
void LCD_Init(void)
{
    LCD_RES_Set();
    HAL_Delay(5);
    LCD_RES_Clr();
    HAL_Delay(5);
    LCD_RES_Set();
    HAL_Delay(12);
    LCD_BLK_Set();

    LCD_WR_REG(0x11);
    HAL_Delay(20);

	LCD_WR_REG(0xff);
    LCD_WR_Byte(0xa5);
    LCD_WR_REG(0x9a);
    LCD_WR_Byte(0x08);
    LCD_WR_REG(0x9b);
    LCD_WR_Byte(0x08);
    LCD_WR_REG(0x9c);
    LCD_WR_Byte(0xb0);
    LCD_WR_REG(0x9d);
    LCD_WR_Byte(0x17);
    LCD_WR_REG(0x9e);
    LCD_WR_Byte(0xc2);
    LCD_WR_REG(0x8f);
    LCD_WR_Byte(0x22);
    LCD_WR_Byte(0x04);
    LCD_WR_REG(0x84);
    LCD_WR_Byte(0x90);
    LCD_WR_REG(0x83);
    LCD_WR_Byte(0x7B);
    LCD_WR_REG(0x85);
    LCD_WR_Byte(0x4F);
    ////GAMMA---------------------------------/////////////
    //V0[3:0]
    LCD_WR_REG(0x6e);
    LCD_WR_Byte(0x0f);
    LCD_WR_REG(0x7e);
    LCD_WR_Byte(0x0f);

    //V63[3:0]
    LCD_WR_REG(0x60);
    LCD_WR_Byte(0x00);
    LCD_WR_REG(0x70);
    LCD_WR_Byte(0x00);
    //V1[5:0] 
    LCD_WR_REG(0x6d);
    LCD_WR_Byte(0x39);
    LCD_WR_REG(0x7d);
    LCD_WR_Byte(0x31);
    //V62[5:0]
    LCD_WR_REG(0x61);
    LCD_WR_Byte(0x0A);
    LCD_WR_REG(0x71);
    LCD_WR_Byte(0x0A);
    //V2[5:0]
    LCD_WR_REG(0x6c);
    LCD_WR_Byte(0x35);
    LCD_WR_REG(0x7c);
    LCD_WR_Byte(0x29);
    //V61[5:0]
    LCD_WR_REG(0x62);
    LCD_WR_Byte(0x0F);
    LCD_WR_REG(0x72);
    LCD_WR_Byte(0x0F);
    //V20[6:0]
    LCD_WR_REG(0x68);
    LCD_WR_Byte(0x4f);
    LCD_WR_REG(0x78);
    LCD_WR_Byte(0x45);
    //V43[6:0]
    LCD_WR_REG(0x66);
    LCD_WR_Byte(0x33);
    LCD_WR_REG(0x76);
    LCD_WR_Byte(0x33);
    //V4[4:0]
    LCD_WR_REG(0x6b);
    LCD_WR_Byte(0x14);
    LCD_WR_REG(0x7b);
    LCD_WR_Byte(0x14);
    //V59[4:0]
    LCD_WR_REG(0x63);
    LCD_WR_Byte(0x09);
    LCD_WR_REG(0x73);
    LCD_WR_Byte(0x09);
    //V6[4:0]
    LCD_WR_REG(0x6a);
    LCD_WR_Byte(0x13);
    LCD_WR_REG(0x7a);
    LCD_WR_Byte(0x16);
    //V57[4:0]
    LCD_WR_REG(0x64);
    LCD_WR_Byte(0x08);
    LCD_WR_REG(0x74);
    LCD_WR_Byte(0x08);
    LCD_WR_REG(0x69);
    LCD_WR_Byte(0x07);
    LCD_WR_REG(0x79);
    LCD_WR_Byte(0x0d);
    LCD_WR_REG(0x65);
    LCD_WR_Byte(0x05);
    LCD_WR_REG(0x75);
    LCD_WR_Byte(0x05);
    LCD_WR_REG(0x67);
    LCD_WR_Byte(0x33);
    LCD_WR_REG(0x77);
    LCD_WR_Byte(0x33);
    LCD_WR_REG(0x6f);
    LCD_WR_Byte(0x00);
    LCD_WR_REG(0x7f);
    LCD_WR_Byte(0x00);
    LCD_WR_REG(0x50);
    LCD_WR_Byte(0x00);
    LCD_WR_REG(0x52);
    LCD_WR_Byte(0xd6);
    LCD_WR_REG(0x53);
    LCD_WR_Byte(0x04);
    LCD_WR_REG(0x54);
    LCD_WR_Byte(0x04);
    LCD_WR_REG(0x55);
    LCD_WR_Byte(0x1b);
    LCD_WR_REG(0x56);
    LCD_WR_Byte(0x1b);
    LCD_WR_REG(0xa0);
    LCD_WR_Byte(0x2a);
    LCD_WR_Byte(0x24);
    LCD_WR_Byte(0x00);
    LCD_WR_REG(0xa1);
    LCD_WR_Byte(0x84);
    LCD_WR_REG(0xa2);
    LCD_WR_Byte(0x85);
    LCD_WR_REG(0xa8);
    LCD_WR_Byte(0x34);
    LCD_WR_REG(0xa9);
    LCD_WR_Byte(0x80);
    LCD_WR_REG(0xaa);
    LCD_WR_Byte(0x73);
    LCD_WR_REG(0xAB);
    LCD_WR_Byte(0x03);
    LCD_WR_Byte(0x61);
    LCD_WR_REG(0xAC);
    LCD_WR_Byte(0x03);
    LCD_WR_Byte(0x65);
    LCD_WR_REG(0xAD);
    LCD_WR_Byte(0x03);
    LCD_WR_Byte(0x60);
    LCD_WR_REG(0xAE);
    LCD_WR_Byte(0x03);
    LCD_WR_Byte(0x64);
    LCD_WR_REG(0xB9);
    LCD_WR_Byte(0x82);
    LCD_WR_REG(0xBA);
    LCD_WR_Byte(0x83);
    LCD_WR_REG(0xBB);
    LCD_WR_Byte(0x80);
    LCD_WR_REG(0xBC);
    LCD_WR_Byte(0x81);
    LCD_WR_REG(0xBD);
    LCD_WR_Byte(0x02);
    LCD_WR_REG(0xBE);
    LCD_WR_Byte(0x01);
    LCD_WR_REG(0xBF);
    LCD_WR_Byte(0x04);
    LCD_WR_REG(0xC0);
    LCD_WR_Byte(0x03);
    LCD_WR_REG(0xc4);
    LCD_WR_Byte(0x33);
    LCD_WR_REG(0xc5);
    LCD_WR_Byte(0x80);
    LCD_WR_REG(0xc6);
    LCD_WR_Byte(0x73);
    LCD_WR_REG(0xc7);
    LCD_WR_Byte(0x00);
    LCD_WR_REG(0xC8);
    LCD_WR_Byte(0x33);
    LCD_WR_Byte(0x33);
    LCD_WR_REG(0xC9);
    LCD_WR_Byte(0x5b);
    LCD_WR_REG(0xCA);
    LCD_WR_Byte(0x5a);
    LCD_WR_REG(0xCB);
    LCD_WR_Byte(0x5d);
    LCD_WR_REG(0xCC);
    LCD_WR_Byte(0x5c);
    LCD_WR_REG(0xCD);
    LCD_WR_Byte(0x33);
    LCD_WR_Byte(0x33);
    LCD_WR_REG(0xCE);
    LCD_WR_Byte(0x5f);
    LCD_WR_REG(0xCF);
    LCD_WR_Byte(0x5e);
    LCD_WR_REG(0xD0);
    LCD_WR_Byte(0x61);
    LCD_WR_REG(0xD1);
    LCD_WR_Byte(0x60);
    LCD_WR_REG(0xB0);
    LCD_WR_Byte(0x3a);
    LCD_WR_Byte(0x3a);
    LCD_WR_Byte(0x00);
    LCD_WR_Byte(0x00);
    LCD_WR_REG(0xB6);
    LCD_WR_Byte(0x32);
    LCD_WR_REG(0xB7);
    LCD_WR_Byte(0x80);
    LCD_WR_REG(0xB8);
    LCD_WR_Byte(0x73);
    LCD_WR_REG(0xe0);
    LCD_WR_Byte(0x00);
    LCD_WR_REG(0xe1);
    LCD_WR_Byte(0x03);
    LCD_WR_Byte(0x0f);
    LCD_WR_REG(0xe2);
    LCD_WR_Byte(0x04);
    LCD_WR_REG(0xe3);
    LCD_WR_Byte(0x01);
    LCD_WR_REG(0xe4);
    LCD_WR_Byte(0x0e);
    LCD_WR_REG(0xe5);
    LCD_WR_Byte(0x01);
    LCD_WR_REG(0xe6);
    LCD_WR_Byte(0x19);
    LCD_WR_REG(0xe7);
    LCD_WR_Byte(0x10);
    LCD_WR_REG(0xe8);
    LCD_WR_Byte(0x10);
    LCD_WR_REG(0xe9);
    LCD_WR_Byte(0x21);
    LCD_WR_REG(0xea);
    LCD_WR_Byte(0x12);
    LCD_WR_REG(0xeb);
    LCD_WR_Byte(0xd0);
    LCD_WR_REG(0xec);
    LCD_WR_Byte(0x04);
    LCD_WR_REG(0xed);
    LCD_WR_Byte(0x07);
    LCD_WR_REG(0xee);
    LCD_WR_Byte(0x07);
    LCD_WR_REG(0xef);
    LCD_WR_Byte(0x09);
    LCD_WR_REG(0xF0);
    LCD_WR_Byte(0xD0);
    LCD_WR_REG(0xF1);
    LCD_WR_Byte(0x0E);
    LCD_WR_REG(0xF9);
    LCD_WR_Byte(0x56);
    LCD_WR_REG(0xf2);
    LCD_WR_Byte(0x26);
    LCD_WR_Byte(0x1b);
    LCD_WR_Byte(0x0b);
    LCD_WR_Byte(0x20);
    LCD_WR_REG(0xec);
    LCD_WR_Byte(0x04);
    LCD_WR_REG(0x35);
    LCD_WR_Byte(0x00);
    LCD_WR_REG(0x44);
    LCD_WR_Byte(0x00);
    LCD_WR_Byte(0x10);
    LCD_WR_REG(0x46);
    LCD_WR_Byte(0x10);
    LCD_WR_REG(0xff);
    LCD_WR_Byte(0x00);
    LCD_WR_REG(0x3a);
    LCD_WR_Byte(0x05);
    LCD_WR_REG(0x36);
    if (USE_HORIZONTIAL == 0)
    {
        LCD_WR_Byte(0x00);
    }
    else if (USE_HORIZONTIAL == 1)
    {
        LCD_WR_Byte(0xC0);
    }
    else if (USE_HORIZONTIAL == 2)
    {
        LCD_WR_Byte(0x70);
    }
    else
    {
        LCD_WR_Byte(0xA0);
    }
    LCD_WR_REG(0x11);
    HAL_Delay(20);
    LCD_WR_REG(0x29);
    HAL_Delay(15);
		
	/* backlight */
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
	
	TIM4->CCR3 = 100;
}

void PY_LCD_ColorFill(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t* color_p)
{
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
	
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
		Error_Handler();
	LCD_Address_Set(x1, y1, x2, y2);
	
	LCD_WR_RS(1);
	hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
		Error_Handler();
	
//	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
	while(HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)color_p, (x2-x1+1) * (y2-y1+1)) != HAL_OK);
	
	
//	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
	
//	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
}

void PY_LCD_SetBackLight(uint8_t percent)
{
	TIM4->CCR3 = percent;
}
