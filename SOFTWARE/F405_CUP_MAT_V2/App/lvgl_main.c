#include "lvgl_main.h"
#include "cmsis_os.h"

#include "lvgl.h"
#include "lv_demos.h"
#include "lv_port_disp.h"
#include "lcd_init.h"
#include "Lime_LvMainFace.h"

#include "audio_player.h"

void lvgl_main(void const * argument)
{
	DEBUG_LOG("Task %s,start\n", __FUNCTION__);
	
	/* 临时  后续UI做一下同步（需要等FatFs挂载且校验通过后，启动后续需要文件系统的UI界面） */
	osDelay(1000);
	
	/* init screen */
	LCD_Init();
	LCD_Fill(0, 0, 428, 142, 0xf800);
	
	/* Init lvgl */
	lv_init();
	lv_port_disp_init();
	
	/* create main face */
	Lime_LvMainFace_Init();
	
	while(1)
	{
		/* lvgl run handle */
		lv_timer_handler();
		
		/* audio run handle */
		Lime_audio_run_handle();
		
		osDelay(1);
	}
}




