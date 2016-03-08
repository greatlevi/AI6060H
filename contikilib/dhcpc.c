/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
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
 * This file is part of the Contiki operating system.
 *
 */

#include <stdio.h>
#include <string.h>

#include "contiki.h"
#include "contiki-net.h"
#include "net/dhcpc.h"
#include "console.h"
#include "systemconf.h"
#include "uip.h"
#include "smartComm.h"

#define STATE_INITIAL         0
#define STATE_SENDING         1
#define STATE_OFFER_RECEIVED  2
#define STATE_CONFIG_RECEIVED 3

static struct dhcpc_state s;

extern TAG_CABRIO_CONFIGURATION gCabrioConf;
extern IEEE80211STATUS gwifistatus;

EVENTMSG_DATA	eventdata;
struct dhcp_msg {
  uint8_t op, htype, hlen, hops;
  uint8_t xid[4];
  uint16_t secs, flags;
  uint8_t ciaddr[4];
  uint8_t yiaddr[4];
  uint8_t siaddr[4];
  uint8_t giaddr[4];
  uint8_t chaddr[16];
#ifndef UIP_CONF_DHCP_LIGHT
  uint8_t sname[64];
  uint8_t file[128];
#endif
  uint8_t options[312];
};

#define BOOTP_BROADCAST 0x8000

#define DHCP_REQUEST        1
#define DHCP_REPLY          2
#define DHCP_HTYPE_ETHERNET 1
#define DHCP_HLEN_ETHERNET  6
#define DHCP_MSG_LEN      236

#define DHCPC_SERVER_PORT  67
#define DHCPC_CLIENT_PORT  68

#define DHCPDISCOVER  1
#define DHCPOFFER     2
#define DHCPREQUEST   3
#define DHCPDECLINE   4
#define DHCPACK       5
#define DHCPNAK       6
#define DHCPRELEASE   7

#define DHCP_OPTION_SUBNET_MASK   1
#define DHCP_OPTION_ROUTER        3
#define DHCP_OPTION_DNS_SERVER    6
#define DHCP_OPTION_REQ_IPADDR   50
#define DHCP_OPTION_LEASE_TIME   51
#define DHCP_OPTION_MSG_TYPE     53
#define DHCP_OPTION_SERVER_ID    54
#define DHCP_OPTION_REQ_LIST     55
#define DHCP_OPTION_CLIENT_ID	61
#define DHCP_OPTION_END         255

static uint32_t xid;
static const uint8_t magic_cookie[4] = {99, 130, 83, 99};
DHCP_STATE dhcpState = NOBEGIN;
/*---------------------------------------------------------------------------*/
PROCESS_NAME(Cabrio_ate_process);
#if (defined(SMART_ICOMM) || defined(SMART_WECHAT))
PROCESS_NAME(smart_conf_ack);
#endif
PROCESS_NAME(bss_reconnect_process);
/*---------------------------------------------------------------------------*/
DHCP_STATE
getDhcpState()
{
    return dhcpState;
}
/*---------------------------------------------------------------------------*/
extern void HF_WakeUp(void);

