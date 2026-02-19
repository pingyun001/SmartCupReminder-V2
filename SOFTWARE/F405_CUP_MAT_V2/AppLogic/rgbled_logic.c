#include "rgbled_logic.h"
#include "ws2812.h"

#if 0
#define LED_LOGIC_DEBUG_LOG DEBUG_LOG
#else
#define LED_LOGIC_DEBUG_LOG(...)
#endif

rgbled_t rgbled = {0};

static void rgbled_rainbow_calculate(uint16_t angle, rgbled_unit_t *light_unit);
static void rgbled_candle_calculate(uint16_t angle, rgbled_unit_t *light_unit);
static void rgbled_starnight_calculate(uint16_t angle, rgbled_unit_t *light_unit);
static void rgbled_white_calculate(uint16_t angle, rgbled_unit_t *light_unit);
static void rgbled_mult_brightness(rgbled_unit_t *light_unit, uint8_t brightness);

void rgbled_set_mode(rgbled_mode_e mode)
{
    rgbled.now_mode = mode;
}
void rgbled_set_brightness(uint8_t brightness)
{
    rgbled.glob_brightness = brightness;
}
void rgbled_run_handler(void)
{
	uint8_t brightness = rgbled.glob_brightness;
	uint32_t angle_add_gap_ms = 10;
	
    /* mode change */
    if(rgbled.last_mode!= rgbled.now_mode)
    {
        rgbled.angle = 0;
    }
    rgbled.last_mode = rgbled.now_mode;

    /* calculate color values */
    switch(rgbled.now_mode)
    {
        case rgbled_mode_off:
        {
            LED_LOGIC_DEBUG_LOG("off mode\n");

            brightness = 0;
			
			angle_add_gap_ms = 200;

            break;
        }
        case rgbled_mode_rainbow:
        {
            LED_LOGIC_DEBUG_LOG("rainbow mode, ang:%d\n", rgbled.angle);

            uint16_t now_angle = rgbled.angle;
            rgbled_rainbow_calculate(now_angle, &rgbled.led_unit[0]);
            rgbled_rainbow_calculate((now_angle + 10) % 360, &rgbled.led_unit[1]);
            rgbled_rainbow_calculate((now_angle + 20) % 360, &rgbled.led_unit[2]);
            rgbled_rainbow_calculate((now_angle + 30) % 360, &rgbled.led_unit[3]);
			
			angle_add_gap_ms = 10;

            break;
        }
        case rgbled_mode_candle:
        {
            LED_LOGIC_DEBUG_LOG("candle mode, ang:%d\n", rgbled.angle);

            uint16_t now_angle = rgbled.angle;
            for(uint8_t i = 0; i < 4; i++)
            {
                rgbled_candle_calculate(now_angle, &rgbled.led_unit[i]);
            }
			
			angle_add_gap_ms = 200;
            
            break;
        }
        case rgbled_mode_starnight:
        {
            LED_LOGIC_DEBUG_LOG("starnight mode, ang:%d\n", rgbled.angle);

            uint16_t now_angle = rgbled.angle;
            rgbled_starnight_calculate((now_angle + 0) % 360, &rgbled.led_unit[0]);
            rgbled_starnight_calculate((now_angle + 90) % 360, &rgbled.led_unit[1]);
            rgbled_starnight_calculate((now_angle + 180) % 360, &rgbled.led_unit[2]);
            rgbled_starnight_calculate((now_angle + 270) % 360, &rgbled.led_unit[3]);
			
			angle_add_gap_ms = 20;

            break;
        }
        case rgbled_mode_white:
        {
            LED_LOGIC_DEBUG_LOG("white mode, ang:%d\n", rgbled.angle);

            uint16_t now_angle = rgbled.angle;
            for(uint8_t i = 0; i < 4; i++)
            {
                rgbled_white_calculate(now_angle, &rgbled.led_unit[i]);
            }
			
			angle_add_gap_ms = 200;
            
            break;
        }
    }

    
	/* angle update with freq limit */
	static uint32_t last_angle_update_time = 0;
	if(HAL_GetTick() - last_angle_update_time > angle_add_gap_ms)
	{
		last_angle_update_time = HAL_GetTick();
		
		/* angle update */
		rgbled.angle = (rgbled.angle + 1) % 360;
	}
		

    /* calculate brightness */
    for(uint8_t i = 0; i < 4; i++)
    {
        rgbled_mult_brightness(&rgbled.led_unit[i], brightness);
    }

    /* sync LEDs to hardware */
    for(uint8_t i = 0; i < 4; i++)
    {
        WS2812_SetRGB(i, rgbled.led_unit[i].r, rgbled.led_unit[i].g, rgbled.led_unit[i].b);
    }
}

