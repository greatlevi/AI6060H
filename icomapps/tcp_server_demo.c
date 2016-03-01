
#include "contiki-net.h"
#include "systemconf.h"
#include "socket_driver.h"
#include <gpio_api.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwm_api.h>
#include <serial_api.h>
#include "sys_status_api.h"
#include "comm_apps_res.h"

extern COMM_APPS_RES app_res;

/*---------------------------------------------------------------------------*/
PROCESS(led_blink_process, "led_blink_process");
PROCESS(tcp_server_demoProcess, "tcp_server_demoProcess");
PROCESS(tcpServerDemo_process, "tcpServerDemo_process");
/*---------------------------------------------------------------------------*/
int SetPWM (char *buff)
{
    int nRet = ERROR_SUCCESS;
    int nCycle = 0;
    
    if (strlen (buff) <= 0) {
        printf("SetPWM get len <= 0\n");
        nRet = ERROR_INVALID_PARAMETER;
    } else {
        nCycle = atoi(buff);
        //printf("enableHwPWM: %d\n", nCycle);
        enableHwPWM(nCycle);
    }
    return ERROR_SUCCESS;
}

int EnableLED()
{
    //printf("EnableLED\n");
    GPIO_CONF conf;
    conf.dirt = OUTPUT;
    conf.driving_en = 0;
    conf.pull_en = 0;
    pinMode(GPIO_1, conf);
    digitalWrite(GPIO_1, 1);

    return ERROR_SUCCESS;
}

int DisableLED()
{
    //printf("DisableLED\n");
    GPIO_CONF conf;
    conf.dirt = OUTPUT;
    conf.driving_en = 0;
    conf.pull_en = 0;
    pinMode(GPIO_1, conf);
    digitalWrite(GPIO_1, 0);

    return ERROR_SUCCESS;
}

int appTcpSend(char *buff, U16 len)
{	
    U8 status = ERROR_SUCCESS;
    
    if(tcpsend(app_res.tcpServerSock, buff, len) == -1) {
        printf("app send data fail\n");
        status = ERROR_TCP_CONNECTION;
    } else {
        printf("app send data pass\n");
    }
    return status;
}

int TCPDataParser (char *buff, int len)
{
    int nRet = ERROR_SUCCESS;
    char *pass = "ok";
    if (len<= 0) {
        printf("tcp server get len <= 0\n");
        nRet = ERROR_INVALID_PARAMETER;
    } else {
        if (!strncmp ((char *)buff, "LedOn", strlen("LedOn")) ) {
            printf("EnableLED\n");
            nRet = EnableLED();
            appTcpSend(pass,  strlen(pass));
        } else if (!strncmp ((char *)buff, "LedOff", strlen("LedOff"))) {
            printf("DisableLED\n");
            nRet = DisableLED();
            appTcpSend(pass,  strlen(pass));
        } else if (!strncmp ((char *)buff, "PWM=", strlen("PWM="))) {
            //printf("%s\n", buff);
            nRet = SetPWM (buff+strlen("PWM="));
        } else {
            nRet = ERROR_INVALID_PARAMETER;
            printf("tcp server get nuknown data: %s\n", buff);
        }
    }
    
    return nRet;
}

PROCESS_THREAD(tcp_server_demoProcess, ev, data)
{
    PROCESS_BEGIN();
    
    SOCKETMSG msg;
    int recvlen;
    printf("tcp_server_process begin\n");
        
    while(1) {
        PROCESS_WAIT_EVENT();
        if(ev == PROCESS_EVENT_EXIT) {
            break;
        } else if(ev == PROCESS_EVENT_MSG) {
            msg = *(SOCKETMSG *)data;
            //TCP connection is closed. Clear the socket number.
            if(msg.status == SOCKET_CLOSED) {
                printf("socked:%d closed\n", (int)msg.socket);
                if(app_res.tcpServerSock == msg.socket)
                    app_res.tcpServerSock = -1;
            //Get ack, the data trasmission is done. We can send data after now.
            } else if(msg.status == SOCKET_SENDACK) {
                printf("socked:%d send data ack\n", (int)msg.socket);
            //There is new data coming. Receive data now.
            } else if(msg.status == SOCKET_NEWDATA) {
                if(0 <= msg.socket && msg.socket < UIP_CONNS) {
                    recvlen = tcprecv(msg.socket, (char *)app_res.rxBuffer, MAX_BUFFER);
                    app_res.rxBuffer[recvlen] = '\0';
                    TCPDataParser((char*)app_res.rxBuffer, recvlen);
                } else {
                    printf("Illegal socket:%d\n", (int)msg.socket);
                }
            //A new connection is created. Get the socket number and attach the calback process if needed.
            } else if(msg.status == SOCKET_NEWCONNECTION) {
                printf("app_res.tcpServerSock : %d\n", app_res.tcpServerSock);
                if(app_res.tcpServerSock == -1) {
                    app_res.tcpServerSock = msg.socket;
                    printf("new connected to listen port(%d), socket:%d\n", msg.lport, (int)msg.socket);
                } else {
                    //Only allow one client connection for this application.
                    tcpclose(msg.socket);
                }
            } else {
                printf("unknow message type: %d\n", msg.status);
            }
        }	
    } 
    printf("tcp_server_process end\n");
    PROCESS_END();
}

PROCESS_THREAD(led_blink_process, ev, data)
{
    static struct etimer blinkTimer;
    static int blinkcount = 0;
    PROCESS_BEGIN();
    printf("led_blink_process begin\n");

    while(1) {
        etimer_set(&blinkTimer, 500 * CLOCK_MINI_SECOND );
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER || ev == PROCESS_EVENT_EXIT);
        if( ev == PROCESS_EVENT_EXIT) {
            process_exit(&led_blink_process);
            break;
        } 

        if( blinkcount%2 == 0) {
            EnableLED();
        } else {
            DisableLED();
        }
        blinkcount++;
    }
    printf("led_blink_process end\n");
    PROCESS_END();
}

PROCESS_THREAD(tcpServerDemo_process, ev, data)
{
    static struct etimer tcpServer_timer;
    PROCESS_BEGIN();
    printf("tcpServerDemo_process begin\n");

    if(app_res.res_use == 0) {
        memset(&app_res, 0, sizeof(COMM_APPS_RES));
        app_res.tcpServerSock = -1;
        app_res.res_use = 1;
        process_start(&led_blink_process, NULL);
        
        while(1) {
            etimer_set(&tcpServer_timer, 1 * CLOCK_SECOND);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER || PROCESS_EVENT_EXIT);
            if(get_wifi_connect_status() == IEEE80211_CONNECTED) {
                break;
            } else if(ev == PROCESS_EVENT_EXIT) {
                break;
            }
        }
        
        process_post_synch(&led_blink_process, PROCESS_EVENT_EXIT, NULL);
        
        if(ev != PROCESS_EVENT_EXIT) {
            process_start(&tcp_server_demoProcess, NULL);
            etimer_set(&tcpServer_timer, 1 * CLOCK_SECOND);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
            if(tcplisten(TCP_SERVER_DMEO_PORT, &tcp_server_demoProcess) == -1) {
                printf("listen tcp fail\n");
            }        
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_EXIT);
            tcpunlisten(TCP_SERVER_DMEO_PORT);
            app_res.tcpServerSock = -1;
            process_post_synch(&tcp_server_demoProcess, PROCESS_EVENT_EXIT, NULL);
        }
        app_res.res_use = 0;
    } else {
        printf("comm res used, please check and exit process\n");
    }
    printf("tcpServerDemo_process end\n");    
    PROCESS_END();
}