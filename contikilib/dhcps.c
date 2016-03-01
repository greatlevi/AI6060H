/* Adapted by Simon Berg from net/dhcpc.c */
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
#include <uip_arp.h>
#include "contiki.h"
#include "contiki-net.h"
#include "dhcps.h"
#include "./ieee80211_bss/soft_ap.h"
//#include <ssv_lib.h>
#include "dbg-printf.h"


extern void dbg_dump_mac(S8 *msg,U8 *pmac);
extern void dbg_dump_ip(S8 *msg,U8 *p_ip);
extern struct dhcps_config g_softap_config;

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
//} CC_BYTE_ALIGNED;
} ;


#define BOOTP_BROADCAST 0x8000

#define DHCP_REQUEST        1
#define DHCP_REPLY          2
#define DHCP_HTYPE_ETHERNET 1
#define DHCP_HLEN_ETHERNET  6
#define DHCP_MSG_LEN      236

#define DHCPS_SERVER_PORT  67
#define DHCPS_CLIENT_PORT  68

#define DHCPDISCOVER  1
#define DHCPOFFER     2
#define DHCPREQUEST   3
#define DHCPDECLINE   4
#define DHCPACK       5
#define DHCPNAK       6
#define DHCPRELEASE   7
#define DHCPINFORM    8

#define DHCP_OPTION_SUBNET_MASK   1
#define DHCP_OPTION_ROUTER        3
#define DHCP_OPTION_DNS_SERVER    6
#define DHCP_OPTION_REQ_IPADDR   50
#define DHCP_OPTION_LEASE_TIME   51
#define DHCP_OPTION_MSG_TYPE     53
#define DHCP_OPTION_SERVER_ID    54
#define DHCP_OPTION_REQ_LIST     55
#define DHCP_OPTION_END         255





static const struct dhcps_config *config;


static uint8_t *
find_option(uint8_t option)
{
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
  uint8_t *optptr = &m->options[4];
  uint8_t *end = (uint8_t*)uip_appdata + uip_datalen();
  while(optptr < end && *optptr != DHCP_OPTION_END) {
    if(*optptr == option) {
      return optptr;
    }
    optptr += optptr[1] + 2;
  }
  return NULL;
}

static const uint8_t magic_cookie[4] = {99, 130, 83, 99};

static int
check_cookie(void)
{
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
  return memcmp(m->options, magic_cookie, 4) == 0;
}

