#ifndef __BSP_SPI_FLASH_H__
#define __BSP_SPI_FLASH_H__

#include "main.h"


#define SPI_FLASH_CMD_RESET_ENABLE  0x66
#define SPI_FLASH_CMD_RESET         0x99
#define SPI_FLASH_CMD_READ_ID       0x9F
#define SPI_FLASH_CMD_READ_STATUS   0x05
#define SPI_FLASH_CMD_WRITE_ENABLE  0x06
#define SPI_FLASH_CMD_READ_UID      0x4B
#define SPI_FLASH_CMD_PAGE_PROGRAM  0x02
#define SPI_FLASH_CMD_SECTOR_ERASE  0x20        //63ms
#define SPI_FLASH_CMD_DATA_READ     0x03




typedef struct
{
    uint8_t jedec_id[3];
    
    uint8_t uid[8];
    uint32_t capacity;          // in bytes
}spi_flash_info_t;

typedef struct
{
    uint8_t cmd;

    bool is_addr_exist;
    uint32_t addr;

    uint8_t dummy_len;

    uint32_t data_len;
    uint8_t *tx_data;
    uint8_t *rx_data;
}spi_flash_do_cmd_t;

HAL_StatusTypeDef spi_flash_do_cmd(spi_flash_do_cmd_t *p_cmd);

HAL_StatusTypeDef spi_flash_init(spi_flash_info_t *p_info);
HAL_StatusTypeDef spi_flash_erase_sector(uint32_t sector_addr);
HAL_StatusTypeDef spi_flash_write(uint8_t *p_data, uint32_t addr, uint32_t len);
HAL_StatusTypeDef spi_flash_read(uint8_t *p_data, uint32_t addr, uint32_t len);

void spi_flash_dma_irq_hook(void);

#endif /* __BSP_SPI_FLASH_H__ */
