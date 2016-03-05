/**
******************************************************************************
* @file     zc_protocol_controller.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    protocol controller
******************************************************************************
*/
#include <zc_protocol_controller.h>
#include <zc_sec_engine.h>
#include <zc_module_config.h>
#include <zc_cloud_event.h>
#include <zc_message_queue.h>
#include <zc_protocol_interface.h>
#include <zc_timer.h>


PTC_ProtocolCon  g_struProtocolController;
extern ZC_Timer g_struTimer[ZC_TIMER_MAX_NUM];
extern u8  g_u8ExAesKey[ZC_HS_SESSION_KEY_LEN];
/*************************************************
* Function: PCT_CheckCrc
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 PCT_CheckCrc(u8 *pu8Crc, u8 *pu8Data, u16 u16Len)
{
    u16 u16Crc;
    u16 u16RecvCrc;
    u16Crc = crc16_ccitt(pu8Data, u16Len);
    u16RecvCrc = (pu8Crc[0] << 8) + pu8Crc[1];
    if (u16Crc == u16RecvCrc)
    {
        return ZC_RET_OK;
    }
    else
    {
        ZC_Printf("Crc is error, recv = %d, calc = %d\n", u16RecvCrc, u16Crc);    
        
        return ZC_RET_ERROR;    
    }
}

/*************************************************
* Function: PCT_SendNotifyMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendNotifyMsg(u8 u8NotifyCode)
{
    u16 u16Len;
    EVENT_BuildMsg(u8NotifyCode, 0, g_u8MsgBuildBuffer, &u16Len, NULL, 0);
    g_struProtocolController.pstruMoudleFun->pfunSendToMoudle(g_u8MsgBuildBuffer, u16Len);
}

/*************************************************
* Function: PCT_SendHeartMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendHeartMsg()
{
    ZC_MessageHead struHeart;
    ZC_SecHead struSecHead;
    u16 u16Len = 0;
    
    EVENT_BuildHeartMsg((u8*)&struHeart, &u16Len);

    /*build sec head*/
    struSecHead.u8SecType = ZC_SEC_ALG_AES;
    struSecHead.u16TotalMsg = ZC_HTONS(u16Len);

    (void)PCT_SendMsgToCloud(&struSecHead, (u8*)&struHeart);

    g_struProtocolController.pstruMoudleFun->pfunSetTimer(PCT_TIMER_SENDHEART, 
        PCT_TIMER_INTERVAL_HEART, &g_struProtocolController.u8HeartTimer);
    
}

/*************************************************
* Function: PCT_Init
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_Init(PTC_ModuleAdapter *pstruAdapter)
{
    g_struProtocolController.pstruMoudleFun = pstruAdapter;

    g_struProtocolController.struCloudConnection.u32Socket = PCT_INVAILD_SOCKET;

    /*config connection type*/
    g_struProtocolController.struCloudConnection.u16Port = ZC_CLOUD_PORT;
    g_struProtocolController.struCloudConnection.u8IpType = ZC_IPTYPE_IPV4;
    g_struProtocolController.struCloudConnection.u8ConnectionType = ZC_CONNECT_TYPE_TCP;

    g_struProtocolController.struClientConnection.u32Socket = PCT_INVAILD_SOCKET;

    /*config connection type*/
    g_struProtocolController.struClientConnection.u16Port = ZC_SERVER_PORT;
    g_struProtocolController.struClientConnection.u8IpType = ZC_IPTYPE_IPV4;
    g_struProtocolController.struClientConnection.u8ConnectionType = ZC_CONNECT_TYPE_TCP;

    memcpy(g_struProtocolController.IvRecv, g_u8ExAesKey, ZC_HS_SESSION_KEY_LEN);
    memcpy(g_struProtocolController.IvSend, g_u8ExAesKey, ZC_HS_SESSION_KEY_LEN);
    memcpy(g_struProtocolController.u8SessionKey, g_u8ExAesKey, ZC_HS_SESSION_KEY_LEN);

    ZC_ConfigInitDefault();
    MSG_Init();
    TIMER_Init();
    g_struProtocolController.u8SendMoudleMsgId = 0;
    g_struProtocolController.u8keyRecv = PCT_KEY_UNRECVED;
    g_struProtocolController.u8ReconnectTimer = PCT_TIMER_INVAILD;
    g_struProtocolController.u8SendMoudleTimer = PCT_TIMER_INVAILD;
    g_struProtocolController.u8HeartTimer = PCT_TIMER_INVAILD;
    g_struProtocolController.u8RegisterTimer = PCT_TIMER_INVAILD;
    g_struProtocolController.u8RebootTimer = PCT_TIMER_INVAILD;

    g_struProtocolController.u8MainState = PCT_STATE_INIT;

    ZC_ClientInit();
    ZC_ConfigInitPara();
}
/*************************************************
* Function: PCT_SendEmptyMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendEmptyMsg(u8 u8MsgId, u8 u8SecType)
{
    ZC_MessageHead struMsg;
    ZC_SecHead struSecHead;
    u16 u16Len = 0;
    /*build msg*/
    EVENT_BuildEmptyMsg(u8MsgId, (u8*)&struMsg, &u16Len);
    
    /*build sec head*/
    struSecHead.u8SecType = u8SecType;
    struSecHead.u16TotalMsg = ZC_HTONS(u16Len);

    
    (void)PCT_SendMsgToCloud(&struSecHead, (u8*)&struMsg);
}
/*************************************************
* Function: PCT_SendErrorMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendErrorMsg(u8 u8MsgId, u8 *pu8Error, u16 u16ErrorLen)
{
    u16 u16Len;
    ZC_SecHead struSecHead;

    EVENT_BuildMsg(ZC_CODE_ERR, u8MsgId, g_u8MsgBuildBuffer, &u16Len, 
        pu8Error, u16ErrorLen);
    
    /*build sec head*/
    struSecHead.u8SecType = ZC_SEC_ALG_AES;
    struSecHead.u16TotalMsg = ZC_HTONS(u16Len);
    
    (void)PCT_SendMsgToCloud(&struSecHead, (u8*)g_u8MsgBuildBuffer);
}