void
dhcpc_configured(const struct dhcpc_state *s)
{
	printf ("dhcp congigured\n");
	//Set ip into gCabrioConf
	memcpy(&gCabrioConf.local_ip_addr, &s->ipaddr, sizeof(s->ipaddr));
	memcpy(&gCabrioConf.net_mask, &s->netmask, sizeof(s->netmask));
	memcpy(&gCabrioConf.gateway_ip_addr, &s->default_router, sizeof(s->netmask));
		
  	uip_sethostaddr(&s->ipaddr);
  	uip_setnetmask(&s->netmask);
  	uip_setdraddr(&s->default_router);
  	resolv_conf(&s->dnsaddr);
	uip_arp_init();
	printf("AT+WIFICONNECT=OK\n");
	gwifistatus.status = IEEE80211_CONNECTED;
	process_post_synch(&bss_reconnect_process, PROCESS_EVENT_CONTINUE, NULL);
  	memset (&eventdata, 0, sizeof (EVENTMSG_DATA));
	eventdata.msgid = MSG_EV_GET_IP;
  	eventdata.data_len = 0;
	process_post (&Cabrio_ate_process, PROCESS_EVENT_CONTINUE, &eventdata);	
    if(gwifistatus.callbackproc)
        process_post_synch(gwifistatus.callbackproc, PROCESS_EVENT_MSG, &gwifistatus.status);
#if (defined(SMART_ICOMM) || defined(SMART_WECHAT))
	process_start (&smart_conf_ack, NULL);
#endif

    HF_WakeUp();
}
void
dhcpc_unconfigured(const struct dhcpc_state *s)
{
	printf ("dhcp uncongigured\n");
  	//set_statustext("Unconfigured.");
 	 //process_post(PROCESS_CURRENT(), SHOWCONFIG, NULL);
}
/*---------------------------------------------------------------------------*/
static uint8_t *
add_msg_type(uint8_t *optptr, uint8_t type)
{
  *optptr++ = DHCP_OPTION_MSG_TYPE;
  *optptr++ = 1;
  *optptr++ = type;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static uint8_t *
add_server_id(uint8_t *optptr)
{
  *optptr++ = DHCP_OPTION_SERVER_ID;
  *optptr++ = 4;
  memcpy(optptr, s.serverid, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
/*static uint8_t *
add_client_id(uint8_t *optptr)
{
  *optptr++ = DHCP_OPTION_CLIENT_ID;
  *optptr++ = 4;
  memcpy(optptr, s.serverid, 4);
  return optptr + 4;
}*/
/*---------------------------------------------------------------------------*/
static uint8_t *
add_req_ipaddr(uint8_t *optptr)
{
  *optptr++ = DHCP_OPTION_REQ_IPADDR;
  *optptr++ = 4;
  memcpy(optptr, s.ipaddr.u16, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
static uint8_t *
add_req_options(uint8_t *optptr)
{
  *optptr++ = DHCP_OPTION_REQ_LIST;
  *optptr++ = 3;
  *optptr++ = DHCP_OPTION_SUBNET_MASK;
  *optptr++ = DHCP_OPTION_ROUTER;
  *optptr++ = DHCP_OPTION_DNS_SERVER;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static uint8_t *
add_end(uint8_t *optptr)
{
  *optptr++ = DHCP_OPTION_END;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static void
create_msg(CC_REGISTER_ARG struct dhcp_msg *m)
{
  //printf ("create_msg +++\n");
  m->op = DHCP_REQUEST;
  m->htype = DHCP_HTYPE_ETHERNET;
  m->hlen = s.mac_len;
  m->hops = 0;
  memcpy(m->xid, &xid, sizeof(m->xid));
  m->secs = 0;
  m->flags = UIP_HTONS(BOOTP_BROADCAST); /*  Broadcast bit. */
  /*  uip_ipaddr_copy(m->ciaddr, uip_hostaddr);*/
  memcpy(m->ciaddr, uip_hostaddr.u16, sizeof(m->ciaddr));
  memset(m->yiaddr, 0, sizeof(m->yiaddr));
  memset(m->siaddr, 0, sizeof(m->siaddr));
  memset(m->giaddr, 0, sizeof(m->giaddr));
  memcpy(m->chaddr, s.mac_addr, s.mac_len);
  memset(&m->chaddr[s.mac_len], 0, sizeof(m->chaddr) - s.mac_len);
#ifndef UIP_CONF_DHCP_LIGHT
  memset(m->sname, 0, sizeof(m->sname));
  memset(m->file, 0, sizeof(m->file));
#endif

  //memcpy(m->options, magic_cookie, sizeof(magic_cookie));
  m->options[0] = 0x63;
  m->options[1] = 0x82;
  m->options[2] = 0x53;
  m->options[3] = 0x63;
  
}
/*---------------------------------------------------------------------------*/
static void
send_discover(void)
{
  uint8_t *end;
  
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;

  printf ("send_discover +++\n");

  create_msg(m);

  end = add_msg_type(&m->options[4], DHCPDISCOVER);
  end = add_req_options(end);
  end = add_end(end);

  uip_send(uip_appdata, (int)(end - (uint8_t *)uip_appdata));
}
/*---------------------------------------------------------------------------*/
static void
send_request(void)
{
  uint8_t *end;
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
  printf ("send_request +++\n");

  create_msg(m);
  
  end = add_msg_type(&m->options[4], DHCPREQUEST);
  end = add_server_id(end);
  end = add_req_ipaddr(end);
  end = add_end(end);
  
  uip_send(uip_appdata, (int)(end - (uint8_t *)uip_appdata));
}
/*---------------------------------------------------------------------------*/
static uint8_t
parse_options(uint8_t *optptr, int len)
{
  uint8_t *end = optptr + len;
  uint8_t type = 0;

  while(optptr < end) {
    switch(*optptr) {
    case DHCP_OPTION_SUBNET_MASK:
      memcpy(s.netmask.u16, optptr + 2, 4);
      break;
    case DHCP_OPTION_ROUTER:
      memcpy(s.default_router.u16, optptr + 2, 4);
      break;
    case DHCP_OPTION_DNS_SERVER:
      memcpy(s.dnsaddr.u16, optptr + 2, 4);
      break;
    case DHCP_OPTION_MSG_TYPE:
      type = *(optptr + 2);
      break;
    case DHCP_OPTION_SERVER_ID:
      memcpy(s.serverid, optptr + 2, 4);
      break;
    case DHCP_OPTION_LEASE_TIME:
      memcpy(s.lease_time, optptr + 2, 4);
      break;
    case DHCP_OPTION_END:
      return type;
    }

    optptr += optptr[1] + 2;
  }
  return type;
}
/*---------------------------------------------------------------------------*/
static uint8_t
parse_msg(void)
{
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
  
  if(m->op == DHCP_REPLY &&
     memcmp(m->xid, &xid, sizeof(xid)) == 0 &&
     memcmp(m->chaddr, s.mac_addr, s.mac_len) == 0) {
    memcpy(s.ipaddr.u16, m->yiaddr, 4);
    return parse_options(&m->options[4], uip_datalen());
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
/*
 * Is this a "fresh" reply for me? If it is, return the type.
 */
static int
msg_for_me(void)
{
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
  uint8_t *optptr = &m->options[4];
  uint8_t *end = (uint8_t*)uip_appdata + uip_datalen();
  
  if(m->op == DHCP_REPLY &&
     memcmp(m->xid, &xid, sizeof(xid)) == 0 &&
     memcmp(m->chaddr, s.mac_addr, s.mac_len) == 0) {
    while(optptr < end) {
      if(*optptr == DHCP_OPTION_MSG_TYPE) {
	return *(optptr + 2);
      } else if (*optptr == DHCP_OPTION_END) {
	return -1;
      }
      optptr += optptr[1] + 2;
    }
  }
  return -1;
}
/*---------------------------------------------------------------------------*/
extern unsigned int g_u32GloablIp;
extern void HF_Sleep(void);
extern void HF_DisconFromCloud(void);
static
PT_THREAD(handle_dhcp(process_event_t ev, void *data))
{
  clock_time_t ticks;
  uint8_t u8ip[4] = {0};

  PT_BEGIN(&s.pt);
  printf ("[handle_dhcp] +++\n");
  
 init:
  printf ("dhcp : init\n");
  xid++;
  s.state = STATE_SENDING;
  s.ticks = CLOCK_SECOND;
  while (1) {
    while(ev != tcpip_event) {
      tcpip_poll_udp(s.conn);
      PT_YIELD(&s.pt);
      if(ev == PROCESS_EVENT_EXIT)
        goto exit;
    }
    send_discover();
    etimer_set(&s.etimer, s.ticks);
    do {
      PT_YIELD(&s.pt);
      if(ev == PROCESS_EVENT_EXIT)
        goto exit;
      if(ev == tcpip_event && uip_newdata() && msg_for_me() == DHCPOFFER) {
	parse_msg();
	s.state = STATE_OFFER_RECEIVED;
	goto selecting;
      }
    } while (!etimer_expired(&s.etimer));

    if(s.ticks < CLOCK_SECOND * 4) {
      s.ticks *= 2;
    }
  }
  
 selecting:
  printf ("dhcp : selecting\n");
  xid++;
  s.ticks = CLOCK_SECOND;
  do {
    while(ev != tcpip_event) {
      tcpip_poll_udp(s.conn);
      PT_YIELD(&s.pt);
      if(ev == PROCESS_EVENT_EXIT)
        goto exit;
    }
    send_request();
    etimer_set(&s.etimer, s.ticks);
    do {
      PT_YIELD(&s.pt);
      if(ev == PROCESS_EVENT_EXIT)
        goto exit;
      if(ev == tcpip_event && uip_newdata() && msg_for_me() == DHCPACK) {
	parse_msg();
	s.state = STATE_CONFIG_RECEIVED;
	goto bound;
      }
    } while (!etimer_expired(&s.etimer));

    if(s.ticks <= CLOCK_SECOND * 4) {
      s.ticks += CLOCK_SECOND;
    } else {
      goto init;
    }
  } while(s.state != STATE_CONFIG_RECEIVED);
  
 bound:
  printf ("dhcp : bound\n");
#if 1 
  u8ip[0] = s.ipaddr.u8[3];
  u8ip[1] = s.ipaddr.u8[2];
  u8ip[2] = s.ipaddr.u8[1];
  u8ip[3] = s.ipaddr.u8[0];
  memcpy((void *)&g_u32GloablIp, u8ip, 4);
  //printf("g_u32GloablIp is 0x%08x\n", g_u32GloablIp);
  printf("Got IP address %d.%d.%d.%d\n", uip_ipaddr_to_quad(&s.ipaddr));
  printf("Got netmask %d.%d.%d.%d\n",	 uip_ipaddr_to_quad(&s.netmask));
  printf("Got DNS server %d.%d.%d.%d\n", uip_ipaddr_to_quad(&s.dnsaddr));
  printf("Got default router %d.%d.%d.%d\n",
	 uip_ipaddr_to_quad(&s.default_router));
  printf("Lease expires in %ld seconds\n",
	 uip_ntohs(s.lease_time[0])*65536ul + uip_ntohs(s.lease_time[1]));
#endif

  dhcpc_configured(&s);
  dhcpState = READY;

#define MAX_TICKS (~((clock_time_t)0) / 2)
#define MAX_TICKS32 (~((uint64_t)0))
#define IMIN(a, b) ((a) < (b) ? (a) : (b))

  if((uip_ntohs(s.lease_time[0])*65536ul + uip_ntohs(s.lease_time[1]))*CLOCK_SECOND/2
     <= MAX_TICKS32) {
    s.ticks = (uip_ntohs(s.lease_time[0])*65536ul + uip_ntohs(s.lease_time[1]));
    s.ticks = s.ticks * CLOCK_SECOND / 2;
  } else {
    s.ticks = MAX_TICKS32;
  }

  while(s.ticks > 0) {
    ticks = IMIN(s.ticks, MAX_TICKS);
    s.ticks -= ticks;
    etimer_set(&s.etimer, ticks);
    PT_YIELD_UNTIL(&s.pt, etimer_expired(&s.etimer) || ev == PROCESS_EVENT_EXIT);
	if(ev == PROCESS_EVENT_EXIT)
	  goto exit;
  }

  if((uip_ntohs(s.lease_time[0])*65536ul + uip_ntohs(s.lease_time[1]))*CLOCK_SECOND/2
     <= MAX_TICKS32) {
    s.ticks = (uip_ntohs(s.lease_time[0])*65536ul + uip_ntohs(s.lease_time[1])
	       )*CLOCK_SECOND/2;
  } else {
    s.ticks = MAX_TICKS32;
  }
  
  /* renewing: */
  xid++;
  do {
    while(ev != tcpip_event) {
      tcpip_poll_udp(s.conn);
      PT_YIELD(&s.pt);
	  if(ev == PROCESS_EVENT_EXIT)
		goto exit;
    }
    send_request();
    ticks = IMIN(s.ticks / 2, MAX_TICKS);
    s.ticks -= ticks;
    etimer_set(&s.etimer, ticks);
    do {
      PT_YIELD(&s.pt);
	  if(ev == PROCESS_EVENT_EXIT)
		goto exit;
      else if(ev == tcpip_event && uip_newdata() && msg_for_me() == DHCPACK) {
		parse_msg();
		goto bound;
      }
    } while(!etimer_expired(&s.etimer));
  } while(s.ticks >= CLOCK_SECOND*3);

  /* rebinding: */

  /* lease_expired: */
  dhcpc_unconfigured(&s);
  gwifistatus.status = IEEE80211_GETIP;
  goto init;

 exit:
  uip_udp_remove(s.conn);
  dhcpState = NOBEGIN;
  printf ("[handle_dhcp] ---\n");
  /* hexin add */
  HF_DisconFromCloud();
  //HF_Sleep();
  PT_END(&s.pt);


}
/*---------------------------------------------------------------------------*/
void
dhcpc_init(const void *mac_addr, int mac_len)
{
  uip_ipaddr_t addr;
  dhcpState = INIT;
  //printf ("[dhcpc_init] +++\n");
  s.mac_addr = mac_addr;
  s.mac_len  = mac_len;

  s.state = STATE_INITIAL;
  uip_ipaddr(&addr, 255,255,255,255);
  s.conn = udp_new(&addr, UIP_HTONS(DHCPC_SERVER_PORT), NULL);
  if(s.conn != NULL) {
    //printf ("[dhcpc_init] : s.conn != NULL\n");
    udp_bind(s.conn, UIP_HTONS(DHCPC_CLIENT_PORT));
  }
  else printf ("[dhcpc_init] : s.conn == NULL\n");
  PT_INIT(&s.pt);
  //printf ("[dhcpc_init] ---\n");
}
/*---------------------------------------------------------------------------*/
void
dhcpc_appcall(process_event_t ev, void *data)
{
	handle_dhcp(ev, data);
}
/*---------------------------------------------------------------------------*/
void
dhcpc_request(void)
{
  uip_ipaddr_t ipaddr;

  //printf ("[dhcpc_request] : +++\n");
  if(s.state == STATE_INITIAL) {
    //printf ("[dhcpc_request] : s.state == STATE_INITIAL\n");
    uip_ipaddr(&ipaddr, 0,0,0,0);
    uip_sethostaddr(&ipaddr);
    handle_dhcp(PROCESS_EVENT_NONE, NULL);
  }
  else printf ("[dhcpc_request] : s.state != STATE_INITIAL\n");
  //printf ("[dhcpc_request] : ---\n");
}
/*---------------------------------------------------------------------------*/
