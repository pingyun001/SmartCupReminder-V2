#include "esp8266.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

esp8266_info_t esp8266_info = 
{
    .statue = esp8266_statue_init,
};

static uint8_t rx_byte = 0;
static bool last_rx_time_is_checked = true;
static uint32_t last_rx_time = 0;
static char dma_send_buffer[128];

HAL_StatusTypeDef esp8266_Init(uint32_t timeout)
{
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
    
    /* find the first character of the ESP8266 */
    if((esp8266_info.statue == esp8266_statue_init) && (rx_byte == '-'))
    {
        esp8266_info.statue = esp8266_statue_online;
    }
    
    if(esp8266_info.statue >= esp8266_statue_online)
    {
        HAL_UART_Transmit(&huart1, &rx_byte, 1, 1000);
    }
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

    if(esp8266_info.statue == esp8266_statue_online)
    {

    }
}