/* Finds any valid lease for a given MAC address */
static struct dhcps_client_lease *
get_leased_mac(const uint8_t *chaddr, uint8_t hlen)
{
    struct dhcps_client_lease *lease = config->leases;
    struct dhcps_client_lease *end = config->leases + config->num_leases;

    struct dhcps_client_lease * rlt = NULL;
    int loop_i = 0;
  
    while(lease != end) 
    {
        icomprintf(GROUP_SOFTAP,LOG_INFO,"loop_i = %d,flags=0x%x\n",loop_i++,lease->flags);
        icomprintf(GROUP_SOFTAP,LOG_INFO,"(lease->chaddr,chaddr)=( %x-%x-%x-%x-%x-%x, %x-%x-%x-%x-%x-%x)\n", lease->chaddr[0],lease->chaddr[1],lease->chaddr[2],lease->chaddr[3],lease->chaddr[4],lease->chaddr[5]
                                                                                                                                                                       ,chaddr[0],chaddr[1],chaddr[2],chaddr[3],chaddr[4],chaddr[5]);

        if (LEASE_FLAGS_ALLOCATED==(lease->flags & LEASE_FLAGS_ALLOCATED))
        {
            if( memcmp(lease->chaddr, chaddr, hlen) == 0)   //check if allocated lease !!
            {
                icomprintf(GROUP_SOFTAP,LOG_INFO,"Allocated!! ip=%d.%d.%d.%d\n", lease->ipaddr.u8[0],lease->ipaddr.u8[1],lease->ipaddr.u8[2],lease->ipaddr.u8[3]);
                rlt = lease;
                break;
            }
        }
        
        lease++;
    }

exit:  
    if(NULL==rlt)
    {
        lease = NULL;
    }
    
#if 1   //for debugging!!
    icomprintf(GROUP_SOFTAP,LOG_INFO,"<%s>\n",__func__);    
    icomprintf(GROUP_SOFTAP,LOG_INFO,"num_leases=%d\n",config->num_leases);
    icomprintf(GROUP_SOFTAP,LOG_INFO,"lease=0x%x\n",lease);
    if(lease!=NULL)
    {    

        icomprintf(GROUP_SOFTAP,LOG_INFO,"lease->ipaddr=%d.%d.%d.%d\n", lease->ipaddr.u8[0]
                                                                                                        ,lease->ipaddr.u8[1]
                                                                                                        ,lease->ipaddr.u8[2]
                                                                                                        ,lease->ipaddr.u8[3]);

        
        icomprintf(GROUP_SOFTAP,LOG_INFO,"lease->chaddr=0x%x-%x-%x-%x-%x-%x\n", lease->chaddr[0]
                                                                                    ,lease->chaddr[1]
                                                                                    ,lease->chaddr[2]
                                                                                    ,lease->chaddr[3]
                                                                                    ,lease->chaddr[4]
                                                                                    ,lease->chaddr[5]);

        icomprintf(GROUP_SOFTAP,LOG_INFO,"chaddr=0x%x-%x-%x-%x-%x-%x\n", chaddr[0]
                                                                                ,chaddr[1]
                                                                                ,chaddr[2]
                                                                                ,chaddr[3]
                                                                                ,chaddr[4]
                                                                                ,chaddr[5]);
    }    
#endif

  return rlt;
}

#if 0
/* Finds any valid lease for a given MAC address */
static struct dhcps_client_lease *
get_free_mac(const uint8_t *chaddr, uint8_t hlen)
{
    struct dhcps_client_lease *lease = config->leases;
    struct dhcps_client_lease *end = config->leases + config->num_leases;

    struct dhcps_client_lease * rlt = NULL;
    int loop_i = 0;
  
    while(lease != end) 
    {
        icomprintf(GROUP_SOFTAP,LOG_INFO,"loop_i = %d,flags=0x%x\n",loop_i++,lease->flags);
        icomprintf(GROUP_SOFTAP,LOG_INFO,"(lease->chaddr,chaddr)=(%x-%x-%x-%x-%x-%x,%x-%x-%x-%x-%x-%x)\n", lease->chaddr[0],lease->chaddr[1],lease->chaddr[2],lease->chaddr[3],lease->chaddr[4],lease->chaddr[5]
                                                                                                                                                                           ,chaddr[0],chaddr[1],chaddr[2],chaddr[3],chaddr[4],chaddr[5]);
        if ( (lease->flags & LEASE_FLAGS_ALLOCATED) ==0) 
        {
            rlt = lease;
            goto exit;
        }
        
        lease++;
    }

exit:  
    rlt = lease;
    
#if 1   //for debugging!!
    icomprintf(GROUP_SOFTAP,LOG_INFO,"<%s>\n",__func__);    
    icomprintf(GROUP_SOFTAP,LOG_INFO,"num_leases=%d\n",config->num_leases);
    if(lease!=NULL){    
      icomprintf(GROUP_SOFTAP,LOG_INFO,"lease->ipaddr=%d.%d.%d.%d\n", lease->ipaddr.u8[0]
                                                                                                        ,lease->ipaddr.u8[1]
                                                                                                        ,lease->ipaddr.u8[2]
                                                                                                        ,lease->ipaddr.u8[3]);

        
        icomprintf(GROUP_SOFTAP,LOG_INFO,"lease->chaddr=0x%x-%x-%x-%x-%x-%x\n", lease->chaddr[0]
                                                                                    ,lease->chaddr[1]
                                                                                    ,lease->chaddr[2]
                                                                                    ,lease->chaddr[3]
                                                                                    ,lease->chaddr[4]
                                                                                    ,lease->chaddr[5]);

       icomprintf(GROUP_SOFTAP,LOG_INFO,"chaddr=0x%x-%x-%x-%x-%x-%x\n", chaddr[0]
                                                                                ,chaddr[1]
                                                                                ,chaddr[2]
                                                                                ,chaddr[3]
                                                                                ,chaddr[4]
                                                                                ,chaddr[5]);
    }    
#endif

  return rlt;
}
#endif

