/**
******************************************************************************
* @file     zc_timer.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    timer
******************************************************************************
*/
#include <zc_timer.h>
#include <zc_protocol_controller.h>
ZC_Timer g_struTimer[ZC_TIMER_MAX_NUM];

/*************************************************
* Function: TIMER_Init
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
void TIMER_Init()
{
    u32 u32Index;
    for (u32Index = 0; u32Index < ZC_TIMER_MAX_NUM; u32Index++)
    {
        g_struTimer[u32Index].u8Status = ZC_TIMER_STATUS_IDLE;
    }
}


/*************************************************
* Function: TIMER_FindIdleTimer
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
u32 TIMER_FindIdleTimer(u8 *pu8TimerIndex)
{
    u8 u8Index;
    for (u8Index = 0; u8Index < ZC_TIMER_MAX_NUM; u8Index++)
    {
        if (ZC_TIMER_STATUS_IDLE == g_struTimer[u8Index].u8Status)
        {
            *pu8TimerIndex = u8Index;
            return ZC_RET_OK;
        }

    }
    return ZC_RET_ERROR;
}

/*************************************************
* Function: TIMER_AllocateTimer
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
void TIMER_AllocateTimer(u8 u8Type, u8 u8TimerIndex, u8 *pu8Timer)
{
    g_struTimer[u8TimerIndex].u8Status = ZC_TIMER_STATUS_USED;
    g_struTimer[u8TimerIndex].u8Type = u8Type;
    g_struTimer[u8TimerIndex].pu8Timer = pu8Timer;
    return;
}
/*************************************************
* Function: TIMER_GetTimerStatus
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
void TIMER_GetTimerStatus(u8 u8TimerIndex, u8 *pu8Status)
{
    *pu8Status = g_struTimer[u8TimerIndex].u8Status;
}
/*************************************************
* Function: TIMER_TimeoutAction
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
void TIMER_TimeoutAction(u8 u8TimerIndex)
{
    switch (g_struTimer[u8TimerIndex].u8Type)
    {
        case PCT_TIMER_RECONNECT:
            ZC_Printf("PCT_TIMER_RECONNECT Time out\n");
            g_struProtocolController.u8ReconnectTimer = PCT_TIMER_INVAILD;
            g_struProtocolController.u8MainState = PCT_STATE_ACCESS_NET;
            //PCT_ConnectCloud(&g_struProtocolController);
            break;
        case PCT_TIMER_REACCESS:
            ZC_Printf("Access Time out\n");
            PCT_DisConnectCloud(&g_struProtocolController);
            break;
        case PCT_TIMER_SENDMOUDLE:            
            PCT_SendMoudleTimeout(&g_struProtocolController);
            break;
        case PCT_TIMER_SENDHEART:            
            PCT_SendHeartMsg();
            break;
        case PCT_TIMER_REGISTER: 
            g_struProtocolController.pstruMoudleFun->pfunSetTimer(PCT_TIMER_REGISTER, 
                PCT_TIMER_INTERVAL_REGISTER, &g_struProtocolController.u8RegisterTimer);
            PCT_SendNotifyMsg(ZC_CODE_WIFI_CONNECTED);
            break;
        case PCT_TIMER_SENDUBIND:
            PCT_SendUnbindMsg();
            break;
         case PCT_TIMER_REBOOT:
            g_struProtocolController.pstruMoudleFun->pfunReboot();
            break;
    }
}

/*************************************************
* Function: TIMER_StopTimer
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
void TIMER_StopTimer(u8 u8TimerIndex)
{
    g_struTimer[u8TimerIndex].u8Status = ZC_TIMER_STATUS_IDLE;
    g_struProtocolController.pstruMoudleFun->pfunStopTimer(u8TimerIndex);
}


/******************************* FILE END ***********************************/

