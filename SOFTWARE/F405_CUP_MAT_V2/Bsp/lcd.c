#include "lcd.h"
#include "string.h"

extern SPI_HandleTypeDef SPI_InitStructure;
extern DMA_HandleTypeDef DMA_InitStructure;

/**
 * @brief       draw a pixel on LCD
 * @param       x: pixel column
 * @param       y: pixel row
 * @param       color: color value
 * @retval      none
 */
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_Address_Set(x,y,x,y);
    LCD_WR_HalfWord(color);
}

/**
 * @brief       power function (internal)
 * @param       m: base
 * @param       n: exponent
 * @retval      m raised to n
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