/*************************************************
* Function: PCT_SendCloudAccessMsg1
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendCloudAccessMsg1(PTC_ProtocolCon *pstruContoller)
{
    u16 u16Len;
    ZC_SecHead struSechead;
    ZC_HandShakeMsg1 struMsg1;
    u8 *pu8DeviceId;
    u32 u32RetVal;
    /*stop reconnection timer*/
    if (PCT_TIMER_INVAILD != pstruContoller->u8ReconnectTimer)
    {
        ZC_Printf("msg1 stop u8ReconnectTimer\n");
        TIMER_StopTimer(pstruContoller->u8ReconnectTimer);
        pstruContoller->u8ReconnectTimer = PCT_TIMER_INVAILD;
    }
    ZC_GetStoreInfor(ZC_GET_TYPE_DEVICEID, &pu8DeviceId);
    memcpy(struMsg1.RandMsg, pstruContoller->RandMsg, ZC_HS_MSG_LEN);
    memcpy(struMsg1.DeviceId, pu8DeviceId, ZC_HS_DEVICE_ID_LEN);
    memcpy(struMsg1.u8Domain, pu8DeviceId + ZC_HS_DEVICE_ID_LEN, ZC_DOMAIN_LEN);
    EVENT_BuildMsg(ZC_CODE_HANDSHAKE_1, 1, g_u8MsgBuildBuffer, &u16Len, 
        (u8*)&struMsg1, sizeof(ZC_HandShakeMsg1));
    struSechead.u8SecType = ZC_SEC_ALG_NONE;//ZC_SEC_ALG_RSA;
    struSechead.u16TotalMsg = ZC_HTONS(u16Len);
    struSechead.u8Resver = 0x5A;
    u32RetVal = PCT_SendMsgToCloud(&struSechead, g_u8MsgBuildBuffer);
    if (ZC_RET_ERROR == u32RetVal)
    {
        ZC_Printf("Send Msg1 fail disconnect\n");
        PCT_DisConnectCloud(pstruContoller);
        return;
    }

    ZC_Printf("Send Msg1 \n");

    pstruContoller->u8MainState = PCT_STATE_WAIT_ACCESSRSP;

    
    pstruContoller->pstruMoudleFun->pfunSetTimer(PCT_TIMER_REACCESS, 
        PCT_TIMER_INTERVAL_RECONNECT * 120, &pstruContoller->u8AccessTimer);

}

/*************************************************
* Function: PCT_SendCloudAccessMsg3
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendCloudAccessMsg3(PTC_ProtocolCon *pstruContoller)
{
    u16 u16Len;
    ZC_HandShakeMsg3 struMsg3;
    ZC_SecHead struSechead;
    u8 *pu8Vesion;

    ZC_GetStoreInfor(ZC_GET_TYPE_VESION, &pu8Vesion);
    
    memcpy(struMsg3.RandMsg, pstruContoller->RandMsg, ZC_HS_MSG_LEN);
    memcpy(struMsg3.u8EqVersion, pu8Vesion, ZC_EQVERSION_LEN);
    struMsg3.u8WifiVerSion[0] = (u8)(ZC_MODULE_VERSION >> 24);
    struMsg3.u8WifiVerSion[1] = (u8)(ZC_MODULE_VERSION >> 16);
    struMsg3.u8WifiVerSion[2] = (u8)(ZC_MODULE_VERSION >> 8);
    struMsg3.u8WifiVerSion[3] = (u8)(ZC_MODULE_VERSION);

    struMsg3.u8WifiType = ZC_MODULE_TYPE;
    /*first set key recv flag*/
    g_struProtocolController.u8keyRecv = PCT_KEY_RECVED;

    EVENT_BuildMsg(ZC_CODE_HANDSHAKE_3, 1, g_u8MsgBuildBuffer, &u16Len, 
        (u8*)&struMsg3, sizeof(ZC_HandShakeMsg3));

    struSechead.u8SecType = ZC_SEC_ALG_AES;
    struSechead.u16TotalMsg = ZC_HTONS(u16Len);

    (void)PCT_SendMsgToCloud(&struSechead, g_u8MsgBuildBuffer);
    
    pstruContoller->u8MainState = PCT_STATE_WAIT_MSG4;
    pstruContoller->pstruMoudleFun->pfunSetTimer(PCT_TIMER_REACCESS, 
        PCT_TIMER_INTERVAL_RECONNECT*10, &pstruContoller->u8AccessTimer);

    return;
}
/*************************************************
* Function: PCT_DisConnectCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_DisConnectCloud(PTC_ProtocolCon *pstruContoller)
{
    pstruContoller->u8MainState = PCT_STATE_DISCONNECT_CLOUD;
    pstruContoller->u8keyRecv = PCT_KEY_UNRECVED;
    memcpy(pstruContoller->IvRecv, g_u8ExAesKey, ZC_HS_SESSION_KEY_LEN);
    memcpy(pstruContoller->IvSend, g_u8ExAesKey, ZC_HS_SESSION_KEY_LEN);
    memcpy(pstruContoller->u8SessionKey, g_u8ExAesKey, ZC_HS_SESSION_KEY_LEN);
    MSG_Init();
    PCT_SendNotifyMsg(ZC_CODE_CLOUD_DISCONNECTED);
}

/*************************************************
* Function: PCT_ConnectCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_ConnectCloud(PTC_ProtocolCon *pstruContoller)
{
    u32 u32Ret = ZC_RET_OK;
    
    if (PCT_TIMER_INVAILD != pstruContoller->u8ReconnectTimer)
    {
        return;
    }
    
    /*Connect*/
    u32Ret = pstruContoller->pstruMoudleFun->pfunConnectToCloud(&pstruContoller->struCloudConnection);
    if (ZC_RET_OK != u32Ret)
    {
        return;
    }
    /*change state to wait access*/
    pstruContoller->u8MainState = PCT_STATE_WAIT_ACCESS;
    pstruContoller->u8keyRecv = PCT_KEY_UNRECVED;
}

