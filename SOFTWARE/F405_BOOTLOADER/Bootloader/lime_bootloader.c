#include "lime_bootloader.h"
#include "ff.h"
#include "file_system_logic.h"
#include <string.h>

#define CPY_BUFF_MAX 1024 * 32
uint8_t copy_buff[CPY_BUFF_MAX] __attribute__((aligned(32))) = {0};

// 定义tag结构
typedef struct 
{ 
	 char head[12]; 
	 uint32_t version; 
	 uint32_t app_len; 
	 uint32_t app_crc; 
	 uint32_t tag_crc; 
	 char end[4]; 
}lime_boot_tag_t;
#define LIME_BOOT_TAG_HEAD	"LIME-TAG-ST"
#define LIME_BOOT_TAG_END	"END"
#define TAG_OFFSET 0x400
#define TAG_SIZE sizeof(lime_boot_tag_t)

// CRC32计算函数
uint32_t calc_crc32(const uint8_t *buf, uint32_t size)
{
	uint32_t crc = 0xFFFFFFFF;
	uint32_t i, j;
	
	for (i = 0; i < size; i++) {
		crc ^= buf[i];
		for (j = 0; j < 8; j++) {
			if (crc & 1) {
				crc = (crc >> 1) ^ 0xEDB88320L;
			} else {
				crc >>= 1;
			}
		}
	}
	
	return crc ^ 0xFFFFFFFF;
}

// 分块CRC32计算函数
uint32_t calc_crc32_chunk(uint32_t crc, const uint8_t *buf, uint32_t size)
{
	uint32_t i, j;
	
	// 如果是第一次调用，初始化CRC值
	if (crc == 0) {
		crc = 0xFFFFFFFF;
	} else {
		// 已经进行过计算，需要将结果转换回中间状态
		crc ^= 0xFFFFFFFF;
	}
	
	for (i = 0; i < size; i++) {
		crc ^= buf[i];
		for (j = 0; j < 8; j++) {
			if (crc & 1) {
				crc = (crc >> 1) ^ 0xEDB88320L;
			} else {
				crc >>= 1;
			}
		}
	}
	
	// 返回最终状态，供下次调用使用
	return crc ^ 0xFFFFFFFF;
}

lime_boot_status_e lime_detech_new_app(void)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
	
	/* file system mount */
	if(file_system_Init() != HAL_OK)
	{
		DEBUG_LOG("file system init failed\n");
		
		return lime_boot_status_no_file_system;
	}
	DEBUG_LOG("file system init success\n");
	
	/* new app check */
	if(Lime_file_exists("D:/update.bin"))
	{
		DEBUG_LOG("find update.bin file\n");
		
		return lime_boot_status_has_new_app;
	}
	DEBUG_LOG("no update.bin\n");
	
	return lime_boot_status_no_need_update;
}

void printBufAdv(uint8_t* buf, uint32_t length, uint32_t showAddr)
{
	uint32_t addr = showAddr;
	size_t size = length;
	uint8_t *data = buf;
	uint32_t i = 0;
	printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
	for (i = 0; i < size; i++)
	{
		if (i % 16 == 0)
		{
			printf("[%08X] ", addr + i);
		}
		if(data[i] != 0)
			printf("%02X ", data[i]);
		else
			printf("-- ");
		if (((i + 1) % 16 == 0) || i == size - 1)
		{
			printf("\r\n");
		}
	}
}

