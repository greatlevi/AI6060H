
#include <stdio.h>
#include "systemconf.h"
#include "serial_api.h"
#include "drv_uart.h"
#include "socket_driver.h"

#define MAX_BUFFER 	(128)
#define TCP_SERVER_DMEO_PORT (12345)


typedef struct t_COMM_APPS_RES
{
    U8 res_use;
    U8 send_data_and_get_ack;
    U8 rxBuffer[MAX_BUFFER];
    U8 txBuffer[MAX_BUFFER];
    U8 tcpServerStatus;
    S32 buf_length;
    S32 tcpServerSock;
} COMM_APPS_RES;

COMM_APPS_RES app_res;
