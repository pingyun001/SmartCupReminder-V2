#include "binSignature.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

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

static const lime_boot_tag_t default_tag = 
{
    .head = LIME_BOOT_TAG_HEAD,
    .end = LIME_BOOT_TAG_END,
};

uint32_t bin_signature(uint8_t *binBuff, uint32_t binLen, uint32_t version)
{
    lime_boot_tag_t *tagBuff = (lime_boot_tag_t *)(binBuff + 1024);

    /* check head */
    if(memcmp(tagBuff->head, default_tag.head, 12))
    {
        printf("tag head error\n");
        return 0;
    }
    else
        printf("head ok\n");

    /* check end */
    if(memcmp(tagBuff->end, default_tag.end, 4))
    {
        printf("tag end error\n");
        return 0;
    }
    else
        printf("tail end ok\n");

    /* fill version */
    tagBuff->version = version;
    printf("firmware version: 0x%08x\n", version);

    /* fill app length (aligned to 4 bytes) */
    uint32_t app_len = binLen;
    if (app_len % 4 != 0) {
        app_len += 4 - (app_len % 4);
    }
    tagBuff->app_len = app_len;
    printf("app length: 0x%08x\n", app_len);

    /* calculate app crc (skip tag itself) */
    uint32_t app_crc = 0;
    if (1024 < app_len) {
        // Calculate CRC for the first part before tag
        app_crc = calc_crc32(binBuff, 1024);
        // Calculate CRC for the part after tag
        if (1024 + sizeof(lime_boot_tag_t) < app_len) {
            uint32_t crc_part2 = calc_crc32(binBuff + 1024 + sizeof(lime_boot_tag_t), app_len - (1024 + sizeof(lime_boot_tag_t)));
            // Combine the two CRCs
            // Note: This is a simplified approach, in reality you might need to handle this differently
            // For demonstration purposes, we'll just calculate CRC for the entire app except tag
            // Create a temporary buffer to hold the app data without tag
            uint8_t *temp_buff = (uint8_t *)malloc(app_len - sizeof(lime_boot_tag_t));
            if (temp_buff) {
                // Copy data before tag
                memcpy(temp_buff, binBuff, 1024);
                // Copy data after tag
                memcpy(temp_buff + 1024, binBuff + 1024 + sizeof(lime_boot_tag_t), app_len - (1024 + sizeof(lime_boot_tag_t)));
                // Calculate CRC for the combined data
                app_crc = calc_crc32(temp_buff, app_len - sizeof(lime_boot_tag_t));
                free(temp_buff);
            }
        }
    } else {
        // App is smaller than tag offset, just calculate CRC for the entire app
        app_crc = calc_crc32(binBuff, app_len);
    }
    tagBuff->app_crc = app_crc;
    printf("app crc: 0x%08x\n", app_crc);

    /* calculate tag crc (include tag head, version, app_len, app_crc) */
    uint32_t tag_crc = calc_crc32((uint8_t *)tagBuff, offsetof(lime_boot_tag_t, tag_crc)); // exclude tag_crc and end field
    tagBuff->tag_crc = tag_crc;
    printf("tag crc: 0x%08x\n", tag_crc);

    return binLen;
}

bool bin_check_signature(uint8_t *binBuff, lime_boot_tag_t *outres)
{
    lime_boot_tag_t *tagBuff = (lime_boot_tag_t *)(binBuff + 1024);

    /* check head */
    if(memcmp(tagBuff->head, default_tag.head, 12))
    {
        printf("tag head error\n");
        return 0;
    }
    else
        printf("head ok\n");

    /* check end */
    if(memcmp(tagBuff->end, default_tag.end, 4))
    {
        printf("tag end error\n");
        return 0;
    }
    else
        printf("tail end ok\n");

    /* check tag crc */
    uint32_t tag_crc = calc_crc32((uint8_t *)tagBuff, offsetof(lime_boot_tag_t, tag_crc)); // exclude tag_crc and end field
    if(tag_crc != tagBuff->tag_crc)
    {
        printf("tag crc error, cal:0x%08x, saved:0x%08x\n", tag_crc, tagBuff->tag_crc);
        return 0;
    }
    else 
        printf("tag crc ok\n");
    
    /* check app crc */
    uint32_t app_crc = 0;
    uint32_t app_len = tagBuff->app_len;
    if (1024 < app_len) {
        // Create a temporary buffer to hold the app data without tag
        uint8_t *temp_buff = (uint8_t *)malloc(app_len - sizeof(lime_boot_tag_t));
        if (temp_buff) {
            // Copy data before tag
            memcpy(temp_buff, binBuff, 1024);
            // Copy data after tag
            if (1024 + sizeof(lime_boot_tag_t) < app_len) {
                memcpy(temp_buff + 1024, binBuff + 1024 + sizeof(lime_boot_tag_t), app_len - (1024 + sizeof(lime_boot_tag_t)));
            }
            // Calculate CRC for the combined data
            app_crc = calc_crc32(temp_buff, app_len - sizeof(lime_boot_tag_t));
            free(temp_buff);
        }
    } else {
        // App is smaller than tag offset, just calculate CRC for the entire app
        app_crc = calc_crc32(binBuff, app_len);
    }
    if(app_crc != tagBuff->app_crc)
    {
        printf("app crc error, cal:0x%08x, saved:0x%08x\n", app_crc, tagBuff->app_crc);
        return 0;
    }
    else 
        printf("app crc ok\n");

    /* output result */
    if(outres)
    {
        memcpy(outres, tagBuff, sizeof(lime_boot_tag_t));
    }

    return 1;
}