/*************************************************
* Function: PCT_ReconnectCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_ReconnectCloud(PTC_ProtocolCon *pstruContoller, u32 u32ReConnectTimer)
{
    u32 u32Index;
    if (PCT_TIMER_INVAILD != pstruContoller->u8ReconnectTimer)
    {
        ZC_Printf("already reconnected \n");
        return;
    }
    

    MSG_Init();
    g_struProtocolController.u8keyRecv = PCT_KEY_UNRECVED;

    for (u32Index = 0; u32Index < ZC_TIMER_MAX_NUM; u32Index++)
    {
        
        if (g_struTimer[u32Index].u8Status == ZC_TIMER_STATUS_USED)
        {
            TIMER_StopTimer((u8)u32Index);
        }
    }
    
    TIMER_Init();
    g_struProtocolController.u8ReconnectTimer = PCT_TIMER_INVAILD;
    g_struProtocolController.u8SendMoudleTimer = PCT_TIMER_INVAILD;
    g_struProtocolController.u8HeartTimer = PCT_TIMER_INVAILD;
    g_struProtocolController.u8MainState = PCT_STATE_INIT;

    memcpy(g_struProtocolController.IvRecv, g_u8ExAesKey, ZC_HS_SESSION_KEY_LEN);
    memcpy(g_struProtocolController.IvSend, g_u8ExAesKey, ZC_HS_SESSION_KEY_LEN);
    memcpy(g_struProtocolController.u8SessionKey, g_u8ExAesKey, ZC_HS_SESSION_KEY_LEN);

    pstruContoller->pstruMoudleFun->pfunSetTimer(PCT_TIMER_RECONNECT, 
        u32ReConnectTimer, &pstruContoller->u8ReconnectTimer);
    pstruContoller->struCloudConnection.u32Socket = PCT_INVAILD_SOCKET;
    pstruContoller->u8keyRecv = PCT_KEY_UNRECVED; 
    pstruContoller->u8MainState = PCT_STATE_INIT;
}

/*************************************************
* Function: PCT_SendMoudleTimeout
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendMoudleTimeout(PTC_ProtocolCon *pstruProtocolController)
{      
    ZC_Printf("send moudle timeout, MsgId = %d\n",pstruProtocolController->u8SendMoudleMsgId);
    
    PCT_SendEmptyMsg(pstruProtocolController->u8SendMoudleMsgId, ZC_SEC_ALG_AES);
    PCT_SendErrorMsg(pstruProtocolController->u8SendMoudleMsgId, NULL, 0);
    pstruProtocolController->u8SendMoudleTimer = PCT_TIMER_INVAILD;  
}
/*************************************************
* Function: PCT_HandleMoudleEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleMoudleEvent(u8 *pu8Msg, u16 u16DataLen)
{
    ZC_SecHead struHead;
    ZC_MessageHead *pstruMsg = (ZC_MessageHead *)pu8Msg;

    if ((PCT_TIMER_INVAILD != g_struProtocolController.u8SendMoudleTimer) &&
        (pstruMsg->MsgCode < ZC_CODE_REPORT_BASE))
    {
        TIMER_StopTimer(g_struProtocolController.u8SendMoudleTimer);
        g_struProtocolController.u8SendMoudleTimer = PCT_TIMER_INVAILD;
        PCT_SendEmptyMsg(g_struProtocolController.u8SendMoudleMsgId, ZC_SEC_ALG_AES);
    }

    struHead.u8SecType = ZC_SEC_ALG_AES;
    struHead.u16TotalMsg = ZC_HTONS(u16DataLen);
    (void)PCT_SendMsgToCloud(&struHead, pu8Msg);
    
    return;
}
/*************************************************
* Function: PCT_RecvAccessMsg2
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_RecvAccessMsg2(PTC_ProtocolCon *pstruContoller)
{
    MSG_Buffer *pstruBuffer;
    ZC_MessageHead *pstruMsg;
    ZC_HandShakeMsg2 *pstruMsg2;
    ZC_AccessPoint *pstruAccessPoint;
    u32 u32Addr;
    u16 u16Port;
    pstruBuffer = (MSG_Buffer *)MSG_PopMsg(&g_struRecvQueue);
    if (NULL == pstruBuffer)
    {
        return;
    }

    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;
    pstruMsg2 = (ZC_HandShakeMsg2*)(pstruMsg + 1);

    if ((ZC_CODE_HANDSHAKE_2 == pstruMsg->MsgCode)||(ZC_CODE_ACCESS_POINT_RSP == pstruMsg->MsgCode))
    {
        if (ZC_RET_ERROR == PCT_CheckCrc(pstruMsg->TotalMsgCrc, (u8*)pstruMsg2, ZC_HTONS(pstruMsg->Payloadlen)))
        {
            PCT_DisConnectCloud(pstruContoller);
        }
        else
        {
            TIMER_StopTimer(pstruContoller->u8AccessTimer);
            if (0 == memcmp(pstruMsg2->RandMsg, pstruContoller->RandMsg, ZC_HS_MSG_LEN))
            {
                if (ZC_CODE_HANDSHAKE_2 == pstruMsg->MsgCode)
                {
                    memcpy(pstruContoller->u8SessionKey, pstruMsg2->SessionKey, ZC_HS_SESSION_KEY_LEN);
                    memcpy(pstruContoller->IvRecv, pstruMsg2->SessionKey, ZC_HS_SESSION_KEY_LEN);
                    memcpy(pstruContoller->IvSend, pstruMsg2->SessionKey, ZC_HS_SESSION_KEY_LEN);

                    PCT_SendCloudAccessMsg3(pstruContoller);
                
                    ZC_Printf("Recv Msg2 ok\n");
                }
                else
                {
                    pstruAccessPoint=(ZC_AccessPoint*)(pstruMsg + 1);
                    u32Addr = ZC_HTONL(pstruAccessPoint->u32ServerAddr);
                    u16Port = ZC_HTONS(pstruAccessPoint->u16ServerPort);

                    ZC_StoreAccessInfo((u8 *)&u32Addr,(u8 *)&u16Port);
                    PCT_DisConnectCloud(pstruContoller);
                    ZC_Printf("Recv Access Point ok,Cloud Addr=%x,port=%x!\n",u32Addr,u16Port);
                }
            }
            else
            {
                PCT_DisConnectCloud(pstruContoller);
                ZC_Printf("Recv Msg2 rand error \n");            
            }
        }
    }
    
    pstruBuffer->u32Len = 0;
    pstruBuffer->u8Status = MSG_BUFFER_IDLE;
}

/*************************************************
* Function: PCT_RecvAccessMsg4
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_RecvAccessMsg4(PTC_ProtocolCon *pstruContoller)
{
    MSG_Buffer *pstruBuffer;
    ZC_MessageHead *pstruMsg;
    ZC_HandShakeMsg4 *pstruMsg4;
    pstruBuffer = (MSG_Buffer *)MSG_PopMsg(&g_struRecvQueue);
    if (NULL == pstruBuffer)
    {
        return;
    }
    
    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;
    pstruMsg4 = (ZC_HandShakeMsg4 *)(pstruMsg + 1);
    
    if (ZC_CODE_HANDSHAKE_4 == pstruMsg->MsgCode)
    {
        if (ZC_RET_ERROR == PCT_CheckCrc(pstruMsg->TotalMsgCrc, (u8*)pstruMsg4, ZC_HTONS(pstruMsg->Payloadlen)))
        {
            PCT_DisConnectCloud(pstruContoller);
        }
        else
        {   
            TIMER_StopTimer(pstruContoller->u8AccessTimer);
            if (0 == memcmp(pstruMsg4->RandMsg, pstruContoller->RandMsg, ZC_HS_MSG_LEN))
            {
                pstruContoller->u8MainState = PCT_STATE_CONNECT_CLOUD;
                
                ZC_Printf("recv msg4 ok\n");
                PCT_SendNotifyMsg(ZC_CODE_CLOUD_CONNECTED);
            }
            else
            {
                PCT_DisConnectCloud(pstruContoller);
                ZC_Printf("Recv msg4 rand error \n");            
            }
        }
    }
    pstruBuffer->u32Len = 0;
    pstruBuffer->u8Status = MSG_BUFFER_IDLE;

    if (PCT_TIMER_INVAILD != pstruContoller->u8HeartTimer)
    {
        TIMER_StopTimer(pstruContoller->u8HeartTimer);
    }
    pstruContoller->pstruMoudleFun->pfunSetTimer(PCT_TIMER_SENDHEART, 
        PCT_TIMER_INTERVAL_HEART, &pstruContoller->u8HeartTimer);
}

/*************************************************
* Function: PCT_HandleUnbindMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleUnbindMsg(PTC_ProtocolCon *pstruContoller)
{
    MSG_Buffer *pstruBuffer;
    ZC_MessageHead *pstruMsg;
    pstruBuffer = (MSG_Buffer *)MSG_PopMsg(&g_struRecvQueue);
    if (NULL == pstruBuffer)
    {
        return;
    }
    
    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;
    
    if (ZC_CODE_UNBIND == pstruMsg->MsgCode)
    {
        g_struProtocolController.u8MainState = PCT_STATE_CONNECT_CLOUD;
        if (PCT_TIMER_INVAILD != pstruContoller->u8SendUnbindTimer)
        {
           TIMER_StopTimer(pstruContoller->u8SendUnbindTimer);           
           pstruContoller->u8SendUnbindTimer = PCT_TIMER_INVAILD;
        }        
        ZC_ConfigUnBind(0xFFFFFFFF);
        ZC_Printf("recv unbind resp ok\n");
    }
    pstruBuffer->u32Len = 0;
    pstruBuffer->u8Status = MSG_BUFFER_IDLE;

    pstruContoller->pstruMoudleFun->pfunSetTimer(PCT_TIMER_SENDHEART, 
        PCT_TIMER_INTERVAL_HEART, &pstruContoller->u8HeartTimer);
}

/*************************************************
* Function: PCT_SendUnbindMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendUnbindMsg()
{
    ZC_MessageHead struUnbind;
    ZC_SecHead struSecHead;
    u16 u16Len = 0;
    u32 u32Timer = 0;
    EVENT_BuildMsg(ZC_CODE_UNBIND, 0, (u8*)&struUnbind, &u16Len, 
        NULL, 0);
    
    /*build sec head*/
    struSecHead.u8SecType = ZC_SEC_ALG_AES;
    struSecHead.u16TotalMsg = ZC_HTONS(u16Len);
    
    (void)PCT_SendMsgToCloud(&struSecHead, (u8*)&struUnbind);
     ZC_Printf("Send Unbind msg ok\n");

    u32Timer = rand();
    u32Timer = (PCT_TIMER_INTERVAL_SENDUBIND) * (u32Timer % 10 + 1);

    g_struProtocolController.pstruMoudleFun->pfunSetTimer(PCT_TIMER_SENDUBIND, 
        u32Timer, &g_struProtocolController.u8SendUnbindTimer);
    
}