/*******************************************************************************
 * Function Name: rgbled_rainbow_calculate
 * Description  : Calculate RGB values for a given hue angle (0-359 degrees)
 *                using direct piecewise linear interpolation
 * Input        : angle - Hue angle (0-359 degrees)
 * Output       : light_unit - Pointer to RGB structure to store result
 * Return       : None
 *******************************************************************************/
static void rgbled_rainbow_calculate(uint16_t angle, rgbled_unit_t *light_unit)
{
    uint16_t h;
    uint8_t region;
    uint16_t remainder;
    uint8_t t;
    
    if (light_unit == NULL) {
        return;
    }
    
    /* 角度归一化到0-359 */
    h = angle % 360;
    
    /* 分成6个60度区域 */
    region = h / 60;
    
    /* 区域内的余数 (0-59) */
    remainder = h % 60;
    
    /* 计算插值因子t (0-255)，直接在0-59和0-255之间进行线性映射 */
    t = (uint8_t)((remainder * 255) / 59);  /* 使用59而不是60，确保边界值正确 */
    
    /* 根据色相区域计算RGB值 */
    switch (region) {
        case 0:  /* 0-59度: 红 -> 黄 (红固定255，绿从0到255，蓝固定0) */
            light_unit->r = 255;
            light_unit->g = t;
            light_unit->b = 0;
            break;
            
        case 1:  /* 60-119度: 黄 -> 绿 (红从255到0，绿固定255，蓝固定0) */
            light_unit->r = 255 - t;
            light_unit->g = 255;
            light_unit->b = 0;
            break;
            
        case 2:  /* 120-179度: 绿 -> 青 (红固定0，绿固定255，蓝从0到255) */
            light_unit->r = 0;
            light_unit->g = 255;
            light_unit->b = t;
            break;
            
        case 3:  /* 180-239度: 青 -> 蓝 (红固定0，绿从255到0，蓝固定255) */
            light_unit->r = 0;
            light_unit->g = 255 - t;
            light_unit->b = 255;
            break;
            
        case 4:  /* 240-299度: 蓝 -> 品红 (红从0到255，绿固定0，蓝固定255) */
            light_unit->r = t;
            light_unit->g = 0;
            light_unit->b = 255;
            break;
            
        case 5:  /* 300-359度: 品红 -> 红 (红固定255，绿固定0，蓝从255到0) */
        default:
            light_unit->r = 255;
            light_unit->g = 0;
            light_unit->b = 255 - t;
            break;
    }
}


/*******************************************************************************
 * Function Name: rgbled_candle_calculate
 * Description  : Simulate candle flicker effect for multiple LEDs in sync
 *                Uses angle as seed for consistent flicker pattern across all LEDs
 *                Orange-yellow color with random brightness flicker
 * Input        : angle - Used as seed for pseudo-random number generation (0-359)
 * Output       : light_unit - Pointer to RGB structure to store result
 * Return       : None
 * Note         : All LEDs will have identical flicker pattern, simulating a single candle
 *******************************************************************************/
