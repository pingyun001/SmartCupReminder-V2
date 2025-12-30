#include "lvgl_main.h"
#include "cmsis_os.h"

#include "lvgl.h"
#include "lv_demos.h"
#include "lv_port_disp.h"

void lvgl_main(void const * argument)
{
	DEBUG_LOG("Task %s,start\n", __FUNCTION__);
	
	/* Init lvgl */
	lv_init();
	lv_port_disp_init();
	
	/* test */
	lv_demo_benchmark();
	
	while(1)
	{
		lv_timer_handler();
		osDelay(1);
	}
}




