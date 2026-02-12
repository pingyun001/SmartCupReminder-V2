#include "rgbled_logic.h"
#include "ws2812.h"

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
            brightness = 0;

            break;
        }
        case rgbled_mode_rainbow:
        {
            uint16_t now_angle = rgbled.angle;
            rgbled_rainbow_calculate(now_angle, &rgbled.led_unit[0]);
            rgbled_rainbow_calculate((now_angle + 10) % 360, &rgbled.led_unit[1]);
            rgbled_rainbow_calculate((now_angle + 20) % 360, &rgbled.led_unit[2]);
            rgbled_rainbow_calculate((now_angle + 30) % 360, &rgbled.led_unit[3]);

            break;
        }
        case rgbled_mode_candle:
        {
            uint16_t now_angle = rgbled.angle;
            for(uint8_t i = 0; i < 4; i++)
            {
                rgbled_candle_calculate(now_angle, &rgbled.led_unit[i]);
            }
            
            break;
        }
        case rgbled_mode_starnight:
        {
            uint16_t now_angle = rgbled.angle;
            rgbled_rainbow_calculate((now_angle + 0) % 360, &rgbled.led_unit[0]);
            rgbled_rainbow_calculate((now_angle + 90) % 360, &rgbled.led_unit[1]);
            rgbled_rainbow_calculate((now_angle + 180) % 360, &rgbled.led_unit[2]);
            rgbled_rainbow_calculate((now_angle + 270) % 360, &rgbled.led_unit[3]);

            break;
        }
        case rgbled_mode_white:
        {
            uint16_t now_angle = rgbled.angle;
            for(uint8_t i = 0; i < 4; i++)
            {
                rgbled_white_calculate(now_angle, &rgbled.led_unit[i]);
            }
            
            break;
        }
    }

    /* angle update */
    rgbled.angle = (rgbled.angle + 1) % 360;

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
    WS2812_Sync();
}

/*******************************************************************************
 * Function Name: rgbled_rainbow_calculate
 * Description  : Calculate RGB values for a given hue angle (0-359 degrees)
 *                using integer-only HSV to RGB conversion
 * Input        : angle - Hue angle (0-359 degrees)
 * Output       : light_unit - Pointer to RGB structure to store result
 * Return       : None
 * Note         : This is a pure integer implementation suitable for embedded systems
 *******************************************************************************/
