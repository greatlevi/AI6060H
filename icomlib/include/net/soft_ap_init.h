#ifndef _SOFT_AP_INIT_H
#define _SOFT_AP_INIT_H

#include <ssv_types.h>
#include <ssv_lib.h>
#include "uip_arp.h"

//void softap_set_gateway_and_client_iprange(U8 gw_digit0,U8 gw_digit1,U8 gw_digit2);
S32 softap_init();
S32 softap_dhcp_init(U8 gw_digit0,U8 gw_digit1,U8 gw_digit2);
S32 softap_init_ex(U8 gw_digit0,U8 gw_digit1,U8 gw_digit2);

#endif //_SOFT_AP_INIT_H
