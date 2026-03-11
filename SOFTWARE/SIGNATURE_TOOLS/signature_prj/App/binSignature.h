#ifndef __SIGNATURE_H__
#define __SIGNATURE_H__

#include <stdint.h>
#include "main.h"

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

uint32_t calc_crc32(const uint8_t *buf, uint32_t size);
uint32_t calc_crc32_chunk(uint32_t crc, const uint8_t *buf, uint32_t size);

uint32_t bin_signature(uint8_t *binBuff, uint32_t binLen, uint32_t version);
bool bin_check_signature(uint8_t *binBuff, lime_boot_tag_t *outres);

#endif /* __SIGNATURE_H__ */
