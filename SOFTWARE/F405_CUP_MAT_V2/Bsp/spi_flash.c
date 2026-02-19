#include "spi_flash.h"
#include <string.h>
#include <stdio.h>
#include "spi.h"

#if 0
#define SPI_FLASH_LOG DEBUG_LOG
#else
#define SPI_FLASH_LOG(...)
#endif

static HAL_StatusTypeDef spi_flash_reset(void);
static HAL_StatusTypeDef spi_flash_read_id(uint8_t jedec_id[3]);
static HAL_StatusTypeDef spi_flash_read_uid(uint8_t uid[8]);
static HAL_StatusTypeDef spi_flash_wait_busy(uint32_t timeout_ms);

static uint8_t cmd_tx_buf[1 + 3 + 10] = {0};
static uint8_t cmd_rx_buf[1 + 3 + 10] = {0};

static spi_flash_info_t spi_flash_info = {0};

HAL_StatusTypeDef spi_flash_do_cmd(spi_flash_do_cmd_t *p_cmd)
{
    uint32_t start_time = 0;
	
    if(p_cmd == NULL)
        return HAL_ERROR;

    if(p_cmd->dummy_len > 10)
        return HAL_ERROR;

    uint8_t first_len = 0;

    /* let CS Low */
    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);

    /* fill CMD */
    memset(cmd_tx_buf, 0xff, sizeof(cmd_tx_buf));
    cmd_tx_buf[0] = p_cmd->cmd;
    first_len += 1;

    /* fill ADDR */
    if(p_cmd->is_addr_exist)
    {
        cmd_tx_buf[1] = (p_cmd->addr >> 16) & 0xFF;
        cmd_tx_buf[2] = (p_cmd->addr >> 8) & 0xFF;
        cmd_tx_buf[3] = p_cmd->addr & 0xFF;
        first_len += 3;
    }

    /* fill DUMMY */
    first_len += p_cmd->dummy_len;
    HAL_SPI_TransmitReceive_DMA(&hspi2, cmd_tx_buf, cmd_rx_buf, first_len);
	
    start_time = HAL_GetTick();
    while(hspi2.State!= HAL_SPI_STATE_READY)
    {
			if(HAL_GetTick() - start_time > 1000)
			{
				SPI_FLASH_LOG("spi flash timeout1...\n");
				return HAL_ERROR;
			}
	}

    /* trans DATA */
    if(p_cmd->data_len > 0)
    {
        if(p_cmd->tx_data != NULL && p_cmd->rx_data == NULL)
            HAL_SPI_Transmit_DMA(&hspi2, p_cmd->tx_data, p_cmd->data_len);
        
        else if(p_cmd->tx_data == NULL && p_cmd->rx_data != NULL)
            HAL_SPI_Receive_DMA(&hspi2, p_cmd->rx_data, p_cmd->data_len);

        else if(p_cmd->tx_data != NULL && p_cmd->rx_data != NULL)
            HAL_SPI_TransmitReceive_DMA(&hspi2, p_cmd->tx_data, p_cmd->rx_data, p_cmd->data_len);

        else 
        {
            /* let CS High */
            HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);
            SPI_FLASH_LOG("err\n");
            return HAL_ERROR;
        }
	
				start_time = HAL_GetTick();
				while(hspi2.State!= HAL_SPI_STATE_READY)
				{
						if(HAL_GetTick() - start_time > 1000)
						{
								SPI_FLASH_LOG("for debug:%#x, %#x, %d\n", (uint32_t)p_cmd->tx_data, (uint32_t)p_cmd->rx_data, p_cmd->data_len);
								SPI_FLASH_LOG("spi flash timeout2...\n");
								return HAL_ERROR;
						}
				}
    }

    /* let CS High */
    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

    return HAL_OK;
}

