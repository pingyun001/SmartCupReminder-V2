#include "lime_bootloader.h"
#include "ff.h"
#include "file_system_logic.h"

lime_boot_status_e lime_detech_new_app(void)
{
	if(file_system_Init() != HAL_OK)
	{
		DEBUG_LOG("file system init failed\n");
		
		return lime_boot_status_no_file_system;
	}
	DEBUG_LOG("file system init success\n");
	
	return lime_boot_status_no_need_update;
}

void lime_jump_app(uint32_t address)
{
	DEBUG_LOG("%s(address:%#x),start\n", __FUNCTION__, address);
	
	/* disable IRQ */
	__disable_irq();
	
	/* make jump app function */
	typedef void (*iapfun_t)(void);
	volatile static iapfun_t jumpToApp1;
	jumpToApp1 = (iapfun_t) *(volatile uint32_t*)(address + 4);
	
	/* set MSP */
	__set_MSP(*(volatile uint32_t*)address);
	
	/* reset vtor */
	SCB->VTOR = address;
	
	/* jump to app */
	jumpToApp1();
	
	DEBUG_LOG("%s(), failed\n", __func__);
}

