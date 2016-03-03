/**
******************************************************************************
* @file     zc_module_interface.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Security module
******************************************************************************
*/

#ifndef  __ZC_MODULE_INTERFACE_H__ 
#define  __ZC_MODULE_INTERFACE_H__
#include <zc_common.h>

/*big endian*/
#define ZC_MODULE_TRACE_FLAG  (0x01020304)       
#define ZC_MODULE_DATA_FLAG   (0x02030405)       

#define ZC_MAGIC_LEN          (4)  
#define ZC_MAX_UARTBUF_LEN    (1000)

typedef struct
{
    u32 u32Status;
    u32 u32RecvLen;
    u8  u8UartBuffer[ZC_MAX_UARTBUF_LEN];
}ZC_UartBuffer;


#ifdef __cplusplus
extern "C" {
#endif

u32 ZC_RecvDataFromMoudle(u8 *pu8Data, u16 u16DataLen);
u32 ZC_DealSessionOpt(ZC_MessageHead *pstruMsg, ZC_OptList *pstruOptList, u8 *pu8PayLoad);
u32 ZC_AssemblePkt(u8 *pu8Data, u32 u32DataLen, u32 *pu32LeftLen);
void ZC_Moudlefunc(u8 *pu8Data, u32 u32DataLen);
u32 ZC_DealExtCode( u8 *pu8Data);
#ifdef __cplusplus
}
#endif

#endif
/******************************* FILE END ***********************************/

