#include "key_main.h"
#include "cmsis_os.h"

#include "key.h"
#include "stm_system_io.h"
#include "Lime_App_Hal.h"

key_info_t key_info = {0};

static void key_scan_handle(void);
static void usb_voltage_scan_handle(void);
static void udisk_mode_deetch(void);


void key_main(void const * argument)
{
	DEBUG_LOG("Task %s,start\n", __FUNCTION__);

	/* init key */
	key_Init();

	while (1)
	{
		key_scan_handle();
		
		usb_voltage_scan_handle();
		
		udisk_mode_deetch();

		osDelay(5);
	}
}

static void key_scan_handle(void)
{
	mult_key_e now_key = key_get_press_with_filter();
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

static void usb_voltage_scan_handle(void)
{
	const float low_level = 4.12f;
	static uint8_t low_count = 0;
	static float last_voltage = 0;
	float now_voltage = key_get_usb_vol();
	
	/* low count */
	if((now_voltage < low_level) && (last_voltage < low_level))
	{
		low_count ++;
	}
	else
	{
		low_count = 0;
	}
	
	/* out of range */
	if(low_count > 30)
	{
		LimeHAL_SetUsbLowVolErr();
		low_count = 0;
	}
	
	/* sync now voltage */
	LimeHAL_SetUsbVoltage(now_voltage);
	
	/* sync last voltage */
	last_voltage = now_voltage * 0.3f + last_voltage * 0.7f;
}

static void udisk_mode_deetch(void)
{
	if( !LimeHAL_IsSetted_UDiskMode())
		return;
	
	DEBUG_LOG("%s()\n", __FUNCTION__);
	
	/* wait LVGL refresh UI */
	osDelay(500);
	
	/* suspend tasks */
	vTaskSuspendAll();
	
	/* call system API, Enter USB Mode */
	lime_stm_system_enter_u_disk_mode();
	
	while(1)
	{
		HAL_Delay(10);
		
		/* scan key status */
		mult_key_e now_key = key_get_press();
		
		/* all key released, break */
		if(now_key == MULT_KEY_NO)
			break;
	}
	
	DEBUG_LOG("%s(),press any key to exit UDisk\n", __FUNCTION__);
	
	while(1)
	{
		HAL_Delay(10);
		
		/* scan key status */
		mult_key_e now_key = key_get_press();
		
		/* key press */
		if((now_key != MULT_KEY_NO) && (now_key != MULT_KEY_SET))
			break;
	}
	
	/* call system API, Exit USB Mode */
	lime_stm_system_exit_u_disk_mode();
	
	/* resume tasks */
	if( !xTaskResumeAll())
	{
		taskYIELD();
	}
}


