/**
******************************************************************************
* @file     zc_hf_adpter.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Event
******************************************************************************
*/
#include <zc_protocol_controller.h>
#include <zc_timer.h>
#include <zc_module_interface.h>
#include <zc_hf_adpter.h>
#include <stdlib.h>
#include <stdio.h> 
#include <stdarg.h>
#include <ac_cfg.h>
#include <ac_api.h>
#include "etimer.h"
#include "wdog_api.h"
#include "sys_status_api.h"
#include "flash_api.h"
#include "socket_driver.h"
#include "clock.h"


extern PTC_ProtocolCon  g_struProtocolController;
PTC_ModuleAdapter g_struHfAdapter;

MSG_Buffer g_struRecvBuffer;
MSG_Buffer g_struRetxBuffer;
MSG_Buffer g_struClientBuffer;


MSG_Queue  g_struRecvQueue;
MSG_Buffer g_struSendBuffer[MSG_BUFFER_SEND_MAX_NUM];
MSG_Queue  g_struSendQueue;

u8 g_u8MsgBuildBuffer[MSG_BULID_BUFFER_MAXLEN];
u8 g_u8ClientSendLen = 0;

u16 g_u16TcpMss;
u16 g_u16LocalPort;

u8 g_u8recvbuffer[HF_MAX_SOCKET_LEN];
ZC_UartBuffer g_struUartBuffer;
struct etimer g_struHfTimer[ZC_TIMER_MAX_NUM];

//HF_TimerInfo g_struHfTimer[ZC_TIMER_MAX_NUM];
//sys_mutex_t g_struTimermutex;
u8  g_u8BcSendBuffer[100];
u32 g_u32BcSleepCount = 800;
//struct sockaddr_in struRemoteAddr;

u16 g_u16TiTimerCount[ZC_TIMER_MAX_NUM];
//flash_t cloud_flash;

u32 newImg2Addr = 0xFFFFFFFF;
u32 oldImg2Addr = 0xFFFFFFFF;

extern u8  g_u8ModuleKey[ZC_MODULE_KEY_LEN];
//extern uart_socket_t *g_uart_socket;

//u32 g_u32SmartConfigFlag = 0;