static struct dhcps_client_lease *
lookup_lease_ip(const uip_ipaddr_t *ip)
{
    struct dhcps_client_lease *lease = config->leases;
    struct dhcps_client_lease *end = config->leases + config->num_leases;

    struct dhcps_client_lease *rlt = NULL;
    S32 vcase;
        
  
  while(lease != end) {
    if (uip_ipaddr_cmp(&lease->ipaddr, ip)) 
    {
        if(LEASE_FLAGS_ALLOCATED==(lease->flags & LEASE_FLAGS_ALLOCATED) )
        {
            rlt = NULL;
            vcase = 5;
        } else {
            rlt = lease;    
            vcase = 4;
        }
        //goto exit;
        break;
    } else {
        vcase = 6;
    }
    lease++;
  }

exit:
#if 1
    icomprintf(GROUP_SOFTAP,LOG_INFO,"<%s>:\n",__func__);
    icomprintf(GROUP_SOFTAP,LOG_INFO,"vcase=%d\n",vcase);
    icomprintf(GROUP_SOFTAP,LOG_INFO,"flags=%d\n",lease->flags);
    if(lease!=NULL)
    {
        icomprintf(GROUP_SOFTAP,LOG_INFO,"lease->ipaddr=%d.%d.%d.%d\n", lease->ipaddr.u8[0], lease->ipaddr.u8[1], lease->ipaddr.u8[2], lease->ipaddr.u8[3]);
    }
    icomprintf(GROUP_SOFTAP,LOG_INFO,"req_ip=%d.%d.%d.%d\n", ip->u8[0], ip->u8[1], ip->u8[2], ip->u8[3]);
    icomprintf(GROUP_SOFTAP,LOG_INFO,"rlt=0x%x\n",rlt);
#endif

    return rlt;
  //return NULL;
}

void dhcp_dump_info()
{
    struct dhcps_client_lease *lease = config->leases;
    struct dhcps_client_lease *end = config->leases + config->num_leases;
    S32 loop_i  = 0;
    U8 *p_mac = NULL;
    U8 *p_ip = NULL;

    printf("<%s>%d,config->num_leases=%d\n",__func__,__LINE__,config->num_leases);
    
    while(lease != end) 
    {
        //printf("----------<%d>----------lease=0x%x\n",loop_i,lease);
        //dbg_dump_mac("mac",&(lease->chaddr[0]));
        // dbg_dump_ip("ip",&(lease->ipaddr.u8[0]));
        //printf("flags=%d\n",lease->flags);
        p_mac = &(lease->chaddr[0]);
        p_ip = &(lease->ipaddr.u8[0]);
        printf("<dhcp-%d> mac:%x-%x-%x-%x-%x-%x, ip:%d.%d.%d.%d, flags=%d,lease=0x%x\n", loop_i,p_mac[0],p_mac[1],p_mac[2],p_mac[3],p_mac[4],p_mac[5]
                                                                                                                    ,p_ip[0],p_ip[1],p_ip[2],p_ip[3],lease->flags,lease);
        
        loop_i++;
        lease++;
    }
}


