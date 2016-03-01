#ifndef _BSP_H_
#define _BSP_H_

#include <ssv_lib.h>
#include <ssv_config.h>
#include <systemconf.h>


void bsp_get_boot_info(BOOL b_dump);
void bsp_get_define_info(BOOL b_dump);
void bsp_get_config_info(BOOL b_dump,COMMON_CONFIG *gp_config);
void bsp_get_mp_info();

int bsp_init(void);
int bsp_test(void);
void set_mrx_filter_sta();
void set_mrx_filter_softap(S32 null_data_ack);
void set_softap_trap_enable();

#endif /* _BSP_H_ */

