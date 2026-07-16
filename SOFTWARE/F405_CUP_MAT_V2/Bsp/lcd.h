#ifndef _LCD_H_
#define _LCD_H_

#include "lcd_init.h"
#include "spi.h"

void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);                                                                                // draw point
void LCD_DrawLine(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);                                                     // draw line
void LCD_DrawRectangle(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);                                                // draw hollow rectangle
void LCD_DrawFillRectangle(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);                                            // draw filled rectangle
void Draw_Circle(int xc, int yc, int x, int y, uint16_t color);                                                                            // circle drawing helper (8-way symmetry)
void LCD_DrawCircle(uint16_t xc, uint16_t yc, uint16_t r, uint16_t color, uint16_t mode);                                                  // draw circle
void LCD_DrawTriangel(uint16_t x, uint16_t y, uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);                         // draw hollow triangle
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);                             // show char
void LCD_ShowString(uint16_t x, uint16_t y,const char *s, uint16_t fc, uint16_t bc, uint16_t sizey, uint8_t mode);                           // show string
uint32_t mypow(uint8_t m, uint8_t n);                                                                                                      // power function (internal)
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);                // show number
void LCD_ShowFloatNum(uint16_t x, uint16_t y, float num, uint8_t pre, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode); // show float
void LCD_ShowChinese12x12(uint16_t x, uint16_t y,const char *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);                      // show 12x12 char
void LCD_ShowChinese16x16(uint16_t x, uint16_t y,const char *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);                      // show 16x16 char
void LCD_ShowChinese24x24(uint16_t x, uint16_t y,const char *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);                      // show 24x24 char
void LCD_ShowChinese32x32(uint16_t x, uint16_t y,const char *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);                      // show 32x32 char
void LCD_ShowChinese(uint16_t x, uint16_t y,const char *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);                           // show Chinese string
void LCD_ShowStr(uint16_t x, uint16_t y,const char *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);                               // show mixed Chinese/English
void LCD_StrCenter(uint16_t x, uint16_t y,const char *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);                             // show centered
void LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t pic[]);                                        // show picture
#endif