static void rgbled_candle_calculate(uint16_t angle, rgbled_unit_t *light_unit)
{
    uint32_t random_val;
    uint8_t flicker_factor;
    uint8_t red, green, blue;
    
    /* Check for null pointer to prevent crashes */
    if (light_unit == NULL) {
        return;
    }
    
    /* Use angle as pseudo-random seed to ensure consistent pattern across all LEDs */
    /* Simple pseudo-random number generator using linear congruential method */
    random_val = (angle * 1103515245UL + 12345) & 0x7FFFFFFF;
    
    /* Extract flicker intensity from random value (0-31 range) */
    flicker_factor = random_val & 0x1F;
    
    /* Calculate base brightness with flicker effect */
    uint8_t brightness = 180;  /* Base candle brightness */
    
    /* Apply random flicker to brightness */
    if (flicker_factor > 25) {
        /* Bright flicker moments (about 20% of the time) */
        brightness += 40;
    } else if (flicker_factor > 20) {
        /* Medium flicker moments (about 15% of the time) */
        brightness += 20;
    } else if (flicker_factor < 5) {
        /* Dim flicker moments (about 15% of the time) */
        brightness -= 30;
    }
    
    /* Clamp brightness to valid range (100-220) for candle-like appearance */
    if (brightness < 100) brightness = 100;
    if (brightness > 220) brightness = 220;
    
    /* Calculate candle color: orange-yellow (warm flame) */
    /* Red dominates, green is about 70% of red, blue is minimal */
    red   = brightness;
    green = (brightness * 180) >> 8;  /* Equivalent to brightness * 0.7 */
    blue  = (brightness * 25)  >> 8;  /* Equivalent to brightness * 0.1 */
    
    /* Add subtle color variations for more natural flame effect */
    /* Use different part of random value for color variation */
    uint8_t color_vary = (random_val >> 8) & 0x0F;
    
    if (color_vary < 2) {
        /* More yellow variation (increase green slightly) */
        if (green < 200) green += 10;
    } else if (color_vary > 12) {
        /* More orange variation (increase red, decrease green slightly) */
        if (red < 240) red += 5;
        if (green > 100) green -= 5;
    } else if (color_vary > 10 && blue < 40) {
        /* Occasional blue tint variation */
        blue += 5;
    }
    
    /* Apply final values to output structure */
    light_unit->r = red;
    light_unit->g = green;
    light_unit->b = blue;
}


/*******************************************************************************
 * Function Name: rgbled_candle_calculate_enhanced
 * Description  : Enhanced candle flicker with angle-controlled frequency variation
 *                Uses angle as time parameter for deterministic but flicker-like pattern
 * Input        : angle - Time parameter (0-359), used to control flicker pattern
 * Output       : light_unit - Pointer to RGB structure to store result
 * Return       : None
 *******************************************************************************/
static void rgbled_candle_calculate_enhanced(uint16_t angle, rgbled_unit_t *light_unit)
{
    uint8_t brightness;
    uint8_t red, green, blue;
    
    /* Check for null pointer */
    if (light_unit == NULL) {
        return;
    }
    
    /* Base brightness calculation with sine wave for natural flicker pattern */
    /* Using sine-like function of angle to create pseudo-random brightness pattern */
    uint8_t sin_wave1 = ((angle * 3) % 360) < 180 ? 
                       ((angle * 3) % 360) * 255 / 180 : 
                       (360 - (angle * 3) % 360) * 255 / 180;
    
    uint8_t sin_wave2 = ((angle * 7) % 360) < 180 ? 
                       ((angle * 7) % 360) * 255 / 180 : 
                       (360 - (angle * 7) % 360) * 255 / 180;
    
    /* Combine multiple sine waves for more complex flicker pattern */
    brightness = 150 + (sin_wave1 >> 2) + (sin_wave2 >> 3);
    
    /* Add occasional "burst" flicker using angle-based condition */
    if ((angle % 37) < 2) {  /* About 5% chance for bright burst */
        brightness += 50;
    } else if ((angle % 53) < 3) {  /* About 5% chance for dim moment */
        brightness -= 40;
    }
    
    /* Clamp brightness to candle-like range */
    if (brightness < 120) brightness = 120;
    if (brightness > 220) brightness = 220;
    
    /* Calculate warm orange-yellow candle color */
    red   = brightness;
    green = (brightness * 7) / 10;  /* Green = 70% of red */
    blue  = (brightness * 1) / 10;  /* Blue = 10% of red */
    
    /* Add small color temperature variation based on angle */
    uint8_t temp_vary = angle % 60;
    
    if (temp_vary < 10) {
        /* Warmer (more yellow) */
        green += 15;
    } else if (temp_vary > 50) {
        /* Slightly cooler (less yellow) */
        if (green > 20) green -= 10;
    }
    
    /* Clamp color values */
    if (red > 255) red = 255;
    if (green > 240) green = 240;
    if (blue > 60) blue = 60;
    
    /* Apply final values */
    light_unit->r = red;
    light_unit->g = green;
    light_unit->b = blue;
}

