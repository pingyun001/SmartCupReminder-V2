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

/* define port level */
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

/* control functions */
void LCD_WR_REG(uint8_t reg);                                                      /* write one byte command */
void LCD_WR_Byte(uint8_t dat);                                                     /* write one byte data */
void LCD_WR_HalfWord(uint16_t dat);                                                /* write one half-word data */
void LCD_GPIOInit(void);                                                           /* init LCD port */
void LCD_SetCursor(uint16_t x, uint16_t y);                                        /* set cursor */
void LCD_Address_Set(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye);          /* set address window */
void LCD_Fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color); /* fill area */
void LCD_Init(void);                                                               /* low-level LCD init */

/* define pen colors */
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
#define BROWN 0XBC40      // brown
#define BRRED 0XFC07      // dark red
#define GRAY 0X8430       // gray
#define DARKBLUE 0X01CF   // dark blue
#define LIGHTBLUE 0X7D7C  // light blue
#define GRAYBLUE 0X5458   // gray blue
#define LIGHTGREEN 0X841F // light green
#define LGRAY 0XC618      // light gray (panel), window bg
#define LGRAYBLUE 0XA651  // light gray-blue (middle layer)
#define LBBLUE 0X2B12     // light blue (selected item inverse)

void PY_LCD_Clear(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);

void PY_LCD_ColorFill(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t* color_p);

void PY_LCD_SetBackLight(uint8_t percent);

#endif
