#include "sensor_main.h"
#include "cmsis_os.h"

#include "ws2812.h"
#include "ds18b20.h"
#include "key.h"
#include "ff.h"
#include "esp8266.h"

/* just test */
#include "spi_flash.h"
#include "spi_flash_test.h"
#include "FatFsSelfTest.h"

void sensor_main(void const * argument)
{
	DEBUG_LOG("Task %s,start\n", __FUNCTION__);
	
	/* test Flash */
//	spi_flash_init(NULL);
//	FatFs_test(0);
	
	/* mount fatfs */
	static FATFS fs;
	FRESULT fr = f_mount(&fs, "D:", 1);
	if (fr != FR_OK)
	{
		printf("file system mount failed\n");
		while(1)
			;
	}
	printf("file system mount success\n");
	
	/* init ws2812 */
	ws2812_Init();
	
	/* init ds18b20 */
	while(ds18b20_Init())
	{
		printf("ds18b20 init err\n");
		HAL_Delay(300);
	}
	
	/* init ESP8266 */
	esp8266_Init(1000);
	esp8266_set_position("Beijing");
	esp8266_set_wifi_info("ziroom1802", "4001001111");
	
	while(1)
	{
		// printf("usb Vol:%.2f V\n", key_get_usb_vol());

		esp8266_sync_handle();
		
		osDelay(10);
	}
}








