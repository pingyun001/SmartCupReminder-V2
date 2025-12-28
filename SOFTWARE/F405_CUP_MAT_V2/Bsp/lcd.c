#include "lcd.h"
//#include "lcdfont.h"
//#include "bsp_dma.h"
#include "string.h"

extern SPI_HandleTypeDef SPI_InitStructure;
extern DMA_HandleTypeDef DMA_InitStructure;

/**
 * @brief       在LCD屏幕上画一个点
 * @param       x:像素点列坐标
 * @param       y:像素点行坐标
 * @param       color:填充颜色值
 * @retval      无
 */
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_Address_Set(x,y,x,y);
    LCD_WR_HalfWord(color);
}

/**
 * @brief       幂运算(内部调用)
 * @param       m:底数
 * @param       n:指数
 * @retval      result:m的n次幂
 */
uint32_t mypow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
    {
        result *= m;
    }
    return result;
}


void LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t pic[])
{

}
