/**
******************************************************************************
* @file     zc_moudle_manager.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief   
******************************************************************************
*/

#include <zc_common.h>
#include <zc_protocol_controller.h>
#include <zc_module_interface.h>
extern u8 g_u8ClientSendLen;
#ifndef NO_NEED_ASSEMBLE
extern ZC_UartBuffer g_struUartBuffer;
#endif
/*************************************************
* Function: ZC_DealSessionOpt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 ZC_DealSessionOpt(ZC_MessageHead *pstruMsg, ZC_OptList *pstruOptList, u8 *pu8PayLoad)
{
    u32 u32Index;
    u32 u32ClientId;
    u16 u16RealLen;
    u32 u32CiperLen;
    ZC_SecHead struSecHead;
    ZC_SsessionInfo *pstruSsession;
    ZC_OptList struOptList;
    ZC_SendParam struParam;
    u8 *pu8Key;
    u8 u8FindFlag = 0;
    u16 u16OptLen = 0;
    u16 crc = 0;
    u8 u8Iv[ZC_HS_SESSION_KEY_LEN];
    u8 *pu8SendBuff = (u8*)pstruMsg;
    pstruSsession = pstruOptList->pstruSsession;
    u32ClientId = ZC_HTONL(pstruSsession->u32SsessionId);
    struOptList.pstruSsession = NULL;
    struOptList.pstruTransportInfo = pstruOptList->pstruTransportInfo;

    for (u32Index = 0; u32Index < ZC_MAX_CLIENT_NUM; u32Index++)
    {
        if (ZC_CLIENT_INVAILD_FLAG == g_struClientInfo.u32ClientVaildFlag[u32Index])
        {
            if (u32ClientId == g_struClientInfo.u32ClientFd[u32Index])
            {
                u8FindFlag = 1;
                break;
            }
        }
    }

    if (0 == u8FindFlag)
    {
        return ZC_RET_OK;
    }

    EVENT_BuildOption(&struOptList, &pstruMsg->OptNum, 
        pu8SendBuff + sizeof(ZC_SecHead) + sizeof(ZC_MessageHead), &u16OptLen);
    
    u16RealLen = ZC_HTONS(pstruMsg->Payloadlen)
                    - (sizeof(ZC_MessageOptHead) + sizeof(ZC_SsessionInfo));
    
    pstruMsg->Payloadlen = ZC_HTONS(u16RealLen);


    /*copy msg*/
    memcpy(pu8SendBuff + sizeof(ZC_SecHead) + sizeof(ZC_MessageHead) + u16OptLen , 
        pu8PayLoad,
        (u16RealLen - u16OptLen));

    crc = crc16_ccitt(pu8SendBuff + sizeof(ZC_SecHead)+ sizeof(ZC_MessageHead),u16RealLen);
    pstruMsg->TotalMsgCrc[0]=(crc&0xff00)>>8;
    pstruMsg->TotalMsgCrc[1]=(crc&0xff);

    /*copy msg head*/
    memmove(pu8SendBuff + sizeof(ZC_SecHead), (u8*)pstruMsg, sizeof(ZC_MessageHead));

    
    ZC_GetStoreInfor(ZC_GET_TYPE_TOKENKEY, &pu8Key);

    u32CiperLen = MSG_BULID_BUFFER_MAXLEN;
    
    memcpy(u8Iv, pu8Key, ZC_HS_SESSION_KEY_LEN);
    AES_CBC_Encrypt(pu8SendBuff + sizeof(ZC_SecHead), u16RealLen + sizeof(ZC_MessageHead),
        pu8Key, ZC_HS_SESSION_KEY_LEN,
        u8Iv, ZC_HS_SESSION_KEY_LEN,
        pu8SendBuff + sizeof(ZC_SecHead), &u32CiperLen);
    
    /*copy sec head*/
    struSecHead.u16TotalMsg = ZC_HTONS((u16)u32CiperLen);
    struSecHead.u8SecType = ZC_SEC_ALG_AES;
    
    memcpy(pu8SendBuff, &struSecHead, sizeof(ZC_SecHead));
    
    
    /*msg len include sec head, msg head, payload len*/
    g_u8ClientSendLen = u32CiperLen + sizeof(ZC_SecHead);
    
    struParam.u8NeedPoll = 1;
 
    g_struProtocolController.pstruMoudleFun->pfunSendTcpData(u32ClientId, pu8SendBuff, g_u8ClientSendLen, &struParam);
    
    g_u8ClientSendLen = 0;
    return ZC_RET_OK;
}

