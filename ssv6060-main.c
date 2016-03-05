/*
 * Copyright (c) 2014, SouthSilicon Valley Corp.
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

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */
#include "contiki.h"
#include <string.h>
#include <stdio.h> /* For printf() */
#include "gpio_api.h"
#include "atcmd.h"
#include "resolv.h"
#include "ieee80211_mgmt.h"
#include "socket_driver.h"
#include "atcmd_icomm.h"
#include "wdog_api.h"
#include "zc_hf_adpter.h"
#include "zc_configuration.h"


#define MAX_RECV_BUFFER 	1472


int AT_Customer(stParam *param);
int AT_Customer2(stParam *param);
int AT_TCPSERVER_DEMO(stParam *param);
int AT_WIFIUART_DEMO(stParam *param);
int AT_SmartLink(stParam *param);
int AT_Reset(stParam *param);

void Ac_BcInit(void);



at_cmd_info atcmd_info_tbl[] = 
{
	{"AT+CUSTOMER_CMD",	&AT_Customer},
	{"AT+CUSTOMER_CMD2=",	&AT_Customer2},
	{"AT+TCPSERVER_DEMO=",	&AT_TCPSERVER_DEMO},
	{"AT+WIFIUART_DEMO=",&AT_WIFIUART_DEMO},
    {"AT+SMNT",&AT_SmartLink},
    {"AT+RESET",&AT_Reset},
    {"",    NULL}
};



int gTcpSocket = -1;
int g_Bcfd = -1;

static char *g_s8RecvBuf = NULL;

unsigned int g_connectflag = 0;

//extern void test1();
/*---------------------------------------------------------------------------*/
PROCESS(main_process, "main process");
PROCESS(ac_nslookup_process, "NSLookup Process");
PROCESS(ac_tcp_connect_process, "Tcp Connect Process");

/*---------------------------------------------------------------------------*/
PROCESS_NAME(tcpServerDemo_process);
PROCESS_NAME(wifiUartDemo_process);
/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&main_process, &ac_tcp_connect_process);

/*---------------------------------------------------------------------------*/

extern void HF_Init(void);

int allocate_buffer_in_ext(void)
{
    int rlt = -1;
    if(NULL == g_s8RecvBuf)
    {
        g_s8RecvBuf = malloc(1472);
        memset(g_s8RecvBuf, 0x0, 1472);
        rlt = 0;
    }
    return rlt;
}

 void TurnOffAllLED()
 {
	 GPIO_CONF conf;
 
	 conf.dirt = OUTPUT;
	 conf.driving_en = 0;
	 conf.pull_en = 0;
 
	 pinMode(GPIO_1, conf);
	 digitalWrite(GPIO_1, 0);	 
	 pinMode(GPIO_3, conf);
	 digitalWrite(GPIO_3, 0);	 
	 pinMode(GPIO_8, conf);
	 digitalWrite(GPIO_8, 0);	 
 
	 return;
 }
/*---------------------------------------------------------------------------*/
 int SetLED (uint8_t nEnable)
 {
 	GPIO_CONF conf;

	conf.dirt = OUTPUT;
	conf.driving_en = 0;
	conf.pull_en = 0;
	
	pinMode(GPIO_1, conf);
 	if(nEnable == 0)
 	{
 		digitalWrite(GPIO_1, 0);
 	}
 	else
 	{
 		digitalWrite(GPIO_1, 1);
 	}
 	return ERROR_SUCCESS;
 }

int AT_Customer(stParam *param)
{
	printf("Call AT_Customer\n");
	return 0;
}
int AT_Customer2(stParam *param)
{
	int i = 0;
	printf("Call AT_Customer2\n");
	for(i=0; i<param->argc; i++)
	{
		printf("Param%d:%s\n", i+1, param->argv[i]);
	}
	return 0;
}
int AT_WIFIUART_DEMO(stParam *param)
{
	int i = 0;
	printf("Call AT_WIFIUART_DEMO\n");
	if(strcmp(param->argv[0] ,"enable") == 0) {
		process_start(&wifiUartDemo_process, NULL);
	} else if(strcmp(param->argv[0] ,"disable") == 0) {
		process_post_synch(&wifiUartDemo_process, PROCESS_EVENT_EXIT, NULL);
	} else {
		printf("wifi uart demo unknown param, please check\n");
	}
	return 0;
}
int AT_TCPSERVER_DEMO(stParam *param)
{
	int i = 0;
	printf("Call AT_TCPSERVER_DEMO\n");
	if(strcmp(param->argv[0] ,"enable") == 0) {
		process_start(&tcpServerDemo_process, NULL);
	} else if(strcmp(param->argv[0] ,"disable") == 0) {
		process_post_synch(&tcpServerDemo_process, PROCESS_EVENT_EXIT, NULL);
	} else {
		printf("tcp server demo unknown param, please check\n");
	}
	return 0;
}

int AT_SmartLink(stParam *param)
{
    At_Disconnect();
    AT_RemoveCfsConf();
    api_wdog_reboot();
}

int AT_Reset(stParam *param)
{
    ZC_ConfigReset();
}


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(main_process, ev, data)
{
    static struct etimer periodic_timer;
    //    EVENTMSG_DATA *pMesg = NULL;
    PROCESS_BEGIN();

    printf("********hello main_process********\n");

    allocate_buffer_in_ext();
    
    HF_Init();

    Ac_BcInit();
    
    TurnOffAllLED();

    etimer_set(&periodic_timer, 50000);
    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        etimer_reset(&periodic_timer);
        HF_Run();
        HF_TimerExpired();
        HF_Cloudfunc();
    }
    PROCESS_END();
}

