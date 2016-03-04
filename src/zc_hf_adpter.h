/**
******************************************************************************
* @file     zc_hf_adpter.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    HANDSHAKE
******************************************************************************
*/

#ifndef  __ZC_HF_ADPTER_H__ 
#define  __ZC_HF_ADPTER_H__

#include <zc_common.h>
#include <zc_protocol_controller.h>
#include <zc_module_interface.h>
//#include "timer_api.h"
//#include "osdep_api.h"


typedef struct 
{
    u32 u32Interval;  
    u8 u8TimerIndex;
    u8 u8ValidFlag;

    //RTL_TIMER *pstruHandle;
    //u8         u8TimerIndex;
    //gtimer_t struHandle;
    //hftimer_handle_t struHandle;
}HF_TimerInfo;


#define HF_MAX_SOCKET_LEN    (1000)

#define FLASH_ADDRESS      (0x307c000)
#define IMAGE_2_BASE	     0x0000B000

#ifdef __cplusplus
extern "C" {
#endif
void HF_Init(void);
void HF_WakeUp(void);
void HF_Sleep(void);
void HF_ReadDataFromFlash(u8 *pu8Data, u16 u16Len);
void HF_WriteDataToFlash(u8 *pu8Data, u16 u16Len);
void HF_Reboot(void);
void HF_BcInit(void);
void HF_Run(void);
void HF_TimerExpired(void);
void HF_Cloudfunc(void);


#ifdef __cplusplus
}
#endif
#endif
/******************************* FILE END ***********************************/

