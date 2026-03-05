#ifndef _LCD_INIT_H_
#define _LCD_INIT_H_

#include "main.h"
#include "spi.h"

#define USE_HORIZONTIAL 2

#if USE_HORIZONTIAL == 0 || USE_HORIZONTIAL == 1
#define LCD_W 142
#define LCD_H 428
#else
#define LCD_W 428
#define LCD_H 142
#endif

/* 定义LCD端口 */

/* 定义端口电平状态 */
#define LCD_RES_Set() HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_SET)
#define LCD_RES_Clr() HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_RESET)

#define LCD_DC_Set() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET)
#define LCD_DC_Clr() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET)

#define LCD_BLK_Set() HAL_GPIO_WritePin(OLED_BLK_GPIO_Port, OLED_BLK_Pin, GPIO_PIN_SET)
#define LCD_BLK_Clr() HAL_GPIO_WritePin(OLED_BLK_GPIO_Port, OLED_BLK_Pin, GPIO_PIN_RESET)


#define	LCD_PWR(n)		(n?\
						HAL_GPIO_WritePin(OLED_BLK_GPIO_Port,OLED_BLK_Pin,GPIO_PIN_SET):\
						HAL_GPIO_WritePin(OLED_BLK_GPIO_Port,OLED_BLK_Pin,GPIO_PIN_RESET))
#define	LCD_WR_RS(n)	(n?\
						HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,GPIO_PIN_SET):\
						HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,GPIO_PIN_RESET))
#define	LCD_RST(n)		(n?\
						HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin,GPIO_PIN_SET):\
						HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin,GPIO_PIN_RESET))

/* 函数声明 */
void LCD_WR_REG(uint8_t reg);                                                      /* LCD写入一个字节指令 */
void LCD_WR_Byte(uint8_t dat);                                                     /* LCD写入一个字节数据 */
void LCD_WR_HalfWord(uint16_t dat);                                                /* LCD写入一个半字数据 */
void LCD_GPIOInit(void);                                                           /* 初始化LCD端口 */
void LCD_SetCursor(uint16_t x, uint16_t y);                                        /* 设置光标位置 */
void LCD_Address_Set(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye);          /* 设置坐标函数 */
void LCD_Fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color); /* 填充函数 */
void LCD_Init(void);                                                               /* 设置LCD初始化 */

/* 定义画笔颜色 */
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40      // 棕色
#define BRRED 0XFC07      // 棕红色
#define GRAY 0X8430       // 灰色
#define DARKBLUE 0X01CF   // 深蓝色
#define LIGHTBLUE 0X7D7C  // 浅蓝色
#define GRAYBLUE 0X5458   // 灰蓝色
#define LIGHTGREEN 0X841F // 浅绿色
#define LGRAY 0XC618      // 浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE 0XA651  // 浅灰蓝色(中间层颜色)
#define LBBLUE 0X2B12     // 浅棕蓝色(选择条目的反色)

void PY_LCD_Clear(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);

void PY_LCD_ColorFill(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t* color_p);

void PY_LCD_SetBackLight(uint8_t percent);

#endif
