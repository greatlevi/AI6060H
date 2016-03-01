#ifndef _SOCKET_APP_H_
#define _SOCKET_APP_H_

#include "net/simple-udp.h"

#define MAX_BUFFER (256)
#define SMART_TCP_PORT (8209)
#define DISCOVER_PORT (8209)
#define APP_TCPSERVER_PORT (1025)

typedef struct  _SOCKET_PARA
{
    U8 otaProcessUp;
    U8 tcpServerUp;
    U8 wifiUartServerUp;
    U8 exitTcpServerCnt;
    U8 exitWifiUartServerCnt;
    U8 remoteMAC[17];
    U8 buffer_in[MAX_BUFFER];
    U32 buffer_out;
    S32 gTcpClientSmartSock;
    S32 gTcpServersock;
    S32 gUdpServersock;
    U8 smartSockStatus;
    U8 tcpServerStatus;
    U8 udpListenStatus;
    U8 exitSmartLinkCnt;
} SOCKET_PARA;

typedef struct  _NET_STATUS
{
    uip_ipaddr_t remote_ip_addr;
} NET_STATUS;

NET_STATUS gSocketNetStatus;

int appTcpListen();
int appTcpUnlisten();
void otaProcessDisable();
void setRrmoteIpAddress(U8 add0, U8 add1, U8 add2, U8 add3);
void setRemoteMAC(U8 *buf, U8 len);
int SetLEDType(U8 nEnable);
int getCmdNum(U8 *data, U16 datalen);

#endif
