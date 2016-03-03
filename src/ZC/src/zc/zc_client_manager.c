/**
******************************************************************************
* @file     zc_client_manager.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief   
******************************************************************************
*/

#include <zc_common.h>
#include <zc_protocol_controller.h>
#include <zc_client_manager.h>

ZC_ClientInfo g_struClientInfo;

/*************************************************
* Function: ZC_ClientDisconnect
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_ClientDisconnect(u32 u32ClientId) 
{
    u32 u32Index;
    for (u32Index = 0; u32Index < ZC_MAX_CLIENT_NUM; u32Index++)
    {
        if (u32ClientId == g_struClientInfo.u32ClientFd[u32Index])
        {
            g_struClientInfo.u32ClientVaildFlag[u32Index] = ZC_CLIENT_VAILD_FLAG;
            if ((ZC_CLIENT_STATUS_BUSY == g_struClientInfo.u8ClientStates)
            && (u32ClientId == g_struClientInfo.u32ClientBusyId))
            {
                ZC_SetClientFree(u32ClientId);
            }
        }
    }
}

/*************************************************
* Function: ZC_ClientConnect
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 ZC_ClientConnect(u32 u32ClientId) 
{
    u32 u32Index;

    for (u32Index = 0; u32Index < ZC_MAX_CLIENT_NUM; u32Index++)
    {
        if (ZC_CLIENT_VAILD_FLAG == g_struClientInfo.u32ClientVaildFlag[u32Index])
        {
            g_struClientInfo.u32ClientFd[u32Index] = u32ClientId;
            g_struClientInfo.u32ClientVaildFlag[u32Index] = ZC_CLIENT_INVAILD_FLAG;
            return ZC_RET_OK;
        }
    }

    return ZC_RET_ERROR;
}

/*************************************************
* Function: ZC_ClientConnect
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_ClientInit(void) 
{
    u32 u32Index;
    g_struClientInfo.u8ClientStates = ZC_CLIENT_STATUS_INIT;
    for (u32Index = 0; u32Index < ZC_MAX_CLIENT_NUM; u32Index++)
    {
        g_struClientInfo.u32ClientVaildFlag[u32Index] = ZC_CLIENT_VAILD_FLAG;
    }
}

/*************************************************
* Function: ZC_CheckClientIdle
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 ZC_CheckClientIdle(u32 u32Fd)
{
    if (ZC_CLIENT_STATUS_IDLE == g_struClientInfo.u8ClientStates)
    {
        return ZC_RET_OK;
    }
    else
    {
        if (u32Fd == g_struClientInfo.u32ClientBusyId)
        {
            return ZC_RET_OK;
        }
        else
        {
            return ZC_RET_ERROR;    
        }
        
    }
}
/*************************************************
* Function: ZC_SetClientBusy
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_SetClientBusy(u32 u32Clientfd)
{
    g_struClientInfo.u8ClientStates = ZC_CLIENT_STATUS_BUSY;
    g_struClientInfo.u32ClientBusyId = u32Clientfd;
}
/*************************************************
* Function: PCT_SetClientFree
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_SetClientFree(u32 u32Clientfd)
{
    g_struClientInfo.u8ClientStates = ZC_CLIENT_STATUS_IDLE;
}

/*************************************************
* Function: ZC_StartClientListen
* Description: 
* Author: zw
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_StartClientListen(void)
{
    u32 u32Ret = ZC_RET_OK;

    if (ZC_CLIENT_STATUS_WAKEUP == g_struClientInfo.u8ClientStates)
    {
        /*Listen*/
        u32Ret = g_struProtocolController.pstruMoudleFun->pfunListenClient(&g_struProtocolController.struClientConnection);
        if (ZC_RET_OK != u32Ret)
        {
            return;
        }

        /*change status to idle, wait accept and recv*/
        g_struClientInfo.u8ClientStates = ZC_CLIENT_STATUS_IDLE;
    }
}
/*************************************************
* Function: ZC_ClientWakeUp
* Description: 
* Author: zw
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_ClientWakeUp(void)
{
    g_struClientInfo.u8ClientStates = ZC_CLIENT_STATUS_WAKEUP;
}
/*************************************************
* Function: ZC_ClientSleep
* Description: 
* Author: zw
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_ClientSleep(void)
{
    ZC_ClientInit();
}


/*************************************************
* Function: ZC_RecvDataFromClient
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_RecvDataFromClient(u32 ClientId, u8 *pu8Data, u32 u32DataLen)
{
    u32 u32RetVal;
    ZC_MessageHead *pstruMsg;
    ZC_MessageOptHead struOpt;
    ZC_SsessionInfo struSessionMsg;
    ZC_SecHead *pstruHead;
    u16 u16Len;
    u32 u32CiperLen;
    u8 *pu8Key;
    ZC_SendParam struParam;
    u8 u8Iv[ZC_HS_SESSION_KEY_LEN];
    u16 crc;

    /*can hanle it,get aes key*/
    
    ZC_GetStoreInfor(ZC_GET_TYPE_TOKENKEY, &pu8Key);

    u32RetVal = ZC_CheckClientIdle(ClientId);
    if (ZC_RET_ERROR == u32RetVal)
    {
        pstruHead = (ZC_SecHead*)(g_u8MsgBuildBuffer);
        
        EVENT_BuildMsg(ZC_CODE_ERR, 0, g_u8MsgBuildBuffer + sizeof(ZC_SecHead), &u16Len, 
            NULL, 0);
        memcpy(u8Iv, pu8Key, ZC_HS_SESSION_KEY_LEN);
        AES_CBC_Encrypt(g_u8MsgBuildBuffer + sizeof(ZC_SecHead), u16Len,
            pu8Key, ZC_HS_SESSION_KEY_LEN,
            u8Iv, ZC_HS_SESSION_KEY_LEN,
            g_u8MsgBuildBuffer + sizeof(ZC_SecHead), &u32CiperLen);

        pstruHead->u8SecType = ZC_SEC_ALG_AES;
        pstruHead->u16TotalMsg = ZC_HTONS((u16)u32CiperLen);

        struParam.u8NeedPoll = 0;            
        g_struProtocolController.pstruMoudleFun->pfunSendTcpData(ClientId, g_u8MsgBuildBuffer, 
            u32CiperLen + sizeof(ZC_SecHead), &struParam);
        return;            
    }
    
    /*set client busy*/
    ZC_SetClientBusy(ClientId);
    
    u32RetVal = MSG_RecvData(&g_struClientBuffer, pu8Data, u32DataLen);
    if ((MSG_BUFFER_FULL == g_struClientBuffer.u8Status)&&(ZC_RET_OK == u32RetVal))
    {  
        do
        {
            pstruHead = (ZC_SecHead *)g_struClientBuffer.u8MsgBuffer;

            if (ZC_HTONS(pstruHead->u16TotalMsg) >= MSG_BULID_BUFFER_MAXLEN)
            {
                break;
            }
         
            memcpy(u8Iv, pu8Key, ZC_HS_SESSION_KEY_LEN);
            AES_CBC_Decrypt(g_struClientBuffer.u8MsgBuffer + sizeof(ZC_SecHead), ZC_HTONS(pstruHead->u16TotalMsg), 
                pu8Key, ZC_HS_SESSION_KEY_LEN, 
                u8Iv, ZC_HS_SESSION_KEY_LEN, 
                g_u8MsgBuildBuffer, &u32CiperLen);

            pstruMsg = (ZC_MessageHead*)(g_u8MsgBuildBuffer);
            if(ZC_RET_ERROR == PCT_CheckCrc(pstruMsg->TotalMsgCrc, (u8 *)(pstruMsg + 1), ZC_HTONS(pstruMsg->Payloadlen)))
            {
                break;
            }

            pstruMsg->Payloadlen = ZC_HTONS(ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_MessageOptHead) + sizeof(ZC_SsessionInfo));
            if (ZC_HTONS(pstruMsg->Payloadlen) > MSG_BULID_BUFFER_MAXLEN)
            {
                break;
            }

            pstruMsg->OptNum = pstruMsg->OptNum + 1;
            struOpt.OptCode = ZC_HTONS(ZC_OPT_SSESSION);
            struOpt.OptLen = ZC_HTONS(sizeof(ZC_SsessionInfo));
            struSessionMsg.u32SsessionId = ZC_HTONL(ClientId);

            u16Len = 0;
            memcpy(g_struClientBuffer.u8MsgBuffer + u16Len, pstruMsg, sizeof(ZC_MessageHead));
 
            /*insert opt*/
            u16Len += sizeof(ZC_MessageHead);
            memcpy(g_struClientBuffer.u8MsgBuffer + u16Len, 
                 &struOpt, sizeof(ZC_MessageOptHead));
            u16Len += sizeof(ZC_MessageOptHead);
            memcpy(g_struClientBuffer.u8MsgBuffer + u16Len, 
                 &struSessionMsg, sizeof(struSessionMsg));
 
            /*copy message*/
            u16Len += sizeof(struSessionMsg);    
            memcpy(g_struClientBuffer.u8MsgBuffer + u16Len, 
                (u8*)(pstruMsg+1), ZC_HTONS(pstruMsg->Payloadlen) - (sizeof(ZC_MessageOptHead) + sizeof(struSessionMsg)));   
 
            u16Len += ZC_HTONS(pstruMsg->Payloadlen) - (sizeof(ZC_MessageOptHead) + sizeof(struSessionMsg));     
            g_struClientBuffer.u32Len = u16Len;
            crc = crc16_ccitt(g_struClientBuffer.u8MsgBuffer+sizeof(ZC_MessageHead),u16Len-sizeof(ZC_MessageHead));
            pstruMsg =  (ZC_MessageHead*)(g_struClientBuffer.u8MsgBuffer);
            pstruMsg->TotalMsgCrc[0]=(crc&0xff00)>>8;
            pstruMsg->TotalMsgCrc[1]=(crc&0xff);

            ZC_TraceData(g_struClientBuffer.u8MsgBuffer, g_struClientBuffer.u32Len);
            
            /*send to moudle*/
            g_struProtocolController.pstruMoudleFun->pfunSendToMoudle(g_struClientBuffer.u8MsgBuffer, g_struClientBuffer.u32Len);
        }while(0);
        g_struClientBuffer.u8Status = MSG_BUFFER_IDLE;
        g_struClientBuffer.u32Len = 0;
        ZC_SetClientFree(ClientId);

    }
  
    return;
}

/******************************* FILE END ***********************************/




