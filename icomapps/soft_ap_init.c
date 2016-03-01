#include "soft_ap_init.h"


#include "ssv_config.h"
#include "systemconf.h"
#include "dhcps.h"
#include "soft_ap_def.h"


#define ETH_ALEN (6)

extern IEEE80211STATUS gwifistatus;

struct dhcps_config g_softap_config;
//struct dhcps_client_lease g_softap_lease;
struct dhcps_client_lease g_softap_lease[CFG_SOFTAP_MAX_STA_NUM];

extern void softap_reset_sta_all(); //to-do refine the soft ap hierachy!!

/*---------------------------------------------------------------------------*/
#if 0
void softap_reset_all()
{
    gwifistatus.softap_enable= 0;
    gwifistatus.softap_state = SOFTAP_OFF;
}
#endif
/*---------------------------------------------------------------------------*/
void softap_combine_ssid_mac(char *pSsid)
{
    S16 loop_i = 0;
    S16 loop_j = 0;
    U8 mac_ch;
    U8 mac_hex;

    *(pSsid++) = '-';   //seperator character between "ssid" and "MAC address"

    for(loop_i=0;loop_i<ETH_ALEN;loop_i++) {
        mac_ch = gwifistatus.local_mac[loop_i];
        for(loop_j=0;loop_j<2;loop_j++){
            if(loop_j==0){
                mac_hex  = (mac_ch>>4) & 0xf;
            } else {
                mac_hex  = (mac_ch>>0) & 0xf;
            }

            if( (mac_hex >=0) && (mac_hex <=9) ){   //character 0~0
                *(pSsid++) = '0' + mac_hex;
            } else {            //must be character 'a'~'f'
                *(pSsid++) = 'a' + mac_hex - 10;
            }

        }
    }

    //printf("pssid=%s\n",pSsid-13);

}

