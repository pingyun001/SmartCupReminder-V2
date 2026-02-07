#include "stm_system_io.h"
#include "usb_device.h"
#include "usbd_core.h"
#include "file_system_logic.h"

void lime_stm_system_restart(void)
{
    DEBUG_LOG("%s()\n", __FUNCTION__);

    /* turn off voice */
    HAL_GPIO_WritePin(AUDIO_EN_GPIO_Port, AUDIO_EN_Pin, GPIO_PIN_RESET);
	
	/* disable interrupt */
	__disable_irq();

    /* reset the system */
    NVIC_SystemReset();
}

void lime_stm_system_restore(void)
{
	/* re-create file system */
	file_system_ReInit();
	
	/* re-start system */
	lime_stm_system_restart();
}

void lime_stm_system_enter_u_disk_mode(void)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
    /* turn off voice */
    HAL_GPIO_WritePin(AUDIO_EN_GPIO_Port, AUDIO_EN_Pin, GPIO_PIN_RESET);

    /* enter u-disk mode */
    MX_USB_DEVICE_Init();
}

void lime_stm_system_exit_u_disk_mode(void)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
    USBD_DeInit(&hUsbDeviceFS);
}