//extern int write_ota_addr_to_system_data(flash_t *flash, u32 ota_addr);
/*************************************************
* Function: HF_ReadDataFormFlash
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_ReadDataFromFlash(u8 *pu8Data, u16 u16Len) 
{
    spi_flash_read(FLASH_ADDRESS, pu8Data, u16Len);
}
/*************************************************
* Function: HF_WriteDataToFlash
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_WriteDataToFlash(u8 *pu8Data, u16 u16Len)
{
    spi_flash_sector_erase(FLASH_ADDRESS);
    spi_flash_write(FLASH_ADDRESS, pu8Data, u16Len);
    spi_flash_finalize(FLASH_ADDRESS);
}
/*************************************************
* Function: HF_TimerExpired
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_TimerExpired(void)
{
    u8 u8Index;
    u8 u8Status;
    for (u8Index = 0; u8Index < ZC_TIMER_MAX_NUM; u8Index++)
    {   
        TIMER_GetTimerStatus(u8Index, &u8Status);
        if (ZC_TIMER_STATUS_USED == u8Status)
        {
            if (etimer_expired(&g_struHfTimer[u8Index]))
            {
                TIMER_StopTimer(u8Index);
                TIMER_TimeoutAction(u8Index);
            }
        }
    }   
}
/*************************************************
* Function: HF_StopTimer
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_StopTimer(u8 u8TimerIndex)
{
    //g_struHfTimer[u8TimerIndex].u8ValidFlag = 0;
}

/*************************************************
* Function: HF_SetTimer
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 HF_SetTimer(u8 u8Type, u32 u32Interval, u8 *pu8TimeIndex)
{
    u8 u8TimerIndex;
    u32 u32Retval;
    u32Retval = TIMER_FindIdleTimer(&u8TimerIndex);
    if (ZC_RET_OK == u32Retval)
    {
        TIMER_AllocateTimer(u8Type, u8TimerIndex, (u8*)&g_struHfTimer[u8TimerIndex]);
        etimer_set(&g_struHfTimer[u8TimerIndex], u32Interval / 1000 * CLOCK_SECOND);
        //timer_set(&g_struHfTimer[u8TimerIndex], u32Interval);
        *pu8TimeIndex = u8TimerIndex;
    }
    else
    {
        ZC_Printf("MT_SetTimer: no idle timer\n");
    }
    return u32Retval;
}
/*************************************************
* Function: HF_FirmwareUpdateFinish
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 HF_FirmwareUpdateFinish(u32 u32TotalLen)
{
    return 0;
}
/*************************************************
* Function: HF_FirmwareUpdate
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 HF_FirmwareUpdate(u8 *pu8FileData, u32 u32Offset, u32 u32DataLen)
{
    return ZC_RET_OK;
}
/*************************************************
* Function: HF_SendDataToMoudle
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 HF_SendDataToMoudle(u8 *pu8Data, u16 u16DataLen)
{
    AC_RecvMessage((ZC_MessageHead *)pu8Data);
    return ZC_RET_OK;
}
/*************************************************
* Function: HF_Rest
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_Rest(void)
{ 
    api_wdog_reboot();
}
/*************************************************
* Function: HF_SendTcpData
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_SendTcpData(u32 u32Fd, u8 *pu8Data, u16 u16DataLen, ZC_SendParam *pstruParam)
{
    //send(u32Fd, pu8Data, u16DataLen, 0);
    tcpsend(u32Fd, pu8Data, u16DataLen);
}
/*************************************************
* Function: HF_SendUdpData
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_SendUdpData(u32 u32Fd, u8 *pu8Data, u16 u16DataLen, ZC_SendParam *pstruParam)
{
    /* just test, need to modify later */
    uip_ip4addr_t server_ip;
#if 0
    sendto(u32Fd,(char*)pu8Data,u16DataLen,0,
        (struct sockaddr *)pstruParam->pu8AddrPara,
        sizeof(struct sockaddr_in)); 