/*************************************************
* Function: PCT_SendAckToCloud
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
void PCT_SendAckToCloud(u8 u8MsgId)
{
    u16 u16Len;
    ZC_SecHead struSechead;

    EVENT_BuildMsg(ZC_CODE_ACK, u8MsgId, g_u8MsgBuildBuffer, &u16Len, NULL, 0);
    struSechead.u8SecType = ZC_SEC_ALG_AES;
    struSechead.u16TotalMsg = ZC_HTONS(u16Len);
    (void)PCT_SendMsgToCloud(&struSechead, g_u8MsgBuildBuffer);
}

/*************************************************
* Function: PCT_HandleOtaBeginMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleOtaBeginMsg(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer)
{
    ZC_MessageHead *pstruMsg;
    ZC_Printf("Ota Begin\n");
    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;
    PCT_SendAckToCloud(pstruMsg->MsgId);    
    
    PCT_SendNotifyMsg(ZC_CODE_ZOTA_BEGIN);
    return;
}
/*************************************************
* Function: PCT_ModuleOtaFileBeginMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_ModuleOtaFileBeginMsg(PTC_ProtocolCon *pstruContoller, u8 *pu8Msg)
{
    ZC_OtaFileBeginReq *pstruOta;
    ZC_Printf("Ota File Begin\n");

    pstruOta = (ZC_OtaFileBeginReq *)(pu8Msg);
    
    pstruContoller->struOtaInfo.u32RecvOffset = 0;
    pstruContoller->struOtaInfo.u32TotalLen = ZC_HTONL(pstruOta->u32FileTotalLen);
    pstruContoller->struOtaInfo.u8Crc[0] = pstruOta->u8TotalFileCrc[0];
    pstruContoller->struOtaInfo.u8Crc[1] = pstruOta->u8TotalFileCrc[1];    
    PCT_SendNotifyMsg(ZC_CODE_ACK);
    return;
}

/*************************************************
* Function: PCT_ModuleOtaFileChunkMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_ModuleOtaFileChunkMsg(PTC_ProtocolCon *pstruContoller, ZC_MessageHead *pstruMsg, u8 *pu8Msg)
{
    ZC_OtaFileChunkReq *pstruOta;
    u32 u32FileLen;
    u32 u32RetVal;
    u32 u32RecvOffset;
    u16 u16CalCrc;
    u16 u16RecvCrc;

    ZC_Printf("Ota File Chunk\n");
    
    pstruOta = (ZC_OtaFileChunkReq *)(pu8Msg);
    u32FileLen = ZC_HTONS(pstruMsg->Payloadlen) - sizeof(ZC_OtaFileChunkReq);
    u32RecvOffset = ZC_HTONL(pstruOta->u32Offset);
    
    /*check para*/
    if ((u32RecvOffset != pstruContoller->struOtaInfo.u32RecvOffset)
        || ((u32RecvOffset + u32FileLen) > pstruContoller->struOtaInfo.u32TotalLen)
      || (u32FileLen > ZC_OTA_MAX_CHUNK_LEN))
    {
        ZC_Printf("recv error %d,%d,%d,%d,\n", u32RecvOffset, 
            pstruContoller->struOtaInfo.u32RecvOffset,
            pstruContoller->struOtaInfo.u32TotalLen,
            u32FileLen);
        PCT_SendNotifyMsg(ZC_CODE_ERR);
        return;
    }
    //Check CRC
    u16CalCrc = crc16_ccitt((u8*)(pstruOta), ZC_HTONS(pstruMsg->Payloadlen));
    u16RecvCrc = (pstruMsg->TotalMsgCrc[0] << 8) + pstruMsg->TotalMsgCrc[1];

    if (u16CalCrc != u16RecvCrc)
    {
        PCT_SendNotifyMsg(ZC_CODE_ERR);
        return;
    }
 
    u32RetVal = pstruContoller->pstruMoudleFun->pfunUpdate((u8*)(pstruOta + 1), u32RecvOffset, u32FileLen);
    //u32RetVal = ZC_RET_OK;
    ZC_Printf("offset = %d, len = %d\n", u32RecvOffset, u32FileLen);

    if (ZC_RET_ERROR == u32RetVal)
    {
        ZC_Printf("OTA Fail\n");
        PCT_SendNotifyMsg(ZC_CODE_ERR);
        return;
    }

    /*update file offset*/
    pstruContoller->struOtaInfo.u32RecvOffset = pstruContoller->struOtaInfo.u32RecvOffset + u32FileLen;
    PCT_SendNotifyMsg(ZC_CODE_ACK);
}
/*************************************************
* Function: PCT_ModuleOtaFileEndMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_ModuleOtaFileEndMsg(PTC_ProtocolCon *pstruContoller, u8 *pu8Msg)
{
    u32 u32RetVal;
    ZC_Printf("Ota File End\n");
    u32RetVal = pstruContoller->pstruMoudleFun->pfunUpdateFinish(pstruContoller->struOtaInfo.u32TotalLen);
    if (ZC_RET_ERROR == u32RetVal)
    {
        PCT_SendNotifyMsg(ZC_CODE_ERR);
    }
    else
    {
        PCT_SendNotifyMsg(ZC_CODE_ACK);
    }
}

/*************************************************
* Function: PCT_HandleOtaFileBeginMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleOtaFileBeginMsg(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer)
{
    ZC_MessageHead *pstruMsg;
    ZC_OtaFileBeginReq *pstruOta;
    u32 u32Ret;
    ZC_Printf("Ota File Begin\n");
    
    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;
    pstruOta = (ZC_OtaFileBeginReq *)(pstruMsg + 1);
    
    pstruContoller->struOtaInfo.u32RecvOffset = 0;
    pstruContoller->struOtaInfo.u32TotalLen = ZC_HTONL(pstruOta->u32FileTotalLen);
    pstruContoller->struOtaInfo.u8Crc[0] = pstruOta->u8TotalFileCrc[0];
    pstruContoller->struOtaInfo.u8Crc[1] = pstruOta->u8TotalFileCrc[1];    

    PCT_SendAckToCloud(pstruMsg->MsgId);

    return;
}
/*************************************************
* Function: PCT_HandleOtaFileChunkMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleOtaFileChunkMsg(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer)
{
    ZC_MessageHead *pstruMsg;
    ZC_OtaFileChunkReq *pstruOta;
    u32 u32FileLen;
    u32 u32RetVal;
    u32 u32RecvOffset;

    ZC_Printf("Ota File Chunk\n");

    
    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;
    pstruOta = (ZC_OtaFileChunkReq *)(pstruMsg + 1);
    u32FileLen = ZC_HTONS(pstruMsg->Payloadlen) - sizeof(ZC_OtaFileChunkReq);
    u32RecvOffset = ZC_HTONL(pstruOta->u32Offset);
    
    /*check para*/
    if ((u32RecvOffset != pstruContoller->struOtaInfo.u32RecvOffset)
        || ((u32RecvOffset + u32FileLen) > pstruContoller->struOtaInfo.u32TotalLen)
      || (u32FileLen > ZC_OTA_MAX_CHUNK_LEN))
    {
        ZC_Printf("recv error %d,%d,%d,%d,\n", u32RecvOffset, 
            pstruContoller->struOtaInfo.u32RecvOffset,
            pstruContoller->struOtaInfo.u32TotalLen,
            u32FileLen);
        PCT_SendErrorMsg(pstruMsg->MsgId, NULL, 0);
        return;
    }

    u32RetVal = pstruContoller->pstruMoudleFun->pfunUpdate((u8*)(pstruOta + 1), u32RecvOffset, u32FileLen);
    //u32RetVal = ZC_RET_OK;
    ZC_Printf("offset = %d, len = %d\n", u32RecvOffset, u32FileLen);

    if (ZC_RET_ERROR == u32RetVal)
    {
        ZC_Printf("OTA Fail\n");
        PCT_SendErrorMsg(pstruMsg->MsgId, NULL, 0);
        return;
    }

    /*update file offset*/
    pstruContoller->struOtaInfo.u32RecvOffset = pstruContoller->struOtaInfo.u32RecvOffset + u32FileLen;
    PCT_SendAckToCloud(pstruMsg->MsgId);
}
/*************************************************
* Function: PCT_HandleOtaFileEndMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleOtaFileEndMsg(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer)
{
    ZC_MessageHead *pstruMsg;
    ZC_Printf("Ota File End\n");

    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;
    PCT_SendAckToCloud(pstruMsg->MsgId);

}
/*************************************************
* Function: PCT_HandleOtaFileEndMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleOtaEndMsg(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer)
{
    ZC_MessageHead *pstruMsg;
    u32 u32RetVal;
    ZC_Printf("Ota End\n");

    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;
    u32RetVal = pstruContoller->pstruMoudleFun->pfunUpdateFinish(pstruContoller->struOtaInfo.u32TotalLen);
    if (ZC_RET_ERROR == u32RetVal)
    {
        PCT_SendErrorMsg(pstruMsg->MsgId, NULL, 0);
    }
    else
    {
        PCT_SendAckToCloud(pstruMsg->MsgId);
        PCT_SendNotifyMsg(ZC_CODE_ZOTA_END);
        pstruContoller->pstruMoudleFun->pfunSetTimer(PCT_TIMER_REBOOT, 
            PCT_TIMER_INTERVAL_REBOOT, &pstruContoller->u8RebootTimer);
    }
}

/*************************************************
* Function: PCT_HandleMoudleMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleMoudleMsg(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer)
{
    ZC_MessageHead *pstruMsg;
    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;

    if((pstruMsg->MsgCode < ZC_CODE_OTA_BEGIN)||(pstruMsg->MsgCode > ZC_CODE_OTA_END))
    {
        pstruContoller->u8SendMoudleMsgId = pstruMsg->MsgId;
        /*start send timer*/
        pstruContoller->pstruMoudleFun->pfunSetTimer(PCT_TIMER_SENDMOUDLE, 
            PCT_TIMER_INTERVAL_SENDMOUDLE, &pstruContoller->u8SendMoudleTimer);
    }
    /*Send to Moudle*/
    pstruContoller->pstruMoudleFun->pfunSendToMoudle((u8*)pstruMsg, pstruBuffer->u32Len);

    /*restart heart timer*/
    if (PCT_TIMER_INVAILD != pstruContoller->u8HeartTimer)
    {
        TIMER_StopTimer(pstruContoller->u8HeartTimer);
    }

    pstruContoller->pstruMoudleFun->pfunSetTimer(PCT_TIMER_SENDHEART, 
        PCT_TIMER_INTERVAL_HEART, &pstruContoller->u8HeartTimer);

}
/*************************************************
* Function: PCT_HandleMoudleMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SetTokenKey(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer)
{
    ZC_MessageHead *pstruMsg;
    ZC_TokenSetReq *pstruSetKey;

    ZC_Printf("PCT_SetTokenKey\n");
    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;
    pstruSetKey = (ZC_TokenSetReq *)(pstruMsg + 1);

    ZC_StoreTokenKey(pstruSetKey->TokenKey);

    PCT_SendEmptyMsg(pstruMsg->MsgId, ZC_SEC_ALG_AES);
    PCT_SendAckToCloud(pstruMsg->MsgId);
    
    return;
}

/*************************************************
* Function: PCT_ResetNetWork
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_ResetNetWork(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer)
{
    ZC_MessageHead *pstruMsg;

    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;

    ZC_ConfigInitPara();
    PCT_DisConnectCloud(pstruContoller);

    PCT_SendEmptyMsg(pstruMsg->MsgId, ZC_SEC_ALG_AES);
    PCT_SendAckToCloud(pstruMsg->MsgId);
    
    return;
}

#define MSG_SERVER_CLIENT_SET_LED_ONOFF_REQ  (68)

typedef struct tag_STRU_LED_STATUS
{		
    u8	     u8LedOnOff ; // 0:关，1：开，2：获取当前开关状态
    u8	     u8Pad[3];		 
}STRU_LED_STATUS;

void PCT_TurnOnOffLight(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer)
{
    ZC_MessageHead *pstruMsg;
    STRU_LED_STATUS *pstruLed;
    u16 u16Len;
    u32 u32Value;
    ZC_SecHead struSechead;
    u8 u8Payload[4] = {1, 0, 0, 0};
    ZC_Printf("PCT_TurnOnOffLight\n");

    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;
    pstruLed = (STRU_LED_STATUS *)(pstruMsg + 1);
    switch (pstruLed->u8LedOnOff)
    {
        case 0:
        case 1:
            u32Value = ((pstruLed->u8LedOnOff == 0) ? 1 : 0);
            //GPIO_OUTPUT_SET(GPIO_ID_PIN(4), u32Value);
            break;
        default:
            ZC_Printf("u8LedOnOff is %d\n", pstruLed->u8LedOnOff);
            return;
    }
    /* 给app回响应 */
    PCT_SendEmptyMsg(pstruMsg->MsgId, ZC_SEC_ALG_AES);
    EVENT_BuildMsg(102, pstruMsg->MsgId, g_u8MsgBuildBuffer, &u16Len, u8Payload, 4);
    struSechead.u8SecType = ZC_SEC_ALG_AES;
    struSechead.u16TotalMsg = ZC_HTONS(u16Len);
    (void)PCT_SendMsgToCloud(&struSechead, g_u8MsgBuildBuffer);
    return;
}

