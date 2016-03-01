#ifndef _SSV_LIB_H_
#define _SSV_LIB_H_

#include <clock.h>
#include <ssv_types.h>
#include <stdarg.h>  // for va_list
#include "dbg-printf.h"
#include <string.h>


#define SSV_DELAY(_t)        clock_delay(_t)


char mac_atoi(char *s);

//int ssv6xxx_strrpos(const char *str, char delimiter);

void *dma_memcpy(void *dest, const void *src, unsigned long count) ATTRIBUTE_SECTION_SRAM;
S32 printf(const S8 *format, ...);
S32 print(S8 **out, const S8 * const out_end, const S8 *_format, va_list args);

void dbg_dump_reg_pc();  /*todo: maybe we can put it in ssv_util.c"*/
void dbg_dump_reg_sp(); /*todo: maybe we can put it in ssv_util.c"*/
void dbg_dump_binary(U32 dump_addr,S32 length);
void dbg_dump_mac(S8 *msg,U8 *pmac);
void dbg_dump_ip(S8 *msg,U8 *p_ip);

#endif /* _SSV_LIB_H_ */
