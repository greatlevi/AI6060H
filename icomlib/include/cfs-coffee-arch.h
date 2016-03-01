/**
 * \addtogroup mbxxx-platform
 *
 * @{
 */

/*
 * Copyright (c) 2010, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file
 *  Coffee architecture-dependent header for the STM32W108-based mb851
 *  platform. STM32W108 has 128KB of program flash.
 * \author
 *  Salvatore Pitrulli <salvopitru@users.sourceforge.net>
 */

#ifndef CFS_COFFEE_ARCH_H
#define CFS_COFFEE_ARCH_H

#include "contiki-conf.h"
#include "drv_flash.h"
#include "ssv_config.h"

//#include "hal/error.h"
//#include "hal/micro/cortexm3/flash.h"

/* 
 * STM32W108 has 128KB of program flash in 128 pages of 1024 bytes each = 128KB.
 * The smallest erasable unit is one page and the smallest writable
 * unit is an aligned 16-bit half-word.
 */
/* Byte page size, starting address on page boundary, and size of file system */

#define KB (1024)
#define FLASH_START               (0x0000)
/* Minimum erasable unit. */
#define CABRIO_WINBOND_SECTOR_SIZE           (4*KB)		//Todo: This is the sector size of winbond W25Q40CL
//#define FLASH_PAGE_SIZE           (4*KB)		//Todo: This is the sector size of winbond W25Q40CL
/* Last 3 pages reserved for NVM. */
#define FLASH_PAGES               (128)			//Todo: you can enlarbe it to 128,we now only setup as 124KB
//#define FLASH_PAGES               (32+4+8)			//Todo: you can enlarbe it to 128,we now only setup as 124KB



/* Minimum reservation unit for Coffee. It can be changed by the user.  */
#define COFFEE_PAGE_SIZE          (FLASH_PAGE_SIZE/4)

/*
 * If using IAR, COFFEE_ADDRESS reflects the static value in the linker script
 * iar-cfg-coffee.icf, so it can't be passed as a parameter for Make.
 */

//#define CABRIO_COFFEE_FS_MARKER_ADDRESS            (FLASH_START+ 384*KB)	//set up file-system offset as 128KB
#define  FLASH_ADDRESS_MASK (0xfcffffff)    //base flash address from ssv_config would be 0x0300-0000,but in the coeffee file system,it should be 0x0000-0000
#define CABRIO_COFFEE_FS_MARKER_ADDRESS            (CFG_FILE_SYSTEM_ADDR & FLASH_ADDRESS_MASK)	//set up file-system offset as 128KB


#define COFFEE_ADDRESS            (CABRIO_COFFEE_FS_MARKER_ADDRESS  + FLASH_PAGE_SIZE)	//set up file-system offset as 128KB
#if (COFFEE_ADDRESS & 0x3FF) != 0
#error "COFFEE_ADDRESS not aligned to a 1024-bytes page boundary."
#endif

#define COFFEE_PAGES              ((FLASH_PAGES*FLASH_PAGE_SIZE-               \
                 (COFFEE_ADDRESS-FLASH_START))/COFFEE_PAGE_SIZE)
#define COFFEE_START              (COFFEE_ADDRESS & ~(COFFEE_PAGE_SIZE-1))
#define COFFEE_SIZE               (COFFEE_PAGES*COFFEE_PAGE_SIZE)

/* These must agree with the parameters passed to makefsdata */
#define COFFEE_SECTOR_SIZE        FLASH_PAGE_SIZE
#define COFFEE_NAME_LENGTH        20

/* These are used internally by the AVR flash read routines */
/* Word reads are faster but take 130 bytes more PROGMEM */
 /* #define FLASH_WORD_READS          1 */
/*
 * 1 = Slower reads, but no page writes after erase and 18 bytes less PROGMEM. 
 * Best for dynamic file system
 */
 /* #define FLASH_COMPLEMENT_DATA     0 */

/* These are used internally by the coffee file system */
#define COFFEE_MAX_OPEN_FILES     (4)
#define COFFEE_FD_SET_SIZE        8
#define COFFEE_DYN_SIZE           (COFFEE_PAGE_SIZE*1)
/* Micro logs are not needed for single page sectors */
#define COFFEE_MICRO_LOGS         0
#define COFFEE_LOG_TABLE_LIMIT    16    /* It doesnt' matter as */
#define COFFEE_LOG_SIZE           128   /* COFFEE_MICRO_LOGS is 0. */

#if COFFEE_PAGES <= 127
#define coffee_page_t int8_t
#elif COFFEE_PAGES <= 0x7FFF
#define coffee_page_t int16_t
#endif

//#define CABRIO_WRITE_FS_MARKER(buf, size) 
//       cabrio_flash_write(CABRIO_COFFEE_FS_MARKER_ADDRESS, buf, size)

#define CABRIO_WRITE_FS_MARKER(buf, size) \
       cabrio_flash_write_only(CABRIO_COFFEE_FS_MARKER_ADDRESS, buf, size)


#define CABRIO_READ_FS_MARKER(buf, size) \
       cabrio_flash_read(CABRIO_COFFEE_FS_MARKER_ADDRESS, buf, size)


//#define COFFEE_WRITE(buf, size, offset) 
//       cabrio_flash_write(COFFEE_START + offset, buf, size)

#define COFFEE_WRITE(buf, size, offset) \
       cabrio_flash_read_erase_write(COFFEE_START + offset, buf, size)


#define COFFEE_READ(buf, size, offset) \
        cabrio_flash_read(COFFEE_START + offset, buf, size)

//####WARNING: this is not a erase only!! it's erase + write zero.####
#define COFFEE_ERASE(sector) \
        cabrio_flash_erase(sector)

#define COFFEE_ERASE_MARKER() \
        cabrio_flash_erase_marker_sector()


#define COFFEE_FLASH_INIT() \
        cabrio_flash_init()

unsigned int cabrio_flash_get_base_adddr();

#if 0
int cabrio_flash_read(uint32_t address, void *data, uint32_t length);
int cabrio_flash_write(uint32_t address, const void *data, uint32_t length);
int cabrio_flash_erase(uint8_t sector);
#endif

//int coffee_file_test(void);

#endif /* !COFFEE_ARCH_H */
/** @} */