/*******************************************************************************
 * Function Name: rgbled_candle_calculate_smooth
 * Description  : Smooth candle flicker with gradual brightness transitions
 *                Maintains state between calls for smooth animation
 * Input        : angle - Time parameter for flicker pattern
 * Output       : light_unit - Pointer to RGB structure to store result
 * Return       : None
 *******************************************************************************/
static void rgbled_candle_calculate_smooth(uint16_t angle, rgbled_unit_t *light_unit)
{
    static uint8_t last_brightness = 180;
    uint8_t target_brightness;
    uint8_t brightness;
    
    /* Check for null pointer */
    if (light_unit == NULL) {
        return;
    }
    
    /* Calculate target brightness using angle-based pseudo-random pattern */
    uint8_t rand_factor = (angle * 1103515245UL + 12345) & 0x3F;  /* 0-63 */
    
    /* Determine target brightness based on random factor */
    if (rand_factor < 5) {
        target_brightness = 120;  /* Dim flicker (8% chance) */
    } else if (rand_factor < 10) {
        target_brightness = 160;  /* Medium-dim (8% chance) */
    } else if (rand_factor > 55) {
        target_brightness = 220;  /* Bright flicker (12% chance) */
    } else if (rand_factor > 50) {
        target_brightness = 200;  /* Medium-bright (8% chance) */
    } else {
        target_brightness = 180;  /* Normal brightness (64% chance) */
    }
    
    /* Smooth transition toward target brightness (simple low-pass filter) */
    brightness = (last_brightness * 3 + target_brightness) >> 2;  /* 75% old, 25% new */
    
    /* Update stored brightness for next call */
    last_brightness = brightness;
    
    /* Calculate candle color */
    uint8_t red = brightness;
    uint8_t green = (brightness * 180) >> 8;  /* ~70% */
    uint8_t blue = (brightness * 25) >> 8;     /* ~10% */
    
    /* Apply slight color variation for realism */
    uint8_t color_vary = (angle >> 2) & 0x07;  /* Use angle for color variation */
    
    if (color_vary == 0) {
        if (green < 220) green += 8;
    } else if (color_vary == 1) {
        if (red < 240) red += 5;
    }
    
    /* Clamp values */
    if (red > 255) red = 255;
    if (green > 240) green = 240;
    if (blue > 50) blue = 50;
    
    /* Apply final values */
    light_unit->r = red;
    light_unit->g = green;
    light_unit->b = blue;
}

/* 星星颜色（明显的彩色） */
static const rgbled_unit_t STAR_COLORS[] = {
    {255, 150, 100},   /* 橙色 */
    {255, 200, 50},    /* 黄色 */
    {100, 200, 255},   /* 天蓝色 */
    {200, 100, 255},   /* 紫色 */
    {255, 100, 200},   /* 粉红色 */
    {100, 255, 150},   /* 青绿色 */
    {255, 50, 50},     /* 红色 */
    {50, 255, 50},     /* 绿色 */
    {50, 100, 255},    /* 蓝色 */
    {255, 150, 50},    /* 橙黄色 */
    {180, 80, 220},    /* 蓝紫色 */
    {255, 100, 100},   /* 珊瑚红 */
};
#define STAR_COLOR_COUNT 12

