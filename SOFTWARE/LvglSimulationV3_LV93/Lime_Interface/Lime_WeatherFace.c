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

 bool is_soft_start = false;
static void scan_weather_timer_cb(lv_timer_t * timer);

LV_IMG_DECLARE(lime_img_pos);
LV_FONT_DECLARE(lime_font_weather_num);
LV_FONT_DECLARE(lime_font_weather_title);

lv_obj_t *lime_weatherface_create(lv_obj_t* parent)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    lv_font_t * weather_font = lime_weather_make_big_font();
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(weather_font);

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
    lv_obj_set_style_text_font(temper_label, &lime_font_weather_title, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(temper_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(temper_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *humidity_label = lv_label_create(parent);
    lv_obj_set_pos(humidity_label, 14, 110);
    lv_obj_set_size(humidity_label, 40, 15);
    lv_label_set_text(humidity_label, "湿度:");
    lv_obj_set_style_text_font(humidity_label, &lime_font_weather_title, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(humidity_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(humidity_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    now_city_name_label = lv_label_create(parent);
    lv_obj_set_pos(now_city_name_label, 30, 71);
    lv_obj_set_size(now_city_name_label, 109, 15);
    lv_label_set_text(now_city_name_label, "北京");
    lv_obj_set_style_text_font(now_city_name_label, weather_font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(now_city_name_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(now_city_name_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(now_city_name_label, LV_OBJ_FLAG_HIDDEN);

    now_temper_label = lv_label_create(parent);
    lv_obj_set_pos(now_temper_label, 46, 95);
    lv_obj_set_size(now_temper_label, 74, 15);
    lv_label_set_text(now_temper_label, "25℃");
    lv_obj_set_style_text_font(now_temper_label, &lime_font_weather_num, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(now_temper_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(now_temper_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    now_humidity_label = lv_label_create(parent);
    lv_obj_set_pos(now_humidity_label, 46, 112);
    lv_obj_set_size(now_humidity_label, 74, 15);
    lv_label_set_text(now_humidity_label, "50\%");
    lv_obj_set_style_text_font(now_humidity_label, &lime_font_weather_num, LV_PART_MAIN | LV_STATE_DEFAULT);
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
    lv_obj_set_pos(today_weather_widget, 146, 26);
    lv_obj_add_flag(today_weather_widget, LV_OBJ_FLAG_HIDDEN);

    tomorrow_weather_widget = lime_weather_widget_create(bj_obj);
    lv_obj_set_pos(tomorrow_weather_widget, 230, 26);
    lv_obj_add_flag(tomorrow_weather_widget, LV_OBJ_FLAG_HIDDEN);

    day_after_tomorrow_weather_widget = lime_weather_widget_create(bj_obj);
    lv_obj_set_pos(day_after_tomorrow_weather_widget, 314, 26);
    lv_obj_add_flag(day_after_tomorrow_weather_widget, LV_OBJ_FLAG_HIDDEN);

    /* create timer to scan weather data */
    lv_timer_create(scan_weather_timer_cb, 200, NULL);

    /* run immediately to let data update */
    scan_weather_timer_cb(NULL);

    return bj_obj;
}

static void scan_weather_timer_cb(lv_timer_t * timer)
{
    static uint8_t cnt = 0;
    static bool last_soft_start = false;
    if(is_soft_start && (!last_soft_start))
    {
        cnt = 0;
        LV_LOG_USER("soft start");
    }
    /* just fot debug run */
    if((!is_soft_start) && last_soft_start)
    {
        lv_obj_add_flag(now_city_name_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(today_weather_widget, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(tomorrow_weather_widget, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(day_after_tomorrow_weather_widget, LV_OBJ_FLAG_HIDDEN);
        LV_LOG_USER("soft hide");
    }
    last_soft_start = is_soft_start;

    switch(cnt)
    {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            lv_obj_remove_flag(now_city_name_label, LV_OBJ_FLAG_HIDDEN);
            break;
        case 4:
            lv_obj_remove_flag(today_weather_widget, LV_OBJ_FLAG_HIDDEN);
            break;
        case 5:
            lv_obj_remove_flag(tomorrow_weather_widget, LV_OBJ_FLAG_HIDDEN);
            break;
        case 6:
            lv_obj_remove_flag(day_after_tomorrow_weather_widget, LV_OBJ_FLAG_HIDDEN);
            break;
        default:
            cnt = 99;
            // is_soft_start = false;
            break;
    }

    if(is_soft_start)
        cnt++;


    /* get hal data */
    const LimeHal_Info_t *hal_info = LimeHAL_GetInfoPin();
    MLV_BASE_OBJ_NULL_CHECK(hal_info);
    const LimeHal_SenserInfo_t *senser_info = &hal_info->senserInfo;

    /* weather data not valid */
    if( !senser_info->isWeatherDataValid)
    {
        lime_base_set_label_string(now_city_name_label, "未知城市");
        lime_base_set_label_string(now_temper_label, "0℃");
        lime_base_set_label_string(now_humidity_label, "0\%");

        lime_weather_widget_set_date(today_weather_widget, 0, 0);
        lime_weather_widget_set_icon(today_weather_widget, 99);
        lime_weather_widget_set_weather_chinese(today_weather_widget, "无数据");
        lime_weather_widget_set_temperature_range(today_weather_widget, 0, 0);
        lime_weather_widget_set_humidity(today_weather_widget, 0);
        lime_weather_widget_set_date(tomorrow_weather_widget, 0, 0);
        lime_weather_widget_set_icon(tomorrow_weather_widget, 99);
        lime_weather_widget_set_weather_chinese(tomorrow_weather_widget, "无数据");
        lime_weather_widget_set_temperature_range(tomorrow_weather_widget, 0, 0);
        lime_weather_widget_set_humidity(tomorrow_weather_widget, 0);
        lime_weather_widget_set_date(day_after_tomorrow_weather_widget, 0, 0);
        lime_weather_widget_set_icon(day_after_tomorrow_weather_widget, 99);
        lime_weather_widget_set_weather_chinese(day_after_tomorrow_weather_widget, "无数据");
        lime_weather_widget_set_temperature_range(day_after_tomorrow_weather_widget, 0, 0);
        lime_weather_widget_set_humidity(day_after_tomorrow_weather_widget, 0);

        return;
    }

    lime_base_set_label_string(now_city_name_label, senser_info->cityName);
    lime_base_set_label_string(now_temper_label, "%.0f℃", senser_info->nowTemper);
    lime_base_set_label_string(now_humidity_label, "%.0f%%", senser_info->nowHumi);

    lime_weather_widget_set_date(today_weather_widget, senser_info->todayWeather.month, senser_info->todayWeather.day);
    lime_weather_widget_set_icon(today_weather_widget, senser_info->todayWeather.weatherLogoID);
    lime_weather_widget_set_weather_chinese(today_weather_widget, senser_info->todayWeather.weatherChinese);
    lime_weather_widget_set_temperature_range(today_weather_widget, senser_info->todayWeather.temperaLow, senser_info->todayWeather.temperaHigh);
    lime_weather_widget_set_humidity(today_weather_widget, senser_info->todayWeather.humidity);
    lime_weather_widget_set_date(tomorrow_weather_widget, senser_info->tomorrowWeather.month, senser_info->tomorrowWeather.day);
    lime_weather_widget_set_icon(tomorrow_weather_widget, senser_info->tomorrowWeather.weatherLogoID);
    lime_weather_widget_set_weather_chinese(tomorrow_weather_widget, senser_info->tomorrowWeather.weatherChinese);
    lime_weather_widget_set_temperature_range(tomorrow_weather_widget, senser_info->tomorrowWeather.temperaLow, senser_info->tomorrowWeather.temperaHigh);
    lime_weather_widget_set_humidity(tomorrow_weather_widget, senser_info->tomorrowWeather.humidity);
    lime_weather_widget_set_date(day_after_tomorrow_weather_widget, senser_info->dayAfterTomorrowWeather.month, senser_info->dayAfterTomorrowWeather.day);
    lime_weather_widget_set_icon(day_after_tomorrow_weather_widget, senser_info->dayAfterTomorrowWeather.weatherLogoID);
    lime_weather_widget_set_weather_chinese(day_after_tomorrow_weather_widget, senser_info->dayAfterTomorrowWeather.weatherChinese);
    lime_weather_widget_set_temperature_range(day_after_tomorrow_weather_widget, senser_info->dayAfterTomorrowWeather.temperaLow, senser_info->dayAfterTomorrowWeather.temperaHigh);
    lime_weather_widget_set_humidity(day_after_tomorrow_weather_widget, senser_info->dayAfterTomorrowWeather.humidity);



}

static lv_font_t *lime_weather_make_big_font(void)
{
    static lv_font_t * weather_font = NULL;
    if(weather_font == NULL)
    {
        LV_LOG_USER("Create weather font 1");
#if !USING_LIME_HARDWARE
        weather_font = lv_tiny_ttf_create_file("D:/LimeLvResources/wryhbold.ttf", 14);
#else
        weather_font = lv_tiny_ttf_create_file("D:/wryhbold.ttf", 14);
#endif
        MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(weather_font);
        LV_LOG_USER("Create weather font 1 success");
    }

    return weather_font;
}

static lv_obj_t *lime_weather_widget_create(lv_obj_t* parent)
{
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(parent);

    const uint32_t default_width = 90;

    lv_font_t * weather_font = lime_weather_make_big_font();
    MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(weather_font);

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
    lv_obj_set_style_text_font(date_label, &lime_font_weather_num, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(date_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(date_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *weather_logo_img = lv_img_create(bj_obj);
#if !USING_LIME_HARDWARE
    lv_img_set_src(weather_logo_img, "D:/LimeLvResources/weather_white/7.bin");
#else
    lv_img_set_src(weather_logo_img, "0:/weather_white/7.bin");
#endif
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
    lv_obj_set_style_text_font(temperature_label, &lime_font_weather_num, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(temperature_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(temperature_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *humidity_label = lv_label_create(bj_obj);
    lv_obj_set_size(humidity_label, default_width, 15);
    lv_obj_align_to(humidity_label, temperature_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_label_set_text(humidity_label, "50%%");
    lv_obj_set_style_text_font(humidity_label, &lime_font_weather_num, LV_PART_MAIN | LV_STATE_DEFAULT);
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
#if !USING_LIME_HARDWARE
    sprintf(icon_path, "D:/LimeLvResources/weather_white/%d.bin", icon_id);
#else
    sprintf(icon_path, "0:/weather_white/%d.bin", icon_id);
#endif
    const char *now_path = lv_img_get_src(weather_logo_img);
    if(now_path != NULL && strcmp(now_path, icon_path) == 0)
    {
        return;
    }

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
    lime_base_set_label_string(humidity_label, "%d%%", humidity);
}

void lime_weatherface_soft_start(bool is_start)
{
    MLV_BASE_OBJ_NULL_CHECK(bj_obj);

    is_soft_start = is_start;

    if(is_start)
        return;

    /* hide all child objects */
    lv_obj_add_flag(now_city_name_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(today_weather_widget, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(tomorrow_weather_widget, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(day_after_tomorrow_weather_widget, LV_OBJ_FLAG_HIDDEN);
}
