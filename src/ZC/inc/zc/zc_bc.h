/**
******************************************************************************
* @file     zc_bc.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief   broadcast
******************************************************************************
*/

#ifndef  __ZC_BC_H__ 
#define  __ZC_BC_H__
#include <zc_common.h>
#include <zc_protocol_controller.h>

extern int g_Bcfd;
extern u8* g_pu8RemoteAddr;
extern u32 g_u32GloablIp;
extern u32 g_u32BcSleepCount;

#ifdef __cplusplus
extern "C" {
#endif
void ZC_SendBc(void);
void ZC_SendClientQueryReq(u8 *pu8Msg, u16 u16RecvLen);

    
#ifdef __cplusplus
}
#endif

#endif
/******************************* FILE END ***********************************/