static void rgbled_rainbow_calculate(uint16_t angle, rgbled_unit_t *light_unit)
{
    uint8_t h;
    uint8_t region, remainder;
    uint8_t p, q, t;
    
    /* Check for null pointer to prevent crashes */
    if (light_unit == NULL) {
        return;
    }
    
    /* Normalize angle to 0-359 range */
    h = angle % 360;
    
    /* Divide hue into 6 regions (60 degrees each) for piecewise linear conversion */
    region = h / 60;
    
    /* Calculate remainder scaled to 0-255 for interpolation within region */
    remainder = (h % 60) * 255 / 60;
    
    /* Define intermediate values for HSV to RGB conversion */
    p = 0;        /* Minimum value in this conversion model */
    q = 255 - remainder;  /* Decreasing component */
    t = remainder;        /* Increasing component */
    
    /* Calculate RGB values based on hue region using piecewise linear function */
    switch (region) {
        case 0:  /* 0-59 degrees: Red to Yellow transition */
            light_unit->r = 255;
            light_unit->g = t;
            light_unit->b = p;
            break;
            
        case 1:  /* 60-119 degrees: Yellow to Green transition */
            light_unit->r = q;
            light_unit->g = 255;
            light_unit->b = p;
            break;
            
        case 2:  /* 120-179 degrees: Green to Cyan transition */
            light_unit->r = p;
            light_unit->g = 255;
            light_unit->b = t;
            break;
            
        case 3:  /* 180-239 degrees: Cyan to Blue transition */
            light_unit->r = p;
            light_unit->g = q;
            light_unit->b = 255;
            break;
            
        case 4:  /* 240-299 degrees: Blue to Magenta transition */
            light_unit->r = t;
            light_unit->g = p;
            light_unit->b = 255;
            break;
            
        case 5:  /* 300-359 degrees: Magenta to Red transition */
        default:
            light_unit->r = 255;
            light_unit->g = p;
            light_unit->b = q;
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

#include <stdint.h>

/* 预定义星星颜色（柔和、自然的星星颜色） */
static const rgbled_unit_t STAR_COLORS[] = {
    {255, 250, 240},  /* 温暖的白色 */
    {255, 245, 230},  /* 淡黄色 */
    {240, 250, 255},  /* 淡蓝色 */
    {245, 240, 255},  /* 淡紫色 */
    {255, 240, 245},  /* 淡粉色 */
    {250, 255, 240}   /* 淡绿色 */
};
#define STAR_COLOR_COUNT 6
#define STAR_COLOR_CHANCE 3  /* 1/3的概率改变颜色 */

/*******************************************************************************
 * Function Name: rgbled_starnight_calculate
 * Description  : Simulate starry night effect with independent LED flickering
 *                Each LED randomly lights up with different colors at random intervals
 * Input        : angle - Used as seed for random number generation and timing
 * Output       : light_unit - Pointer to RGB structure to store result
 * Return       : None
 * Note         : Each LED operates independently, simulating 4 separate stars
 *******************************************************************************/
static void rgbled_starnight_calculate(uint16_t angle, rgbled_unit_t *light_unit)
{
    static uint8_t led_states[4] = {0, 0, 0, 0};       /* LED当前亮度 */
    static uint8_t target_brightness[4] = {0, 0, 0, 0}; /* LED目标亮度 */
    static uint8_t color_indices[4] = {0, 0, 0, 0};    /* 当前颜色索引 */
    static uint8_t flicker_counter[4] = {0, 0, 0, 0};  /* 闪烁计数器 */
    
    uint8_t led_index;
    uint8_t brightness;
    
    /* Check for null pointer */
    if (light_unit == NULL) {
        return;
    }
    
    /* Determine LED index based on angle (each LED gets unique angle offset) */
    led_index = (angle / 90) % 4;
    
    /* Use angle as seed for pseudo-random number generation */
    uint32_t random_seed = angle * 1103515245UL + 12345;
    
    /* Generate brightness value (0-255) for randomness */
    uint8_t random_brightness = (random_seed >> 8) & 0xFF;
    
    /* Generate random chance value for state changes */
    uint8_t random_chance = (random_seed >> 16) & 0xFF;
    
    /* Generate time-based value for timing */
    uint8_t time_value = (random_seed >> 24) & 0xFF;
    
    /* Star flicker logic */
    if (flicker_counter[led_index] > 0) {
        /* Currently flickering - decrement counter */
        flicker_counter[led_index]--;
        
        /* Smooth transition to target brightness */
        if (led_states[led_index] < target_brightness[led_index]) {
            led_states[led_index] += 4;
            if (led_states[led_index] > target_brightness[led_index]) {
                led_states[led_index] = target_brightness[led_index];
            }
        } else if (led_states[led_index] > target_brightness[led_index]) {
            led_states[led_index] -= 4;
            if (led_states[led_index] < target_brightness[led_index]) {
                led_states[led_index] = target_brightness[led_index];
            }
        }
        
        /* Random chance to change color during flicker */
        if (flicker_counter[led_index] > 0 && random_chance < (255 / STAR_COLOR_CHANCE)) {
            color_indices[led_index] = (random_brightness * STAR_COLOR_COUNT) >> 8;
        }
    } else {
        /* Not flickering - check if it's time to start a new flicker */
        /* Very low probability to start a flicker (simulates rare star twinkles) */
        if (random_chance < 8) {  /* About 3% chance per call */
            /* Start a new flicker */
            flicker_counter[led_index] = 20 + (time_value & 0x1F);  /* 20-51 frames */
            target_brightness[led_index] = 80 + (random_brightness >> 1);  /* 80-207 brightness */
            
            /* Randomly select a new star color */
            color_indices[led_index] = (random_brightness * STAR_COLOR_COUNT) >> 8;
        } else {
            /* Slowly fade out if currently lit */
            if (led_states[led_index] > 0) {
                led_states[led_index]--;
            }
        }
    }
    
    /* Get current brightness */
    brightness = led_states[led_index];
    
    /* If brightness is 0, set all colors to 0 */
    if (brightness == 0) {
        light_unit->r = 0;
        light_unit->g = 0;
        light_unit->b = 0;
        return;
    }
    
    /* Apply brightness to selected star color */
    uint8_t color_idx = color_indices[led_index];
    if (color_idx >= STAR_COLOR_COUNT) {
        color_idx = 0;
    }
    
    /* Scale star color by current brightness */
    light_unit->r = (STAR_COLORS[color_idx].r * brightness) >> 8;
    light_unit->g = (STAR_COLORS[color_idx].g * brightness) >> 8;
    light_unit->b = (STAR_COLORS[color_idx].b * brightness) >> 8;
    
    /* Add subtle brightness variation for twinkling effect */
    if (flicker_counter[led_index] > 0) {
        uint8_t twinkle = (random_seed >> 4) & 0x0F;
        if (twinkle < 3) {
            /* Slightly brighter twinkle */
            if (light_unit->r < 250) light_unit->r += 5;
            if (light_unit->g < 250) light_unit->g += 5;
            if (light_unit->b < 250) light_unit->b += 5;
        } else if (twinkle > 12) {
            /* Slightly dimmer moment */
            if (light_unit->r > 5) light_unit->r -= 5;
            if (light_unit->g > 5) light_unit->g -= 5;
            if (light_unit->b > 5) light_unit->b -= 5;
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
