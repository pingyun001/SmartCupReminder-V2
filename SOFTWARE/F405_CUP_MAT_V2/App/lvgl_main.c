#include "lvgl_main.h"
#include "cmsis_os.h"

#include "lvgl.h"
#include "lv_demos.h"
#include "lv_port_disp.h"
#include "lcd_init.h"
#include "Lime_LvMainFace.h"

/* just test */
#include "spi_flash.h"
#include "spi_flash_test.h"
#include "FatFsSelfTest.h"

void lvgl_main(void const * argument)
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
	
	/* init screen */
	LCD_Init();
//	LCD_Fill(0, 0, 428, 142, 0xf800);
	
	/* Init lvgl */
	lv_init();
	lv_port_disp_init();
	
	/* test */
//	lv_demo_benchmark();
	Lime_LvMainFace_Init();
	
	while(1)
	{
		lv_timer_handler();
		osDelay(1);
	}
}