FIL fsrc = {0};							// 文件对象
HAL_StatusTypeDef lime_copy_app(void)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
	
	HAL_FLASH_Unlock();
	
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGAERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGPERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR);
	
	/* erase sector */
	FLASH_EraseInitTypeDef EraseInitStruct = {0};
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;    //页擦除
    EraseInitStruct.Banks = FLASH_BANK_1;					//只有1个bank
	EraseInitStruct.Sector = FLASH_SECTOR_4;				//
    EraseInitStruct.NbSectors = 8;    						//Number of sectors to be erased.
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;   //Device operating range: 2.7V to 3.6V
    uint32_t PageError = 0;            						//记录擦除出错时的起始地址
	DEBUG_LOG("start erase\n");
    if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError)!=HAL_OK)
    {
        DEBUG_LOG("Flash Erase Fail,ErrorStartAddress:%#x\r\n", PageError);
        return HAL_ERROR;
    }
	DEBUG_LOG("Erase Success\n");
	
	/* open update.bin */
	FRESULT fr;
	UINT br;
	fr = f_open(&fsrc, "D:/update.bin", FA_OPEN_EXISTING | FA_READ);
	if(fr != FR_OK)
	{
		DEBUG_LOG("Open File Fail: %d\r\n", fr);
		return HAL_ERROR;
	}
	DEBUG_LOG("Open File Success\n");
	
	/* copy firmware */
	uint32_t dst_address = 0x08010000;
	uint8_t *src_address = (uint8_t*)copy_buff;
	int32_t readded_bytes = 0;
	uint32_t total_bytes = 0;
	do
	{
		fr = f_read(&fsrc, copy_buff, CPY_BUFF_MAX, &br);
		
		if(fr != FR_OK)
		{
			DEBUG_LOG("File Read Fail: %d\r\n", fr);
			f_close(&fsrc);
			return HAL_ERROR;
		}
		
		/* write firmware to flash */
		for(uint32_t i = 0; i < br; i++)
		{
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, ((uint32_t)dst_address) + i, copy_buff[i]) != HAL_OK)
			{
				DEBUG_LOG("Flash Write Fail\n");
				f_close(&fsrc);
				HAL_FLASH_Lock();
				
				return HAL_ERROR;
			}
		}
		
		dst_address += br;
		total_bytes += br;
		DEBUG_LOG("Written %d kbytes\r\n", total_bytes / 1024);
		
	}while(br != 0);
	
	f_close(&fsrc);
	
	HAL_FLASH_Lock();
	
	return HAL_OK;
}

HAL_StatusTypeDef lime_confirm_flash_app(uint32_t address)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
	
	uint8_t *flash_pin = (uint8_t*)address;
	
	/* open update.bin */
	FRESULT fr;
	UINT br;
	fr = f_open(&fsrc, "D:/update.bin", FA_OPEN_EXISTING | FA_READ);
	if(fr != FR_OK)
	{
		DEBUG_LOG("Open File Fail: %d\r\n", fr);
		return HAL_ERROR;
	}
	DEBUG_LOG("Open File Success\n");
	
	/* copy firmware */
	uint32_t dst_address = 0x08010000;
	uint8_t *src_address = (uint8_t*)copy_buff;
	int32_t readded_bytes = 0;
	uint32_t total_bytes = 0;
	do
	{
		fr = f_read(&fsrc, copy_buff, CPY_BUFF_MAX, &br);
		
		if(fr != FR_OK)
		{
			DEBUG_LOG("File Read Fail: %d\r\n", fr);
			f_close(&fsrc);
			return HAL_ERROR;
		}
		
		/* confirm firmware part to flash */
		for(uint32_t i = 0; i < br; i++)
		{
			if(((uint8_t*)dst_address)[i] != copy_buff[i])
			{
				DEBUG_LOG("Confirm failed: address:0x%08x,value:%x, != tar:%x\n", dst_address + i, ((uint8_t*)dst_address)[i], copy_buff[i]);
				
				return HAL_ERROR;
			}
		}
		
		dst_address += br;
		total_bytes += br;
		DEBUG_LOG("Confirmed %d kbytes, pass\r\n", total_bytes / 1024);
		
	}while(br != 0);
	
	f_close(&fsrc);
	
	DEBUG_LOG("all confirm passed\n");
	
	return HAL_OK;
}

