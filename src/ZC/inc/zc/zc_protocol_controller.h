/**
******************************************************************************
* @file     zc_protocol_controller.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    protocol controller
******************************************************************************
*/
#ifndef  __ZC_PROTOCOL_CONTROLLER_H__ 
#define  __ZC_PROTOCOL_CONTROLLER_H__

#include <zc_common.h>
#include <zc_module_config.h>
#include <zc_protocol_interface.h>
#include <zc_message_queue.h>
#include <zc_sec_engine.h>
#include <zc_cloud_event.h>
#include <zc_client_manager.h>
#include <zc_timer.h>
#include <zc_configuration.h>
#include <zc_bc.h>


/*PCT Main State Machine*/
#define    PCT_STATE_SLEEP                  (0)
#define    PCT_STATE_INIT                   (1)
#define    PCT_STATE_ACCESS_NET             (2)
#define    PCT_STATE_WAIT_ACCESS            (3)
#define    PCT_STATE_WAIT_ACCESSRSP         (4)
#define    PCT_STATE_WAIT_MSG4              (5)
#define    PCT_STATE_CONNECT_CLOUD          (6)
#define    PCT_STATE_WAIT_UNBIND            (7)
#define    PCT_STATE_DISCONNECT_CLOUD       (8)

#define    PCT_INVAILD_SOCKET               (0xFFFFFFFF)

#define    PCT_TIMER_RECONNECT              (0)
#define    PCT_TIMER_REACCESS               (1)
#define    PCT_TIMER_SENDMOUDLE             (2)
#define    PCT_TIMER_SENDHEART              (3)
#define    PCT_TIMER_REGISTER               (4)
#define    PCT_TIMER_SENDUBIND              (5)
#define    PCT_TIMER_REBOOT                 (6)


#define    PCT_TIMER_INTERVAL_RECONNECT     (1000)
#define    PCT_TIMER_INTERVAL_HEART         (1000 * 60)
#define    PCT_TIMER_INTERVAL_SENDMOUDLE    (5000)
#define    PCT_TIMER_INTERVAL_REGISTER      (1000)
#define    PCT_TIMER_INTERVAL_SENDUBIND     (1000)
#define    PCT_TIMER_INTERVAL_REBOOT        (1000)


#define    PCT_SEND_BC_MAX_NUM              (300)       /*5 minutes*/


#define    PCT_KEY_UNRECVED     (0)
#define    PCT_KEY_RECVED       (1)

#define    PCT_TIMER_INVAILD        (0xFF)

#define    PCT_OTA_REST_ON       (1)
#define    PCT_OTA_REST_OFF       (0)




typedef struct
{
    u32 u32Socket;
    
    u8  u8IpType;             /*IP Type,IPV6,IPV4*/
    u8  u8ConnectionType;
    u16 u16Port;
    u32 u32ConnectionTimes;
}PTC_Connection;

typedef struct
{
    u8 u8NeedPoll;
    u8 u8Pad[3];

    u8 *pu8AddrPara;
}ZC_SendParam;

typedef void (*pFunSendTcpData)(u32 u32Fd, u8 *pu8Data, u16 u16DataLen, ZC_SendParam *pstruParam);
typedef void (*pFunSendUdpData)(u32 u32Fd, u8 *pu8Data, u16 u16DataLen, ZC_SendParam *pstruParam);

typedef u32 (*pFunFirmwareUpdate)(u8 *pu8FileData, u32 u32Offset, u32 u32DataLen);
typedef u32 (*pFunFirmwareUpdateFinish)(u32 u32TotalLen);
typedef u32 (*pFunSendDataToMoudle)(u8 *pu8Data, u16 u16DataLen);
typedef u32 (*pFunRecvDataFromMoudle)(u8 *pu8Data, u16 u16DataLen);
typedef void (*pFunWriteFlashData)(u8 *pu8Data, u16 u16DataLen);
typedef void (*pFunReadFlashData)(u8 *pu8Data, u16 u16DataLen);
typedef u32 (*pFunConnectToCloud)(PTC_Connection *pstruConnection);
typedef u32 (*pFunListenClient)(PTC_Connection *pstruConnection);
typedef u32 (*pFunSetTimer)(u8 u8Type, u32 Interval, u8 *pu8Index);
typedef void (*pFunStopTimer)(u8 u8TimerIndex);
typedef void (*pFunRest)(void);
typedef void (*pFunGetMac)(u8 *pu8Mac);
typedef void (*pFunReboot)();
typedef void (*pFunPrintf)(const char *pu8format, ...);
typedef void *(*pFunMalloc)(u32 size);
typedef void (*pFunFree)(void *ptr);