static struct dhcps_client_lease *
find_free_lease(void)
{
    struct dhcps_client_lease *found = NULL;
    struct dhcps_client_lease *lease = config->leases;
    struct dhcps_client_lease *end = config->leases + config->num_leases;

    while(lease != end) 
    {
#if 0    
        if ( (lease->flags & LEASE_FLAGS_VALID) != 0) {
            return lease;
        }
        
        if ( (lease->flags & LEASE_FLAGS_ALLOCATED) != 0) 
        {
            found = lease;
        }
#endif
        if ( (lease->flags & LEASE_FLAGS_ALLOCATED) != LEASE_FLAGS_ALLOCATED)   //get not-allocated lease!!
        {
            found = lease;
            break;
        }
        
        lease++;
    }
    return found;
}

struct dhcps_client_lease * 
init_lease(struct dhcps_client_lease *lease,const uint8_t *chaddr, uint8_t hlen)
{
    if (lease) 
    {
        memcpy(lease->chaddr, chaddr, hlen);
        lease->flags = LEASE_FLAGS_VALID;
    }
    return lease;
}


static struct dhcps_client_lease * dhcp_offer_choose_address()
    {
    struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
    struct dhcps_client_lease *lease;
    struct dhcps_client_lease *rlt=NULL;
#if 1    
    S32 vcase=0x0;  //case condition
#endif

    icomprintf(GROUP_SOFTAP,LOG_INFO,"<%s>\n",__func__);
    lease = get_leased_mac(m->chaddr, m->hlen);   //This MAC is already valid now!!
    
    if (lease) 
    {
        //###already have leased###
        //return lease;
        vcase = 1;
        rlt = lease;
        goto exit;
    }


    uint8_t *opt=NULL;

    opt = find_option(DHCP_OPTION_REQ_IPADDR);
    //####We don't support user's specified address which is not in our range 192.168.0.10~192.168.0.13

    if( opt != NULL )
    {
        icomprintf(GROUP_SOFTAP,LOG_DEBUG,"%s req_ip=%d.%d.%d.%d\n", __func__,opt[2],opt[3],opt[4],opt[5]);
        lease = lookup_lease_ip((uip_ipaddr_t*)&opt[2]);   
        if( (lease->flags & LEASE_FLAGS_ALLOCATED) == LEASE_FLAGS_ALLOCATED)    //already allocated
        {
            lease = NULL;
            rlt = NULL;
            vcase = 2;
            goto exit;
        }
    }


    lease = find_free_lease();
    if (lease) 
    {
        rlt = init_lease(lease, m->chaddr,m->hlen);
        vcase = 3;
        goto exit;
    }

    
exit:  
    icomprintf(GROUP_SOFTAP,LOG_INFO,"<%s>lease = 0x%x,vcase=%d\n", __func__,(U32)lease,vcase);
    return rlt;
}

static struct dhcps_client_lease *
dhcp_request_allocate_address()
{
    struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
    struct dhcps_client_lease *lease = NULL;
    S32 vcase = 0;
    uint8_t *opt=NULL;    
  
    lease = get_leased_mac(m->chaddr, m->hlen);
    
    if(LEASE_FLAGS_ALLOCATED==(lease->flags & LEASE_FLAGS_ALLOCATED))   //give clie the old address!!
    {
        goto exit;
    }
   
  
    if (lease !=NULL) 
    {
        opt = find_option(DHCP_OPTION_REQ_IPADDR);  //get the dhcp client requested ip-address

        if(opt!=NULL)
        {
            if(0==(lease->flags & LEASE_FLAGS_ALLOCATED))   //if not not allocated yet!!
            {
                lease = lookup_lease_ip((uip_ipaddr_t*)&opt[2]);        
            }
        }
    }

    if (lease !=NULL) {
        lease->lease_end = clock_seconds()+config->default_lease_time;
        lease->flags |= LEASE_FLAGS_ALLOCATED;
        memcpy( lease->chaddr, m->chaddr, m->hlen);
    }

    vcase = 4;
  
exit:
    icomprintf(GROUP_SOFTAP,LOG_INFO,"<%s>:\n",__func__);
    icomprintf(GROUP_SOFTAP,LOG_INFO,"vcase=%d\n",vcase);
    icomprintf(GROUP_SOFTAP,LOG_INFO,"lease=0x%x,opt=0x%x\n",lease,opt);
    if(lease!=NULL)  
    {
        //if lease == NULL,it will crash here!!
        icomprintf(GROUP_SOFTAP,LOG_INFO,"lease_end=%d\n",lease->lease_end);

       icomprintf(GROUP_SOFTAP,LOG_INFO,"lease->chaddr=0x%x-%x-%x-%x-%x-%x\n", lease->chaddr[0]
                                                                        ,lease->chaddr[1]
                                                                        ,lease->chaddr[2]
                                                                        ,lease->chaddr[3]
                                                                        ,lease->chaddr[4]
                                                                        ,lease->chaddr[5]);
    }        

    if(opt!=NULL)
    {
        icomprintf(GROUP_SOFTAP,LOG_INFO,"opt=0x%x,opt[2]=%d.%d.%d.%d\n", opt, opt[2],opt[3],opt[4],opt[5]);
    }
  return lease;
}