HAL_StatusTypeDef lime_confirm_fatfs_app(void)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
	
	FRESULT fr;
	UINT br;
	uint32_t current_offset = 0;
	uint32_t crc = 0;
	lime_boot_tag_t tag;
	
	/* 打开update.bin文件 */
	fr = f_open(&fsrc, "D:/update.bin", FA_OPEN_EXISTING | FA_READ);
	if(fr != FR_OK)
	{
		DEBUG_LOG("Open File Fail: %d\r\n", fr);
		return HAL_ERROR;
	}
	DEBUG_LOG("Open File Success\n");
	
	/* 读取tag信息 */
	fr = f_lseek(&fsrc, TAG_OFFSET);
	if(fr != FR_OK)
	{
		DEBUG_LOG("Seek to tag offset fail: %d\r\n", fr);
		f_close(&fsrc);
		return HAL_ERROR;
	}
	
	fr = f_read(&fsrc, &tag, sizeof(tag), &br);
	if(fr != FR_OK || br != sizeof(tag))
	{
		DEBUG_LOG("Read tag fail: %d\r\n", fr);
		f_close(&fsrc);
		return HAL_ERROR;
	}
	
	/* 校验tag头 */
	if(memcmp(tag.head, LIME_BOOT_TAG_HEAD, 12))
	{
		DEBUG_LOG("Tag head error\r\n");
		f_close(&fsrc);
		return HAL_ERROR;
	}
	DEBUG_LOG("Tag head ok\r\n");
	
	/* 校验tag尾 */
	if(memcmp(tag.end, LIME_BOOT_TAG_END, 4))
	{
		DEBUG_LOG("Tag end error\r\n");
		f_close(&fsrc);
		return HAL_ERROR;
	}
	DEBUG_LOG("Tag end ok\r\n");
	
	/* 校验tag crc */
	uint32_t tag_crc = calc_crc32((uint8_t *)&tag, offsetof(lime_boot_tag_t, tag_crc));
	if(tag_crc != tag.tag_crc)
	{
		DEBUG_LOG("Tag crc error, cal:0x%08x, saved:0x%08x\r\n", tag_crc, tag.tag_crc);
		f_close(&fsrc);
		return HAL_ERROR;
	}
	DEBUG_LOG("Tag crc ok\r\n");
	
	/* 重置文件指针到文件开头 */
	fr = f_lseek(&fsrc, 0);
	if(fr != FR_OK)
	{
		DEBUG_LOG("Seek to file start fail: %d\r\n", fr);
		f_close(&fsrc);
		return HAL_ERROR;
	}
	
	/* 分块读取固件并计算CRC */
	current_offset = 0;
	crc = 0;
	
	do
	{
		fr = f_read(&fsrc, copy_buff, CPY_BUFF_MAX, &br);
		
		if(fr != FR_OK)
		{
			DEBUG_LOG("File Read Fail: %d\r\n", fr);
			f_close(&fsrc);
			return HAL_ERROR;
		}
		
		if(br == 0)
		{
			break;
		}
		
		/* 检查当前块是否包含tag */
		if (current_offset <= TAG_OFFSET && current_offset + br > TAG_OFFSET)
		{
			/* 计算tag开始和结束在当前缓冲中的位置 */
			uint32_t tag_start_in_buf = TAG_OFFSET - current_offset;
			uint32_t tag_end_in_buf = tag_start_in_buf + TAG_SIZE;
			
			/* 确保tag_end_in_buf不超过缓冲区大小 */
			if (tag_end_in_buf > br)
			{
				tag_end_in_buf = br;
			}
			
			/* 计算tag之前的数据 */
			if (tag_start_in_buf > 0)
			{
				crc = calc_crc32_chunk(crc, copy_buff, tag_start_in_buf);
			}
			
			/* 跳过tag，直接计算tag之后的数据 */
			if (tag_end_in_buf < br)
			{
				crc = calc_crc32_chunk(crc, copy_buff + tag_end_in_buf, br - tag_end_in_buf);
			}
		}
		else
		{
			/* 整个块都在tag之外，直接计算 */
			crc = calc_crc32_chunk(crc, copy_buff, br);
		}
		
		current_offset += br;
		DEBUG_LOG("Processed %d kbytes\r\n", current_offset / 1024);
		
	}while(br != 0);
	
	/* 校验app crc */
	if(crc != tag.app_crc)
	{
		DEBUG_LOG("App crc error, cal:0x%08x, saved:0x%08x\r\n", crc, tag.app_crc);
		f_close(&fsrc);
		return HAL_ERROR;
	}
	DEBUG_LOG("App crc ok\r\n");
	
	/* 校验app长度 */
	if(current_offset != tag.app_len)
	{
		DEBUG_LOG("App length error, cal:%d, saved:%d\r\n", current_offset, tag.app_len);
		f_close(&fsrc);
		return HAL_ERROR;
	}
	DEBUG_LOG("App length ok\r\n");
	
	f_close(&fsrc);
	
	DEBUG_LOG("Firmware verification passed\r\n");
	
	return HAL_OK;
}

HAL_StatusTypeDef lime_del_fatfs_app(void)
{
	if(Lime_file_delete("D:/update.bin") != FR_OK)
		return HAL_ERROR;
	
	return HAL_OK;
}

