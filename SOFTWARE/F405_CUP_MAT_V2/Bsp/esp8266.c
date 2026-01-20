#include "esp8266.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "cmsis_os.h"

#include "Lime_App_Hal.h"

esp8266_info_t esp8266_info = 
{
    .statue = esp8266_statue_init,
};

static uint8_t rx_byte = 0;
static bool last_rx_time_is_checked = true;
static uint32_t last_rx_time = 0;
static char dma_send_buffer[128];

static void esp8266_decode_message(uint8_t step, const char *message, uint32_t len);

typedef struct
{
	uint32_t size;
	char buffer[124];
}rx_message_t;
QueueHandle_t rx_queue = NULL;

HAL_StatusTypeDef esp8266_Init(uint32_t timeout)
{
		/* create queue to receive message */
		if(rx_queue == NULL)
		{
			rx_queue = xQueueCreate(10, sizeof(rx_message_t));
		}
		
    /* reset the ESP8266 */
    HAL_GPIO_WritePin(ESP8266_EN_GPIO_Port, ESP8266_EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(ESP8266_EN_GPIO_Port, ESP8266_EN_Pin, GPIO_PIN_SET);

    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);

    /* wait for the ESP8266 to boot up */
    uint32_t start_time = HAL_GetTick();
    while(HAL_GetTick() - start_time < timeout)
    {
        if(esp8266_info.statue >= esp8266_statue_online)
        {
            return HAL_OK;
        }
    }

    return HAL_TIMEOUT;
}

void esp8266_recv_hook(void)
{
	HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
	last_rx_time = HAL_GetTick();
	last_rx_time_is_checked = false;

    static bool find_head = false;
    static bool head_confirmed = false;
    static uint16_t head_count = 0;
    static uint8_t last_rx_byte = 0;
    static rx_message_t cat_message = {0};
	
	/* find the first character of the ESP8266 */
	if((esp8266_info.statue == esp8266_statue_init) && (rx_byte == '-'))
	{
		esp8266_info.statue = esp8266_statue_online;
	}
	
    /* online, receive message */
	if(esp8266_info.statue >= esp8266_statue_online)
	{
		// HAL_UART_Transmit(&huart1, &rx_byte, 1, 1000);

        // return;

        /*
        一条消息长这样：---Humidity:40 以---开头，以换行结尾
        */

        /* find the head of the message */
        if((last_rx_byte != '-') && (rx_byte == '-') && (!find_head) && ( !head_confirmed))
        {
            find_head = true;
            head_confirmed = false;
            head_count = 0;

            // DEBUG_LOG("sss\n");
        }
        if(find_head)
        {
            if(rx_byte != '-')
            {
                find_head = false;
                goto sync_end;
            }
            cat_message.buffer[head_count] = rx_byte;
            head_count++;
            if(head_count == 3)
            {
                if(cat_message.buffer[0] == '-' && cat_message.buffer[1] == '-' && cat_message.buffer[2] == '-')
                {
                    // DEBUG_LOG("hhh\n");
                    head_confirmed = true;
                }
                else
                {
                    // DEBUG_LOG("fff\n");
                }

                find_head = false;
            }

            goto sync_end;
        }

        /* recv message body */
        if(head_confirmed)
        {
            cat_message.buffer[head_count] = rx_byte;
            head_count++;
            if(head_count >= sizeof(cat_message.buffer) - 1)
            {
                // DEBUG_LOG("ooo\n");
                head_confirmed = false;
                head_count = 0;
                goto sync_end;
            }
            if(rx_byte == '\n')
            {
                // DEBUG_LOG("zzz%d\n", head_count);
                head_count -= 2;
                cat_message.size = head_count;
                xQueueSendFromISR(rx_queue, &cat_message, 0);
                head_confirmed = false;
                
                memset(cat_message.buffer, 0, head_count);
                head_count = 0;
            }
        }
	}

    /* sync last rx byte */
sync_end:
    last_rx_byte = rx_byte;
}