typedef struct
{
    /*action function*/
    pFunConnectToCloud          pfunConnectToCloud;
    pFunListenClient            pfunListenClient;
    pFunSendTcpData             pfunSendTcpData;
    pFunSendUdpData             pfunSendUdpData;    
    pFunFirmwareUpdate          pfunUpdate;
    pFunFirmwareUpdateFinish    pfunUpdateFinish;    
    pFunSendDataToMoudle        pfunSendToMoudle;
    pFunRest                    pfunRest;
    pFunReboot                  pfunReboot;
    
    pFunSetTimer                pfunSetTimer;
    pFunStopTimer               pfunStopTimer;
    pFunWriteFlashData          pfunWriteFlash;
    pFunReadFlashData           pfunReadFlash;
    pFunGetMac                  pfunGetMac;

    pFunPrintf                  pfunPrintf;
    pFunMalloc                  pfunMalloc;
    pFunFree                    pfunFree;
}PTC_ModuleAdapter;

typedef struct
{
    u32 u32TotalLen;
    u32 u32RecvOffset;
    u8 u8Crc[2];
    u8 u8Pad[2];
}PTC_OtaInfo;

typedef struct
{
    u8   u8MainState;                         /*State*/
    u8   u8keyRecv;
    u8   u8ReconnectTimer;
    u8   u8AccessTimer;
    u8   u8SendUnbindTimer;
    
    u8   u8RebootTimer;   
    u8   u8HeartTimer;
    u8   u8SendMoudleTimer;
    u8   u8RegisterTimer;
    
    u8   u8SendMoudleMsgId;
    
    PTC_Connection struCloudConnection;
    PTC_Connection struClientConnection;

    u8   u8SessionKey[ZC_HS_SESSION_KEY_LEN];
    u8   IvSend[16];
    u8   IvRecv[16];
    u8   RandMsg[ZC_HS_MSG_LEN];

    u16   u16SendBcNum;
    u8    u8Pad[2];
    PTC_ModuleAdapter *pstruMoudleFun;      /*Communication With Cloud*/
    PTC_OtaInfo struOtaInfo;
}PTC_ProtocolCon;



extern PTC_ProtocolCon  g_struProtocolController;
extern MSG_Buffer g_struRecvBuffer;
extern MSG_Buffer g_struClientBuffer;

extern MSG_Queue  g_struRecvQueue;
extern MSG_Buffer g_struSendBuffer[MSG_BUFFER_SEND_MAX_NUM];
extern MSG_Queue  g_struSendQueue;

extern u8 g_u8MsgBuildBuffer[MSG_BULID_BUFFER_MAXLEN];

extern u16 g_u16TcpMss;
extern ZC_ClientInfo g_struClientInfo;

#ifdef __cplusplus
extern "C" {
#endif
u32 PCT_CheckCrc(u8 *pu8Crc, u8 *pu8Data, u16 u16Len);
void PCT_SendNotifyMsg(u8 u8NotifyCode);
void PCT_SendHeartMsg(void);
void PCT_Init(PTC_ModuleAdapter *pstruAdapter);
void PCT_SendEmptyMsg(u8 u8MsgId, u8 u8SecType);
void PCT_SendErrorMsg(u8 u8MsgId, u8 *pu8Error, u16 u16ErrorLen);
void PCT_SendCloudAccessMsg1(PTC_ProtocolCon *pstruContoller);
void PCT_SendCloudAccessMsg3(PTC_ProtocolCon *pstruContoller);
void PCT_DisConnectCloud(PTC_ProtocolCon *pstruContoller);
void PCT_ConnectCloud(PTC_ProtocolCon *pstruContoller);
void PCT_ReconnectCloud(PTC_ProtocolCon *pstruContoller, u32 u32ReConnectTimer);
void PCT_SendMoudleTimeout(PTC_ProtocolCon *pstruProtocolController);
void PCT_HandleMoudleEvent(u8 *pu8Msg, u16 u16DataLen);
void PCT_RecvAccessMsg2(PTC_ProtocolCon *pstruContoller);
void PCT_RecvAccessMsg4(PTC_ProtocolCon *pstruContoller);
void PCT_HandleEvent(PTC_ProtocolCon *pstruContoller);
void PCT_Run(void);
void PCT_WakeUp(void);
void PCT_Sleep(void);
u32  PCT_SendMsgToCloud(ZC_SecHead *pstruSecHead, u8 *pu8PlainData);
void PCT_SendAckToCloud(u8 u8MsgId);
void PCT_ModuleOtaFileBeginMsg(PTC_ProtocolCon *pstruContoller, u8 *pu8Msg);
void PCT_ModuleOtaFileChunkMsg(PTC_ProtocolCon *pstruContoller, ZC_MessageHead *pstruMsg, u8 *pu8Msg);
void PCT_ModuleOtaFileEndMsg(PTC_ProtocolCon *pstruContoller, u8 *pu8Msg);
void PCT_HandleOtaBeginMsg(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer);
void PCT_HandleOtaFileBeginMsg(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer);
void PCT_HandleOtaFileChunkMsg(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer);
void PCT_HandleOtaFileEndMsg(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer);
void PCT_HandleOtaEndMsg(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer);
void PCT_HandleMoudleMsg(PTC_ProtocolCon *pstruContoller, MSG_Buffer *pstruBuffer);
void PCT_SendUnbindMsg(void);

#ifdef __cplusplus
}
#endif
#endif
/******************************* FILE END ***********************************/