static HAL_StatusTypeDef spi_flash_reset(void)
{
    HAL_StatusTypeDef ret = HAL_OK;

    /* reset flash */
    spi_flash_do_cmd_t cmd_reset_en = 
    {
        .cmd = SPI_FLASH_CMD_RESET_ENABLE,
    };
    ret += spi_flash_do_cmd(&cmd_reset_en);
    
    spi_flash_do_cmd_t cmd_reset = 
    {
        .cmd = SPI_FLASH_CMD_RESET,
    };
    ret += spi_flash_do_cmd(&cmd_reset);

    return ret;
}


static HAL_StatusTypeDef spi_flash_read_id(uint8_t jedec_id[3])
{
    HAL_StatusTypeDef ret = HAL_OK;
    
    /* read JEDEC ID */
    spi_flash_do_cmd_t cmd_read_id = 
    {
        .cmd = SPI_FLASH_CMD_READ_ID,
        .rx_data = jedec_id,
        .data_len = 3,
    };
    ret += spi_flash_do_cmd(&cmd_read_id);

    return ret;
}

static HAL_StatusTypeDef spi_flash_read_uid(uint8_t uid[8])
{
    HAL_StatusTypeDef ret = HAL_OK;

    /* read UID */
    spi_flash_do_cmd_t cmd_read_uid = 
    {
        .cmd = SPI_FLASH_CMD_READ_UID,
        .dummy_len = 4,
        .rx_data = uid,
        .data_len = 8,
    };
    ret += spi_flash_do_cmd(&cmd_read_uid);

    return ret;
}

static HAL_StatusTypeDef spi_flash_wait_busy(uint32_t timeout_ms)
{
    uint32_t start_time = HAL_GetTick();

    uint8_t status_reg = 0;

    while(1)
    {
        /* read status register */
        spi_flash_do_cmd_t cmd_read_status = 
        {
            .cmd = SPI_FLASH_CMD_READ_STATUS,
            .rx_data = &status_reg,
            .data_len = 1,
        };
        spi_flash_do_cmd(&cmd_read_status);

        /* check busy bit */
        if((status_reg & 0x01) == 0)
            break;

        /* check timeout */
        if((HAL_GetTick() - start_time) > timeout_ms)
            return HAL_TIMEOUT;
    }

    return HAL_OK;
}