// 检查固件是否已签名
#define APP_START_ADDRESS 0x08010000
bool lime_is_app_signed(void)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
	
	// 读取tag信息
	lime_boot_tag_t *tag = (lime_boot_tag_t *)(APP_START_ADDRESS + TAG_OFFSET);
	
	// 检查tag头
	if(memcmp(tag->head, LIME_BOOT_TAG_HEAD, 12))
	{
		DEBUG_LOG("Tag head error\r\n");
		return false;
	}
	
	// 检查tag尾
	if(memcmp(tag->end, LIME_BOOT_TAG_END, 4))
	{
		DEBUG_LOG("Tag end error\r\n");
		return false;
	}
	
	// 检查version是否为初始值
	if(tag->version == 0xffffffff && tag->app_len == 0xffffffff && 
	   tag->app_crc == 0xffffffff && tag->tag_crc == 0xffffffff)
	{
		DEBUG_LOG("App not signed (initial values)\r\n");
		return false;
	}
	
	return true;
}

// 验证Flash中的固件是否合法
HAL_StatusTypeDef lime_confirm_flash_signature(void)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
	
	// 检查固件是否已签名
	if(!lime_is_app_signed())
	{
		DEBUG_LOG("App not signed, skipping verification\r\n");
		return HAL_OK; // 未签名，不报错
	}
	
	// 读取tag信息
	lime_boot_tag_t *tag = (lime_boot_tag_t *)(APP_START_ADDRESS + TAG_OFFSET);
	
	// 校验tag crc
	uint32_t tag_crc = calc_crc32((uint8_t *)tag, offsetof(lime_boot_tag_t, tag_crc));
	if(tag_crc != tag->tag_crc)
	{
		DEBUG_LOG("Tag crc error, cal:0x%08x, saved:0x%08x\r\n", tag_crc, tag->tag_crc);
		return HAL_ERROR;
	}
	DEBUG_LOG("Tag crc ok\r\n");
	
	// 计算app crc（跳过tag）
	uint32_t crc = 0;
	uint32_t current_offset = 0;
	uint8_t *app_start = (uint8_t *)APP_START_ADDRESS;
	
	// 分块计算CRC
	while(current_offset < tag->app_len)
	{
		// 计算当前块的大小
		uint32_t chunk_size = CPY_BUFF_MAX;
		if(current_offset + chunk_size > tag->app_len)
		{
			chunk_size = tag->app_len - current_offset;
		}
		
		// 检查当前块是否包含tag
		if (current_offset <= TAG_OFFSET && current_offset + chunk_size > TAG_OFFSET)
		{
			// 计算tag开始和结束在当前块中的位置
			uint32_t tag_start_in_chunk = TAG_OFFSET - current_offset;
			uint32_t tag_end_in_chunk = tag_start_in_chunk + TAG_SIZE;
			
			// 确保tag_end_in_chunk不超过当前块大小
			if (tag_end_in_chunk > chunk_size)
			{
				tag_end_in_chunk = chunk_size;
			}
			
			// 计算tag之前的数据
			if (tag_start_in_chunk > 0)
			{
				crc = calc_crc32_chunk(crc, app_start + current_offset, tag_start_in_chunk);
			}
			
			// 跳过tag，直接计算tag之后的数据
			if (tag_end_in_chunk < chunk_size)
			{
				crc = calc_crc32_chunk(crc, app_start + current_offset + tag_end_in_chunk, chunk_size - tag_end_in_chunk);
			}
		}
		else
		{
			// 整个块都在tag之外，直接计算
			crc = calc_crc32_chunk(crc, app_start + current_offset, chunk_size);
		}
		
		current_offset += chunk_size;
		DEBUG_LOG("Processed %d kbytes\r\n", current_offset / 1024);
	}
	
	// 校验app crc
	if(crc != tag->app_crc)
	{
		DEBUG_LOG("App crc error, cal:0x%08x, saved:0x%08x\r\n", crc, tag->app_crc);
		return HAL_ERROR;
	}
	DEBUG_LOG("App crc ok\r\n");
	
	DEBUG_LOG("Flash firmware verification passed\r\n");
	
	return HAL_OK;
}

HAL_StatusTypeDef lime_jump_app(uint32_t address)
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
	
	return HAL_OK;
}

HAL_StatusTypeDef lime_re_create_filesystem(void)
{
	/* re-create file system */
	return file_system_ReInit();
}