void Ac_Dns(char *buf)
{ 
	process_start(&ac_nslookup_process, NULL);
	bss_nslookup(buf);
}

void Ac_ConnectGateway(uip_ipaddr_t *ripaddr, uint16_t rport)
{
    gTcpSocket = tcpconnect(ripaddr, rport, &ac_tcp_connect_process);
    printf("1 create tcp socket:%d\n", gTcpSocket);
    g_connectflag = 1;
}

void Ac_BcInit(void)
{ 
	g_Bcfd = udpcreate(ZC_MOUDLE_PORT, &ac_tcp_connect_process);

	if(g_Bcfd == -1)
	{
		printf("create udp socket fail\n");
	}
	else
	{
		printf("create udp socket:%d\n", g_Bcfd);
	}

    return;
}


PROCESS_THREAD(ac_nslookup_process, ev, data)
{
    static NETSOCKET httpsock;
    SOCKETMSG msg;
    
	PROCESS_BEGIN();
	PROCESS_WAIT_EVENT_UNTIL(ev == resolv_event_found);
	{
		uip_ipaddr_t addr;
		uip_ipaddr_t *addrptr;
		addrptr = &addr;
	
		char *pHostName = (char*)data;
		if(ev == resolv_event_found)
		{
			resolv_lookup(pHostName, &addrptr);
			uip_ipaddr_copy(&addr, addrptr);
			printf("AT+NSLOOKUP=%d.%d.%d.%d\n", addr.u8[0], addr.u8[1], addr.u8[2], addr.u8[3]);

            gTcpSocket = tcpconnect( &addr, ZC_CLOUD_PORT, &ac_tcp_connect_process);
            printf("2 create tcp socket:%d\n", gTcpSocket);
		}
	}
	PROCESS_END();
}

PROCESS_THREAD(ac_tcp_connect_process, ev, data)
{
	PROCESS_BEGIN();
	SOCKETMSG msg;
	int recvlen;
	uip_ipaddr_t peeraddr;
	U16 peerport;

	while(1) 
	{
		PROCESS_WAIT_EVENT();

		if(ev == PROCESS_EVENT_EXIT) 
		{
			break;
		} 
		else if(ev == PROCESS_EVENT_MSG) 
		{
			msg = *(SOCKETMSG *)data;
			//The TCP socket is connected, This socket can send data after now.
			if(msg.status == SOCKET_CONNECTED)
			{
				printf("socket:%d connect cloud ok\n", msg.socket);
				if(msg.socket == gTcpSocket)
                {            
                    g_struProtocolController.u8MainState = PCT_STATE_WAIT_ACCESS;
                    g_struProtocolController.struCloudConnection.u32Socket = gTcpSocket;
		            g_struProtocolController.struCloudConnection.u32ConnectionTimes = 0;
                    /*Send Access to Cloud*/
                    ZC_Rand(g_struProtocolController.RandMsg);
                    PCT_SendCloudAccessMsg1(&g_struProtocolController);
                }
			}
			//TCP connection is closed. Clear the socket number.
			else if(msg.status == SOCKET_CLOSED)
			{
				printf("socked:%d closed\n", msg.socket);
				if(gTcpSocket == msg.socket && 1 == g_connectflag)
                {
                    tcpclose(gTcpSocket);
					gTcpSocket = -1;  
                    g_connectflag = 0;
                    /* ÖØÁ¬ */
                    g_struProtocolController.u8MainState = PCT_STATE_ACCESS_NET;
                }
                else
                {
                    tcpclose(msg.socket);
                }
			}
			//Get ack, the data trasmission is done. We can send data after now.
			else if(msg.status == SOCKET_SENDACK)
			{
				printf("socked:%d send data ack\n", msg.socket);
			}
			//There is new data coming. Receive data now.
			else if(msg.status == SOCKET_NEWDATA)
			{
				if(0 <= msg.socket && msg.socket < UIP_CONNS)
				{
				    if (gTcpSocket == msg.socket)
                    {            
    					recvlen = tcprecv(msg.socket, g_s8RecvBuf, MAX_RECV_BUFFER);
                        if (recvlen < 0)
                        {
                            printf("tcprecv error\n");
                            break;
                        }
                        printf("Recv from cloud, data len is %d\n", recvlen);
                        MSG_RecvDataFromCloud(g_s8RecvBuf, recvlen);
                    }
                    else
                    {
                        printf("Recv from client, socket is %d\n", msg.socket);
                    }
				}
				else if(UIP_CONNS <= msg.socket && msg.socket < UIP_CONNS + UIP_UDP_CONNS)
				{
					recvlen = udprecvfrom(msg.socket, g_s8RecvBuf, MAX_RECV_BUFFER, &peeraddr, &peerport);
					g_s8RecvBuf[recvlen] = 0;
					printf("UDP socked:%d recvdata:%s from %d.%d.%d.%d:%d\n", msg.socket, g_s8RecvBuf, peeraddr.u8[0], peeraddr.u8[1], peeraddr.u8[2], peeraddr.u8[3], peerport);
				}
				else
		        {
					printf("Illegal socket:%d\n", msg.socket);
				}
			}
			//A new connection is created. Get the socket number and attach the calback process if needed.
			else if(msg.status == SOCKET_NEWCONNECTION)
			{
			#if 0
				if(gserversock == -1)
				{
					gserversock = msg.socket;
					printf("new connected to listen port(%d), socket:%d\n", msg.lport, msg.socket);
				}
				else
				{
					//Only allow one client connection for this application.
					//tcpclose(msg.socket);
				}
            #endif
			}
			else
			{
				printf("unknow message type\n");
			}
		}	
	}	

	PROCESS_END();
}

