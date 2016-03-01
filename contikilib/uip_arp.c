/**
 * \addtogroup uip
 * @{
 */

/**
 * \defgroup uiparp uIP Address Resolution Protocol
 * @{
 *
 * The Address Resolution Protocol ARP is used for mapping between IP
 * addresses and link level addresses such as the Ethernet MAC
 * addresses. ARP uses broadcast queries to ask for the link level
 * address of a known IP address and the host which is configured with
 * the IP address for which the query was meant, will respond with its
 * link level address.
 *
 * \note This ARP implementation only supports Ethernet.
 */
 
/**
 * \file
 * Implementation of the ARP Address Resolution Protocol.
 * \author Adam Dunkels <adam@dunkels.com>
 *
 */

/*
 * Copyright (c) 2001-2003, Adam Dunkels.
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
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 *
 */

/*
 * The arp packet format is different compare to TCP, UDP, ICMP packet. We can not use uip_buf as tcp process.
    So we need an independance buffer and api for arp packet operation.
    There are two changes here
    1. Create an buffer for arp process
    2. Create an API in NET driver for sending down the arp packet
*/

#include "net/uip_arp.h"
#include <ssv_lib.h>
//#include <string.h>

extern void arp_packet_sent(void *payload, unsigned short payload_len);
PROCESS(get_gatewaymac_process, "BSS Reconnect Process");
PROCESS(arptable_polling_process, "ARP table polling Process");

struct arp_hdr {
  struct uip_eth_hdr ethhdr;
  uint16_t hwtype;
  uint16_t protocol;
  uint8_t hwlen;
  uint8_t protolen;
  uint16_t opcode;
  struct uip_eth_addr shwaddr;
  uip_ipaddr_t sipaddr;
  struct uip_eth_addr dhwaddr;
  uip_ipaddr_t dipaddr;
};


#define ARP_REQUEST 1
#define ARP_REPLY   2

#define ARP_HWTYPE_ETH 1

struct arp_entry {
  uip_ipaddr_t ipaddr;
  struct uip_eth_addr ethaddr;
  uint8_t time;
};

//unsigned char arp_packet_buff[ARP_BUF_SIZE];
//unsigned short arp_packet_len;

static const struct uip_eth_addr broadcast_ethaddr =
  {{0xff,0xff,0xff,0xff,0xff,0xff}};
static const uint16_t broadcast_ipaddr[2] = {0xffff,0xffff};

static struct arp_entry arp_table[UIP_ARPTAB_SIZE];
static uip_ipaddr_t ipaddr;

static uint32_t arptime;
static uint8_t tmpage;

