#include "Lime_WeatherFace.h"
#include "Lime_App_Base.h"
#include "Lime_App_Hal.h"

static lv_obj_t *bj_obj = NULL;

static lv_obj_t *now_city_name_label = NULL;
static lv_obj_t *now_temper_label = NULL;
static lv_obj_t *now_humidity_label = NULL;
static lv_obj_t *gap_line_obj = NULL;

static lv_obj_t *today_weather_widget = NULL;
static lv_obj_t *tomorrow_weather_widget = NULL;
static lv_obj_t *day_after_tomorrow_weather_widget = NULL;


LV_IMG_DECLARE(lime_img_pos);

lv_obj_t *lime_weatherface_create(lv_obj_t* parent)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    lv_font_t * weather_font = lime_weather_make_big_font();
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(weather_font);

    lv_font_t * weather_small_font = lime_weather_make_small_font();
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(weather_small_font);

    bj_obj = lv_obj_create(parent);
    lv_obj_set_size(bj_obj, 428, 142);
    lv_obj_set_style_bg_opa(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(bj_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(bj_obj);

#if 1
    lv_obj_set_style_bg_color(bj_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bj_obj, LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);
#endif

    lv_obj_t* pos_img = lv_img_create(bj_obj);
    lv_img_set_src(pos_img, &lime_img_pos);
    lv_obj_set_size(pos_img, 17, 22);
    lv_obj_set_pos(pos_img, 74, 44);

    lv_obj_t *temper_label = lv_label_create(parent);
    lv_obj_set_pos(temper_label, 14, 93);
    lv_obj_set_size(temper_label, 40, 15);
    lv_label_set_text(temper_label, "温度:");
    lv_obj_set_style_text_font(temper_label, weather_font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(temper_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(temper_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *humidity_label = lv_label_create(parent);
    lv_obj_set_pos(humidity_label, 14, 110);
    lv_obj_set_size(humidity_label, 40, 15);
    lv_label_set_text(humidity_label, "湿度:");
    lv_obj_set_style_text_font(humidity_label, weather_font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(humidity_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(humidity_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    now_city_name_label = lv_label_create(parent);
    lv_obj_set_pos(now_city_name_label, 30, 71);
    lv_obj_set_size(now_city_name_label, 109, 15);
    lv_label_set_text(now_city_name_label, "北京");
    lv_obj_set_style_text_font(now_city_name_label, weather_font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(now_city_name_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(now_city_name_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    now_temper_label = lv_label_create(parent);
    lv_obj_set_pos(now_temper_label, 46, 93);
    lv_obj_set_size(now_temper_label, 74, 15);
    lv_label_set_text(now_temper_label, "25℃");
    lv_obj_set_style_text_font(now_temper_label, weather_small_font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(now_temper_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(now_temper_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    now_humidity_label = lv_label_create(parent);
    lv_obj_set_pos(now_humidity_label, 46, 110);
    lv_obj_set_size(now_humidity_label, 74, 15);
    lv_label_set_text(now_humidity_label, "50\%");
    lv_obj_set_style_text_font(now_humidity_label, weather_small_font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(now_humidity_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(now_humidity_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    gap_line_obj = lv_obj_create(parent);
    lv_obj_set_pos(gap_line_obj, 124, 26);
    lv_obj_set_size(gap_line_obj, 3, 101);
    lv_obj_set_style_bg_color(gap_line_obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(gap_line_obj, LV_OPA_70, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(gap_line_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(gap_line_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(gap_line_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(gap_line_obj, LV_OBJ_FLAG_SCROLLABLE);

    today_weather_widget = lime_weather_widget_create(bj_obj);
    lv_obj_set_pos(today_weather_widget, 146, 23);
    lime_weather_widget_set_date(today_weather_widget, 1, 7);
    lime_weather_widget_set_icon(today_weather_widget, 1);
    lime_weather_widget_set_weather_chinese(today_weather_widget, "晴朗");
    lime_weather_widget_set_temperature_range(today_weather_widget, -25, -10);
    lime_weather_widget_set_humidity(today_weather_widget, 50);

    tomorrow_weather_widget = lime_weather_widget_create(bj_obj);
    lv_obj_set_pos(tomorrow_weather_widget, 230, 23);
    lime_weather_widget_set_date(tomorrow_weather_widget, 1, 8);
    lime_weather_widget_set_icon(tomorrow_weather_widget, 2);
    lime_weather_widget_set_weather_chinese(tomorrow_weather_widget, "小雨");
    lime_weather_widget_set_temperature_range(tomorrow_weather_widget, -10, 3);
    lime_weather_widget_set_humidity(tomorrow_weather_widget, 63);

    day_after_tomorrow_weather_widget = lime_weather_widget_create(bj_obj);
    lv_obj_set_pos(day_after_tomorrow_weather_widget, 314, 23);
    lime_weather_widget_set_date(day_after_tomorrow_weather_widget, 12, 31);
    lime_weather_widget_set_icon(day_after_tomorrow_weather_widget, 8);
    lime_weather_widget_set_weather_chinese(day_after_tomorrow_weather_widget, "多云");
    lime_weather_widget_set_temperature_range(day_after_tomorrow_weather_widget, 12, 27);
    lime_weather_widget_set_humidity(day_after_tomorrow_weather_widget, 12);

    return bj_obj;
}

static lv_font_t *lime_weather_make_big_font(void)
{
    static lv_font_t * weather_font = NULL;
    if(weather_font == NULL)
    {
        weather_font = lv_tiny_ttf_create_file("D:/LimeLvResources/wryhbold.ttf", 14);
        MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(weather_font);
    }

    return weather_font;
}

static lv_font_t *lime_weather_make_small_font(void)
{
    static lv_font_t * weather_font = NULL;
    if(weather_font == NULL)
    {
        weather_font = lv_tiny_ttf_create_file("D:/LimeLvResources/wryhbold.ttf", 12);
        MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(weather_font);
    }

    return weather_font;
}

static lv_obj_t *lime_weather_widget_create(lv_obj_t* parent)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    const uint32_t default_width = 90;

    lv_font_t * weather_font = lime_weather_make_big_font();
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(weather_font);

    lv_font_t * weather_small_font = lime_weather_make_small_font();
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(weather_small_font);

    lv_obj_t* bj_obj = lv_obj_create(parent);
    lv_obj_set_size(bj_obj, default_width, 104);
    lv_obj_set_style_bg_opa(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bj_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(bj_obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *date_label = lv_label_create(bj_obj);
    lv_obj_set_size(date_label, default_width, 15);
    lv_obj_align(date_label, LV_ALIGN_TOP_MID, 0, 4);
    lv_label_set_text(date_label, "1/7");
    lv_obj_set_style_text_font(date_label, weather_font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(date_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(date_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *weather_logo_img = lv_img_create(bj_obj);
    lv_img_set_src(weather_logo_img, "D:/LimeLvResources/weather_white/7.bin");
    lv_obj_set_size(weather_logo_img, 32, 32);
    lv_obj_align_to(weather_logo_img, date_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t *weather_chinese_label = lv_label_create(bj_obj);
    lv_obj_set_size(weather_chinese_label, default_width, 15);
    lv_obj_align_to(weather_chinese_label, weather_logo_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_label_set_text(weather_chinese_label, "晴朗");
    lv_obj_set_style_text_font(weather_chinese_label, weather_font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(weather_chinese_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(weather_chinese_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *temperature_label = lv_label_create(bj_obj);
    lv_obj_set_size(temperature_label, default_width, 15);
    lv_obj_align_to(temperature_label, weather_chinese_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_label_set_text(temperature_label, "-25℃~-30℃");
    lv_obj_set_style_text_font(temperature_label, weather_small_font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(temperature_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(temperature_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *humidity_label = lv_label_create(bj_obj);
    lv_obj_set_size(humidity_label, default_width, 15);
    lv_obj_align_to(humidity_label, temperature_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_label_set_text(humidity_label, "50\%");
    lv_obj_set_style_text_font(humidity_label, weather_small_font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(humidity_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(humidity_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    return bj_obj;
}
static void lime_weather_widget_set_date(lv_obj_t* weather_widget, uint8_t month, uint8_t day)
{
    MLV_BASE_OBJ_NULL_CHECK(weather_widget);

    /* get all child objects */
    lv_obj_t *date_label = lv_obj_get_child(weather_widget, 0);
    lv_obj_t *weather_logo_img = lv_obj_get_child(weather_widget, 1);
    lv_obj_t *weather_chinese_label = lv_obj_get_child(weather_widget, 2);
    lv_obj_t *temperature_label = lv_obj_get_child(weather_widget, 3);
    lv_obj_t *humidity_label = lv_obj_get_child(weather_widget, 4);

    /* limit */
    month = month > 12 ? 12 : month == 0 ? 1 : month;
    day = day > 31 ? 31 : day == 0 ? 1 : day;

    /* set date label */
    lime_base_set_label_string(date_label, "%d/%d", month, day);
}
static void lime_weather_widget_set_icon(lv_obj_t* weather_widget, uint8_t icon_id)
{
    MLV_BASE_OBJ_NULL_CHECK(weather_widget);

    /* get all child objects */
    lv_obj_t *date_label = lv_obj_get_child(weather_widget, 0);
    lv_obj_t *weather_logo_img = lv_obj_get_child(weather_widget, 1);
    lv_obj_t *weather_chinese_label = lv_obj_get_child(weather_widget, 2);
    lv_obj_t *temperature_label = lv_obj_get_child(weather_widget, 3);
    lv_obj_t *humidity_label = lv_obj_get_child(weather_widget, 4);

    /* calculate icon path and set it to the image object */
    char icon_path[128] = {0};
    sprintf(icon_path, "D:/LimeLvResources/weather_white/%d.bin", icon_id);
    lv_img_set_src(weather_logo_img, icon_path);
}
static void lime_weather_widget_set_weather_chinese(lv_obj_t* weather_widget, const char* weather_chinese)
{
    MLV_BASE_OBJ_NULL_CHECK(weather_widget);
    MLV_BASE_OBJ_NULL_CHECK(weather_chinese);

    /* get all child objects */
    lv_obj_t *date_label = lv_obj_get_child(weather_widget, 0);
    lv_obj_t *weather_logo_img = lv_obj_get_child(weather_widget, 1);
    lv_obj_t *weather_chinese_label = lv_obj_get_child(weather_widget, 2);
    lv_obj_t *temperature_label = lv_obj_get_child(weather_widget, 3);
    lv_obj_t *humidity_label = lv_obj_get_child(weather_widget, 4);

    /* limit */
    if(strlen(weather_chinese) > 20)
    {
        LV_LOG_WARN("Weather chinese string is too long, it will be truncated to 20 characters.");
    }

    /* set weather chinese label */
    lime_base_set_label_string(weather_chinese_label, weather_chinese);
}
static void lime_weather_widget_set_temperature_range(lv_obj_t* weather_widget, int16_t min_temp, int16_t max_temp)
{
    MLV_BASE_OBJ_NULL_CHECK(weather_widget);

    /* get all child objects */
    lv_obj_t *date_label = lv_obj_get_child(weather_widget, 0);
    lv_obj_t *weather_logo_img = lv_obj_get_child(weather_widget, 1);
    lv_obj_t *weather_chinese_label = lv_obj_get_child(weather_widget, 2);
    lv_obj_t *temperature_label = lv_obj_get_child(weather_widget, 3);
    lv_obj_t *humidity_label = lv_obj_get_child(weather_widget, 4);

    /* set temperature range label */
    lime_base_set_label_string(temperature_label, "%d℃~%d℃", min_temp, max_temp);
}
static void lime_weather_widget_set_humidity(lv_obj_t* weather_widget, uint8_t humidity)
{
    MLV_BASE_OBJ_NULL_CHECK(weather_widget);

    /* get all child objects */
    lv_obj_t *date_label = lv_obj_get_child(weather_widget, 0);
    lv_obj_t *weather_logo_img = lv_obj_get_child(weather_widget, 1);
    lv_obj_t *weather_chinese_label = lv_obj_get_child(weather_widget, 2);
    lv_obj_t *temperature_label = lv_obj_get_child(weather_widget, 3);
    lv_obj_t *humidity_label = lv_obj_get_child(weather_widget, 4);

    /* set humidity label */
    lime_base_set_label_string(humidity_label, "%d\%", humidity);
}
