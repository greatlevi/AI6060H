
#include <stdio.h>
#include "systemconf.h"
#include "serial_api.h"
#include "drv_uart.h"
#include "socket_driver.h"
#include "comm_apps_res.h"
#include "ssv_lib.h"

extern COMM_APPS_RES app_res;

PROCESS(wifiUartDemo_process, "wifiUartDemo_process");
PROCESS(tcp_server_wifiUartProcess, "tcp_server_wifiUartProcess");

/*---------------------------------------------------------------------------*/
int sampleAppTCPSend(unsigned char *buff, int len)
{	
    U8 status = ERROR_SUCCESS;

    if(app_res.send_data_and_get_ack == 1) {
            printf("tcp send data and not get ack\n");
            status = ERROR_TCP_CONNECTION;
    } else {
        printf("tcpsend tcpServerSock: %d\n", app_res.tcpServerSock);
        if(tcpsend(app_res.tcpServerSock, buff, len) == -1) {
            printf("app send data fail\n");
            status = ERROR_TCP_CONNECTION;
        } else {
            app_res.send_data_and_get_ack = 1;
            printf("app send data pass\n");
        }
    }

    return status;
}
/*---------------------------------------------------------------------------*/
void sampleDataUartRead(void *dara)
{
    S32 input;
    
    while(1) {
        input = drv_uart_rx(SSV6XXX_UART1);//SerialRead();
        if(input == -1) {
            break;
        } else {
            app_res.txBuffer[app_res.buf_length++] = (U8)input;
            if((U8)input==0x0d && app_res.buf_length <= MAX_BUFFER) {
                printf("%s\n", app_res.txBuffer);
                sampleAppTCPSend((U8 *)&app_res.txBuffer, app_res.buf_length);
                app_res.buf_length=0;
                break;
            } else if(app_res.buf_length == MAX_BUFFER) {
                printf("datauart full, send tcp data and clear buf\n");
                printf("%s\n", app_res.rxBuffer);
                sampleAppTCPSend((U8 *)&app_res.txBuffer, app_res.buf_length);
                app_res.buf_length=0;
                break;
            }
        }
    }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(tcp_server_wifiUartProcess, ev, data)
{
    PROCESS_BEGIN();
    
    SOCKETMSG msg;
    int recvlen;
    
    printf("tcp_server_wifiUartProcess begin\n");
    
    while(1) {
        PROCESS_WAIT_EVENT();
        if(ev == PROCESS_EVENT_EXIT) {
            break;
        } else if(ev == PROCESS_EVENT_MSG) {
            msg = *(SOCKETMSG *)data;
            //TCP connection is closed. Clear the socket number.
            if(msg.status == SOCKET_CLOSED) {
                printf("socked:%d closed\n", msg.socket);
                app_res.tcpServerStatus = SOCKET_CLOSED;
                if(app_res.tcpServerSock == msg.socket)
                    app_res.tcpServerSock = -1;
            //Get ack, the data trasmission is done. We can send data after now.
            } else if(msg.status == SOCKET_SENDACK) {
                printf("socked:%d send data ack\n", msg.socket);
                app_res.tcpServerStatus = SOCKET_SENDACK;
                app_res.send_data_and_get_ack = 0;
            //There is new data coming. Receive data now.
            } else if(msg.status == SOCKET_NEWDATA) {
                if(0 <= msg.socket && msg.socket < UIP_CONNS) {
                    app_res.tcpServerStatus = SOCKET_NEWDATA;
                    recvlen = tcprecv(msg.socket, app_res.rxBuffer, MAX_BUFFER);
                    app_res.rxBuffer[recvlen] = 0;
                    printf("rece: %s\n",  app_res.rxBuffer);
                    SerialWrite(app_res.rxBuffer, recvlen);
                } else {
                    printf("Illegal socket:%d\n", msg.socket);
                }
            //A new connection is created. Get the socket number and attach the calback process if needed.
            } else if(msg.status == SOCKET_NEWCONNECTION) {
                printf("app_res.tcpServerSock : %d\n", app_res.tcpServerSock);
                app_res.tcpServerStatus = SOCKET_NEWCONNECTION;
                if(app_res.tcpServerSock == -1) {
                    app_res.tcpServerSock = msg.socket;
                    printf("new connected to listen port(%d), socket:%d\n", msg.lport, msg.socket);
                } else {
                    //Only allow one client connection for this application.
                    tcpclose(msg.socket);
                }
            } else {
                printf("unknow message type: %d\n", msg.status);
            }
        }	
    } 
    
    printf("tcp_server_wifiUartProcess end\n");
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(wifiUartDemo_process, ev, data)
{

    PROCESS_BEGIN();
    printf("wifiUartDemo_process begin\n");

    if(app_res.res_use == 0) {
        memset(&app_res, 0, sizeof(COMM_APPS_RES));
        app_res.tcpServerSock = -1;
        app_res.res_use = 1;
        app_res.send_data_and_get_ack = 0;
        process_start(&tcp_server_wifiUartProcess, NULL);
        if(tcplisten(TCP_SERVER_DMEO_PORT, &tcp_server_wifiUartProcess) == -1) {
            printf("listen tcp fail\n");
        }
        SerialInit(BAUD_115200, sampleDataUartRead, NULL);
        
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_EXIT);
        tcpunlisten(TCP_SERVER_DMEO_PORT);
        app_res.tcpServerSock = -1;
        process_post_synch(&tcp_server_wifiUartProcess, PROCESS_EVENT_EXIT, NULL);
        SerialDeinit();
        app_res.res_use = 0;
    } else {
        printf("comm res used, please check and exit process\n");
    }
    printf("wifiUartDemo_process end\n");
    PROCESS_END();
}