/*************************************************
* Function: PCT_HandleEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleEvent(PTC_ProtocolCon *pstruContoller)
{
    MSG_Buffer *pstruBuffer;
    ZC_MessageHead *pstruMsg;
    
	if (ZC_MAGIC_FLAG == g_struZcConfigDb.struDeviceInfo.u32UnBindFlag)
	{
        g_struProtocolController.u8MainState = PCT_STATE_WAIT_UNBIND;
        if (PCT_TIMER_INVAILD != pstruContoller->u8SendUnbindTimer)
        {
            TIMER_StopTimer(pstruContoller->u8SendUnbindTimer);
        }
        PCT_SendUnbindMsg();
        return;
    }
    pstruBuffer = (MSG_Buffer *)MSG_PopMsg(&g_struRecvQueue);
    if (NULL == pstruBuffer)
    {
        return;
    }
    
    pstruMsg = (ZC_MessageHead*)pstruBuffer->u8MsgBuffer;
    ZC_Printf("event %d recv len =%d\n", pstruMsg->MsgId, ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_MessageHead));
    ZC_TraceData((u8*)pstruMsg, ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_MessageHead));
    if (ZC_RET_ERROR == PCT_CheckCrc(pstruMsg->TotalMsgCrc, (u8*)(pstruMsg + 1), ZC_HTONS(pstruMsg->Payloadlen)))
    {
        PCT_SendEmptyMsg(pstruMsg->MsgId, ZC_SEC_ALG_AES);
        PCT_SendErrorMsg(pstruMsg->MsgId, NULL, 0);

        pstruBuffer->u32Len = 0;
        pstruBuffer->u8Status = MSG_BUFFER_IDLE;
        
        return;
    }
				
    /*when do OTA, does not send empty*/
    switch (pstruMsg->MsgCode)
    {
        case ZC_CODE_ZOTA_BEGIN:
            PCT_HandleOtaBeginMsg(pstruContoller, pstruBuffer);
            break;   
        case ZC_CODE_ZOTA_FILE_BEGIN:
            PCT_HandleOtaFileBeginMsg(pstruContoller, pstruBuffer);
            break;
        case ZC_CODE_ZOTA_FILE_CHUNK:
            PCT_HandleOtaFileChunkMsg(pstruContoller, pstruBuffer);
            break;
        case ZC_CODE_ZOTA_FILE_END:
            PCT_HandleOtaFileEndMsg(pstruContoller, pstruBuffer);
            break;  
        case ZC_CODE_ZOTA_END:
            PCT_HandleOtaEndMsg(pstruContoller, pstruBuffer);
            break; 
        case ZC_CODE_OTA_BEGIN:
        case ZC_CODE_OTA_FILE_BEGIN:     
        case ZC_CODE_OTA_FILE_CHUNK:
        case ZC_CODE_OTA_FILE_END:
        case ZC_CODE_OTA_END:
        case ZC_CODE_OTA_CONFIRM:
            PCT_HandleMoudleMsg(pstruContoller, pstruBuffer);
            break;
        case ZC_CODE_TOKEN_SET:
            PCT_SetTokenKey(pstruContoller, pstruBuffer);
            break;
        case ZC_CODE_RESET_NETWORK:
            PCT_ResetNetWork(pstruContoller, pstruBuffer);
            break;
        case ZC_CODE_UNBIND:
            break;
        default:
            PCT_HandleMoudleMsg(pstruContoller, pstruBuffer);
            break;                                    
    }

    pstruBuffer->u32Len = 0;
    pstruBuffer->u8Status = MSG_BUFFER_IDLE;

    
    return;
} 

