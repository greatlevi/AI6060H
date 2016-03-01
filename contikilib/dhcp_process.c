#include "systemconf.h"
#include "ssv_lib.h"
#include "dhcpc.h"
#include "smartConf.h"
#include "smartComm.h"
#include "airkiss.h"
#include "uip_arp.h"

extern TAG_CABRIO_CONFIGURATION gCabrioConf;
extern IEEE80211STATUS 			gwifistatus;
extern DHCP_STATE dhcpState;

PROCESS(dhcp_process, "Dhcp process");
#if (defined(SMART_ICOMM) || defined(SMART_WECHAT))
PROCESS_NAME(smart_conf_ack);
#endif
PROCESS_NAME(bss_reconnect_process);

void replyAck()
{
#if (defined(SMART_ICOMM) || defined(SMART_WECHAT))
    process_start (&smart_conf_ack, NULL); 
#endif
}

PROCESS_THREAD(dhcp_process, ev, data)
{
	PROCESS_BEGIN();
	
	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_INIT || ev == tcpip_event || ev == PROCESS_EVENT_EXIT || ev == PROCESS_EVENT_TIMER);
//		printf("dhcp_process continue  0x%x...  wifi_connect = %d state: %d\n", ev, gwifistatus.wifi_connect, dhcpState);	
		if (gwifistatus.wifi_connect == 1 && (gwifistatus.status == IEEE80211_CONNECTED || gwifistatus.status == IEEE80211_GETIP)) {
			if (ev == PROCESS_EVENT_INIT) 
			{
				if(dhcpState == NOBEGIN)
				{
					uip_lladdr.addr[0] = gwifistatus.local_mac[0]; //host_mac.addr[0];
					uip_lladdr.addr[1] = gwifistatus.local_mac[1]; //host_mac.addr[1];
					uip_lladdr.addr[2] = gwifistatus.local_mac[2]; //host_mac.addr[2];
					uip_lladdr.addr[3] = gwifistatus.local_mac[3]; //host_mac.addr[3];
					uip_lladdr.addr[4] = gwifistatus.local_mac[4]; //host_mac.addr[4];
					uip_lladdr.addr[5] = gwifistatus.local_mac[5]; //host_mac.addr[5];
					printf("dhcpc init %x %x %x %x %x %x\n", uip_lladdr.addr[0], uip_lladdr.addr[1], uip_lladdr.addr[2], uip_lladdr.addr[3], uip_lladdr.addr[4], uip_lladdr.addr[5]);
					dhcpc_init(uip_lladdr.addr, sizeof(uip_lladdr.addr));
					dhcpc_request();
				}
			}
			else if (ev == tcpip_event || ev == PROCESS_EVENT_TIMER) 
			{
				if(dhcpState != NOBEGIN)
				{
					dhcpc_appcall(ev, data);
				}
				if(dhcpState ==READY && gCabrioConf.enableSmartSoftFilter == TRUE)
				{
					if(gwifistatus.slink_mode == WECHAT)
						gCabrioConf.enableSmartSoftFilter = FALSE;
				}
			}
		}
		else if( ev == PROCESS_EVENT_EXIT)
		{
			dhcpc_appcall(ev, data);
		}
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void set_node_ip ()
{
	//uip_ipaddr_t read_host_ip;
	//uip_ipaddr_t read_netmask;
	//uip_ipaddr_t read_gatway;

	uip_sethostaddr (&(gCabrioConf).local_ip_addr);
	uip_setnetmask (&(gCabrioConf).net_mask);
	uip_setdraddr(&(gCabrioConf).gateway_ip_addr);

	// Strange that call uip_setethaddr not working, so hard core for uip_lladdr
	//uip_setethaddr(&host_mac);
	uip_lladdr.addr[0] = gwifistatus.local_mac[0]; //host_mac.addr[0];
	uip_lladdr.addr[1] = gwifistatus.local_mac[1]; //host_mac.addr[1];
	uip_lladdr.addr[2] = gwifistatus.local_mac[2]; //host_mac.addr[2];
	uip_lladdr.addr[3] = gwifistatus.local_mac[3]; //host_mac.addr[3];
	uip_lladdr.addr[4] = gwifistatus.local_mac[4]; //host_mac.addr[4];
	uip_lladdr.addr[5] = gwifistatus.local_mac[5]; //host_mac.addr[5];
#if 0
	memset (&read_host_ip, 0, sizeof (uip_ipaddr_t));
	memset (&read_netmask, 0, sizeof (uip_ipaddr_t));
	memset (&read_gatway, 0, sizeof (uip_ipaddr_t));

	uip_gethostaddr(&read_host_ip);
	uip_getnetmask(&read_netmask);
	uip_getdraddr(&read_gatway);

	printf ("Host IP : %d.%d.%d.%d\n", read_host_ip.u8[0], read_host_ip.u8[1], read_host_ip.u8[2], read_host_ip.u8[3]);
	printf ("Netmask : %d.%d.%d.%d\n", read_netmask.u8[0], read_netmask.u8[1], read_netmask.u8[2], read_netmask.u8[3]);
	printf ("Gatway : %d.%d.%d.%d\n", read_gatway.u8[0], read_gatway.u8[1], read_gatway.u8[2], read_gatway.u8[3]);
	printf ("Host MAC : %02x %02x %02x %02x %02x %02x\n", uip_lladdr.addr[0], uip_lladdr.addr[1], uip_lladdr.addr[2], uip_lladdr.addr[3], uip_lladdr.addr[4], uip_lladdr.addr[5]);
#endif	
}
/*---------------------------------------------------------------------------*/
void
ip_configure ()
{
	set_node_ip ();
	uip_arp_init ();
}
/*---------------------------------------------------------------------------*/
void
dhcp_configure ()
{
	if (gwifistatus.wifi_connect == 0) {
		return;
	}

	if (gCabrioConf.dhcp_enable == 0) {	// Fix IP process
		gCabrioConf.enableSmartSoftFilter = FALSE;
		ip_configure ();
		replyAck();
		printf ("+CONNECT:%s\r\n", "SUCCESS");
		printf ("AT+WIFICONNECT OK");
		gwifistatus.status = IEEE80211_CONNECTED;
		process_post_synch(&bss_reconnect_process, PROCESS_EVENT_CONTINUE, NULL);
		return;
	}

	// DHCP process

	if(gCabrioConf.dhcp_enable == 1)
	{
		printf ("+CONNECT:%s\r\n", "SUCCESS");
		printf ("Enable DHCP Process\n");
		process_post (&dhcp_process, PROCESS_EVENT_INIT, NULL);
	}
	
}
/*---------------------------------------------------------------------------*/