/*******************************************************************************
 * Function Name: rgbled_starnight_calculate
 * Description  : Simulate starry night effect with random colored flickering
 *                Each LED independently flickers with random colors and timing
 * Input        : angle - Current angle (0-359, used for timing)
 * Output       : light_unit - Pointer to RGB structure to store result
 * Return       : None
 *******************************************************************************/
static void rgbled_starnight_calculate(uint16_t angle, rgbled_unit_t *light_unit)
{
    static uint8_t brightness[4] = {0, 0, 0, 0};           /* 当前亮度 */
    static uint8_t target_brightness[4] = {0, 0, 0, 0};    /* 目标亮度 */
    static uint8_t color_indices[4] = {0, 0, 0, 0};        /* 颜色索引 */
    static uint8_t flicker_timers[4] = {0, 0, 0, 0};       /* 闪烁计时器 */
    static uint8_t star_active[4] = {0, 0, 0, 0};          /* 星星是否活跃 */
    static uint8_t fade_direction[4] = {0, 0, 0, 0};       /* 淡入淡出方向: 0=淡入, 1=淡出 */
    
    /* 使用角度作为伪随机种子 */
    uint32_t seed = (uint32_t)angle * 1103515245UL + 12345;
    uint8_t random_val = (seed >> 8) & 0xFF;
    uint8_t led_index = (angle / 90) % 4;
    
    if (light_unit == NULL) {
        return;
    }
    
    /* 星星闪烁逻辑 */
    if (flicker_timers[led_index] > 0) {
        /* 计时器递减 */
        flicker_timers[led_index]--;
        
        /* 平滑过渡到目标亮度 */
        if (!fade_direction[led_index]) {
            /* 淡入: 快速增加亮度 */
            if (brightness[led_index] < target_brightness[led_index]) {
                brightness[led_index] += 15;  /* 加快淡入速度 */
                if (brightness[led_index] > target_brightness[led_index]) {
                    brightness[led_index] = target_brightness[led_index];
                }
            } else {
                /* 达到目标亮度后，几乎立即开始淡出 */
                if (flicker_timers[led_index] < 5) {  /* 大幅减少保持时间 */
                    fade_direction[led_index] = 1;  /* 开始淡出 */
                }
            }
        } else {
            /* 淡出: 快速减小亮度 */
            if (brightness[led_index] > 0) {
                brightness[led_index] -= 12;  /* 加快淡出速度 */
                if (brightness[led_index] > 200) {  /* 防止下溢 */
                    brightness[led_index] = 0;
                }
            }
            
            /* 完全淡出后重置状态 */
            if (brightness[led_index] == 0) {
                star_active[led_index] = 0;
                flicker_timers[led_index] = 0;
                fade_direction[led_index] = 0;
            }
        }
    } else {
        /* 不在闪烁状态，检查是否应该开始新的闪烁 */
        
        /* 低概率开始新的闪烁（大约4%的概率） */
        if (random_val < 10) {  /* 10/256 ≈ 4% */
            /* 开始新的闪烁 */
            star_active[led_index] = 1;
            
            /* 大幅减少闪烁时长（8-20个角度单位） */
            uint8_t time_seed = (seed >> 16) & 0xFF;
            flicker_timers[led_index] = 8 + (time_seed % 13);  /* 8-20帧 */
            
            /* 目标亮度（120-255） */
            uint8_t brightness_seed = (seed >> 8) & 0xFF;
            target_brightness[led_index] = 120 + (brightness_seed % 136);  /* 120-255 */
            
            /* 随机选择星星颜色 */
            uint8_t color_seed = (seed >> 24) & 0xFF;
            color_indices[led_index] = (color_seed * color_seed) % STAR_COLOR_COUNT;
            
            /* 重置淡入淡出方向 */
            fade_direction[led_index] = 0;
            
            /* 如果当前有亮度，先快速淡出 */
            if (brightness[led_index] > 0) {
                brightness[led_index] = 0;
            }
        } else if (star_active[led_index]) {
            /* 如果之前活跃但现在没有计时器，快速淡出 */
            if (brightness[led_index] > 0) {
                brightness[led_index] -= 6;  /* 加快淡出速度 */
                if (brightness[led_index] > 200) {  /* 防止下溢 */
                    brightness[led_index] = 0;
                    star_active[led_index] = 0;
                }
            } else {
                star_active[led_index] = 0;
            }
        }
    }
    
    /* 输出颜色 */
    if (brightness[led_index] == 0) {
        light_unit->r = 0;
        light_unit->g = 0;
        light_unit->b = 0;
        return;
    }
    
    /* 应用亮度到选定的星星颜色 */
    uint8_t color_idx = color_indices[led_index];
    if (color_idx >= STAR_COLOR_COUNT) {
        color_idx = 0;
    }
    
    /* 使用整数运算进行亮度缩放 */
    uint32_t scaled_r = (uint32_t)STAR_COLORS[color_idx].r * brightness[led_index];
    uint32_t scaled_g = (uint32_t)STAR_COLORS[color_idx].g * brightness[led_index];
    uint32_t scaled_b = (uint32_t)STAR_COLORS[color_idx].b * brightness[led_index];
    
    light_unit->r = (uint8_t)(scaled_r / 255);
    light_unit->g = (uint8_t)(scaled_g / 255);
    light_unit->b = (uint8_t)(scaled_b / 255);
    
    /* 添加微妙的随机闪烁效果 - 降低频率 */
    if (flicker_timers[led_index] > 0 && (random_val & 0x0F) == 0) {  /* 进一步降低频率 */
        /* 偶尔增加亮度，模拟星星闪烁 */
        uint8_t twinkle = (seed >> 12) & 0x0F;
        if (twinkle < 2) {  /* 进一步减少增亮概率 */
            /* 轻微增亮 */
            if (light_unit->r < 250 && light_unit->r + 8 <= 255) light_unit->r += 8;
            if (light_unit->g < 250 && light_unit->g + 8 <= 255) light_unit->g += 8;
            if (light_unit->b < 250 && light_unit->b + 8 <= 255) light_unit->b += 8;
        } else if (twinkle > 13) {  /* 增加变暗概率 */
            /* 轻微变暗 */
            if (light_unit->r > 8) light_unit->r -= 8;
            if (light_unit->g > 8) light_unit->g -= 8;
            if (light_unit->b > 8) light_unit->b -= 8;
        }
    }
}

