/**
******************************************************************************
* @file     zc_timer.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Security module
******************************************************************************
*/

#ifndef  __ZC_TIEMR_H__ 
#define  __ZC_TIEMR_H__
#include <zc_common.h>

#define ZC_TIMER_MAX_NUM        (10)

#define ZC_TIMER_STATUS_IDLE    (0)
#define ZC_TIMER_STATUS_USED    (1)


typedef struct
{
    u8 u8Status;
    u8 u8Type;
    u8 u8Pad[2];

    u8 *pu8Timer;
}ZC_Timer;
#ifdef __cplusplus
extern "C" {
#endif
void TIMER_Init(void);
u32 TIMER_FindIdleTimer(u8 *pu8TimerIndex);
void TIMER_AllocateTimer(u8 u8Type, u8 u8TimerIndex, u8 *pu8Timer);
void TIMER_GetTimerStatus(u8 u8TimerIndex, u8 *pu8Status);
void TIMER_TimeoutAction(u8 u8TimerIndex);
void TIMER_StopTimer(u8 u8TimerIndex);

#ifdef __cplusplus
}
#endif

#endif
/******************************* FILE END ***********************************/

