#include "key_main.h"
#include "cmsis_os.h"

#include "key.h"
#include "Lime_App_Hal.h"

key_info_t key_info = {0};

static void key_scan_handle(void);

void key_main(void const * argument)
{
  DEBUG_LOG("Task %s,start\n", __FUNCTION__);

  /* init key */
	key_Init();

	while (1)
	{
		key_scan_handle();

		osDelay(10);
	}
}

static void key_scan_handle(void)
{
	mult_key_e now_key = key_get_press();
	static mult_key_e last_key = MULT_KEY_NO;
	
	/* scan now status */
	if(now_key != last_key)
	{
		switch(now_key)
		{
			/* no key pressed, release all key */
			case MULT_KEY_NO:
					key_info.sw_up += key_info.sw_up % 2;
					key_info.sw_down += key_info.sw_down % 2;
					key_info.sw_left += key_info.sw_left % 2;
					key_info.sw_right += key_info.sw_right % 2;
					key_info.sw_set += key_info.sw_set % 2;
					break;

			/* key pressed */
			case MULT_KEY_UP:
					key_info.sw_up ++;
					break;
			case MULT_KEY_DOWN:
					key_info.sw_down ++;
					break;
			case MULT_KEY_LEFT:
					key_info.sw_left ++;
					break;
			case MULT_KEY_RIGHT:
					key_info.sw_right ++;
					break;
			case MULT_KEY_SET:
					key_info.sw_set ++;
					break;
			default:
					break;
		}
	}
	
	/* sync data */
	LimeHAL_SyncKeyInfo((const LimeHal_KeyInfo_t *)&key_info);

	last_key = now_key;
}
