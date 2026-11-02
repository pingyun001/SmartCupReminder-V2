#include "spi_flash.h"
#include "spi_flash_test.h"

#include <string.h>

uint8_t testTxBuf[512] __attribute__((aligned(32))) = {0};
uint8_t testRxBuf[512] __attribute__((aligned(32))) = {0};

static float dwtSpeedTester(uint8_t cmd);

static void printfBuf(uint8_t* buf, uint32_t length)
{
	DEBUG_LOG("Buf:\n");
	for(uint32_t i = 0; i < length; i++)
	{
		DEBUG_LOG("0x%02x,",buf[i]);
		if(i % 16 == 15)
			DEBUG_LOG("\n");
	}
	DEBUG_LOG("end\n");
}

void W25QFlash_SmallDataCheck(void)
{
	DEBUG_LOG("%s()\n", __func__);
	
	memset(testTxBuf, 0, sizeof(testTxBuf));
	memset(testRxBuf, 0, sizeof(testRxBuf));
	
	for(uint32_t i = 0; i < sizeof(testTxBuf); i++)
		testTxBuf[i] = i;
	
	if(spi_flash_erase_sector(0) != HAL_OK)
		DEBUG_LOG("Erase Err\n");
	else
		DEBUG_LOG("Erase Success!\n");
	
	if(spi_flash_write(testTxBuf, 0, sizeof(testTxBuf)) != HAL_OK)
		DEBUG_LOG("Write Err\n");
	else
		DEBUG_LOG("Write Success!\n");
	
	if(spi_flash_read(testRxBuf, 0, sizeof(testRxBuf)) != HAL_OK)
		DEBUG_LOG("Read Err\n");
	else
		DEBUG_LOG("Read Success!\n");
	
	printfBuf(testRxBuf, sizeof(testRxBuf));
}


static uint32_t getRandomData(uint32_t* seed) 
{
  uint32_t x = *seed;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  *seed = x;
  return x;
}

uint32_t temp4KBuf[1024];				/* 4K byte buffer */

void W25QFlash_FullChipCheck(uint32_t totalSize)
{
  uint32_t seedA = 0x12345678;
  uint32_t i = 0;
  
  DEBUG_LOG("func:%s(totalSize:%.6f MB), start!\n", __func__, totalSize / 1024.0f / 1024.0f);
  
  for(i = 0; i < totalSize / 4; i++)
  {
    temp4KBuf[i % 1024] = getRandomData(&seedA);
		
		if(i % 1024 == 1023)		/* reached 4K boundary */
		{
			spi_flash_erase_sector((i - 1023) * 4);
			spi_flash_write((uint8_t*)temp4KBuf,  (i - 1023) * 4, 4096);
		}
    
		if(((i * 4) % (100 * 1024)) == (100 * 1024 - 4))
    {
      DEBUG_LOG("write random num %d kB, %.3f MB\n", (i + 1) * 4 / 1024, (float)(i + 1) * 4.0f / 1024.0f / 1024.0f);
    }
  }
  
  DEBUG_LOG("write random num %d kB, %.3f MB\n", (i + 1) * 4 / 1024, (float)(i + 1) * 4.0f / 1024.0f / 1024.0f);
  DEBUG_LOG("Write random Finish\n");
  
  seedA = 0x12345678;
  for(i = 0; i < totalSize / 4; i++)
  {
		if(i % 1024 == 0)
		{
			spi_flash_read((uint8_t*)temp4KBuf, i * 4, 4096);
		}
		
    uint32_t randomNum = getRandomData(&seedA);
    if(temp4KBuf[i % 1024] != randomNum)
    {
      DEBUG_LOG("Address:%#x, failed!,read:%#x, target:%#x\n", i * 4, temp4KBuf[i], randomNum);
    }
    
    if(((i * 4) % (100 * 1024)) == (100 * 1024 - 4))
    {
      DEBUG_LOG("check memory %d kB, %.3f MB\n", (i + 1) * 4 / 1024, (float)(i + 1) * 4.0f / 1024.0f / 1024.0f);
    }
  }
  DEBUG_LOG("check memory %d kB, %.3f MB\n", (i + 1) * 4 / 1024, (float)(i + 1) * 4.0f / 1024.0f / 1024.0f);
	
	DEBUG_LOG("%s()finish\n", __func__);
}