/*************************************************
* Function: ZC_DealSessionOpt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 ZC_DealExtCode(u8 *pu8Data)
{
    ZC_ExtMessageHead *pstruExtMsg;
    u8 *pu8Payload = NULL; 
    pstruExtMsg = (ZC_ExtMessageHead *)pu8Data;
    pu8Payload = pu8Data + sizeof(ZC_ExtMessageHead);
    switch(pstruExtMsg->ExtMsgCode)
    {
        case ZC_CODE_EXT_REGSITER:
        {
            if ((g_struProtocolController.u8MainState >= PCT_STATE_ACCESS_NET) &&
            (g_struProtocolController.u8MainState < PCT_STATE_DISCONNECT_CLOUD)
            )
            {
                PCT_SendNotifyMsg(ZC_CODE_CLOUD_CONNECTED);                
                return ZC_RET_OK;
            }
            else if (PCT_STATE_DISCONNECT_CLOUD == g_struProtocolController.u8MainState)
            {
                PCT_SendNotifyMsg(ZC_CODE_CLOUD_DISCONNECTED);                
                return ZC_RET_OK;
            }
            
            ZC_StoreRegisterInfo(pu8Payload,1);
            g_struProtocolController.u8MainState = PCT_STATE_ACCESS_NET; 
            if (PCT_TIMER_INVAILD != g_struProtocolController.u8RegisterTimer)
            {
                TIMER_StopTimer(g_struProtocolController.u8RegisterTimer);
                g_struProtocolController.u8RegisterTimer = PCT_TIMER_INVAILD;
            }
            break;
        }
        case ZC_CODE_EXT_REBOOT:
        {
            g_struProtocolController.pstruMoudleFun->pfunReboot();
            break;
        }
        default:
            break;
    }
    
    return ZC_RET_OK;
}

/*************************************************
* Function: ZC_RecvDataFromMoudle
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 ZC_RecvDataFromMoudle(u8 *pu8Data, u16 u16DataLen)
{
    ZC_MessageHead *pstruMsg;
    ZC_OptList struOptList;
    u16 u16OptLen= 0;
    u8 *pu8Payload = NULL;
 
    if (0 == u16DataLen)
    {
        return ZC_RET_ERROR;
    }
    
    pstruMsg = (ZC_MessageHead *)pu8Data;
    if(ZC_RET_ERROR == PCT_CheckCrc(pstruMsg->TotalMsgCrc, (u8 *)(pstruMsg + 1), ZC_HTONS(pstruMsg->Payloadlen)))
    {
        ZC_TraceData((u8 *)(pstruMsg + 1), ZC_HTONS(pstruMsg->Payloadlen));
        return ZC_RET_ERROR;
    }

    struOptList.pstruSsession = NULL;
    struOptList.pstruTransportInfo = NULL;
    
    EVENT_ParseOption(pstruMsg, &struOptList, &u16OptLen);

    pu8Payload = (u8*)pstruMsg + u16OptLen + sizeof(ZC_MessageHead);

    if (NULL != struOptList.pstruSsession)
    {
       (void)ZC_DealSessionOpt(pstruMsg, &struOptList, pu8Payload);
       return ZC_RET_OK;
    }
    
    switch(pstruMsg->MsgCode)
    {
        case ZC_CODE_REGSITER:
        {
            if ((g_struProtocolController.u8MainState >= PCT_STATE_ACCESS_NET) &&
            (g_struProtocolController.u8MainState < PCT_STATE_DISCONNECT_CLOUD)
            )
            {
                PCT_SendNotifyMsg(ZC_CODE_CLOUD_CONNECTED);                
                return ZC_RET_OK;
            }
            else if (PCT_STATE_DISCONNECT_CLOUD == g_struProtocolController.u8MainState)
            {
                PCT_SendNotifyMsg(ZC_CODE_CLOUD_DISCONNECTED);                
                return ZC_RET_OK;
            }
            
            ZC_StoreRegisterInfo(pu8Payload,0);
            g_struProtocolController.u8MainState = PCT_STATE_ACCESS_NET; 
            
            if (PCT_TIMER_INVAILD != g_struProtocolController.u8RegisterTimer)
            {
                TIMER_StopTimer(g_struProtocolController.u8RegisterTimer);
                g_struProtocolController.u8RegisterTimer = PCT_TIMER_INVAILD;
            }
            break;
        }
        case ZC_CODE_EQ_BEGIN:
        {
            PCT_SendNotifyMsg(ZC_CODE_EQ_DONE);
            if (g_struProtocolController.u8MainState >= PCT_STATE_ACCESS_NET)
            {
                PCT_SendNotifyMsg(ZC_CODE_WIFI_CONNECTED);
            }
            break;
        }    
        case ZC_CODE_ZOTA_FILE_BEGIN:
            PCT_ModuleOtaFileBeginMsg(&g_struProtocolController, pu8Payload);
            break;
        case ZC_CODE_ZOTA_FILE_CHUNK:
            PCT_ModuleOtaFileChunkMsg(&g_struProtocolController, pstruMsg, pu8Payload);
            break;
        case ZC_CODE_ZOTA_FILE_END:
            PCT_ModuleOtaFileEndMsg(&g_struProtocolController, pu8Payload);
            PCT_SendNotifyMsg(ZC_CODE_ZOTA_END);
            break;
        case ZC_CODE_REST:
            ZC_ConfigReset();
            break;
        case ZC_CODE_CONFIG:
            ZC_ConfigPara(pu8Payload);
            break;
        case ZC_CODE_UNBIND:
            ZC_ConfigUnBind(ZC_MAGIC_FLAG);
            if (*pu8Payload)
            {
               ZC_ConfigReset();
            }
            break;
        case ZC_CODE_EXT:				
            ZC_DealExtCode(pu8Payload);
            break;
        default:
            if(PCT_STATE_CONNECT_CLOUD == g_struProtocolController.u8MainState)
            {
                PCT_HandleMoudleEvent(pu8Data, u16DataLen);
            }
            else
            {
                PCT_SendNotifyMsg(ZC_CODE_CLOUD_DISCONNECTED); 
            }
            break;
    }
    
    return ZC_RET_OK;
}

#ifndef NO_NEED_ASSEMBLE
/*************************************************
* Function: ZC_AssemblePkt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 ZC_AssemblePkt(u8 *pu8Data, u32 u32DataLen, u32 *pu32LeftLen) 
{
    ZC_MessageHead *pstruMsg;
    u8 u8MagicHead[ZC_MAGIC_LEN] = {0x02,0x03,0x04,0x05};
    u32 u32HeadLen;
    u32 u32MsgLen;
    u32 u32MagicLen = ZC_MAGIC_LEN;

    u32HeadLen = u32MagicLen + sizeof(ZC_MessageHead);
    *pu32LeftLen = 0;
    
    if (MSG_BUFFER_FULL == g_struUartBuffer.u32Status)
    {
        return ZC_RET_ERROR;
    }
    
    if (MSG_BUFFER_IDLE == g_struUartBuffer.u32Status)
    {

        if (u32DataLen < u32HeadLen)
        {
            memcpy(g_struUartBuffer.u8UartBuffer, pu8Data, u32DataLen);
            g_struUartBuffer.u32Status = MSG_BUFFER_SEGMENT_NOHEAD;
            g_struUartBuffer.u32RecvLen = u32DataLen;
        }
        else
        {
            if (0 != memcmp(pu8Data, u8MagicHead, 4))
            {
                g_struUartBuffer.u32Status = MSG_BUFFER_IDLE;
                g_struUartBuffer.u32RecvLen = 0;
                return ZC_RET_ERROR;
            }
            
            pstruMsg = (ZC_MessageHead *)(pu8Data + u32MagicLen);
            u32MsgLen =  ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_MessageHead) + u32MagicLen;

            if (u32MsgLen > ZC_MAX_UARTBUF_LEN)
            {
                g_struUartBuffer.u32Status = MSG_BUFFER_IDLE;
                g_struUartBuffer.u32RecvLen = 0;
                return ZC_RET_ERROR;
            }

            if (u32MsgLen <= u32DataLen)
            {
                memcpy(g_struUartBuffer.u8UartBuffer, pu8Data, u32MsgLen);
                g_struUartBuffer.u32Status = MSG_BUFFER_FULL;
                g_struUartBuffer.u32RecvLen = u32MsgLen;
                *pu32LeftLen = u32DataLen - u32MsgLen;
            }
            else
            {
                memcpy(g_struUartBuffer.u8UartBuffer, pu8Data, u32DataLen);
                g_struUartBuffer.u32Status = MSG_BUFFER_SEGMENT_HEAD;
                g_struUartBuffer.u32RecvLen = u32DataLen;
            }

        }

        return ZC_RET_OK;

    }

    if (MSG_BUFFER_SEGMENT_HEAD == g_struUartBuffer.u32Status)
    {
        pstruMsg = (ZC_MessageHead *)(g_struUartBuffer.u8UartBuffer + u32MagicLen);
        u32MsgLen = ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_MessageHead) + u32MagicLen;

        if (u32MsgLen <= u32DataLen + g_struUartBuffer.u32RecvLen)
        {
            memcpy((g_struUartBuffer.u8UartBuffer + g_struUartBuffer.u32RecvLen), 
                pu8Data, 
                (u32MsgLen - g_struUartBuffer.u32RecvLen));
           *pu32LeftLen = u32DataLen + g_struUartBuffer.u32RecvLen - u32MsgLen; 
            g_struUartBuffer.u32Status = MSG_BUFFER_FULL;
            g_struUartBuffer.u32RecvLen = u32MsgLen;
        }
        else
        {
            memcpy((g_struUartBuffer.u8UartBuffer + g_struUartBuffer.u32RecvLen), 
                pu8Data, 
                u32DataLen);
            g_struUartBuffer.u32RecvLen += u32DataLen;
            g_struUartBuffer.u32Status = MSG_BUFFER_SEGMENT_HEAD;
        }

        return ZC_RET_OK;
    }

    if (MSG_BUFFER_SEGMENT_NOHEAD == g_struUartBuffer.u32Status)
    {
        if ((g_struUartBuffer.u32RecvLen + u32DataLen) < u32HeadLen)
        {
            memcpy((g_struUartBuffer.u8UartBuffer + g_struUartBuffer.u32RecvLen), 
                pu8Data,
                u32DataLen);
            g_struUartBuffer.u32RecvLen += u32DataLen;
            g_struUartBuffer.u32Status = MSG_BUFFER_SEGMENT_NOHEAD;
        }
        else
        {
            memcpy((g_struUartBuffer.u8UartBuffer + g_struUartBuffer.u32RecvLen), 
                pu8Data,
                (u32HeadLen - g_struUartBuffer.u32RecvLen));

            if (0 != memcmp(g_struUartBuffer.u8UartBuffer, u8MagicHead, 4))
            {
                g_struUartBuffer.u32Status = MSG_BUFFER_IDLE;
                g_struUartBuffer.u32RecvLen = 0;
                return ZC_RET_ERROR;
            }

            pstruMsg = (ZC_MessageHead *)(g_struUartBuffer.u8UartBuffer + u32MagicLen);
            u32MsgLen = ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_MessageHead) + u32MagicLen;

            if (u32MsgLen > ZC_MAX_UARTBUF_LEN)
            {
                g_struUartBuffer.u32Status = MSG_BUFFER_IDLE;
                g_struUartBuffer.u32RecvLen = 0;                
                return ZC_RET_ERROR;
            }

            if (u32MsgLen <= u32DataLen + g_struUartBuffer.u32RecvLen)
            {
                memcpy((g_struUartBuffer.u8UartBuffer + g_struUartBuffer.u32RecvLen), 
                    pu8Data,
                    u32MsgLen - g_struUartBuffer.u32RecvLen);
                *pu32LeftLen = u32DataLen + g_struUartBuffer.u32RecvLen - u32MsgLen; 
                g_struUartBuffer.u32Status = MSG_BUFFER_FULL;
                g_struUartBuffer.u32RecvLen = u32MsgLen;
            }
            else
            {
                memcpy((g_struUartBuffer.u8UartBuffer + g_struUartBuffer.u32RecvLen), 
                    pu8Data,
                    u32DataLen);
                g_struUartBuffer.u32Status = MSG_BUFFER_SEGMENT_HEAD;
                g_struUartBuffer.u32RecvLen += u32DataLen;
            }

        }

        return ZC_RET_OK;

    }
    return ZC_RET_ERROR;
}


/*************************************************
* Function: HF_Moudlefunc
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_Moudlefunc(u8 *pu8Data, u32 u32DataLen) 
{
    u32 u32RetVal;
    u32 u32MagicLen = ZC_MAGIC_LEN;
    u32 u32LeftLen = 0;

    if (0 == u32DataLen)
    {
        return;
    }

    u32RetVal = ZC_AssemblePkt(pu8Data, u32DataLen, &u32LeftLen);

    if (ZC_RET_ERROR == u32RetVal)
    {
        return;
    }

    if (MSG_BUFFER_FULL == g_struUartBuffer.u32Status)
    {
        ZC_RecvDataFromMoudle(g_struUartBuffer.u8UartBuffer + u32MagicLen, 
            g_struUartBuffer.u32RecvLen - u32MagicLen);
        g_struUartBuffer.u32Status = MSG_BUFFER_IDLE;
        g_struUartBuffer.u32RecvLen = 0;
    }

    /*deal left data*/
    if (u32LeftLen > 0)
    {
        ZC_Moudlefunc(pu8Data + (u32DataLen - u32LeftLen), u32LeftLen);
    }

    return; 
}

#endif
/******************************* FILE END ***********************************/




