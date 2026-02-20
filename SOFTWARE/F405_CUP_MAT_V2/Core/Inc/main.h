/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY_MULT_Pin GPIO_PIN_3
#define KEY_MULT_GPIO_Port GPIOC
#define ESP8266_EN_Pin GPIO_PIN_1
#define ESP8266_EN_GPIO_Port GPIOA
#define AUDIO_EN_Pin GPIO_PIN_6
#define AUDIO_EN_GPIO_Port GPIOA
#define DS18B20_Pin GPIO_PIN_0
#define DS18B20_GPIO_Port GPIOB
#define CUP_DETECH_Pin GPIO_PIN_1
#define CUP_DETECH_GPIO_Port GPIOB
#define FLASH_CS_Pin GPIO_PIN_12
#define FLASH_CS_GPIO_Port GPIOB
#define WS2812_Pin GPIO_PIN_6
#define WS2812_GPIO_Port GPIOC
#define DEBUG_PC8_Pin GPIO_PIN_8
#define DEBUG_PC8_GPIO_Port GPIOC
#define DEBUG_PC9_Pin GPIO_PIN_9
#define DEBUG_PC9_GPIO_Port GPIOC
#define OLED_RES_Pin GPIO_PIN_6
#define OLED_RES_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_PIN_7
#define OLED_DC_GPIO_Port GPIOB
#define OLED_BLK_Pin GPIO_PIN_8
#define OLED_BLK_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#include <stdio.h>
#include <stdbool.h>

#define GLOBAL_DEVELOP_MODE		1

#if GLOBAL_DEVELOP_MODE
#define DEBUG_LOG(...)	printf(__VA_ARGS__);
#define SOFTWARE_VERSION	"V2.0 DEBUG"
#else
#define DEBUG_LOG(...)
#define SOFTWARE_VERSION	"V2.0"
#endif

#define GLOBAL_SETTING_FILE_PATH		"D:/setting.txt"
#define GLOBAL_DEFAULT_WIFI_NAME		"wifi_name"
#define GLOBAL_DEFAULT_WIFI_PASSWORD	"wifi_passord"
#define GLOBAL_DEFAULT_CITY_NAME		"BeiJing"

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
