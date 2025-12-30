#include "sensor_main.h"
#include "cmsis_os.h"

void sensor_main(void const * argument)
{
	DEBUG_LOG("Task %s,start\n", __FUNCTION__);
	
	while(1)
	{
		osDelay(100);
	}
}







