#ifndef __LIME_BOOTLOADER__
#define __LIME_BOOTLOADER__

#include "main.h"

typedef enum
{
	lime_boot_status_no_file_system,
	lime_boot_status_has_new_app,
	lime_boot_status_no_need_update,
}lime_boot_status_e;

lime_boot_status_e lime_detech_new_app(void);

HAL_StatusTypeDef lime_copy_app(void);

HAL_StatusTypeDef lime_confirm_flash_app(uint32_t address);

HAL_StatusTypeDef lime_confirm_fatfs_app(void);

HAL_StatusTypeDef lime_del_fatfs_app(void);

HAL_StatusTypeDef lime_confirm_flash_signature(void);

HAL_StatusTypeDef lime_jump_app(uint32_t address);

HAL_StatusTypeDef lime_re_create_filesystem(void);

#endif	//__LIME_BOOTLOADER__