static float dwtSpeedTester(uint8_t cmd)
{
#define TOTAL_TRANS_DATA_BYTES	(32* 1024 * 256)
    static uint32_t start_time = 0;
    static uint32_t end_time = 0;
    static uint8_t is_timing = 0;
    uint32_t elapsed_time = 0;

    if ((CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk) == 0) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    switch (cmd) {
        case 1:
            DWT->CYCCNT = 0;
            start_time = DWT->CYCCNT;
            is_timing = 1;
            break;

        case 2:
            if (is_timing) {
                end_time = DWT->CYCCNT;
                is_timing = 0;
            }
            return end_time - start_time;

        case 3:
            if (!is_timing && end_time >= start_time) {
                elapsed_time = end_time - start_time;
                float time_seconds = (float)elapsed_time / SystemCoreClock;
#if	(!TOTAL_TRANS_DATA_BYTES)
                DEBUG_LOG("Time elapsed: %.4f ms\n", time_seconds * 1000.0f);
#else
																float speed = (float)TOTAL_TRANS_DATA_BYTES / (time_seconds * 1000000.0f);  /* convert bytes to MB, calc transfer speed */
                DEBUG_LOG("Time elapsed: %.4f ms, totalSpeed: %.4f MBytes/s\n", time_seconds * 1000.0f, speed);
#endif
                return time_seconds * 1000.0f;
            }
            break;

        default:
            elapsed_time = 0;
            break;
    }

    return 0;
}

uint32_t tempTestSpeedBuffer[32 * 1024 / 4] = {0};		//32kB read Speed Test Buffer

void W25QFlash_ReadSpeedTest(void)
{
	DEBUG_LOG("%s()\n", __func__);
	dwtSpeedTester(1);
	for(uint32_t i = 0; i < 256; i++)
		spi_flash_read((uint8_t*)tempTestSpeedBuffer, 0, 32 * 1024);
	dwtSpeedTester(2);
	dwtSpeedTester(3);
}

void W25QFlash_AIPReadSpeedTest(void)
{
	DEBUG_LOG("%s()\n", __func__);
	
	uint64_t i = 0;
	
	volatile uint64_t j = 0;
	volatile uint64_t *pBuf = NULL;
	
	pBuf = (uint64_t*)(0x90000000);
	
	dwtSpeedTester(1);
	
	for(i = 0; i < 8 * 1024 * 1024 / 8 / 64; i++)
	{
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
//
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
//
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
//
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;					//32
//    
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
//
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
//
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
//
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;
		j = *pBuf ++;					//64
	}
	
	dwtSpeedTester(2);
	dwtSpeedTester(3);
  
  DEBUG_LOG("EndPin:%#x\n", (uint32_t)pBuf);
}

void STM32InternalFlash_ReadSpeedTest(void)
{
	DEBUG_LOG("%s()\n", __func__);
	
	uint64_t i = 0;
	
	volatile uint64_t j = 0;
	volatile uint64_t *pBuf = NULL;
	
	dwtSpeedTester(1);
	
	
	/*8MB*/
	for(uint32_t p = 0; p < 8 * 1024 / 512; p++)
	{
		pBuf = (uint64_t*)(0x08000000);
		
		/*512KB*/
		for(i = 0; i < 512 * 1024 / 8 / 64; i++)
		{
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
	//
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
	//
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
	//
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;					//32
	//    
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
	//
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
	//
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
	//
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;
			j = *pBuf ++;					//64
		}
	}
	
	dwtSpeedTester(2);
	dwtSpeedTester(3);
  
  DEBUG_LOG("EndPin:%#x\n", (uint32_t)pBuf);
}



