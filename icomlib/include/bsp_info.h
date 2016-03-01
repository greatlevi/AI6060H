#ifndef _BSP_INFO_H_
#define _BSP_INFO_H_

#include <ssv_types.h>
#include <systemconf.h>

void bsp_get_boot_info(BOOL b_dump);
U32 get_boot_stack_fence();
void bsp_get_mp_info(BOOL b_dump);
void bsp_get_define_info(BOOL b_dump);
void bsp_get_config_info(BOOL b_dump,COMMON_CONFIG *gp_config);


#endif //_BSP_INFO_H_
