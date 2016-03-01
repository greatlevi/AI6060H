#ifndef FLASH_API_H
#define FLASH_API_H

#include "ssv_types.h"
#include "drv_flash.h"

//initialization
S32 spi_flash_init();

//U32 spi_flash_get_base_addr();

S32 spi_flash_read(U32 addr,void *p_des,S32 length);
S32 spi_flash_sector_erase(U32 addr);
S32 spi_flash_write(U32 addr,void* p_src,S32 length);
S32 spi_flash_finalize(U32 addr);


S32 spi_flash_test(U32 addr,S32 length);

S32 spi_flash_write_directly(U32 addr,void* p_src,S32 write_size);

S32 spi_flash_cutom_cmd();

#endif  //FLASH_API_H
