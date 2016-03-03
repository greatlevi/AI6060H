/**
******************************************************************************
* @file     zc_messgage_queue.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Msg Queue
******************************************************************************
*/

#include <zc_message_queue.h>
#include <zc_protocol_controller.h>




/*************************************************
* Function: MSG_Init()
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MSG_Init()
{
    u32 u32Index;
    MSG_InitQueue(&g_struRecvQueue);
    MSG_InitQueue(&g_struSendQueue);


    g_struRecvBuffer.u32Len = 0;
    g_struRecvBuffer.u8Status = MSG_BUFFER_IDLE;    

    for (u32Index = 0; u32Index < MSG_BUFFER_SEND_MAX_NUM; u32Index++)
    {
        g_struSendBuffer[u32Index].u32Len = 0;
        g_struSendBuffer[u32Index].u8Status = MSG_BUFFER_IDLE;
    }
    
    g_struClientBuffer.u32Len = 0;    
    g_struClientBuffer.u8Status = MSG_BUFFER_IDLE;    
}

/*************************************************
* Function: MSG_InitQueue
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MSG_InitQueue(MSG_Queue *pstruMsgQue)   
{
    u32 u32Index;
    MSG_QueNode *pstrNode = NULL;
    for (u32Index = 0; u32Index < MSG_QUEUE_MAXLEN + 1; u32Index++)
    {
        pstruMsgQue->struQueue[u32Index].pstruNext = pstrNode;
        pstruMsgQue->struQueue[u32Index].pu8Msg = NULL;
        pstrNode = &pstruMsgQue->struQueue[u32Index];
    }
    /*Init a loop queue*/
    pstruMsgQue->struQueue[0].pstruNext = &pstruMsgQue->struQueue[MSG_QUEUE_MAXLEN];
    
    pstruMsgQue->pstruHead = &pstruMsgQue->struQueue[0];
    pstruMsgQue->pstruTail = pstruMsgQue->pstruHead;
    return;
}

/*************************************************
* Function: MSG_PushMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MSG_PushMsg(MSG_Queue *pstruMsgQueue, u8 *pu8Msg)
{
    /*tail next == head, queue is full*/
    if (pstruMsgQueue->pstruTail->pstruNext == pstruMsgQueue->pstruHead)
    {
        return ZC_RET_ERROR;
    }
    pstruMsgQueue->pstruTail->pu8Msg = pu8Msg;
    pstruMsgQueue->pstruTail = pstruMsgQueue->pstruTail->pstruNext;
    return ZC_RET_OK;
}

/*************************************************
* Function: MSG_PopMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u8* MSG_PopMsg(MSG_Queue *pstruMsgQueue)
{
    u8 *pu8Msg = NULL;
    
    /*head == tail, no msg*/
    if (pstruMsgQueue->pstruHead == pstruMsgQueue->pstruTail)
    {
        return pu8Msg;
    }
    
    pu8Msg = pstruMsgQueue->pstruHead->pu8Msg;
    pstruMsgQueue->pstruHead->pu8Msg = NULL;
    pstruMsgQueue->pstruHead = pstruMsgQueue->pstruHead->pstruNext;
    
    return pu8Msg;
}