#endif
    if(udpsendto(u32Fd, pu8Data, u16DataLen, &server_ip, 1111) == -1)
    {
        printf("udpsendto fail\n");
    }
}
/*************************************************
* Function: HF_GetMac
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_GetMac(u8 *pu8Mac)
{
#if 0
    u8 mac[32] = {0};
    u8 macTmp[12] = {0};
	wifi_get_mac_address((char*)mac);
    sscanf(mac,
            "%2x:%2x:%2x:%2x:%2x:%2x",
            macTmp,macTmp+1,macTmp+2,macTmp+3,macTmp+4,macTmp+5);
    
    ZC_HexToString(mac, macTmp, ZC_SERVER_MAC_LEN / 2);
    memcpy(pu8Mac, mac, ZC_SERVER_MAC_LEN);
#endif
    u8 macTmp[6] = {0};
    u8 mac[12] = {0};
    get_local_mac(macTmp, 6);
    ZC_HexToString(mac, macTmp, ZC_SERVER_MAC_LEN / 2);
    memcpy(pu8Mac, mac, ZC_SERVER_MAC_LEN);
}

/*************************************************
* Function: HF_Reboot
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_Reboot(void)
{
    api_wdog_reboot();
}
/*************************************************
* Function: HF_ConnectToCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
extern void Test_Connect(void);
extern void Test_Connect_GateWay(uip_ipaddr_t *ripaddr, unsigned short rport);

u32 HF_ConnectToCloud(PTC_Connection *pstruConnection)
{
    //struct uip_conn *conn=NULL;
    uip_ipaddr_t ip;
    u16  u16Port;
    u16_t  *ipaddr = NULL;
    ipaddr = &ip;

    if (1 == g_struZcConfigDb.struSwitchInfo.u32ServerAddrConfig)
    {
        ZC_Printf("Connect ip=0x%x,port =%d \n",g_struZcConfigDb.struSwitchInfo.u32ServerIp,g_struZcConfigDb.struSwitchInfo.u16ServerPort); 
        u16Port = g_struZcConfigDb.struSwitchInfo.u16ServerPort;
        u8 *ipaddr =(u8 *) &(g_struZcConfigDb.struSwitchInfo.u32ServerIp);
        uip_ipaddr(&ip,ipaddr[3],ipaddr[2],ipaddr[1],ipaddr[0]);
        Test_Connect_GateWay(&ip, u16Port);
    }
    else
    { 
       //u16Port = pstruConnection->u16Port;
       Test_Connect();
#if 0
       ipaddr =  resolv_lookup(g_struZcConfigDb.struCloudInfo.u8CloudAddr);
       if(NULL == ipaddr)
       {
           return ZC_RET_ERROR;
       } 
#endif
       ZC_Printf("Connect %s\n",g_struZcConfigDb.struCloudInfo.u8CloudAddr);
       
    }
#if 0
    if (ZC_CONNECT_TYPE_TCP == pstruConnection->u8ConnectionType)
    {
        conn = uip_connect((uip_ipaddr_t *)ipaddr, ZC_HTONS(u16Port));
        if (NULL == conn) 
        {
            return ZC_RET_ERROR;
        }

        if (conn) {
            conn->lport = (u16)rand();
        }
        
        //g_struProtocolController.struCloudConnection.u32ConnectionTimes = 0;
        
        pstruConnection->u32Socket = conn->fd;
        g_u16LocalPort = ZC_HTONS(conn->lport);

        ZC_Printf("Connection Sokcet = %d, conn->lport = %d\n",conn->fd, g_u16LocalPort);
        /* add for g_struProtocolController.RandMsg*/
        ZC_Rand(g_struProtocolController.RandMsg);
    }
    else
    {

    }
#endif
    return ZC_RET_OK;
}

