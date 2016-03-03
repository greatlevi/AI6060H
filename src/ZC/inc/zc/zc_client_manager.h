/**
******************************************************************************
* @file     zc_client_manager.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    
******************************************************************************
*/

#ifndef  __ZC_CLIENT_MANAGER_H__ 
#define  __ZC_CLIENT_MANAGER_H__

#include <zc_common.h>

#define    ZC_CLIENT_STATUS_INIT       (0)
#define    ZC_CLIENT_STATUS_WAKEUP     (1)
#define    ZC_CLIENT_STATUS_IDLE       (2)
#define    ZC_CLIENT_STATUS_BUSY       (3)

#define    ZC_CLIENT_INVAILD_FLAG      (0)
#define    ZC_CLIENT_VAILD_FLAG        (1)

#define    ZC_MAX_CLIENT_NUM           (4)


typedef struct
{
    u8  u8ClientStates;
    u8  u8Pad[3];

    u8  u32ClientBusyId;
    
    u32 u32ClientVaildFlag[ZC_MAX_CLIENT_NUM];
    u32 u32ClientFd[ZC_MAX_CLIENT_NUM];
}ZC_ClientInfo;


#ifdef __cplusplus
extern "C" {
#endif

void ZC_ClientDisconnect(u32 u32ClientId);
u32 ZC_ClientConnect(u32 u32ClientId); 
void ZC_ClientInit(void);
u32 ZC_CheckClientIdle(u32 u32Fd);
void ZC_SetClientBusy(u32 u32Clientfd);
void ZC_SetClientFree(u32 u32Clientfd);
void ZC_StartClientListen(void);
void ZC_ClientWakeUp(void);
void ZC_ClientSleep(void);
void ZC_RecvDataFromClient(u32 ClientId, u8 *pu8Data, u32 u32DataLen);

#ifdef __cplusplus
}
#endif
#endif
/******************************* FILE END ***********************************/