HAL_StatusTypeDef spi_flash_init(spi_flash_info_t *p_info)
{
    HAL_StatusTypeDef ret = HAL_OK;

    /* reset flash */
    ret += spi_flash_reset();

    /* read JEDEC ID */
    ret += spi_flash_read_id(spi_flash_info.jedec_id);
    if(ret == HAL_OK)
    {
        switch(spi_flash_info.jedec_id[2])
        {
            case 0x15:
                spi_flash_info.capacity = (16 / 8) * 1024 * 1024;
                break;
            case 0x16:
                spi_flash_info.capacity = (32 / 8) * 1024 * 1024;
                break;
            case 0x17:
                spi_flash_info.capacity = (64 / 8) * 1024 * 1024;
                break;
            case 0x18:
                spi_flash_info.capacity = (128 / 8) * 1024 * 1024;
                break;
            case 0x19:
                spi_flash_info.capacity = (256 / 8) * 1024 * 1024;
                break;
            default:
                spi_flash_info.capacity = 0;
                break;
        }
    }

    /* read UID */
    ret += spi_flash_read_uid(spi_flash_info.uid);
		
		/* copy p_info */
		if(p_info != NULL)
		{
			memcpy(p_info, &spi_flash_info, sizeof(spi_flash_info_t));
		}

    SPI_FLASH_LOG("JEDEC ID: %02X %02X %02X\n", spi_flash_info.jedec_id[0], spi_flash_info.jedec_id[1], spi_flash_info.jedec_id[2]);
    SPI_FLASH_LOG("Capacity: %d MB\n", spi_flash_info.capacity / (1024 * 1024));
    SPI_FLASH_LOG("UID: %02X %02X %02X %02X %02X %02X %02X %02X\n", spi_flash_info.uid[0], spi_flash_info.uid[1], spi_flash_info.uid[2], spi_flash_info.uid[3], spi_flash_info.uid[4], spi_flash_info.uid[5], spi_flash_info.uid[6], spi_flash_info.uid[7]);

    return ret;
}
HAL_StatusTypeDef spi_flash_erase_sector(uint32_t sector_addr)
{
    HAL_StatusTypeDef ret = HAL_OK;

    /* write enable */
    spi_flash_do_cmd_t cmd_write_enable = 
    {
        .cmd = SPI_FLASH_CMD_WRITE_ENABLE,
    };
    ret += spi_flash_do_cmd(&cmd_write_enable);

    /* erase sector */
    spi_flash_do_cmd_t cmd_erase_sector = 
    {
        .cmd = SPI_FLASH_CMD_SECTOR_ERASE,
        .is_addr_exist = true,
        .addr = sector_addr,
    };
    ret += spi_flash_do_cmd(&cmd_erase_sector);

    /* wait busy */
    spi_flash_wait_busy(500);

    return ret;
}
HAL_StatusTypeDef spi_flash_write(uint8_t *p_data, uint32_t addr, uint32_t len)
{
    HAL_StatusTypeDef ret = HAL_OK;

    uint32_t step_size = 0;
    uint32_t total_transfered_size = 0;
    uint32_t elaps_size = len;
    while(1)
    {
        step_size = elaps_size > 256 ? 256 : elaps_size;

        /* write enable */
        spi_flash_do_cmd_t cmd_write_enable = 
        {
            .cmd = SPI_FLASH_CMD_WRITE_ENABLE,
        };
        ret += spi_flash_do_cmd(&cmd_write_enable);

        /* program page */
        spi_flash_do_cmd_t cmd_program_page = 
        {
            .cmd = SPI_FLASH_CMD_PAGE_PROGRAM,
            .is_addr_exist = true,
            .addr = addr + total_transfered_size,
            .tx_data = p_data + total_transfered_size,
            .data_len = step_size,
        };
        ret += spi_flash_do_cmd(&cmd_program_page);

        /* wait busy */
        ret += spi_flash_wait_busy(100);

        if(ret != HAL_OK)
            return ret;

        total_transfered_size += step_size;
        elaps_size -= step_size;

        if(elaps_size == 0)
            break;
    }

    return HAL_OK;
}

static uint8_t rx_cache_buf[4096] __attribute__((aligned(4))) = {0};
HAL_StatusTypeDef spi_flash_read(uint8_t *p_data, uint32_t addr, uint32_t len)
{
    HAL_StatusTypeDef ret = HAL_OK;

    uint32_t step_size = 0;
    uint32_t total_transfered_size = 0;
    uint32_t elaps_size = len;
    const uint32_t gap = 4096;

    while(1)
    {
        step_size = elaps_size > gap ? gap : elaps_size;

        /* read data */
        spi_flash_do_cmd_t cmd_read_data = 
        {
            .cmd = SPI_FLASH_CMD_DATA_READ,
            .is_addr_exist = true,
            .addr = addr + total_transfered_size,
//            .rx_data = p_data + total_transfered_size,
						.rx_data = rx_cache_buf,
            .data_len = step_size,
        };
        ret += spi_flash_do_cmd(&cmd_read_data);
				
				memcpy(p_data + total_transfered_size, rx_cache_buf, step_size);

        if(ret != HAL_OK)
            return ret;

        total_transfered_size += step_size;
        elaps_size -= step_size;

        if(elaps_size == 0)
            break;
    }

    return HAL_OK;
}

uint32_t spi_flash_get_capacity_bytes(void)
{
	if(spi_flash_info.capacity == 0)
	{
		spi_flash_init(NULL);
	}
	
	return spi_flash_info.capacity;
}

uint32_t spi_flash_get_sector_count(void)
{
	return spi_flash_get_capacity_bytes() / 4096;
}

uint32_t spi_flash_get_jedec_id(void)
{
	uint32_t jedec_id = 0;
	
	jedec_id = spi_flash_info.jedec_id[2] | (spi_flash_info.jedec_id[1] << 8) | (spi_flash_info.jedec_id[0] << 16);
	
	return jedec_id;
}

void spi_flash_dma_irq_hook(void)
{

}