/*************************************************
* Function: HF_ListenClient
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 HF_ListenClient(PTC_Connection *pstruConnection)
{
#if 0
    int fd; 
    struct sockaddr_in servaddr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd<0)
        return ZC_RET_ERROR;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port = htons(pstruConnection->u16Port);
    if(bind(fd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
    {
        close(fd);
        return ZC_RET_ERROR;
    }
    
    if (listen(fd, TCP_DEFAULT_LISTEN_BACKLOG)< 0)
    {
        close(fd);
        return ZC_RET_ERROR;
    }

    ZC_Printf("Tcp Listen Port = %d\n", pstruConnection->u16Port);
    g_struProtocolController.struClientConnection.u32Socket = fd;
#endif
    return ZC_RET_OK;
}

/*************************************************
* Function: HF_Printf
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_Printf(const char *pu8format, ...)
{
    char buffer[100 + 1]={0};
    va_list arg;
    va_start (arg, pu8format);
    vsnprintf(buffer, 100, pu8format, arg);
    va_end (arg);
    printf(buffer);
}
/*************************************************
* Function: HF_BcInit
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_BcInit(void)
{
#if 0
    int tmp=1;
    struct sockaddr_in addr; 

    addr.sin_family = AF_INET; 
    addr.sin_port = htons(ZC_MOUDLE_PORT); 
    addr.sin_addr.s_addr=htonl(INADDR_ANY);

    g_Bcfd = socket(AF_INET, SOCK_DGRAM, 0); 

    tmp=1; 
    setsockopt(g_Bcfd, SOL_SOCKET,SO_BROADCAST,&tmp,sizeof(tmp)); 

    //hfnet_set_udp_broadcast_port_valid(ZC_MOUDLE_PORT, ZC_MOUDLE_PORT + 1);

    bind(g_Bcfd, (struct sockaddr*)&addr, sizeof(addr)); 
    g_struProtocolController.u16SendBcNum = 0;

    memset((char*)&struRemoteAddr,0,sizeof(struRemoteAddr));
    struRemoteAddr.sin_family = AF_INET; 
    struRemoteAddr.sin_port = htons(ZC_MOUDLE_BROADCAST_PORT); 
    struRemoteAddr.sin_addr.s_addr=inet_addr("255.255.255.255"); 
    g_pu8RemoteAddr = (u8*)&struRemoteAddr;
    //g_u32BcSleepCount = 2.5 * 250000;
    g_u32BcSleepCount = 10;
#endif
    return;
}

/*************************************************
* Function: HF_Cloudfunc
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
static void HF_Cloudfunc(void* arg) 
{
#if 0
    int fd;
    u32 u32Timer = 0;

    HF_BcInit();

    while(1) 
    {
        fd = g_struProtocolController.struCloudConnection.u32Socket;
        PCT_Run();
        
        if (PCT_STATE_DISCONNECT_CLOUD == g_struProtocolController.u8MainState)
        {
            close(fd);
            if (0 == g_struProtocolController.struCloudConnection.u32ConnectionTimes)
            {
                u32Timer = 1000;
            }
            else
            {
                u32Timer = rand();
                u32Timer = (PCT_TIMER_INTERVAL_RECONNECT) * (u32Timer % 10 + 1);
            }
            PCT_ReconnectCloud(&g_struProtocolController, u32Timer);
            g_struUartBuffer.u32Status = MSG_BUFFER_IDLE;
            g_struUartBuffer.u32RecvLen = 0;
        }
        else
        {
            MSG_SendDataToCloud((u8*)&g_struProtocolController.struCloudConnection);
        }
        ZC_SendBc();
        sys_msleep(100);
    } 
#endif
}

/*************************************************
* Function: HF_Init
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_Init(void)
{
    u8 u8Mac[16];
    //网络通信接口
    g_struHfAdapter.pfunConnectToCloud = HF_ConnectToCloud;

    g_struHfAdapter.pfunListenClient = HF_ListenClient;
    g_struHfAdapter.pfunSendTcpData = HF_SendTcpData; 
    g_struHfAdapter.pfunSendUdpData = HF_SendUdpData; 
    g_struHfAdapter.pfunUpdate = HF_FirmwareUpdate;  
    //设备内部通信接口
    g_struHfAdapter.pfunSendToMoudle = HF_SendDataToMoudle; 
    //定时器类接口
    g_struHfAdapter.pfunStopTimer = HF_StopTimer;  
    g_struHfAdapter.pfunSetTimer = HF_SetTimer;  

    //存储类接口
    g_struHfAdapter.pfunUpdateFinish = HF_FirmwareUpdateFinish;
    g_struHfAdapter.pfunWriteFlash = HF_WriteDataToFlash;
    g_struHfAdapter.pfunReadFlash = HF_ReadDataFromFlash;
    //系统类接口    
    g_struHfAdapter.pfunRest = HF_Rest;    
    g_struHfAdapter.pfunGetMac = HF_GetMac;
    g_struHfAdapter.pfunReboot = HF_Reboot;
    g_struHfAdapter.pfunMalloc = malloc;
    g_struHfAdapter.pfunFree = free;
    g_struHfAdapter.pfunPrintf = HF_Printf;
    g_u16TcpMss = 1000;
    //g_u32SmartConfigFlag = 0;

    PCT_Init(&g_struHfAdapter);
    // Initial a periodical timer
    //gtimer_init(&g_struTimer1, TIMER0);
    //gtimer_start_periodical(&g_struTimer1, 1000000, (void*)Timer_callback, 0);
    
    printf("QD Init\n");

    memcpy(g_struRegisterInfo.u8PrivateKey, g_u8ModuleKey, ZC_MODULE_KEY_LEN);
    memcpy(g_struRegisterInfo.u8DeviciId, u8Mac, ZC_HS_DEVICE_ID_LEN);

    g_struRegisterInfo.u8DeviciId[23] = SUB_DOMAIN_ID & 0xff;
    g_struRegisterInfo.u8DeviciId[22] = (SUB_DOMAIN_ID & 0xff00) >> 8;
    g_struRegisterInfo.u8DeviciId[21] = MAJOR_DOMAIN_ID & 0xff;
    g_struRegisterInfo.u8DeviciId[20] = (MAJOR_DOMAIN_ID & 0xff00) >> 8;
    g_struRegisterInfo.u8DeviciId[19] = (MAJOR_DOMAIN_ID & 0xff0000) >> 16;
    g_struRegisterInfo.u8DeviciId[18] = (MAJOR_DOMAIN_ID & 0xff000000) >> 24;
    g_struRegisterInfo.u8DeviciId[17] = (MAJOR_DOMAIN_ID & 0xff00000000) >> 32;
    g_struRegisterInfo.u8DeviciId[16] = (MAJOR_DOMAIN_ID & 0xff0000000000) >> 40;

    //uart_socket();

    g_struUartBuffer.u32Status = MSG_BUFFER_IDLE;
    g_struUartBuffer.u32RecvLen = 0;
#if 0
	if(xTaskCreate(HF_Cloudfunc, ((const char*)"HF_Cloudfunc"), 512, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {   
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
    }
	if(xTaskCreate(HF_CloudRecvfunc, ((const char*)"HF_CloudRecvfunc"), 512, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {   
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
    }     
#endif
}

/*************************************************
* Function: HF_WakeUp
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_WakeUp(void)
{
    ZC_Printf("HF_WakeUp\n\r");
    //g_u32SmartConfigFlag = 0;
    PCT_WakeUp();
}
/*************************************************
* Function: HF_Sleep
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_Sleep()
{
#if 0
    u32 u32Index;
    ZC_Printf("HF_Sleep\r\n");
    close(g_Bcfd);

    if (PCT_INVAILD_SOCKET != g_struProtocolController.struClientConnection.u32Socket)
    {
        close(g_struProtocolController.struClientConnection.u32Socket);
        g_struProtocolController.struClientConnection.u32Socket = PCT_INVAILD_SOCKET;
    }

    if (PCT_INVAILD_SOCKET != g_struProtocolController.struCloudConnection.u32Socket)
    {
        close(g_struProtocolController.struCloudConnection.u32Socket);
        g_struProtocolController.struCloudConnection.u32Socket = PCT_INVAILD_SOCKET;
    }
    
    for (u32Index = 0; u32Index < ZC_MAX_CLIENT_NUM; u32Index++)
    {
        if (0 == g_struClientInfo.u32ClientVaildFlag[u32Index])
        {
            close(g_struClientInfo.u32ClientFd[u32Index]);
            g_struClientInfo.u32ClientFd[u32Index] = PCT_INVAILD_SOCKET;
        }
    }
#endif
    PCT_Sleep();
    
    g_struUartBuffer.u32Status = MSG_BUFFER_IDLE;
    g_struUartBuffer.u32RecvLen = 0;
}

void HF_Run(void)
{
#if 1
    if(PCT_STATE_WAIT_ACCESS != g_struProtocolController.u8MainState)
    {
        PCT_Run();
    }
#else
    if(PCT_STATE_ACCESS_NET == g_struProtocolController.u8MainState)
    {
        PCT_Run();
    }
#endif
}


/*************************************************
* Function: AC_UartSend
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_UartSend(u8* inBuf, u32 datalen)
{
    //uart_write(g_uart_socket, inBuf, datalen);
}
/******************************* FILE END ***********************************/