/*************************************************
* Function: PCT_Run
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_Run()
{
    static u32 count = 0;
    PTC_ProtocolCon *pstruContoller = &g_struProtocolController;
    
    //if ((count++) % 100 == 0)
        //ZC_Printf("state is %d\n", pstruContoller->u8MainState);
    switch(pstruContoller->u8MainState)
    {
        case PCT_STATE_SLEEP:
            break;
        case PCT_STATE_INIT:
            /*smart connection*/
            break;
        case PCT_STATE_ACCESS_NET:
            PCT_ConnectCloud(pstruContoller);
            break;
        case PCT_STATE_WAIT_ACCESS:
        	PCT_SendCloudAccessMsg1(pstruContoller);        
            break;
        case PCT_STATE_WAIT_ACCESSRSP:
            PCT_RecvAccessMsg2(pstruContoller);
            break;    
        case PCT_STATE_WAIT_MSG4:
            PCT_RecvAccessMsg4(pstruContoller);
            break;
        case PCT_STATE_CONNECT_CLOUD:
            PCT_HandleEvent(pstruContoller);
            break;
        case PCT_STATE_WAIT_UNBIND:
            PCT_HandleUnbindMsg(pstruContoller);
            break;
    }
}
/*************************************************
* Function: PCT_WakeUp
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_WakeUp()
{
    if (PCT_STATE_INIT == g_struProtocolController.u8MainState)
    {
    #if 0
        g_struProtocolController.pstruMoudleFun->pfunSetTimer(PCT_TIMER_REGISTER, 
            PCT_TIMER_INTERVAL_REGISTER, &g_struProtocolController.u8RegisterTimer);
    #endif
        ZC_Printf("PCT_WakeUp\n");
        /*Intial Bc send Num*/
        g_struProtocolController.u16SendBcNum = 0;
        PCT_SendNotifyMsg(ZC_CODE_WIFI_CONNECTED);
        ZC_ClientWakeUp();
        g_struProtocolController.u8MainState = PCT_STATE_ACCESS_NET;
    }
    else
    {
        ZC_Printf("PCT_WakeUp: state is %d\n", g_struProtocolController.u8MainState);
    }
    
}

