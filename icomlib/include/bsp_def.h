#ifndef _BSP_DEF_H_
#define _BSP_DEF_H_

#include <ssv_types.h>

#define BOOT_INFO_MAGIC_NUMBER (0x5e11aa11) 
#define MP_INFO_MAGIC_NUMBER (0x5e11aa11) 
#define CONFIG_INFO_MAGIC_NUMBER (0x5e11aa11) 

typedef struct _BOOT_INFO{
   U32 tbeg_src;
   U32 tbeg_dst;
   U32 tend_dst;
   U32 dbeg_src;
   U32 dbeg_dst;
   U32 dend_dst;
   U32 bss_beg;
   U32 bss_end;
   U32 stack_end;
   U32 magic_num;       //     .word 0x5e11aa11
   U32 version;             //     .word 0x00010001    
   U32 und_stack_size;
   U32 abt_stack_size;
   U32 fiq_stack_size;
   U32 irq_stack_size;
   U32 svc_stack_size;
   U32 flash_size;
   U32 ram_size;
} BOOT_INFO;


typedef struct _MP_INFO{
   U32 magic_num;       //     .word 0x5e11aa11
   U32 table_num;
} MP_INFO;

typedef struct _CONFIG_INFO{
   U32 magic_num;       //     .word 0x5e11aa11
   U32 table_num;
} CONFIG_INFO;



#endif  //_BSP_DEF_H_
