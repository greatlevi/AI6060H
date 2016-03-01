
#include "contiki-net.h"
#include "systemconf.h"
#include "socket_driver.h"
#include "socket_app.h"

#include "smartComm.h"
#include <gpio_api.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ota_api.h"
#include "sys_status_api.h"

/*---------------------------------------------------------------------------*/
//struct data
static SOCKET_PARA gSocketNetPara;
//extern data
extern IEEE80211STATUS gwifistatus;
extern smartComm smart;
extern TAG_CABRIO_CONFIGURATION gCabrioConf;
/*---------------------------------------------------------------------------*/
PROCESS(tcp_client_smartProcess, "tcp client smartProcess");
PROCESS(udp_server_discoverProcess, "udp server discoverProcess");
PROCESS(discover_process, "discover Process");
PROCESS(tcp_smartlink_process, "tcp smartlink Process");
/*---------------------------------------------------------------------------*/
#if (FUNC_OTA_ON==1) 
PROCESS_NAME(ota_updateProcess);
#endif
PROCESS_NAME(wifiUartDemo_process);
PROCESS_NAME(tcpServerDemo_process);
/*---------------------------------------------------------------------------*/
void otaProcessDisable()
{
    gSocketNetPara.otaProcessUp = 0;
}
/*---------------------------------------------------------------------------*/
void setRemoteMAC(U8 *buf, U8 len)
{
    sprintf((char *)gSocketNetPara.remoteMAC, "%x:%x:%x:%x:%x:%x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
}
/*---------------------------------------------------------------------------*/
void setRrmoteIpAddress(U8 add0, U8 add1, U8 add2, U8 add3)
{
    gSocketNetStatus.remote_ip_addr.u8[0]  = add0;
    gSocketNetStatus.remote_ip_addr.u8[1]  = add1;
    gSocketNetStatus.remote_ip_addr.u8[2]  = add2;
    gSocketNetStatus.remote_ip_addr.u8[3]  = add3;
}
/*---------------------------------------------------------------------------*/
int getCmdNum(U8 *data, U16 datalen)
{
    int status = ERROR_SUCCESS;
    int i = 0, cmd = 0;
    char *pCmd = 0;
    char *checkMagicNum = "iComm=";
    
    if(datalen <= strlen(checkMagicNum)) {
        status = -1;
        goto exit;
    }
    for(i = 0; i < strlen(checkMagicNum); i++) {
        if(*(data+ i) != checkMagicNum[i]) {
            status = -1;
            goto exit;
        }
    }
    
    pCmd = (char *)data + strlen(checkMagicNum);
    cmd = atoi(pCmd);
    status = cmd;
    if(cmd <= MINCMD || cmd >= MAXCMD) {
        printf("unkonw cmd: %d##\n",  cmd);
        status = -1;
    }
    
exit:
    return status;
}
/*---------------------------------------------------------------------------*/
int smartTcpSend(U8 *buff, U16 len)
{	
    U8 status = ERROR_SUCCESS;
    
    if(tcpsend(gSocketNetPara.gTcpClientSmartSock, (char *)buff, len) == -1) {
        printf("smart send data fail\n");
        status = ERROR_TCP_CONNECTION;
        
    } else {
        printf("smart send data pass\n");
    }
    return status;
}
/*---------------------------------------------------------------------------*/
int smartTcpConnect()
{
    gSocketNetPara.smartSockStatus = SOCKET_CREATE;
    printf("connect to %d.%d.%d.%d\n", gSocketNetStatus.remote_ip_addr.u8[0],gSocketNetStatus.remote_ip_addr.u8[1],
                                                                          gSocketNetStatus.remote_ip_addr.u8[2],gSocketNetStatus.remote_ip_addr.u8[3]);
    gSocketNetPara.gTcpClientSmartSock = tcpconnect(&gSocketNetStatus.remote_ip_addr, SMART_TCP_PORT, &tcp_client_smartProcess);
    printf("create tcp smart socket:%d\n", gSocketNetPara.gTcpClientSmartSock);
    
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int smartTcpDisConnect()
{
    U8 status = ERROR_SUCCESS;
    
    if(tcpclose(gSocketNetPara.gTcpClientSmartSock) == -1) {
        printf("smart tcp disconnect fail\n");
        status = ERROR_TCP_CONNECTION;
    }
    
    return status;
}
/*---------------------------------------------------------------------------*/
int appUdpCreate()
{
    U8 status = ERROR_SUCCESS;
    gSocketNetPara.gUdpServersock = udpcreate(DISCOVER_PORT, &udp_server_discoverProcess);

    if(gSocketNetPara.gUdpServersock == -1) {
        printf("create udp socket fail\n");
        status = ERROR_UDP_CONNECTION;
    } else {
        //printf("create socket:%d\n", gSocketNetPara.gUdpServersock);
        status = ERROR_SUCCESS;
    }
    return status;
}
/*---------------------------------------------------------------------------*/
int appUdpSend(uip_ipaddr_t peeraddr, U16 peerport, char *pData,  U16 dataLen)
{
    U8 status = ERROR_SUCCESS;
    
    if(udpsendto(gSocketNetPara.gUdpServersock, pData, dataLen, &peeraddr, peerport) == -1) {
        status = -1;
    }
    
    return status;
}
/*---------------------------------------------------------------------------*/
int appUdpClose()
{
    U8 status = ERROR_SUCCESS;
    
    if(udpclose(gSocketNetPara.gUdpServersock) == -1) {
        status = ERROR_UDP_CONNECTION;
    } else {
        gSocketNetPara.gUdpServersock = -1;
        status = ERROR_SUCCESS;
    }
    return status;
}
/*---------------------------------------------------------------------------*/
void checkCmdAndSendData(uip_ipaddr_t peeraddr, U16 peerport)
{
    int cmd = 0, i = 0;
    U16 len = 0;
    char buf[32];
    char mac[6];
    cmd = getCmdNum(gSocketNetPara.buffer_in, (U16)strlen((char *)gSocketNetPara.buffer_in));
    if(cmd== DISCOVER) {
        len = 6;
        get_local_mac(mac,len);
        memcpy(buf, mac, len);
        get_connectAP_mac(mac,len);
        //printf("%x:%x:%x:%x:%x:%x (%d)\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], get_ap_lsit_total_num());
        for (i=0; i<get_ap_lsit_total_num(); i++) {
            if (strncmp (mac, (char*)(&(ap_list[i]).mac), 6) == 0) {
                buf[len] = ap_list[i].rssi_level + 0x30;
                len = 7;
                buf[len] = 0;
                appUdpSend(peeraddr, peerport, buf, len);
                break;
            }
        }
        gSocketNetPara.exitTcpServerCnt = 0;
        gSocketNetPara.exitWifiUartServerCnt = 0;
    } else if(cmd == TCPUP) {
        if(gSocketNetPara.wifiUartServerUp == 1) {
                process_post_synch(&wifiUartDemo_process, PROCESS_EVENT_EXIT, NULL);
                gSocketNetPara.wifiUartServerUp = 0;
        }
        if(gSocketNetPara.tcpServerUp == 0) {
            gSocketNetPara.gTcpServersock = -1;
            process_start(&tcpServerDemo_process, NULL);
            gSocketNetPara.tcpServerUp = 1;
        }
        gSocketNetPara.exitTcpServerCnt = 0;
    } else if(cmd == WIFIUART) {
        if(gSocketNetPara.tcpServerUp == 1) {
                process_post_synch(&tcpServerDemo_process, PROCESS_EVENT_EXIT, NULL);
                gSocketNetPara.tcpServerUp = 0;
        }
        if(gSocketNetPara.wifiUartServerUp == 0) {
            gSocketNetPara.gTcpServersock = -1;
            process_start(&wifiUartDemo_process, NULL);
            gSocketNetPara.wifiUartServerUp = 1;
        }
        gSocketNetPara.exitWifiUartServerCnt = 0;
#if (FUNC_OTA_ON==1)      
    } else if(cmd == OTA) {
        if(gSocketNetPara.otaProcessUp == 0) {
            setOtaRrmoteIp(peeraddr);
            process_start(&ota_updateProcess, NULL);
            gSocketNetPara.otaProcessUp = 1;
        }
        gSocketNetPara.exitTcpServerCnt = 0;
#endif        
    } else {
        len = 7;
        memcpy(buf, "unknown", len);
    }
    
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(tcp_client_smartProcess, ev, data)
{
    PROCESS_BEGIN();
    
    SOCKETMSG msg;
    int recvlen;
    
    gSocketNetPara.smartSockStatus = SOCKET_CLOSED;
    
    while(1)  {
        PROCESS_WAIT_EVENT();

        if( ev == PROCESS_EVENT_EXIT) {
            break;
        } else if(ev == PROCESS_EVENT_MSG) {
            msg = *(SOCKETMSG *)data;
            //The TCP socket is connected, This socket can send data after now.
            if(msg.status == SOCKET_CONNECTED) {
                printf("smart socked:%d connected\n", (int)msg.socket);
                gSocketNetPara.smartSockStatus = SOCKET_CONNECTED;
                smartTcpSend(gSocketNetPara.remoteMAC, (U16)strlen((char *)gSocketNetPara.remoteMAC));
            } else if(msg.status == SOCKET_CLOSED) {
                if(gSocketNetPara.gTcpClientSmartSock == msg.socket)
                    gSocketNetPara.gTcpClientSmartSock = -1;
                 if(gSocketNetPara.smartSockStatus == SOCKET_CREATE) {
                    printf("smart socked:%d create fail and retry\n", (int)msg.socket);
                } else {
                    printf("smart socked:%d closed\n", (int)msg.socket);
                    gSocketNetPara.smartSockStatus = SOCKET_CLOSED;
                }
            //Get ack, the data trasmission is done. We can send data after now.
            } else if(msg.status == SOCKET_SENDACK) {
                printf("socked:%d send data ack\n", (int)msg.socket);
                gSocketNetPara.smartSockStatus = SOCKET_SENDACK;
            //There is new data coming. Receive data now.
            } else if(msg.status == SOCKET_NEWDATA) {
                if(0 <= msg.socket && msg.socket < UIP_CONNS) {
                    recvlen = tcprecv(msg.socket, (char *)gSocketNetPara.buffer_in, MAX_BUFFER);
                    gSocketNetPara.buffer_in[recvlen] = 0;
                    printf("TCP socked:%d recvdata:%s\n", (int)msg.socket, gSocketNetPara.buffer_in);
                    gSocketNetPara.smartSockStatus = SOCKET_NEWDATA;
                    if(gSocketNetPara.buffer_in[0] == 'o' && gSocketNetPara.buffer_in[1]  == 'k') {
                        smartTcpDisConnect();
                    }
                } else {
                    printf("Illegal socket:%d\n", (int)msg.socket);
                }
            } else {
                printf("unknow message type: %d\n", msg.status);
            }
        }	
    } 
    
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_discoverProcess, ev, data)
{
    PROCESS_BEGIN();
    SOCKETMSG msg;
    int recvlen;
    uip_ipaddr_t peeraddr;
    U16 peerport;

    appUdpCreate();
    
    while(1) {
        PROCESS_WAIT_EVENT();
        
        if( ev == PROCESS_EVENT_EXIT) {
            gSocketNetPara.udpListenStatus = SOCKET_CLOSED;
            gSocketNetPara.gUdpServersock = -1;
            break;
        } else if(ev == PROCESS_EVENT_MSG) {
            msg = *(SOCKETMSG *)data;
            //The TCP socket is connected, This socket can send data after now.
            if(msg.status == SOCKET_CLOSED) {
                printf("socked:%d closed\n", (int)msg.socket);
                gSocketNetPara.udpListenStatus = SOCKET_CLOSED;
                if(gSocketNetPara.gUdpServersock == msg.socket)
                    gSocketNetPara.gUdpServersock = -1;
            //Get ack, the data trasmission is done. We can send data after now.
            } else if(msg.status == SOCKET_SENDACK) {
                printf("socked:%d send data ack\n", (int)msg.socket);
                gSocketNetPara.udpListenStatus = SOCKET_SENDACK;
            //There is new data coming. Receive data now.
            } else if(msg.status == SOCKET_NEWDATA) {
                if(UIP_CONNS <= msg.socket && msg.socket < UIP_CONNS + UIP_UDP_CONNS) {
                    gSocketNetPara.udpListenStatus = SOCKET_NEWDATA;
                    recvlen = udprecvfrom(msg.socket, (char *)gSocketNetPara.buffer_in, MAX_BUFFER, &peeraddr, &peerport);
                    gSocketNetPara.buffer_in[recvlen] = 0;
                    //printf("app UDP socked:%d recvdata:%s from %d.%d.%d.%d:%d\n", msg.socket, gSocketNetPara.buffer_in, peeraddr.u8[0], peeraddr.u8[1], peeraddr.u8[2], peeraddr.u8[3], peerport);
                    checkCmdAndSendData(peeraddr, peerport);
                } else {
                    printf("Illegal socket:%d\n", (int)msg.socket);
                }
            //A new connection is created. Get the socket number and attach the calback process if needed.
            } else {
                printf("unknow message type: %d\n", msg.status);
            }
        }	
    }
    
    appUdpClose();
    
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(discover_process, ev, data)
{
    static struct etimer periodic_timer;
    
    PROCESS_BEGIN();
    //icomprintf("enable iocmm discover process");
    gSocketNetPara.tcpServerUp = 0;
    gSocketNetPara.wifiUartServerUp = 0;
    process_start(&udp_server_discoverProcess, NULL);
    etimer_set(&periodic_timer, 1 * CLOCK_SECOND);
    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        etimer_reset(&periodic_timer);
        if(gSocketNetPara.tcpServerUp == 1) {
            gSocketNetPara.exitTcpServerCnt++;
            if(gSocketNetPara.exitTcpServerCnt > 3) {
                process_post_synch(&tcpServerDemo_process, PROCESS_EVENT_EXIT, NULL);
                gSocketNetPara.tcpServerUp = 0;
            }
        } else if(gSocketNetPara.wifiUartServerUp == 1) {
            gSocketNetPara.exitWifiUartServerCnt++;
            if(gSocketNetPara.exitWifiUartServerCnt > 3) {
                process_post_synch(&wifiUartDemo_process, PROCESS_EVENT_EXIT, NULL);
                gSocketNetPara.wifiUartServerUp = 0;
            }
        }
    }
    process_exit(&udp_server_discoverProcess);
    //icomprintf("disable iocmm discover process");
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(tcp_smartlink_process, ev, data)
{
    static struct etimer periodic_timer;
    
    PROCESS_BEGIN();
    printf("tcp_smartlink_process begin\n");
    
    gSocketNetPara.exitSmartLinkCnt = 0;
    while(1) {
        send_needip_arp_request(gSocketNetStatus.remote_ip_addr);
        etimer_set(&periodic_timer, 1 * CLOCK_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        if(check_ip_in_arptable(gSocketNetStatus.remote_ip_addr) == 0) {
            process_start(&tcp_client_smartProcess, NULL);
            smartTcpConnect();
            break;
        }
        gSocketNetPara.exitSmartLinkCnt++;
        if(gSocketNetPara.exitSmartLinkCnt > 10) {
            printf("cant not find ip in arp table, exit tcp smartlink process\n");
            break;
        }
    }
    etimer_set(&periodic_timer, 1 * CLOCK_SECOND);
    gSocketNetPara.exitSmartLinkCnt = 0;
    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        etimer_reset(&periodic_timer);
        if(gSocketNetPara.smartSockStatus == SOCKET_CLOSED) {
            break;
        } else if(gSocketNetPara.smartSockStatus == SOCKET_CREATE) {
            smartTcpConnect();
        }
        gSocketNetPara.exitSmartLinkCnt++;
        if(gSocketNetPara.exitSmartLinkCnt > 10) {
            smartTcpDisConnect();
            printf("cant not rece message with phone, exit tcp smartlink process\n");
            break;
        }
    }
    process_post_synch(&tcp_client_smartProcess, PROCESS_EVENT_EXIT, NULL);
    gCabrioConf.enableSmartSoftFilter = FALSE;
    printf("tcp_smartlink_process end\n");
    PROCESS_END();
}