/*******************************************************************************
 * Function Name: rgbled_starnight_calculate_simple
 * Description  : Simplified starry night effect without static variables
 *                Uses deterministic algorithm based on angle
 * Input        : angle - Used for deterministic random generation
 * Output       : light_unit - Pointer to RGB structure to store result
 * Return       : None
 *******************************************************************************/
static void rgbled_starnight_calculate_simple(uint16_t angle, rgbled_unit_t *light_unit)
{
    uint8_t led_index;
    uint8_t brightness = 0;
    uint8_t color_idx = 0;
    
    /* Check for null pointer */
    if (light_unit == NULL) {
        return;
    }
    
    /* Determine LED index (0-3) from angle */
    led_index = (angle / 90) % 4;
    
    /* Generate pseudo-random numbers based on angle and LED index */
    uint32_t rand1 = (angle + led_index * 1000) * 1103515245UL + 12345;
    uint32_t rand2 = (angle + led_index * 2000) * 1103515245UL + 54321;
    
    /* Extract random values for different purposes */
    uint8_t chance = (rand1 >> 16) & 0xFF;      /* 0-255 for probability */
    uint8_t intensity = (rand1 >> 8) & 0xFF;    /* 0-255 for brightness */
    uint8_t timing = (rand2 >> 16) & 0xFF;      /* 0-255 for timing */
    
    /* Calculate if star should be visible at this angle */
    uint16_t time_cycle = angle + led_index * 90;
    uint8_t cycle_pos = time_cycle % 256;
    
    /* Star appears based on timing cycle (rare occurrence) */
    if (chance < 6) {  /* About 2.3% chance */
        /* Star is visible - calculate brightness with sine-like pattern */
        uint8_t sin_val = (cycle_pos < 128) ? 
                         (cycle_pos * 255 / 128) : 
                         ((255 - cycle_pos) * 255 / 128);
        
        brightness = sin_val >> 1;  /* 0-127 range */
        
        /* Select color based on random value */
        color_idx = (intensity * STAR_COLOR_COUNT) >> 8;
    } else if (chance < 10) {  /* About 1.5% chance for dim star */
        /* Dim star appearance */
        brightness = 20 + (intensity >> 3);  /* 20-51 range */
        color_idx = (timing * STAR_COLOR_COUNT) >> 8;
    }
    
    /* Apply brightness to selected color (if any) */
    if (brightness > 0 && color_idx < STAR_COLOR_COUNT) {
        light_unit->r = (STAR_COLORS[color_idx].r * brightness) >> 8;
        light_unit->g = (STAR_COLORS[color_idx].g * brightness) >> 8;
        light_unit->b = (STAR_COLORS[color_idx].b * brightness) >> 8;
    } else {
        /* Star is not visible (dark) */
        light_unit->r = 0;
        light_unit->g = 0;
        light_unit->b = 0;
    }
}