struct arp_hdr *ARPBUF = (struct arp_hdr *)&arp_packet_buff[0];
struct ethip_hdr *IPARPBUF = (struct ethip_hdr *)&uip_buf[0];

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*-----------------------------------------------------------------------------------*/
static void send_arp_request(uip_ipaddr_t addr)
{
	/* The destination address was not in our ARP table, so we
	overwrite the IP packet with an ARP request. */
	memset(ARPBUF->ethhdr.dest.addr, 0xff, 6);
	memset(ARPBUF->dhwaddr.addr, 0x00, 6);
	memcpy(ARPBUF->ethhdr.src.addr, uip_lladdr.addr, 6);
	memcpy(ARPBUF->shwaddr.addr, uip_lladdr.addr, 6);
	
	uip_ipaddr_copy(&ARPBUF->dipaddr, &addr);
	uip_ipaddr_copy(&ARPBUF->sipaddr, &uip_hostaddr);
	ARPBUF->opcode = UIP_HTONS(ARP_REQUEST); 
	ARPBUF->hwtype = UIP_HTONS(ARP_HWTYPE_ETH);
	ARPBUF->protocol = UIP_HTONS(UIP_ETHTYPE_IP);
	ARPBUF->hwlen = 6;
	ARPBUF->protolen = 4;
	ARPBUF->ethhdr.type = UIP_HTONS(UIP_ETHTYPE_ARP);
	
	arp_packet_len = sizeof(struct arp_hdr);
	arp_packet_sent (ARPBUF, arp_packet_len);
}
/*-----------------------------------------------------------------------------------*/
void uip_arp_print()
{
	U8 i;
	
	for(i = 0; i < UIP_ARPTAB_SIZE; i++)
	{
		if(arp_table[i].ipaddr.u8[0] != 0)
		{
			printf("%d.%d.%d.%d - %02X:%02X:%02X:%02X:%02X:%02X\n", arp_table[i].ipaddr.u8[0], arp_table[i].ipaddr.u8[1], arp_table[i].ipaddr.u8[2], arp_table[i].ipaddr.u8[3],
                arp_table[i].ethaddr.addr[0], arp_table[i].ethaddr.addr[1], arp_table[i].ethaddr.addr[2], arp_table[i].ethaddr.addr[3], arp_table[i].ethaddr.addr[4], arp_table[i].ethaddr.addr[5]);
		}
	}
}
/*-----------------------------------------------------------------------------------*/
void uip_arp_delete(uip_ipaddr_t *ipaddr)
{
	U8 i;
	
	for(i = 1; i < UIP_ARPTAB_SIZE; i++)
	{
		if(uip_ipaddr_cmp(&arp_table[i].ipaddr, ipaddr))
		{
			send_arp_request(arp_table[i].ipaddr);
			memset(&arp_table[i].ipaddr, 0, 4);
		}
	}
}
/*-----------------------------------------------------------------------------------*/
/**
 * Initialize the ARP module.
 *
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_init(void)
{
	U8 i;
	
	for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {
		memset(&arp_table[i].ipaddr, 0, 4);
	}

	arptime = 0;
	arp_packet_len = 0;
	memset (arp_packet_buff, 0, ARP_BUF_SIZE);
	process_start(&get_gatewaymac_process, NULL);
	process_start(&arptable_polling_process, NULL);
}
/*-----------------------------------------------------------------------------------*/
/*void uip_arp_showtable()
{
	int i;

	for(i = 0; i < UIP_ARPTAB_SIZE; i++)
	{
		if(arp_table[i].ipaddr.u8[0] != 0)
			printf("%d ~~ %d.%d.%d.%d - %02X:%02X:%02X:%02X:%02X:%02X\n", i,
			arp_table[i].ipaddr.u8[0], arp_table[i].ipaddr.u8[1], arp_table[i].ipaddr.u8[2], arp_table[i].ipaddr.u8[3],
			arp_table[i].ethaddr.addr[0], arp_table[i].ethaddr.addr[1], arp_table[i].ethaddr.addr[2], arp_table[i].ethaddr.addr[3], arp_table[i].ethaddr.addr[4], arp_table[i].ethaddr.addr[5]);
	}
}*/
/*-----------------------------------------------------------------------------------*/
/**
 * Periodic ARP processing function.
 *
 * This function performs periodic timer processing in the ARP module
 * and should be called at regular intervals. The recommended interval
 * is 10 seconds between the calls.
 *
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_timer(void)
{
	struct arp_entry *tabptr;
	U8 i;
	
	++arptime;
	//ARP table 0 is for gateway mac, don't delete it.
	for(i = 1; i < UIP_ARPTAB_SIZE; ++i) {
		tabptr = &arp_table[i];
		if((tabptr->ipaddr.u8[0] != 0) && ((arptime - tabptr->time) >= UIP_ARP_MAXAGE)) {
			//clear arp table for out of life time and 
			//sned a request to get new status.
			send_arp_request(tabptr->ipaddr);
			memset(&tabptr->ipaddr, 0, 4);
//			uip_arp_showtable();
		}
	}
}
/*-----------------------------------------------------------------------------------*/
void
uip_arp_update(uip_ipaddr_t *ipaddr, struct uip_eth_addr *ethaddr)
{
	register struct arp_entry *tabptr = arp_table;
	U8 i, c;
	
	//ARP table 0 is for gateway mac, don't place others
	if(uip_ipaddr_cmp(&ARPBUF->sipaddr, &uip_draddr))
	{	
		memcpy(&arp_table[0].ipaddr, &ARPBUF->sipaddr, sizeof(uip_ipaddr_t));
		memcpy(&arp_table[0].ethaddr, &ARPBUF->shwaddr, sizeof(uip_eth_addr));
		arp_table[0].time = arptime;
		return;
	}
	
	/* Walk through the ARP mapping table and try to find an entry to
     update. If none is found, the IP -> MAC address mapping is
     inserted in the ARP table. */
	for(i = 1; i < UIP_ARPTAB_SIZE; ++i) {
		tabptr = &arp_table[i];
		/* Only check those entries that are actually in use. */
		if(!uip_ipaddr_cmp(&tabptr->ipaddr, &uip_all_zeroes_addr)) {
			/* Check if the source IP address of the incoming packet matches
			the IP address in this ARP table entry. */
			if(uip_ipaddr_cmp(ipaddr, &tabptr->ipaddr)) {
				/* An old entry found, update this and return. */
				memcpy(tabptr->ethaddr.addr, ethaddr->addr, 6);
				tabptr->time = arptime;
				return;
			}
		}
	}

	/* If we get here, no existing ARP table entry was found, so we
     create one. */

	/* First, we try to find an unused entry in the ARP table. */
	for(i = 1; i < UIP_ARPTAB_SIZE; ++i) {
		tabptr = &arp_table[i];
		if(uip_ipaddr_cmp(&tabptr->ipaddr, &uip_all_zeroes_addr)) {
			break;
		}
	}

	/* If no unused entry is found, we try to find the oldest entry and
     throw it away. */
	if(i == UIP_ARPTAB_SIZE) {
		tmpage = 0;
		c = 0;
		//table[0] is place the gateway mac address, we can not replace it.
		for(i = 1; i < UIP_ARPTAB_SIZE; ++i) {
			tabptr = &arp_table[i];
			if(arptime - tabptr->time > tmpage) {
				tmpage = arptime - tabptr->time;
				c = i;
			}
		}
		i = c;
		tabptr = &arp_table[i];
	}

	/* Now, i is the ARP table entry which we will fill with the new
     information. */
	uip_ipaddr_copy(&tabptr->ipaddr, ipaddr);
	memcpy(tabptr->ethaddr.addr, ethaddr->addr, 6);
	tabptr->time = arptime;
//	uip_arp_showtable();
}
/*-----------------------------------------------------------------------------------*/
/**
 * ARP processing for incoming ARP packets.
 *
 * This function should be called by the device driver when an ARP
 * packet has been received. The function will act differently
 * depending on the ARP packet type: if it is a reply for a request
 * that we previously sent out, the ARP cache will be filled in with
 * the values from the ARP reply. If the incoming ARP packet is an ARP
 * request for our IP address, an ARP reply packet is created and put
 * into the uip_buf[] buffer.
 *
 * When the function returns, the value of the global variable uip_len
 * indicates whether the device driver should send out a packet or
 * not. If uip_len is zero, no packet should be sent. If uip_len is
 * non-zero, it contains the length of the outbound packet that is
 * present in the uip_buf[] buffer.
 *
 * This function expects an ARP packet with a prepended Ethernet
 * header in the uip_buf[] buffer, and the length of the packet in the
 * global variable uip_len.
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_arpin(void)
{
	if(arp_packet_len < sizeof(struct arp_hdr)) {
		arp_packet_len = 0;
		return;
	}
	arp_packet_len = 0;

	switch(ARPBUF->opcode) {
		case UIP_HTONS(ARP_REQUEST):

#if 0       //debug why arp response not being handled!!
            dbg_dump_ip("arp-a0",&ARPBUF->dipaddr);
            dbg_dump_ip("arp-a1",&uip_hostaddr);
#endif

		if(uip_ipaddr_cmp(&ARPBUF->dipaddr, &uip_hostaddr)) {
				/* First, we register the one who made the request in our ARP
				table, since it is likely that we will do more communication
				with this host in the future. */
				uip_arp_update(&ARPBUF->sipaddr, &ARPBUF->shwaddr);
	      
				ARPBUF->opcode = UIP_HTONS(ARP_REPLY);

				memcpy(ARPBUF->dhwaddr.addr, ARPBUF->shwaddr.addr, 6);
				memcpy(ARPBUF->shwaddr.addr, uip_lladdr.addr, 6);
				memcpy(ARPBUF->ethhdr.src.addr, uip_lladdr.addr, 6);
				memcpy(ARPBUF->ethhdr.dest.addr, ARPBUF->dhwaddr.addr, 6);
	      
				uip_ipaddr_copy(&ARPBUF->dipaddr, &ARPBUF->sipaddr);
				uip_ipaddr_copy(&ARPBUF->sipaddr, &uip_hostaddr);

				ARPBUF->ethhdr.type = UIP_HTONS(UIP_ETHTYPE_ARP);
				arp_packet_len = sizeof(struct arp_hdr);
				arp_packet_sent (ARPBUF, arp_packet_len);
			}
			break;
		case UIP_HTONS(ARP_REPLY):
	    	/* ARP reply. We insert or update the ARP table if it was meant
       		for us. */
			if(uip_ipaddr_cmp(&ARPBUF->dipaddr, &uip_hostaddr)) {
				uip_arp_update(&ARPBUF->sipaddr, &ARPBUF->shwaddr);
			}
			break;
	}
	return;
}
/*-----------------------------------------------------------------------------------*/
/**
 * Prepend Ethernet header to an outbound IP packet and see if we need
 * to send out an ARP request.
 *
 * This function should be called before sending out an IP packet. The
 * function checks the destination IP address of the IP packet to see
 * what Ethernet MAC address that should be used as a destination MAC
 * address on the Ethernet.
 *
 * If the destination IP address is in the local network (determined
 * by logical ANDing of netmask and our IP address), the function
 * checks the ARP cache to see if an entry for the destination IP
 * address is found. If so, an Ethernet header is prepended and the
 * function returns. If no ARP cache entry is found for the
 * destination IP address, the packet in the uip_buf[] is replaced by
 * an ARP request packet for the IP address. The IP packet is dropped
 * and it is assumed that they higher level protocols (e.g., TCP)
 * eventually will retransmit the dropped packet.
 *
 * If the destination IP address is not on the local network, the IP
 * address of the default router is used instead.
 *
 * When the function returns, a packet is present in the uip_buf[]
 * buffer, and the length of the packet is in the global variable
 * uip_len.
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_out(void)
{
  struct arp_entry *tabptr = arp_table;
  U8 i;
  
  /* Find the destination IP address in the ARP table and construct
     the Ethernet header. If the destination IP addres isn't on the
     local network, we use the default router's IP address instead.

     If not ARP table entry is found, we overwrite the original IP
     packet with an ARP request for the IP address. */

	/* First check if destination is a local broadcast. */
	if(uip_ipaddr_cmp(&IPARPBUF->destipaddr, &uip_broadcast_addr)) {
		memcpy(IPARPBUF->ethhdr.dest.addr, broadcast_ethaddr.addr, 6);
	} else if(IPARPBUF->destipaddr.u8[0] == 224) {
		/* Multicast. */
	    IPARPBUF->ethhdr.dest.addr[0] = 0x01;
	    IPARPBUF->ethhdr.dest.addr[1] = 0x00;
	    IPARPBUF->ethhdr.dest.addr[2] = 0x5e;
	    IPARPBUF->ethhdr.dest.addr[3] = IPARPBUF->destipaddr.u8[1] & 0x7F;
	    IPARPBUF->ethhdr.dest.addr[4] = IPARPBUF->destipaddr.u8[2];
	    IPARPBUF->ethhdr.dest.addr[5] = IPARPBUF->destipaddr.u8[3];
	} else {
		/* Check if the destination address is on the local network. */
		if(!uip_ipaddr_maskcmp(&IPARPBUF->destipaddr, &uip_hostaddr, &uip_netmask)) {
			/* Destination address was not on the local network, so we need to
			use the default router's IP address instead of the destination
			address when determining the MAC address. */
			memcpy(IPARPBUF->ethhdr.dest.addr, arp_table[0].ethaddr.addr, 6);
#if 0       
               dbg_dump_mac("1", arp_table[0].ethaddr.addr);
#endif          
			return;
		} else {
			/* Else, we use the destination IP address. */
			uip_ipaddr_copy(&ipaddr, &IPARPBUF->destipaddr);
#if 0       
               dbg_dump_mac("2", &ipaddr);
#endif          
            
	    }
		
	    for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {
			if(uip_ipaddr_cmp(&ipaddr, &tabptr->ipaddr)) {
				break;
			}
			tabptr++;
	    }

	    if(i == UIP_ARPTAB_SIZE) {
#if 0       
               dbg_dump_mac("3", &arp_table[0].ethaddr.addr[0]);
#endif          
		  send_arp_request(ipaddr);
		  memcpy(IPARPBUF->ethhdr.dest.addr, arp_table[0].ethaddr.addr, 6);
	      return;
	    }

#if 0
    dbg_dump_mac("2a", tabptr->ethaddr.addr);
#endif
		/* Build an ethernet header. */
	    memcpy(IPARPBUF->ethhdr.dest.addr, tabptr->ethaddr.addr, 6);
	}
}
/*-----------------------------------------------------------------------------------*/
void uip_arp_softap_init(U8 *p_mac)
{
    //to-do modify the mac address 
    //memcpy( ARPBUF->shwaddr.addr, paddr, 6);    
    
    uip_lladdr_t eth_addr;

    //to-do modify the mac address 
    eth_addr.addr[0] = *(p_mac++);
    eth_addr.addr[1] = *(p_mac++);
    eth_addr.addr[2] = *(p_mac++);
    eth_addr.addr[3] = *(p_mac++);
    eth_addr.addr[4] = *(p_mac++);
    eth_addr.addr[5] = *(p_mac++);
                
    uip_setethaddr(eth_addr);
}
/*-----------------------------------------------------------------------------------*/
S8 check_ip_in_arptable(uip_ipaddr_t ipatble_addr) {
    S8 status = -1, i = 0;
    for(i = 0; i < UIP_ARPTAB_SIZE; i++) {
        if(memcmp(arp_table[i].ipaddr.u8, ipatble_addr.u8, 4) == 0) {
            status =  0;
            break;
        }
    }
    return status;
}
/*-----------------------------------------------------------------------------------*/
void send_needip_arp_request(uip_ipaddr_t ipatble_addr) {
    send_arp_request(ipatble_addr);
}
/*-----------------------------------------------------------------------------------*/
U8 uip_arp_ready(uip_ipaddr_t ipv4addr) {
    U8 i, ret = 1;
    
    if(uip_ipaddr_maskcmp(&ipv4addr, &uip_hostaddr, &uip_netmask))
    {
	    for(i = 1; i < UIP_ARPTAB_SIZE; i++) {
			if(uip_ipaddr_cmp(&ipv4addr, &arp_table[i].ipaddr)) {
				break;
			}
	    }
        
        if(i == UIP_ARPTAB_SIZE) {
            send_arp_request(ipv4addr);
            ret = 0;
        }
    }
    
    return ret;
}
/*-----------------------------------------------------------------------------------*/
PROCESS_THREAD(get_gatewaymac_process, ev, data)
{
	static struct etimer timer;
	
	PROCESS_BEGIN();

	while(1)
	{
		send_arp_request(uip_draddr);

		etimer_set(&timer, 500 * CLOCK_MINI_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
		
		if(arp_table[0].ipaddr.u8[0] != 0)
			break;
	}
	
	PROCESS_END();
}

PROCESS_THREAD(arptable_polling_process, ev, data)
{
	static struct etimer polltimer;
	
	PROCESS_BEGIN();

	while(1)
	{
		etimer_set(&polltimer, 30 * CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
		
		uip_arp_timer();
	}
	
	PROCESS_END();
}
