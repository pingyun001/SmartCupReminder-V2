/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "spi_flash.h"
#include "ws2812.h"

#include "FatFsSelfTest.h"
#include "stm_system_io.h"
#include "lime_bootloader.h"
#include "stm_system_io.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t nvic_vector_tab[256] __attribute__((section(".bss.ARM.__at_0x20010000"))) ;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
//  __disable_irq();
//  uint32_t *nvic_tab_default = (uint32_t*)0x08000000;
//  for(uint32_t i = 0; i < 256; i++)
//  {
//	  nvic_vector_tab[i] = nvic_tab_default[i];
//  }
//  SCB->VTOR = (uint32_t)nvic_vector_tab;
//  __enable_irq();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_TIM8_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM4_Init();
  MX_ADC2_Init();
  MX_TIM7_Init();
  MX_TIM14_Init();
//  MX_USB_DEVICE_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  DEBUG_LOG(">>>CUP_MAT Bootloader Start!\n");
  DEBUG_LOG(">>>Compile Time:%s,%s\n", __DATE__, __TIME__);
  
  /* turn off all light */
  ws2812_Init();
  WS2812_Sync();
  HAL_Delay(10);
  
  /* confirm bin file */
  lime_boot_status_e boot_status = lime_detech_new_app();
  switch(boot_status)
  {
	case lime_boot_status_no_file_system:
	{
		/* confirm flash signature status */
		if(lime_confirm_flash_signature() != HAL_OK)
		{
			DEBUG_LOG(">>>Confirm internal app, Failed\n");
			
			/* new device, try to create file system */
			if(lime_re_create_filesystem() != HAL_OK)
			{
				DEBUG_LOG(">>>Create File System, Failed\n");
				
				goto errend;
			}
			
			/* create success, open USB mode */
			lime_stm_system_enter_u_disk_mode();
			
			/* then, do nothing */
			while(1)
				;
		}
		DEBUG_LOG(">>>Confirm internal app, Success\n");
		
		/* has internal app, run it */
		goto run_app;
	}
	break;
	
	case lime_boot_status_has_new_app:
	{
		DEBUG_LOG(">>>FatFs has new app\n");
		
		if(lime_confirm_fatfs_app() != HAL_OK)
		{
			DEBUG_LOG(">>>Confirm FatFs New App Failed\n");
			
			goto run_app;
		}
		DEBUG_LOG(">>>Confirm FatFs New App Success\n");
		
		/* yellow light, means update */
		WS2812_SetRGB(0, 15, 13, 0);
		WS2812_Sync();
		HAL_Delay(10);
		
		/* copy app from FatFs to Flash */
		if(lime_copy_app() != HAL_OK)
		{
			DEBUG_LOG(">>>Copy failed\n");
			
			goto errend;
		}
		DEBUG_LOG(">>>Copy New App Success\n");

		/* re-confirm */
		if(lime_confirm_flash_app(0x08010000) != HAL_OK)
		{
			DEBUG_LOG(">>>Confirm FatFs New App Failed\n");
			
			goto errend;
		}
		DEBUG_LOG(">>>Confirm FatFs New App Success\n");
		
		/* del FatFs app */
		if(lime_del_fatfs_app() != HAL_OK)
		{
			DEBUG_LOG(">>>Del update.bin Failed\n");
		}
		DEBUG_LOG(">>>Del update.bin Success\n");
	}
	break;
	
	case lime_boot_status_no_need_update:
	{
		DEBUG_LOG(">>>No update.bin, no need update\n");
		
		/* confirm flash signature status */
		if(lime_confirm_flash_signature() != HAL_OK)
		{
			DEBUG_LOG(">>>Confirm now app, Failed\n");
			
			goto errend;
		}
		DEBUG_LOG(">>>Confirm now app, Success\n");
		
		goto run_app;
	}
	break;
	default:break;
  }
  
run_app:
  /* green light, means app check pass */
  WS2812_SetRGB(0, 0, 13, 0);
  WS2812_Sync();
  HAL_Delay(10);
  
  /* jump to application */
  lime_jump_app(0x08010000);
  
  while(1)
	  ;
  
errend:
  DEBUG_LOG("error occurred, please re-power up\n");
  
  WS2812_SetRGB(0, 15, 0, 0);
  WS2812_Sync();
  
  while(1)
	  ;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */



/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM13 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM13) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	DEBUG_LOG("\n%s()\n", __FUNCTION__);
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
