
#include "ssv_lib.h"
#include "sys_status_api.h"
#include "ieee80211_mac.h"
#include "ieee80211_mgmt.h"
#include "smartComm.h"

extern IEEE80211STATUS gwifistatus;

U8 get_local_mac(U8 *mac, U8 len)
{
    U8 status = 0;
    if(len > ETH_ALEN) {
        printf("over local mac len: (%d), please check\n", len);
        status = -1;
    } else {
        memcpy(mac, &(gwifistatus).local_mac[0], ETH_ALEN);
    }
    return status;
}

U8 get_connectAP_mac(U8 *mac, U8 len)
{
    U8 status = 0;
    if(len > ETH_ALEN) {
        printf("over local mac len: (%d), please check\n", len);
        status = -1;
    } else {
        memcpy(mac, &(gwifistatus).connAP.mac[0], ETH_ALEN);
    }
    return status;
}

ieee80211_state get_wifi_connect_status()
{
    return gwifistatus.status;
}

U8 get_wifi_connected()
{
    return gwifistatus.wifi_connect;
}

U8 get_ap_lsit_total_num()
{
    return getAvailableIndex();
}

void set_nslookup(char *pIp)
{
    bss_nslookup(pIp);
}

void set_slink_ready_and_ack(struct process *p)
{
    set_slink_ready_ack(p);
}