#ifndef _XMEM_H
#define _XMEM_H

#include "ssv_types.h"

/*-----------keep_in_flash-------------*/
void drv_flash_init();
void drv_dump_hw_spi_status();
int cabrio_flash_read_erase_write(U32 address, const void *data, U32 length);
int cabrio_flash_write_only(U32 address, void *data, U32 length);
int cabrio_flash_read(U32 address, void *data, U32 length);
int cabrio_flash_write(U32 address, const void *data, U32 length);
int cabrio_flash_erase(U8 sector);
S32 cabrio_flash_erase_marker_sector();
void cabrio_flash_init(void);




/*-----------keep_in_sram-------------*/
int drv_flash_page_program(unsigned int u32addr,void *data,unsigned int u32length) ATTRIBUTE_SECTION_SRAM;
int drv_flash_page_program_for_erase(unsigned int u32addr,void *data,unsigned int u32length) ATTRIBUTE_SECTION_SRAM;
unsigned int drv_flash_read_spi_status_regsiter(volatile unsigned int u32cmd,volatile unsigned int _u32cmd_len) ATTRIBUTE_SECTION_SRAM;
int drv_flash_sector_erase(unsigned int u32addr) ATTRIBUTE_SECTION_SRAM;
int drv_flash_wait_spi_busy() ATTRIBUTE_SECTION_SRAM;
int drv_flash_wait_spi_status_regsiter() ATTRIBUTE_SECTION_SRAM;
int drv_flash_write_cache_buffer(U32 address, void * data, U32 length) ATTRIBUTE_SECTION_SRAM;
int drv_flash_write_cmd(unsigned int u32cmd_len) ATTRIBUTE_SECTION_SRAM;
int drv_flash_write_enable() ATTRIBUTE_SECTION_SRAM;
int cabrio_flash_flush_cache_buffer(U32 address) ATTRIBUTE_SECTION_SRAM;

int cabrio_flash_read_erase_write_in_sector_range(U32 address, const void *data, U32 length) ATTRIBUTE_SECTION_SRAM;


#endif //_XMEM_H
