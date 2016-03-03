/**
******************************************************************************
* @file     zc_bc.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    broadcast
******************************************************************************
*/
#include <zc_bc.h>
#include <zc_protocol_controller.h>
int g_Bcfd = 0;
u8* g_pu8RemoteAddr = NULL;
unsigned int g_u32GloablIp;
extern u32 g_u32BcSleepCount;
/*************************************************
* Function: ZC_SendBc
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_SendBc()
{
    u16 u16Len;
    static int sleepcount = 0;
    ZC_SendParam struParam;

    if (PCT_STATE_CONNECT_CLOUD != g_struProtocolController.u8MainState)
    {
        sleepcount = 0;
        return;
    }
    if (ZC_MAGIC_FLAG == g_struZcConfigDb.struDeviceInfo.u32UnBcFlag)
    {
        sleepcount = 0;
        return;
    }        
    sleepcount++;
    if (sleepcount > g_u32BcSleepCount)
    {
        
        EVENT_BuildBcMsg(g_u8MsgBuildBuffer, &u16Len);

        if (g_struProtocolController.u16SendBcNum < (PCT_SEND_BC_MAX_NUM))
        {
            struParam.u8NeedPoll = 0;
            struParam.pu8AddrPara = g_pu8RemoteAddr;
            g_struProtocolController.pstruMoudleFun->pfunSendUdpData(g_Bcfd, g_u8MsgBuildBuffer, u16Len, &struParam);  
            g_struProtocolController.u16SendBcNum++;
        }
        sleepcount = 0;
    }
    
}
/*************************************************
* Function: ZC_SendClientQueryReq
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_SendClientQueryReq(u8 *pu8Msg, u16 u16RecvLen)
{
    ZC_MessageHead *pstruMsg;
    ZC_ClientQueryRsp struRsp;
    u16 u16Len;
    u8 *pu8DeviceId;
    u8 *pu8Domain;    
    u32 u32Index;
    ZC_ClientQueryReq *pstruQuery;
    ZC_SendParam struParam;

    if (g_struProtocolController.u8MainState < PCT_STATE_ACCESS_NET)
    {
        return;
    }
    
    if (u16RecvLen != sizeof(ZC_MessageHead) + sizeof(ZC_ClientQueryReq))
    {
        return;
    }
    
    pstruMsg = (ZC_MessageHead *)pu8Msg;
    pstruQuery = (ZC_ClientQueryReq *)(pstruMsg + 1);

    if (ZC_CODE_CLIENT_QUERY_REQ != pstruMsg->MsgCode)
    {
        return;
    }
    ZC_GetStoreInfor(ZC_GET_TYPE_DEVICEID, &pu8DeviceId);
    pu8Domain = pu8DeviceId + ZC_HS_DEVICE_ID_LEN;

    /*Only first 6 bytes is vaild*/
    for (u32Index = 0; u32Index < 6; u32Index++)
    {
        if (pstruQuery->u8Domain[u32Index] != pu8Domain[u32Index])
        {
            return;
        }
        
    }
    
    struRsp.addr[3] = g_u32GloablIp & 0xff;
    struRsp.addr[2] = (g_u32GloablIp >> 8) & 0xff;
    struRsp.addr[1] = (g_u32GloablIp >> 16) & 0xff;
    struRsp.addr[0] = (g_u32GloablIp >> 24)  & 0xff;
    memcpy(struRsp.u8Domain, pu8Domain, ZC_DOMAIN_LEN);
    memcpy(struRsp.DeviceId, pu8DeviceId, ZC_HS_DEVICE_ID_LEN);
    EVENT_BuildMsg(ZC_CODE_CLIENT_QUERY_RSP, 0, g_u8MsgBuildBuffer, &u16Len, (u8*)&struRsp, sizeof(ZC_ClientQueryRsp));
    struParam.u8NeedPoll = 0;
    struParam.pu8AddrPara = g_pu8RemoteAddr;
    g_struProtocolController.pstruMoudleFun->pfunSendUdpData(g_Bcfd, g_u8MsgBuildBuffer, u16Len, &struParam);  
}
/******************************* FILE END ***********************************/


