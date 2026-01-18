#ifndef __ESP8266_H
#define __ESP8266_H

#include "main.h"

typedef enum
{
    esp8266_statue_init = 0,
    esp8266_statue_offline = -1,
    esp8266_statue_online = 1,
    esp8266_statue_need_send_pos,
    esp8266_statue_need_send_wifi_name,
    esp8266_statue_need_send_wifi_password,
    esp8266_statue_send_data_finish,
    esp8266_statue_data_ok,
}esp8266_statue_e;

typedef struct
{
    esp8266_statue_e statue;
    bool is_wait_ack;
    char firmware_version[8];
    char compile_time[32];

    /* position info(for weather API) */
    char api_position[64];

    /* wifi info */
    char wifi_name[64];
    char wifi_password[64];
    bool wifi_is_connected;
    uint8_t wifi_getted_ip[4];
    uint8_t wifi_getted_rssi;

}esp8266_info_t;


HAL_StatusTypeDef esp8266_Init(uint32_t timeout);
HAL_StatusTypeDef esp8266_set_position(char *position);
HAL_StatusTypeDef esp8266_set_wifi_info(char *wifi_name, char *wifi_password);

void esp8266_sync_handle(void);

void esp8266_recv_hook(void);

#endif //__ESP8266_H
