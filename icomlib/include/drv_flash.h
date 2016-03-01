#ifndef DRV_FLASH_H
#define DRV_FLASH_H

#include "ssv_types.h"
#include "ssv_config.h"

#define FLASH_PAGE_SIZE           (4*KB)		//Todo: This is the sector size of winbond W25Q40CL
#define MAX_SPI_DATA_WRITE_SIZE (256)

#define SRAM_BOOT_FLASH_BASE_ADDR   (0x3000000)


/*
//initialization
S32 drv_spi_flash_init();
S32 drv_spi_flash_get_base_addr();

//Read
S32 drv_spi_flash_read(U32 addr,void *p_des);   //flash to sram

//Erase,Write
S32 drv_spi_flash_erase(U32 erase_type,U32 addr);
S32 drv_spi_flash_write(U32 addr,void* p_src,U32 size); //sram|flash to flash

//Custom command
S32 drv_spi_flash_cutom_cmd();
*/

void drv_flash_init();

U8* drv_flash_get_cache_addr();
U32 drv_flash_get_base_addr();
int drv_flash_sector_erase(unsigned int u32addr);

int drv_flash_page_program(unsigned int u32addr,void *data,unsigned int u32length);
int drv_flash_page_program_for_erase(unsigned int u32addr,void *data,unsigned int u32length);

int drv_flash_write_enable();

S32 drv_flash_read(U32 u32addr,void *data,U32 u32length);

#endif  //DRV_FLASH_H
