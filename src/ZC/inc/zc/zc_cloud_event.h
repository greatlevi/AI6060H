/**
******************************************************************************
* @file     zc_cloud_event.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    dealevent
******************************************************************************
*/
#ifndef  __ZC_CLOUD_EVENT_H__ 
#define  __ZC_CLOUD_EVENT_H__

#include <zc_common.h>
#include <zc_protocol_controller.h>

#define      ZC_VERSION       (1)

typedef struct 
{
    ZC_TransportInfo *pstruTransportInfo;
    ZC_SsessionInfo *pstruSsession;
    
}ZC_OptList;


#ifdef __cplusplus
extern "C" {
#endif


u32  EVENT_BuildMsg(u8 u8MsgCode, u8 u8MsgId, u8 *pu8Msg, u16 *pu16Len, 
    u8 *pu8Payload, u16 u16PayloadLen);
u32  EVENT_BuildEmptyMsg(u8 u8MsgId, u8 *pu8Msg, u16 *pu16Len);
u32  EVENT_BuildHeartMsg(u8 *pu8Msg, u16 *pu16Len);
u32  EVENT_BuildBcMsg(u8 *pu8Msg, u16 *pu16Len);
void EVENT_ParseOption(ZC_MessageHead *pstruMsg, ZC_OptList *pstruOptList, u16 *pu16OptLen);
void EVENT_BuildOption(ZC_OptList *pstruOptList, u8 *pu8OptNum, u8 *pu8Buffer, u16 *pu16Len);

#ifdef __cplusplus
}
#endif

#endif
/******************************* FILE END ***********************************/

