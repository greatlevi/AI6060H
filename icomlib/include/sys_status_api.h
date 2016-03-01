#ifndef _SYS_STATUS_API_H_
#define _SYS_STATUS_API_H_

#include "systemconf.h"

U8 get_local_mac(U8 *mac, U8 len);
U8 get_connectAP_mac(U8 *mac, U8 len);
ieee80211_state get_wifi_connect_status();
U8 get_wifi_connected();
U8 get_ap_lsit_total_num();
void set_slink_ready_and_ack(struct process *p);

void set_nslookup(char *pIp);

#endif