/*******************************************************************************
 * Function Name: rgbled_starnight_calculate_advanced
 * Description  : Advanced starry night with complex twinkling patterns
 *                Each star has unique twinkling frequency and brightness pattern
 * Input        : angle - Used for timing and random seed
 * Output       : light_unit - Pointer to RGB structure to store result
 * Return       : None
 *******************************************************************************/
static void rgbled_starnight_calculate_advanced(uint16_t angle, rgbled_unit_t *light_unit)
{
    static uint8_t star_phase[4] = {0, 45, 90, 135};  /* Phase offset for each star */
    static uint8_t star_freq[4] = {3, 5, 7, 11};      /* Twinkling frequency for each star */
    
    uint8_t led_index;
    uint8_t brightness = 0;
    uint8_t color_idx = 0;
    
    /* Check for null pointer */
    if (light_unit == NULL) {
        return;
    }
    
    /* Determine LED index */
    led_index = (angle / 90) % 4;
    
    /* Calculate star-specific phase */
    uint16_t star_angle = (angle + star_phase[led_index]) * star_freq[led_index];
    
    /* Generate random-like pattern using trigonometric functions */
    uint8_t pattern1 = (star_angle % 180) < 90 ? 
                      (star_angle % 180) * 255 / 90 : 
                      (180 - (star_angle % 180)) * 255 / 90;
    
    uint8_t pattern2 = ((star_angle + 60) % 180) < 90 ? 
                      ((star_angle + 60) % 180) * 255 / 90 : 
                      (180 - ((star_angle + 60) % 180)) * 255 / 90;
    
    uint8_t pattern3 = ((star_angle + 120) % 180) < 90 ? 
                      ((star_angle + 120) % 180) * 255 / 90 : 
                      (180 - ((star_angle + 120) % 180)) * 255 / 90;
    
    /* Combine patterns for complex twinkling */
    brightness = (pattern1 + pattern2 + pattern3) / 3;
    
    /* Apply threshold - star is only visible when brightness exceeds threshold */
    uint8_t threshold = 100 + ((star_angle >> 4) & 0x3F);  /* 100-155 threshold */
    
    if (brightness > threshold) {
        /* Star is visible - scale brightness */
        brightness = (brightness - threshold) * 255 / (255 - threshold);
        
        /* Add occasional bright twinkles */
        if ((star_angle % 37) < 2) {  /* About 5% chance for bright twinkle */
            brightness = brightness + 50 > 255 ? 255 : brightness + 50;
        }
        
        /* Select color based on star_angle (slowly changing) */
        color_idx = ((star_angle >> 3) * STAR_COLOR_COUNT) >> 8;
        if (color_idx >= STAR_COLOR_COUNT) {
            color_idx = color_idx % STAR_COLOR_COUNT;
        }
        
        /* Apply brightness to color */
        light_unit->r = (STAR_COLORS[color_idx].r * brightness) >> 8;
        light_unit->g = (STAR_COLORS[color_idx].g * brightness) >> 8;
        light_unit->b = (STAR_COLORS[color_idx].b * brightness) >> 8;
        
        /* Add subtle color variation */
        uint8_t color_vary = (star_angle >> 2) & 0x07;
        if (color_vary == 0) {
            /* Slightly more blue */
            if (light_unit->b < 250) light_unit->b += 5;
        } else if (color_vary == 1) {
            /* Slightly more red */
            if (light_unit->r < 250) light_unit->r += 5;
        }
    } else {
        /* Star is not visible or very faint */
        if (brightness > (threshold - 20)) {
            /* Very faint star */
            uint8_t faint_brightness = (brightness - (threshold - 20)) * 10;
            light_unit->r = faint_brightness >> 2;
            light_unit->g = faint_brightness >> 2;
            light_unit->b = faint_brightness;
        } else {
            /* Completely dark */
            light_unit->r = 0;
            light_unit->g = 0;
            light_unit->b = 0;
        }
    }
}