/*************************************************
* Function: MSG_RecvDataFromCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MSG_RecvData(MSG_Buffer *pstruRecvBuffer, u8 *pu8Data, u32 u32DataLen)
{
    ZC_SecHead *pstruMsg;
    u32 u32MsgLen;
    
    if (MSG_BUFFER_FULL == pstruRecvBuffer->u8Status)
    {
        return ZC_RET_ERROR;
    }
    if (MSG_BUFFER_IDLE == pstruRecvBuffer->u8Status)
    {

        if (u32DataLen < sizeof(ZC_SecHead))
        {
            memcpy(pstruRecvBuffer->u8MsgBuffer, pu8Data, u32DataLen);
            pstruRecvBuffer->u8Status = MSG_BUFFER_SEGMENT_NOHEAD;
            pstruRecvBuffer->u32Len = u32DataLen;
        }
        else
        {
            pstruMsg = (ZC_SecHead *)(pu8Data);
            u32MsgLen =  ZC_HTONS(pstruMsg->u16TotalMsg) + sizeof(ZC_SecHead);

            if (u32MsgLen > MSG_BUFFER_MAXLEN)
            {
                pstruRecvBuffer->u8Status = MSG_BUFFER_IDLE;
                pstruRecvBuffer->u32Len = 0;
                return ZC_RET_ERROR;
            }

            if (u32MsgLen <= u32DataLen)
            {
                memcpy(pstruRecvBuffer->u8MsgBuffer, pu8Data, u32MsgLen);
                pstruRecvBuffer->u8Status = MSG_BUFFER_FULL;
                pstruRecvBuffer->u32Len = u32MsgLen;
            }
            else
            {
                memcpy(pstruRecvBuffer->u8MsgBuffer, pu8Data, u32DataLen);
                pstruRecvBuffer->u8Status = MSG_BUFFER_SEGMENT_HEAD;
                pstruRecvBuffer->u32Len = u32DataLen;
            }

        }

        return ZC_RET_OK;

    }

    if (MSG_BUFFER_SEGMENT_HEAD == pstruRecvBuffer->u8Status)
    {
        pstruMsg = (ZC_SecHead *)(pstruRecvBuffer->u8MsgBuffer);
        u32MsgLen = ZC_HTONS(pstruMsg->u16TotalMsg) + sizeof(ZC_SecHead);

        if (u32MsgLen <= u32DataLen + pstruRecvBuffer->u32Len)
        {
            memcpy((pstruRecvBuffer->u8MsgBuffer + pstruRecvBuffer->u32Len), 
                pu8Data, 
                (u32MsgLen - pstruRecvBuffer->u32Len));

            pstruRecvBuffer->u8Status = MSG_BUFFER_FULL;
            pstruRecvBuffer->u32Len = u32MsgLen;
        }
        else
        {
            memcpy((pstruRecvBuffer->u8MsgBuffer + pstruRecvBuffer->u32Len), 
                pu8Data, 
                u32DataLen);
            pstruRecvBuffer->u32Len += u32DataLen;
            pstruRecvBuffer->u8Status = MSG_BUFFER_SEGMENT_HEAD;
        }

        return ZC_RET_OK;
    }

    if (MSG_BUFFER_SEGMENT_NOHEAD == pstruRecvBuffer->u8Status)
    {
        if ((pstruRecvBuffer->u32Len + u32DataLen) < sizeof(ZC_SecHead))
        {
            memcpy((pstruRecvBuffer->u8MsgBuffer + pstruRecvBuffer->u32Len), 
                pu8Data,
                u32DataLen);
            pstruRecvBuffer->u32Len += u32DataLen;
            pstruRecvBuffer->u8Status = MSG_BUFFER_SEGMENT_NOHEAD;
        }
        else
        {
            memcpy((pstruRecvBuffer->u8MsgBuffer + pstruRecvBuffer->u32Len), 
                pu8Data,
                (sizeof(ZC_SecHead) - pstruRecvBuffer->u32Len));

            pstruMsg = (ZC_SecHead *)(pstruRecvBuffer->u8MsgBuffer);
            u32MsgLen = ZC_HTONS(pstruMsg->u16TotalMsg) + sizeof(ZC_SecHead);

            if (u32MsgLen > MSG_BUFFER_MAXLEN)
            {
                pstruRecvBuffer->u8Status = MSG_BUFFER_IDLE;
                pstruRecvBuffer->u32Len = 0;                
                return ZC_RET_ERROR;
            }

            if (u32MsgLen <= u32DataLen + pstruRecvBuffer->u32Len)
            {
                memcpy((pstruRecvBuffer->u8MsgBuffer + pstruRecvBuffer->u32Len), 
                    pu8Data,
                    u32MsgLen - pstruRecvBuffer->u32Len);
                pstruRecvBuffer->u8Status = MSG_BUFFER_FULL;
                pstruRecvBuffer->u32Len = u32MsgLen;

            }
            else
            {
                memcpy((pstruRecvBuffer->u8MsgBuffer + pstruRecvBuffer->u32Len), 
                    pu8Data,
                    u32DataLen);
                pstruRecvBuffer->u8Status = MSG_BUFFER_SEGMENT_HEAD;
                pstruRecvBuffer->u32Len += u32DataLen;
            }

        }

        return ZC_RET_OK;

    }
    
    return ZC_RET_ERROR;
    

}
/*************************************************
* Function: MSG_RecvDataFromCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MSG_RecvDataFromCloud(u8 *pu8Data, u32 u32DataLen)
{
    u32 u32RetVal;
    u16 u16PlainLen;
    u32RetVal = MSG_RecvData(&g_struRecvBuffer, pu8Data, u32DataLen);

    if (ZC_RET_OK == u32RetVal)
    {
        if (MSG_BUFFER_FULL == g_struRecvBuffer.u8Status)
        {
            u32RetVal = SEC_Decrypt((ZC_SecHead*)g_struRecvBuffer.u8MsgBuffer, 
                g_struRecvBuffer.u8MsgBuffer + sizeof(ZC_SecHead), g_u8MsgBuildBuffer, &u16PlainLen);

            /*copy data*/
            memcpy(g_struRecvBuffer.u8MsgBuffer, g_u8MsgBuildBuffer, u16PlainLen);

            g_struRecvBuffer.u32Len = u16PlainLen;
            if (ZC_RET_OK == u32RetVal)
            {
                u32RetVal = MSG_PushMsg(&g_struRecvQueue, (u8*)&g_struRecvBuffer);
            }
        }
    }
    
    return;
}

/*************************************************
* Function: MSG_SendDataToCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MSG_SendDataToCloud(u8 *pu8Connection)
{
    PTC_Connection *pstruConnection = (PTC_Connection *)pu8Connection;
    MSG_Buffer *pstruBuf = NULL;
    ZC_SendParam struParam;

    u16 u16DataLen; 
    pstruBuf = (MSG_Buffer *)MSG_PopMsg(&g_struSendQueue); 
    
    if (NULL == pstruBuf)
    {
        return;
    }
    
    u16DataLen = pstruBuf->u32Len; 
    struParam.u8NeedPoll = 0;
    g_struProtocolController.pstruMoudleFun->pfunSendTcpData(pstruConnection->u32Socket, pstruBuf->u8MsgBuffer, u16DataLen, &struParam);
 
    ZC_Printf("send data len = %d\n", u16DataLen);
    pstruBuf->u8Status = MSG_BUFFER_IDLE;
    pstruBuf->u32Len = 0;
    return;
}


/******************************* FILE END ***********************************/

