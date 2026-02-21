# 文件夹概览

- ESP8266_WifiWeather：ESP8266 Wifi天气小组件源码
- F4_UDISK_RESOURCES：提醒喝水杯垫主板U盘资源文件（需要自行拷贝到文件系统中）
- F405_CUP_MAT_V2：提醒喝水杯垫F405主板固件，使用Keil 5 MDK编写
- F405_USB_ESP_Downloader：可以将F405模拟成ESP8266下载器（未完成！暂时使用CH340等USB转TTL转换器下载ESP8266固件）
- LvglSimulationV3_LV93：LVGL图形库仿真源码

# 主板固件说明
## 主板固件阅读说明：
- 主板固件使用Keil 5 MDK编写，需要AC6编译器，推荐Keil版本V5.38及以上！
- 缩进为4，以方便阅读
- 主板固件源码中，有些地方可能有中文（LVGL界面相关），请注意不要修改文件编码格式（当前为UTF-8）

![keil_set_tab_4](../DOC/Readme_Pictures/keilTab4.jpg)

## 默认参数设置说明：
默认参数位于main.h中，目前有如下参数：
```C
#define GLOBAL_SETTING_FILE_PATH		"D:/setting.txt"
#define GLOBAL_DEFAULT_WIFI_NAME		"wifi_name"     //max length 64 bytes
#define GLOBAL_DEFAULT_WIFI_PASSWORD	"wifi_passord"  //max length 64 bytes
#define GLOBAL_DEFAULT_CITY_NAME		"BeiJing"       //max length 64 bytes
```