/*******************************************************************************
 * Function Name: rgbled_white_calculate
 * Description  : Returns pure white color for LED
 *                Angle parameter is ignored, kept for API consistency
 * Input        : angle - Not used, kept for function signature consistency
 * Output       : light_unit - Pointer to RGB structure to store white color
 * Return       : None
 * Note         : Always returns full white (255,255,255) regardless of angle
 *******************************************************************************/
static void rgbled_white_calculate(uint16_t angle, rgbled_unit_t *light_unit)
{
    /* Check for null pointer */
    if (light_unit == NULL) {
        return;
    }
    
    /* Suppress unused parameter warning */
    (void)angle;
    
    /* Set full white color */
    light_unit->r = 255;
    light_unit->g = 255;
    light_unit->b = 255;
}

/*******************************************************************************
 * Function Name: rgbled_mult_brightness
 * Description  : Fast brightness adjustment with slightly lower precision
 *                Uses right shift instead of division for maximum speed
 * Input        : light_unit - Pointer to RGB structure (input/output)
 *                brightness - Brightness level (0-255, 0=off, 255=full)
 * Return       : None
 * Note         : Uses brightness as 8-bit scaling factor, right shift by 8
 *                Equivalent to divide by 256, not 255 (slight error)
 *******************************************************************************/
static void rgbled_mult_brightness(rgbled_unit_t *light_unit, uint8_t brightness)
{
    uint16_t temp_r, temp_g, temp_b;
    
    /* Check for null pointer */
    if (light_unit == NULL) {
        return;
    }
    
    /* Fast path: full brightness, no calculation needed */
    if (brightness == 255) {
        return;
    }
    
    /* Fast path: zero brightness, set all to zero */
    if (brightness == 0) {
        light_unit->r = 0;
        light_unit->g = 0;
        light_unit->b = 0;
        return;
    }
    
    /* Fast multiplication and right shift (divide by 256) */
    /* This is faster but slightly darker than divide by 255 */
    light_unit->r = (light_unit->r * brightness) >> 8;
    light_unit->g = (light_unit->g * brightness) >> 8;
    light_unit->b = (light_unit->b * brightness) >> 8;
}
