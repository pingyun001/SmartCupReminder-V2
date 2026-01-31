#ifndef __FILE_SYSTEM_LOGIC
#define __FILE_SYSTEM_LOGIC

#include "main.h"
#include "ff.h"
#include "cmsis_os.h"

#define LIME_COPY_BUF_SIZE		64
#define LIME_FATFS_DYNAMIC_MEM_MELLOC		1

#if LIME_FATFS_DYNAMIC_MEM_MELLOC
#include "cmsis_os.h"
#define LIME_FATFS_MELLOC(size)		pvPortMalloc(size)
#define LIME_FATFS_FREE(pin)			vPortFree(pin)
#endif

FRESULT Lime_file_system_get_free(char *pdrv, uint32_t *total, uint32_t *free);

FRESULT Lime_file_copy(const char* src_path, const char* dst_path);

FRESULT Lime_file_path_copy(const char* src_path, const char* dst_path);

FRESULT Lime_file_create(const char* filename, const uint8_t* buffer, uint32_t buffer_size);

FRESULT Lime_file_system_mkfs(const char *root_path);

FRESULT Lime_file_path_clean(const char *dst_path);

FRESULT Lime_file_system_get_folder_size(const char *dst_path, float* size_kB);

bool Lime_file_exists(const char* filename);

FRESULT Lime_file_delete(const char *dst_path);


/* detech file system, if no filesystem, try to crate one */
HAL_StatusTypeDef file_system_Init(void);

/* re-create new system */
HAL_StatusTypeDef file_system_ReInit(void);

/* confirm  */
HAL_StatusTypeDef file_system_confirm(void);

/* setting info in setting.txt */
typedef struct
{
	char wifi_name[64];
	char wifi_password[64];
	char city_name[64];
}setting_file_info_t;

HAL_StatusTypeDef file_system_read_setting_file(setting_file_info_t *info);

#endif		//__FILE_SYSTEM_LOGIC