/*---------------------------------------------------------------------------*/
void softap_dump_dhcp_setting()
{
  S32 loop_i = 0;
    printf("------------------DHCP setting-----------------\n");
    printf("num_leases = %d\n",g_softap_config.num_leases);


    for( loop_i=0; loop_i < CFG_SOFTAP_MAX_STA_NUM; loop_i++)
    {
        printf("[%d] flasgs=%d,ip=%d.%d.%d.%d lease_end=%d\n", loop_i
                                                                                        ,g_softap_lease[loop_i].flags
                                                                                        ,g_softap_lease[loop_i].ipaddr.u8[0]
                                                                                        ,g_softap_lease[loop_i].ipaddr.u8[1]
                                                                                        ,g_softap_lease[loop_i].ipaddr.u8[2]
                                                                                        ,g_softap_lease[loop_i].ipaddr.u8[3]
                                                                                        ,g_softap_lease[loop_i].lease_end);
    }

}
/*---------------------------------------------------------------------------*/
#if 0
void softap_set_gateway_and_client_iprange(U8 gw_digit0,U8 gw_digit1,U8 gw_digit2)
{
    S32 loop_i = 0;
    //S32 rlt = 0;

    uip_set_hostaddr(gw_digit0,gw_digit1,gw_digit2,1);  //xxx.xxx.xxx.1

    printf("<%s>%d.%d.%d\n",__func__,gw_digit0,gw_digit1,gw_digit2);
    
    //dns addr: no dns server being provided in this machine!!
    g_softap_config.dnsaddr.u8[0] = gw_digit0;
    g_softap_config.dnsaddr.u8[1] = gw_digit1;
    g_softap_config.dnsaddr.u8[2] = gw_digit2;
    g_softap_config.dnsaddr.u8[3] = 1;

    //default router: 192.168.0.1
    g_softap_config.default_router.u8[0] = gw_digit0;
    g_softap_config.default_router.u8[1] = gw_digit1;
    g_softap_config.default_router.u8[2] = gw_digit2;
    g_softap_config.default_router.u8[3] = 1;


    //i_lease.chaddr don't care specific mac address,so not setting here!!
    for( loop_i=0; loop_i < CFG_SOFTAP_MAX_STA_NUM; loop_i++)
    {
        memset( &(g_softap_lease[loop_i].chaddr[0]),0x0,ETH_ALEN);
        g_softap_lease[loop_i].flags = LEASE_FLAGS_VALID;
        g_softap_lease[loop_i].ipaddr.u8[0] = gw_digit0;
        g_softap_lease[loop_i].ipaddr.u8[1] = gw_digit1;
        g_softap_lease[loop_i].ipaddr.u8[2] = gw_digit2;
        g_softap_lease[loop_i].ipaddr.u8[3] = 10 + loop_i;
        g_softap_lease[loop_i].lease_end = 0;  //will be set when allocate_address
    }
    

}
#endif
/*---------------------------------------------------------------------------*/
S32 softap_dhcp_init(U8 gw_digit0,U8 gw_digit1,U8 gw_digit2)
{
    S32 rlt = 0;
    S32 loop_i = 0;

    uip_set_hostaddr( gw_digit0, gw_digit1, gw_digit2, 1); //set our own ip address to uip
    uip_arp_softap_init(&gwifistatus.local_mac[0]);
    
    g_softap_config.default_lease_time = CFG_SOFTAP_DHCP_LEASE_TIME;  // 1 day = 3600 seconds!!

    icomprintf(GROUP_SOFTAP,LOG_INFO,"<%s>g_softap_config.default_lease_time = %d,addr=0x%x\n",__func__,g_softap_config.default_lease_time, &g_softap_config);
    icomprintf(GROUP_SOFTAP,LOG_INFO,"addr (&g_softap_config,&g_softap_config.default_lease_time) = (0x%x,0x%x)\n", &g_softap_config, &g_softap_config.default_lease_time);

    //netmask: 255.255.255.0
    g_softap_config.netmask.u8[0] = 255;
    g_softap_config.netmask.u8[1] = 255;
    g_softap_config.netmask.u8[2] = 255;
    g_softap_config.netmask.u8[3] = 0;

    //dns addr: no dns server being provided in this machine!!
    g_softap_config.dnsaddr.u8[0] = gw_digit0;
    g_softap_config.dnsaddr.u8[1] = gw_digit1;
    g_softap_config.dnsaddr.u8[2] = gw_digit2;
    g_softap_config.dnsaddr.u8[3] = 1;

    //default router: 192.168.0.1
    g_softap_config.default_router.u8[0] = gw_digit0;
    g_softap_config.default_router.u8[1] = gw_digit1;
    g_softap_config.default_router.u8[2] = gw_digit2;
    g_softap_config.default_router.u8[3] = 1;

    
    //i_lease.chaddr don't care specific mac address,so not setting here!!
    for( loop_i=0; loop_i < CFG_SOFTAP_MAX_STA_NUM; loop_i++)
    {
        memset( &(g_softap_lease[loop_i].chaddr[0]),0x0,ETH_ALEN);
        g_softap_lease[loop_i].flags = LEASE_FLAGS_VALID;
        g_softap_lease[loop_i].ipaddr.u8[0] = gw_digit0;
        g_softap_lease[loop_i].ipaddr.u8[1] = gw_digit1;
        g_softap_lease[loop_i].ipaddr.u8[2] = gw_digit2;
        g_softap_lease[loop_i].ipaddr.u8[3] = 10 + loop_i;
        g_softap_lease[loop_i].lease_end = 0;  //will be set when allocate_address
    }
    
   
    g_softap_config.leases = &g_softap_lease[0];
    g_softap_config.num_leases = CFG_SOFTAP_MAX_STA_NUM;  //only one client can lease!!

    softap_dump_dhcp_setting();

    //g_softap_config.flags = DHCP_CONF_NETMASK;  //or it won't be added into DHCP R_ACK
    g_softap_config.flags = DHCP_CONF_NETMASK | DHCP_CONF_DEFAULT_ROUTER | DHCP_CONF_DNSADDR ;  //or it won't be added into DHCP R_ACK

    dhcps_init(&g_softap_config);

    return rlt;
}
/*---------------------------------------------------------------------------*/
S32 softap_init_ex(U8 gw_digit0,U8 gw_digit1,U8 gw_digit2)
{
    S32 rlt = 0;

    //config from #defne
    if( (gwifistatus.softap_channel==0) || (gwifistatus.softap_channel>15) )
    {
        gwifistatus.softap_channel = CFG_SOFTAP_CHANNEL_NO;
    }        

    gwifistatus.softap_beacon_interval = CFG_SOFTAP_BEACON_INTERVAL;

    if(gwifistatus.softap_ack_timeout < CFG_SOFTAP_ACK_TIMEOUT)
    {
        gwifistatus.softap_ack_timeout  = CFG_SOFTAP_ACK_TIMEOUT;
    }

    gwifistatus.softap_data_timeout  = CFG_SOFTAP_DATA_TIMEOUT;


    if(0==gwifistatus.softap_ssid_length)
    {
    //    gwifistatus.softap_ssid_length = strlen(&gwifistatus.softap_ssid[0]);
        memset(&gwifistatus.softap_ssid[0],0,CFG_SOFTAP_SSID_LENGTH);
        memcpy(&gwifistatus.softap_ssid[0],CFG_SOFTAP_SSID, strlen(CFG_SOFTAP_SSID) );
        
        softap_combine_ssid_mac( (&gwifistatus.softap_ssid[0]) + strlen(CFG_SOFTAP_SSID) );
        gwifistatus.softap_ssid_length = strlen( (&gwifistatus.softap_ssid[0]));
        //dbg_dump_mac("local_mac", &gwifistatus.local_mac[0]);
    }

    rlt = softap_dhcp_init(gw_digit0,gw_digit1,gw_digit2);
    if(rlt!=0)
    {
        //rlt = -1;
        goto error_exit;
    }

    //softap_reset_all();
    softap_reset_sta_all();
    
    return rlt;

error_exit:
    return rlt;
}
/*---------------------------------------------------------------------------*/
S32 softap_init()
{
    S32 rlt = 0;

    rlt = softap_init_ex( 192, 168, 0); //it means to set gateway as 192.168.0.1

    return rlt;
}    
/*---------------------------------------------------------------------------*/

