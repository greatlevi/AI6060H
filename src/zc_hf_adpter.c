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
#include "uiplib.h"
#include "atcmd_icomm.h"

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

u8  g_u8BcSendBuffer[100];
u32 g_u32BcSleepCount = 800;

extern u8  g_u8ModuleKey[ZC_MODULE_KEY_LEN];

uip_ipaddr_t g_remote_ip;

extern void Ac_Dns(char *buf);
extern void Ac_ConnectGateway(uip_ipaddr_t *ripaddr, unsigned short rport);
extern void Ac_ListenClient(PTC_Connection *pstruConnection);

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
        *pu8TimeIndex = u8TimerIndex;
    }
    else
    {
        ZC_Printf("HF_SetTimer: no idle timer\n");
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
    return ZC_RET_OK;
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
    At_Disconnect();
    AT_RemoveCfsConf();
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
    if(udpsendto(u32Fd, pu8Data, u16DataLen, &g_remote_ip, ZC_MOUDLE_BROADCAST_PORT) == -1)
    {
        printf("udpsendto fail\n");
    }
    else
    {
        //printf("Send udp data\n");
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
u32 HF_ConnectToCloud(PTC_Connection *pstruConnection)
{
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
        Ac_ConnectGateway(&ip, u16Port);
    }
    else
    { 
       Ac_Dns(g_struZcConfigDb.struCloudInfo.u8CloudAddr);
       ZC_Printf("Connect %s\n",g_struZcConfigDb.struCloudInfo.u8CloudAddr);
       
    }
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
    Ac_ListenClient(pstruConnection);
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
* Function: HF_Cloudfunc
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_Cloudfunc(void) 
{

    int fd;
    u32 u32Timer = 0;

    fd = g_struProtocolController.struCloudConnection.u32Socket;
    
    if (PCT_STATE_DISCONNECT_CLOUD == g_struProtocolController.u8MainState)
    {
        tcpclose(fd);
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

    g_u32BcSleepCount = 10;

    PCT_Init(&g_struHfAdapter);
    
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

    g_struUartBuffer.u32Status = MSG_BUFFER_IDLE;
    g_struUartBuffer.u32RecvLen = 0;

    if( uiplib_ipaddrconv("255.255.255.255", &g_remote_ip) == 0)
    {
        printf("erro ip format\n");
    }
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
    PCT_WakeUp();
    ZC_StartClientListen();
}
/*************************************************
* Function: HF_Sleep
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_Sleep(void)
{
    u32 u32Index;

    ZC_Printf("HF_Sleep\n");

    if (PCT_INVAILD_SOCKET != g_struProtocolController.struClientConnection.u32Socket)
    {
        tcpclose(g_struProtocolController.struClientConnection.u32Socket);
        g_struProtocolController.struClientConnection.u32Socket = PCT_INVAILD_SOCKET;
    }

    if (PCT_INVAILD_SOCKET != g_struProtocolController.struCloudConnection.u32Socket)
    {
        tcpclose(g_struProtocolController.struCloudConnection.u32Socket);
        g_struProtocolController.struCloudConnection.u32Socket = PCT_INVAILD_SOCKET;
    }    

    for (u32Index = 0; u32Index < ZC_MAX_CLIENT_NUM; u32Index++)
    {
        if (0 == g_struClientInfo.u32ClientVaildFlag[u32Index])
        {
            tcpclose(g_struClientInfo.u32ClientFd[u32Index]);
            g_struClientInfo.u32ClientFd[u32Index] = PCT_INVAILD_SOCKET;
        }
    }

    PCT_Sleep();
    
    g_struUartBuffer.u32Status = MSG_BUFFER_IDLE;
    g_struUartBuffer.u32RecvLen = 0;
}
/*************************************************
* Function: HF_Run
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void HF_Run(void)
{
    if(PCT_STATE_WAIT_ACCESS != g_struProtocolController.u8MainState)
    {
        PCT_Run();
    }
}
/*************************************************
* Function: ac_rand
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 ac_rand(void)
{    
    u32 u32i;
    u32 sum = 0;
    for (u32i = 0; u32i < 20; u32i++)
    {
        sum += clock_time();
    }

    return sum;
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