HAL_StatusTypeDef esp8266_set_position(char *position)
{
    uint32_t len = strlen(position);
    if(len < sizeof(esp8266_info.api_position))
    {
        strcpy(esp8266_info.api_position, position);

        return HAL_OK;
    }

    return HAL_ERROR;
}
HAL_StatusTypeDef esp8266_set_wifi_info(char *wifi_name, char *wifi_password)
{
    uint32_t len = strlen(wifi_name);
    if(len < sizeof(esp8266_info.wifi_name))
    {
        strcpy(esp8266_info.wifi_name, wifi_name);
    }
    else
    {
        return HAL_ERROR;
    }

    len = strlen(wifi_password);
    if(len < sizeof(esp8266_info.wifi_password))
    {
        strcpy(esp8266_info.wifi_password, wifi_password);
    }
    else
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

void esp8266_sync_handle(void)
{
    /* software sim IDLE Interrupt */
    if((!last_rx_time_is_checked) && (HAL_GetTick() - last_rx_time > 1000))
    {
        last_rx_time_is_checked = true;

        if(esp8266_info.statue == esp8266_statue_online)
        {
            esp8266_info.statue = esp8266_statue_need_send_pos;
        }

        if(esp8266_info.statue == esp8266_statue_need_send_pos)
        {
            memset(dma_send_buffer, 0, sizeof(dma_send_buffer));
            snprintf(dma_send_buffer, sizeof(dma_send_buffer), "set_position:%s", esp8266_info.api_position);
            HAL_UART_Transmit_DMA(&huart2, (uint8_t*)dma_send_buffer, strlen(dma_send_buffer));
            esp8266_info.statue = esp8266_statue_need_send_wifi_name;
        }
        else if(esp8266_info.statue == esp8266_statue_need_send_wifi_name)
        {
            memset(dma_send_buffer, 0, sizeof(dma_send_buffer));
            snprintf(dma_send_buffer, sizeof(dma_send_buffer), "set_wifiname:%s", esp8266_info.wifi_name);
            HAL_UART_Transmit_DMA(&huart2, (uint8_t*)dma_send_buffer, strlen(dma_send_buffer));
            esp8266_info.statue = esp8266_statue_need_send_wifi_password;
        }
        else if(esp8266_info.statue == esp8266_statue_need_send_wifi_password)
        {
            memset(dma_send_buffer, 0, sizeof(dma_send_buffer));
            snprintf(dma_send_buffer, sizeof(dma_send_buffer), "set_wifipasswd:%s", esp8266_info.wifi_password);
            HAL_UART_Transmit_DMA(&huart2, (uint8_t*)dma_send_buffer, strlen(dma_send_buffer));
            esp8266_info.statue = esp8266_statue_send_data_finish;
        }
    }

    if(esp8266_info.statue >= esp8266_statue_online)
    {
        rx_message_t message = {0};
        static uint8_t step = 0;
        if(xQueueReceive(rx_queue, &message, pdMS_TO_TICKS(5)) == pdPASS)
        {
            DEBUG_LOG("threat message, len: %d, data: %s\n", message.size, message.buffer);

            if(memcmp(message.buffer, "---Lime ESP8266 wifi weather kit start...", message.size) == 0)
            {
                DEBUG_LOG("find head\n");
                step = 0;
            }
            else
            {
                step ++;
            }

            esp8266_decode_message(step, message.buffer, message.size);
        }
        else
        {
            // DEBUG_LOG("no threat message\n");
        }

        /* scan every 5 minutes */
        static uint32_t last_scan_time = 0;
        if(HAL_GetTick() - last_scan_time > 5 * 60 * 1000)
        {
            last_scan_time = HAL_GetTick();
            
            HAL_UART_Transmit_DMA(&huart2, (uint8_t*)"set_refreshnow", 10);

            step = 8;
        }
    }
}

static void esp8266_decode_message(uint8_t step, const char *message, uint32_t len)
{
    static dayWeather_t temp_weather_data = {0};

    switch(step)
    {
        /* ---version:1.0.0 */
        case 1:
        {
            uint8_t version[3];
            sscanf(message, "---version:%d.%d.%d", (int*)&version[0], (int*)&version[1], (int*)&version[2]);
            DEBUG_LOG("decode version: %d.%d.%d\n", version[0], version[1], version[2]);
            LimeHAL_SyncEspFirmwareVersion(version);
            break;
        }
        case 2:
            break;

        /* ---WiFi connected:ok */
        case 3:
        {
            bool is_wifi_connected = false;
            if(memcmp(message, "---WiFi connected:ok", len) == 0)
                is_wifi_connected = true;
            DEBUG_LOG("decode wifi connected: %d\n", is_wifi_connected);
            LimeHAL_SetWifiStatus(is_wifi_connected);
            break;
        }

        /* ---IP address:192.168.124.71 */
        case 4:
        {
            if(memcmp(message, "---IP address:", 14) != 0)
                break;

            uint8_t ip_address[4];
            sscanf(message, "---IP address:%d.%d.%d.%d", (int*)&ip_address[0], (int*)&ip_address[1], (int*)&ip_address[2], (int*)&ip_address[3]);
            DEBUG_LOG("decode ip address: %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
            LimeHAL_SetIpAddress(ip_address);
            break;
        }

        /* ---Signal strength (RSSI):-54dBm */
        case 5:
        {
            if(memcmp(message, "---Signal strength (RSSI):", 26) != 0)
                break;

            int8_t rssi = 0;
            sscanf(message, "---Signal strength (RSSI):%ddBm", (int*)&rssi);
            DEBUG_LOG("decode rssi: %ddBm\n", rssi);
            // LimeHAL_SetSignalStrength(rssi);
            break;
        }

        /* ---nowtime:00:00:15 */
        case 6:
        {
            uint8_t now_time[3];
            sscanf(message, "---nowtime:%d:%d:%d", (int*)&now_time[0], (int*)&now_time[1], (int*)&now_time[2]);
            DEBUG_LOG("decode now time: %d:%d:%d\n", now_time[0], now_time[1], now_time[2]);
            LimeHAL_SetTime(now_time[1], now_time[2]);
            break;
        }

        /* ---City:北京 */
        case 7:
        {
            char city[32];
            
            if(memcmp(message, "---City:", 8) != 0)
                break;

            if(strlen(message) > 32 + 8)
                break;

            sscanf(message, "---City:%s", city);
            DEBUG_LOG("decode city: %s\n", city);
            LimeHAL_SetCityName(city);
            LimeHAL_SetWeatherDataValid(true);
            break;
        }

        /* ---Temperature:-18 */
        case 8:
        {
            int16_t temperature = 0;
            sscanf(message, "---Temperature:%d", (int*)&temperature);
            DEBUG_LOG("decode temperature: %d\n", temperature);
            float now_temperature = temperature;
            LimeHAL_SetNowTemper(now_temperature);
            break;
        }

        /* ---Date: 2026-01-20 */
        case 9:
        case 16:
        case 23:
        {
            int date[3];
            sscanf(message, "---Date: %d-%d-%d", &date[0], &date[1], &date[2]);
            DEBUG_LOG("decode date: %d-%d-%d\n", date[0], date[1], date[2]);
            
            memset(&temp_weather_data, 0, sizeof(temp_weather_data));
            temp_weather_data.month = date[1];
            temp_weather_data.day = date[2];

            break;
        }

        /* ---code_day:0 */
        case 10:
        case 17:
        case 24:
        {
            uint8_t code_day = 0;
            sscanf(message, "---code_day:%d", (int*)&code_day);
            DEBUG_LOG("decode code_day: %d\n", code_day);
            
            temp_weather_data.weatherLogoID = code_day;

            break;
        }

        /* ---Day:晴 */
        case 11:
        case 18:
        case 25:
        {
            char day[16];
            sscanf(message, "---Day:%s", day);
            DEBUG_LOG("decode day: %s\n", day);
            
            strcpy(temp_weather_data.weatherChinese, day);

            break;
        }

        /* ---TemperatureLow:-22 */
        case 12:
        case 19:
        case 26:
        {
            int16_t temperature_low = 0;
            sscanf(message, "---TemperatureLow:%d", (int*)&temperature_low);
            DEBUG_LOG("decode temperature_low: %d\n", temperature_low);
            
            temp_weather_data.temperaLow = temperature_low;

            break;
        }
        
        /* ---TemperatureHigh:-11 */
        case 13:
        case 20:
        case 27:
        {
            int16_t temperature_high = 0;
            sscanf(message, "---TemperatureHigh:%d", (int*)&temperature_high);
            DEBUG_LOG("decode temperature_high: %d\n", temperature_high);
            
            temp_weather_data.temperaHigh = temperature_high;

            break;
        }

        /* ---Humidity:41 */
        case 14:
        case 21:
        case 28:
        {
            uint8_t humidity = 0;
            sscanf(message, "---Humidity:%d", (int*)&humidity);
            DEBUG_LOG("decode humidity: %d\n", humidity);
            
            temp_weather_data.humidity = humidity;

            break;
        }

        /* ---end */
        case 15:
        {
            LimeHAL_SetTodayWeather(&temp_weather_data);
            break;
        }
        case 22:
        {
            LimeHAL_SetTomorrowWeather(&temp_weather_data);
            break;
        }
        case 29:
        {
            LimeHAL_SetDayAfterTomorrowWeather(&temp_weather_data);
            break;
        }
    }
}