/*************************************************
* Function: PCT_Sleep
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_Sleep()
{
    u32 u32Index;
    MSG_Init();

    g_struProtocolController.u8keyRecv = PCT_KEY_UNRECVED;
    for (u32Index = 0; u32Index < ZC_TIMER_MAX_NUM; u32Index++)
    {
        if (g_struTimer[u32Index].u8Status == ZC_TIMER_STATUS_USED)
        {
            TIMER_StopTimer((u8)u32Index);
        }
    }

    TIMER_Init();
    g_struProtocolController.u8ReconnectTimer = PCT_TIMER_INVAILD;
    g_struProtocolController.u8SendMoudleTimer = PCT_TIMER_INVAILD;
    g_struProtocolController.u8HeartTimer = PCT_TIMER_INVAILD;
    g_struProtocolController.u8MainState = PCT_STATE_INIT;
    g_struProtocolController.u8RegisterTimer = PCT_TIMER_INVAILD;
    memcpy(g_struProtocolController.IvRecv, g_u8ExAesKey, ZC_HS_SESSION_KEY_LEN);
    memcpy(g_struProtocolController.IvSend, g_u8ExAesKey, ZC_HS_SESSION_KEY_LEN);
    memcpy(g_struProtocolController.u8SessionKey, g_u8ExAesKey, ZC_HS_SESSION_KEY_LEN);
    PCT_SendNotifyMsg(ZC_CODE_WIFI_DISCONNECTED);
    ZC_ClientSleep();
}

/*************************************************
* Function: PCT_SendMsgToCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 PCT_SendMsgToCloud(ZC_SecHead *pstruSecHead, u8 *pu8PlainData)
{
    u32 u32Index;
    u32 u32RetVal;
    u16 u16Len;
    u16 u16PaddingLen;
    
    u32RetVal = SEC_PaddingCheck(pstruSecHead->u8SecType, ZC_HTONS(pstruSecHead->u16TotalMsg), &u16PaddingLen);
    
    if (ZC_RET_ERROR == u32RetVal)
    {
        return ZC_RET_ERROR;
    }

    u16Len = ZC_HTONS(pstruSecHead->u16TotalMsg) + sizeof(ZC_SecHead) + u16PaddingLen;    
    
    if (u16Len > MSG_BUFFER_MAXLEN)
    {
        return ZC_RET_ERROR;
    }

    for (u32Index = 0; u32Index < MSG_BUFFER_SEND_MAX_NUM; u32Index++)
    {
        if (MSG_BUFFER_IDLE == g_struSendBuffer[u32Index].u8Status)
        {
            u16Len = ZC_HTONS(pstruSecHead->u16TotalMsg) + u16PaddingLen;

            /*first check padding,then Encrypt, final copy sechead*/
            u32RetVal = SEC_Encrypt(pstruSecHead, g_struSendBuffer[u32Index].u8MsgBuffer + sizeof(ZC_SecHead), pu8PlainData, &u16Len);
            
            if (ZC_RET_ERROR == u32RetVal)
            {
                return ZC_RET_ERROR;
            }
            
            pstruSecHead->u16TotalMsg = ZC_HTONS(u16Len);
            /*copy sechead*/
            memcpy(g_struSendBuffer[u32Index].u8MsgBuffer, (u8*)pstruSecHead, sizeof(ZC_SecHead));
        
            g_struSendBuffer[u32Index].u32Len = u16Len + sizeof(ZC_SecHead);
            g_struSendBuffer[u32Index].u8Status = MSG_BUFFER_FULL;
            MSG_PushMsg(&g_struSendQueue, (u8*)&g_struSendBuffer[u32Index]);

            return ZC_RET_OK;
        }
    }

    return ZC_RET_ERROR;
}

/******************************* FILE END ***********************************/