static struct dhcps_client_lease *
release_address()
{
    struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
    struct dhcps_client_lease *lease;
    lease = get_leased_mac(m->chaddr, m->hlen);
    if (!lease) {
        return NULL;
    }

    memset( &lease->chaddr[0],0x0,ETH_ALEN);
    lease->flags &= ~LEASE_FLAGS_ALLOCATED;
    return lease;
}
/*---------------------------------------------------------------------------*/
S32 release_address_by_mac(U8 *p_mac)
{
    S32 rlt = 0;
    struct dhcps_client_lease *lease = NULL;

    //printf("release_address_by_mac  mac=%x:%x:%x:%x:%x:%x\n",p_mac[0],p_mac[1],p_mac[2],p_mac[3],p_mac[4],p_mac[5]);
    
    lease = get_leased_mac(p_mac, ETH_ALEN);

    printf("release_address_by_mac  mac=%x:%x:%x:%x:%x:%x lease=0x%x\n",p_mac[0],p_mac[1],p_mac[2],p_mac[3],p_mac[4],p_mac[5],lease);
    
    
    if (!lease) {
        icomprintf(GROUP_SOFTAP,LOG_INFO,"mac=%x:%x:%x:%x:%x:%x\n",p_mac[0],p_mac[1],p_mac[2],p_mac[3],p_mac[4],p_mac[5]);
        rlt = -1;
        goto error_exit;
    }

    memset(&lease->chaddr[0],0x0,ETH_ALEN);
    lease->flags &= ~LEASE_FLAGS_ALLOCATED;

    dhcp_dump_info();

error_exit:    
    return rlt;
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
  memcpy(optptr, &uip_hostaddr, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
static uint8_t *
add_lease_time(uint8_t *optptr)
{
  uint32_t lt;
  *optptr++ = DHCP_OPTION_LEASE_TIME;
  *optptr++ = 4;
  lt = UIP_HTONL(config->default_lease_time);
  memcpy(optptr, &lt, 4);
  return optptr + 4;
}

/*---------------------------------------------------------------------------*/
static uint8_t *
add_end(uint8_t *optptr)
{
  *optptr++ = DHCP_OPTION_END;
  return optptr;
}

static uint8_t *
add_config(uint8_t *optptr)
{
  if (config->flags & DHCP_CONF_NETMASK) {
    *optptr++ = DHCP_OPTION_SUBNET_MASK;
    *optptr++ = 4;
    memcpy(optptr, &config->netmask, 4);
    optptr += 4;
  }
  if (config->flags & DHCP_CONF_DNSADDR) {
    *optptr++ = DHCP_OPTION_DNS_SERVER;
    *optptr++ = 4;
    memcpy(optptr, &config->dnsaddr, 4);
    optptr += 4;
  }
  if (config->flags & DHCP_CONF_DEFAULT_ROUTER) {
    *optptr++ = DHCP_OPTION_ROUTER;
    *optptr++ = 4;
    memcpy(optptr, &config->default_router, 4);
    optptr += 4;
  }
  return optptr;
}

static void
create_msg(CC_REGISTER_ARG struct dhcp_msg *m)
{
  m->op = DHCP_REPLY;
  /* m->htype = DHCP_HTYPE_ETHERNET; */
/*   m->hlen = DHCP_HLEN_ETHERNET; */
/*   memcpy(m->chaddr, &uip_lladdr,DHCP_HLEN_ETHERNET); */
  m->hops = 0;
  m->secs = 0;
  memcpy(m->siaddr, &uip_hostaddr, 4);
  m->sname[0] = '\0';
  m->file[0] = '\0';


    //ip address known by client
    m->siaddr[0] = 0;
    m->siaddr[1] = 0;
    m->siaddr[2] = 0;
    m->siaddr[3] = 0;

#if 0
    //client ip addr given by server
    m->yiaddr[0] = 192;
    m->yiaddr[1] = 168;
    m->yiaddr[2] = 0;
    m->yiaddr[3] = 10;

    //server ip
    m->siaddr[0] = 192;
    m->siaddr[1] = 168;
    m->siaddr[2] = 0;
    m->siaddr[3] = 1;

    //gateway ip
    m->giaddr[0] = 192;
    m->giaddr[1] = 168;
    m->giaddr[2] = 0;
    m->giaddr[3] = 1;
#endif


#if 0
//client ip addr given by server
    m->yiaddr[0] = g_softap_config.default_router.u8[0];
    m->yiaddr[1] = g_softap_config.default_router.u8[0];
    m->yiaddr[2] = g_softap_config.default_router.u8[0];
    m->yiaddr[3] = g_softap_config.default_router.u8[0];
#endif

    //server ip
    m->siaddr[0] = g_softap_config.default_router.u8[0];
    m->siaddr[1] = g_softap_config.default_router.u8[1];
    m->siaddr[2] = g_softap_config.default_router.u8[2];
    m->siaddr[3] = g_softap_config.default_router.u8[3];

    //gateway ip
    m->giaddr[0] = g_softap_config.default_router.u8[0];
    m->giaddr[1] = g_softap_config.default_router.u8[1];
    m->giaddr[2] = g_softap_config.default_router.u8[2];
    m->giaddr[3] = g_softap_config.default_router.u8[3];



  memcpy(m->options, magic_cookie, sizeof(magic_cookie));
}

static uip_ipaddr_t any_addr;
static uip_ipaddr_t bcast_addr;

/*---------------------------------------------------------------------------*/
static void
send_offer(struct uip_udp_conn *conn, struct dhcps_client_lease *lease)
{
  uint8_t *end;
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
  
  create_msg(m);
  memcpy(&m->yiaddr, &lease->ipaddr,4);

  end = add_msg_type(&m->options[4], DHCPOFFER);
  end = add_server_id(end);
  end = add_lease_time(end);
  end = add_config(end);
  end = add_end(end);

 icomprintf(GROUP_SOFTAP,LOG_INFO,"config->default_lease_time = %d\n",config->default_lease_time);
 icomprintf(GROUP_SOFTAP,LOG_INFO,"config->netmask = %d.%d.%d.%d\n",config->netmask.u8[0],config->netmask.u8[1],config->netmask.u8[2],config->netmask.u8[3]);
 icomprintf(GROUP_SOFTAP,LOG_INFO,"config->default_router = %d.%d.%d.%d,addr=0x%x\n",config->default_router.u8[0],config->default_router.u8[1],config->default_router.u8[2],config->default_router.u8[3], &(config->default_router.u8[0]));

  
  uip_ipaddr_copy(&conn->ripaddr, &bcast_addr);
  uip_send(uip_appdata, (int)(end - (uint8_t *)uip_appdata));
}





static void
send_ack(struct uip_udp_conn *conn, struct dhcps_client_lease *lease)
{
  uint8_t *end;
  uip_ipaddr_t ciaddr;
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
  
  create_msg(m);
  memcpy(&m->yiaddr, &lease->ipaddr,4);
   
  end = add_msg_type(&m->options[4], DHCPACK);
  end = add_server_id(end);
  end = add_lease_time(end);
  end = add_config(end);
  end = add_end(end);
  memcpy(&ciaddr, &lease->ipaddr,4);
  uip_ipaddr_copy(&conn->ripaddr, &bcast_addr);
  uip_send(uip_appdata, (int)(end - (uint8_t *)uip_appdata));

  icomprintf(GROUP_SOFTAP,LOG_INFO,"%s ip=%d.%d.%d.%d\n",__func__,lease->ipaddr.u8[0],lease->ipaddr.u8[1],lease->ipaddr.u8[2],lease->ipaddr.u8[3]);

  //set softap cenetrol control knows the status!!
  softap_set_state_dhcp_rack();
  softap_set_ip_info(&lease->chaddr[0],&lease->ipaddr.u8[0]);
  dhcp_dump_info();
  
//  icomprintf(GROUP_SOFTAP,LOG_INFO,"%s ACK\n",__func__);
}

static void
send_nack(struct uip_udp_conn *conn)
{
  uint8_t *end;
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
  
  create_msg(m);
  memset(&m->yiaddr, 0, 4);

  end = add_msg_type(&m->options[4], DHCPNAK);
  end = add_server_id(end);
  end = add_end(end);

  uip_ipaddr_copy(&conn->ripaddr, &bcast_addr);
  uip_send(uip_appdata, (int)(end - (uint8_t *)uip_appdata));
//  icomprintf(GROUP_SOFTAP,LOG_INFO,"%s NACK\n",__func__);
}

/*---------------------------------------------------------------------------*/
PROCESS(dhcp_server_process, "DHCP server");
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(dhcp_server_process, ev , data)
{
    static struct uip_udp_conn *conn;
    static struct uip_udp_conn *send_conn;
    static struct dhcps_client_lease *lease;
    
    PROCESS_BEGIN();
    icomprintf(GROUP_SOFTAP,LOG_INFO,"DHCP server starting\n");
    uip_ipaddr(&any_addr, 0,0,0,0);
    uip_ipaddr(&bcast_addr, 255,255,255,255);
    

    //uip_ipaddr(&uip_hostaddr,192,168,0,1);  //added for softap host-address!!


    conn = udp_new(&any_addr, UIP_HTONS(DHCPS_CLIENT_PORT), NULL);              //read any data!!
    if (!conn) {
        goto exit;
    }
    send_conn = udp_new(&bcast_addr, UIP_HTONS(DHCPS_CLIENT_PORT), NULL);   //write broadcast data!!
    if (!send_conn) {
        goto exit;
    }

    uip_udp_bind(conn, UIP_HTONS(DHCPS_SERVER_PORT));
    uip_udp_bind(send_conn, UIP_HTONS(DHCPS_SERVER_PORT));



    while(1) 
    {
        PROCESS_WAIT_EVENT();
        if(ev == tcpip_event) {
            if (uip_newdata()) 
            {
                struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
                //	struct uip_udpip_hdr *header = (struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN];

#if 0
                printf("m->op=0x%x\n",m->op);        
#endif
                icomprintf(GROUP_SOFTAP,LOG_INFO,"<%d>lease_time=%d &config->default_lease_time=0x%x\n",__LINE__,config->default_lease_time, &config->default_lease_time);

                if (m->op == DHCP_REQUEST && check_cookie() && m->hlen <= MAX_HLEN) 
                {
                    uint8_t *opt = find_option(DHCP_OPTION_MSG_TYPE);
                    if (opt) {
                        uint8_t mtype = opt[2];
                        if (opt[2] == DHCPDISCOVER) 
                        {
                            icomprintf(GROUP_SOFTAP,LOG_INFO,"***DHCP_Discover***\n");
                            icomprintf(GROUP_SOFTAP,LOG_INFO,"<%d>lease_time=%d &config->default_lease_time=0x%x\n",__LINE__,config->default_lease_time, &config->default_lease_time);

                            lease = dhcp_offer_choose_address();
                            icomprintf(GROUP_SOFTAP,LOG_INFO,"<%d>lease_time=%d &config->default_lease_time=0x%x\n",__LINE__,config->default_lease_time, &config->default_lease_time);

                            if (lease) 
                            {
                                lease->lease_end = clock_seconds()+config->default_lease_time;
                                tcpip_poll_udp(send_conn);
                                PROCESS_WAIT_EVENT_UNTIL(uip_poll());
                                icomprintf(GROUP_SOFTAP,LOG_INFO,"***DHCP_Offer***\n");

                                send_offer(conn,lease);
                            } else {
                                //icomprintf(GROUP_SOFTAP,LOG_ERROR,"*********Ignore the discover*********\n");
                                send_nack(send_conn);
                            }

                            
                        } else {
                            uint8_t *opt = find_option(DHCP_OPTION_SERVER_ID);
                            if (!opt || uip_ipaddr_cmp((uip_ipaddr_t*)&opt[2], &uip_hostaddr)) {
                                if (mtype == DHCPREQUEST) 
                                {
                                    icomprintf(GROUP_SOFTAP,LOG_INFO,"***DHCP_Request***\n");
                                    lease = dhcp_request_allocate_address();
                                    tcpip_poll_udp(send_conn);
                                    PROCESS_WAIT_EVENT_UNTIL(uip_poll());
#if 1
                                    //    printf("<%s>:\n",__func__);
                                    //    printf("lease=0x%x\n",lease);

                                    if(lease==NULL)
                                    {
                                        icomprintf(GROUP_SOFTAP,LOG_INFO,"<%s>mtype=%d,XXXlease=0xXXX%x\n",__func__,mtype,lease);
                                    } else {
                                        icomprintf(GROUP_SOFTAP,LOG_INFO,"<%s>mtype=%d,lease=0x%x\n",__func__,mtype,lease);
                                    }
#endif
                                    if (!lease) {
                                        icomprintf(GROUP_SOFTAP,LOG_INFO,"***DHCP_NACK(X)***\n"); 
                                        send_nack(send_conn);
                                    } else {
                                        icomprintf(GROUP_SOFTAP,LOG_INFO,"***DHCP_ACK(O)***\n"); 
                                        send_ack(send_conn,lease);
                                    }
                                } else if (mtype == DHCPRELEASE) {
                                    //printf("<%s>Release\n",__func__);
                                    icomprintf(GROUP_SOFTAP,LOG_INFO,"***DHCP_Release***\n");
                                    release_address();
                                } else if (mtype ==  DHCPDECLINE) {
                                    //printf("<%s>Decline\n",__func__);
                                    icomprintf(GROUP_SOFTAP,LOG_INFO,"***DHCP_Decline***\n");
                                } else if (mtype == DHCPINFORM) {
                                    //printf("<%s>Inform\n",__func__);
                                    icomprintf(GROUP_SOFTAP,LOG_INFO,"***DHCP_Inform***\n");
                                }
                            }
                        }
                    }
                }
            }
        } 
        else if (uip_poll()) 
        {
            icomprintf(GROUP_SOFTAP,LOG_INFO,"<%s> uip_poll\n",__func__);
        }

    }
    
 exit:
  icomprintf(GROUP_SOFTAP,LOG_INFO,"DHCP server exiting\n");
  
  PROCESS_END();
}

void dhcps_init(const struct dhcps_config *conf)
{
    config = conf;

    icomprintf(GROUP_SOFTAP,LOG_INFO,"<%s> conf = 0x%x, default_lease_time=%d\n",__func__,(U32)conf, conf->default_lease_time);
    icomprintf(GROUP_SOFTAP,LOG_INFO,"<%s> config = 0x%x, default_lease_time=%d\n",__func__,(U32)config, config->default_lease_time);
    process_start(&dhcp_server_process,NULL);
}

void dhcps_exit()
{
  process_exit(&dhcp_server_process);
}


