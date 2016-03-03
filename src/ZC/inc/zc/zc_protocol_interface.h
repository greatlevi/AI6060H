/**
******************************************************************************
* @file     zc_protocol_interface.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Security module
******************************************************************************
*/
#ifndef  __ZC_PROTOCOL_INTERFACE_H__ 
#define  __ZC_PROTOCOL_INTERFACE_H__
#include <zc_common.h>

#define ZC_HS_MSG_LEN                       (32)
#define ZC_HS_DEVICE_ID_LEN                 (16)
#define ZC_HS_SESSION_KEY_LEN               (16)
#define ZC_HS_SESSION_IV_LEN                (16)
#define ZC_MODULE_KEY_LEN                   (112)
#define ZC_DOMAIN_LEN                       (8)
#define ZC_EQVERSION_LEN                    (4)


#define ZC_SEC_ALG_NONE                     (0) 
#define ZC_SEC_ALG_RSA                      (1) 
#define ZC_SEC_ALG_AES                      (2) 


#define ZC_OTA_MAX_CHUNK_LEN                (384)

#define ZC_SSID_MAX_LEN                     (32)

#define ZC_PASSWORD_MAX_LEN                 (64)

#define ZC_CLOUD_ADDR_MAX_LEN               (24)
#define ZC_CLOUD_KEY_MAX_LEN                (36)

#define ZC_SERVER_ADDR_MAX_LEN               (4)
#define ZC_SERVER_PORT_MAX_LEN               (2)

#define ZC_SERVER_MAC_LEN                    (12)
/****************************************************************************************
*message format: 
*|ZC_SecHead||ZC_MessageHead||ZC_MessageOptHead||ZC_MessageOption|.......|ZC_MessagePayload|
*****************************************************************************************/

/*Security Head*/
typedef struct
{
    u16 u16TotalMsg;
    u8  u8SecType;
    u8  u8Resver;
}ZC_SecHead;

/*ZCloud Message*/
typedef struct
{
    u8  Version;
    u8  MsgId;
    u8  MsgCode;
    u8  OptNum;        
    
    u16 Payloadlen;     //msg payload len + opt len + opt head len
    u8  TotalMsgCrc[2];
    
}ZC_MessageHead;

/*ACloud Ext Message*/
typedef struct
{
    u8  ExtMsgCode;
    u8  Pad[3];            
}ZC_ExtMessageHead;

/*ZCloud Option Head*/
typedef struct
{
    u16 OptCode;
    u16 OptLen;
}ZC_MessageOptHead;

/*******************************Message and Option opcode***********************************/

/*ZCloud Message code*/
typedef enum 
{
    ZC_CODE_EQ_BEGIN = 0,
    ZC_CODE_EQ_DONE = 1,
    ZC_CODE_WIFI_CONNECTED = 2,
    ZC_CODE_WIFI_DISCONNECTED = 3,
    ZC_CODE_CLOUD_CONNECTED = 4,
    ZC_CODE_CLOUD_DISCONNECTED = 5,
    ZC_CODE_CONFIG = 6,
    ZC_CODE_REGSITER = 7,                 
    ZC_CODE_REST = 8,                

    /*HandShake Code*/
    ZC_CODE_HANDSHAKE_1 = 9,
    ZC_CODE_HANDSHAKE_2 = 10,
    ZC_CODE_HANDSHAKE_3 = 11,
    ZC_CODE_HANDSHAKE_4 = 12,

    /*Respone Code*/
    ZC_CODE_HEARTBEAT = 13,   /*no payload*/
    ZC_CODE_EMPTY = 14,		 /*no payload, send by moudle when can recv another msg*/
    ZC_CODE_ACK = 15,         /*user define payload*/
    ZC_CODE_ERR = 16,         /*use ZC_ErrorMsg*/

    /*OTA Code*/
    ZC_CODE_OTA_BEGIN = 17,
    ZC_CODE_OTA_FILE_BEGIN = 18,      /*file name, len, version*/
    ZC_CODE_OTA_FILE_CHUNK = 19,
    ZC_CODE_OTA_FILE_END = 20,
    ZC_CODE_OTA_END = 21,
    
    /*OTA Wifi Moudle Code*/
    ZC_CODE_ZOTA_BEGIN = 22,
    ZC_CODE_ZOTA_FILE_BEGIN = 23,      /*file name, len, version*/
    ZC_CODE_ZOTA_FILE_CHUNK = 24,
    ZC_CODE_ZOTA_FILE_END = 25,
    ZC_CODE_ZOTA_END = 26,
    ZC_CODE_BC_INFO = 27,
    ZC_CODE_CLIENT_ACCESS_REQ = 28,
    ZC_CODE_CLIENT_ACCESS_RSP = 29,
    
    ZC_CODE_CLIENT_QUERY_REQ = 30,
    ZC_CODE_CLIENT_QUERY_RSP = 31,

    ZC_CODE_TOKEN_SET = 32,
    ZC_CODE_ACCESS_POINT_RSP = 33,
    ZC_CODE_RESET_NETWORK = 34,
	
    ZC_CODE_OTA_CONFIRM = 35,
    ZC_CODE_UNBIND = 36,

    ZC_CODE_KLV_RSP = 60,
    ZC_CODE_JSON_RSP = 61,
    ZC_CODE_EXT = 63, 
    ZC_CODE_EVENT_BASE = 64,
    ZC_CODE_REPORT_BASE = 200
}ZC_MsgCode;

/*ZCloud Message Ext code*/
typedef enum 
{
    ZC_CODE_EXT_REGSITER = 0,
    ZC_CODE_EXT_REBOOT = 1,
}ZC_Ext_MsgCode;

typedef enum 
{
    ZC_OPT_TRANSPORT = 0,
    ZC_OPT_SSESSION = 1,
}ZC_OptCode;

/*******************************Message definition***********************************/

/*Error Msg*/
typedef struct{
    u8 ErrorCode;
    //u8 ErrorMsg[0];
}ZC_ErrorMsg;


/*first handshake msg, send by moudle to cloud
encrypt by cloud public key*/
typedef struct
{
    u8 RandMsg[ZC_HS_MSG_LEN];
    u8 DeviceId[ZC_HS_DEVICE_ID_LEN];
    u8 u8Domain[ZC_DOMAIN_LEN];
}ZC_HandShakeMsg1;

/*Second handshake msg, send by cloud to moudle, 
encrypt by moudle public key*/
typedef struct
{
    u8 RandMsg[ZC_HS_MSG_LEN];
    u8 SessionKey[ZC_HS_SESSION_KEY_LEN];
}ZC_HandShakeMsg2;

/*3rd handshake msg, send by moudle to cloud,
encrypt by session key*/
typedef struct
{
    u8 RandMsg[ZC_HS_MSG_LEN];
    u8 u8EqVersion[ZC_EQVERSION_LEN];
    u8 u8WifiVerSion[ZC_EQVERSION_LEN];
    u8 u8WifiType;    
    u8 u8Pad[3];
}ZC_HandShakeMsg3;


/*4th handshake msg, send by cloud to moudle,
encrypt by session key*/
typedef struct
{
    u8 RandMsg[ZC_HS_MSG_LEN];
}ZC_HandShakeMsg4;

typedef struct
{
    u8 TokenKey[ZC_HS_SESSION_KEY_LEN];
}ZC_TokenSetReq;

typedef struct
{
    u8 RandMsg[ZC_HS_MSG_LEN];
    u32 u32ServerAddr;
    u16 u16ServerPort;
    u16 u16Pad;
}ZC_AccessPoint;


/*msg code: ZC_CODE_DESCRIBE*/
typedef struct 
{
    u8  u8EqVersion[ZC_EQVERSION_LEN];      //eq vision
    u8  u8ModuleKey[ZC_MODULE_KEY_LEN];
    u8  u8Domain[ZC_DOMAIN_LEN];
    u8  u8DeviceId[ZC_HS_DEVICE_ID_LEN];
}ZC_RegisterReq;

/*msg code: ZC_CODE_EXT_REGSITER*/
typedef struct 
{
    ZC_ExtMessageHead struExtMessageHead;
    ZC_RegisterReq struRegReq;
}ZC_ExtRegisterReq;

/*msg code: ZC_CODE_OTA_BEGIN*/
typedef struct
{
    u8 u8FileNum;
    u8 u8Pad[3];
    //u8 u8FileType[0];
}ZC_OtaBeginReq;

/*msg code: ZC_CODE_OTA_FILE_BEGIN*/
typedef struct
{
    u8 u8FileType;
    u8 u8FileVersion;
    u8  u8TotalFileCrc[2];
    
    u32 u32FileTotalLen;
}ZC_OtaFileBeginReq;

/*msg code: ZC_CODE_OTA_FILE_CHUNK, max data len is ZC_OTA_MAX_CHUNK_LEN*/
typedef struct
{
    u32 u32Offset;
}ZC_OtaFileChunkReq;

/*BC info£¬ send after connect with cloud£¬ in PCT_SEND_BC_MAX_NUM*/
typedef struct
{
    u8 u8Domain[ZC_DOMAIN_LEN];
    u8 DeviceId[ZC_HS_DEVICE_ID_LEN];
    u8 RandMsg[ZC_HS_MSG_LEN];
}ZC_BroadCastInfo;

/*BC info£¬ send after connect with cloud£¬ in PCT_SEND_BC_MAX_NUM*/
typedef struct
{
    u8 addr[4];
    u8 DeviceId[ZC_HS_DEVICE_ID_LEN];
    u8 u8Domain[ZC_DOMAIN_LEN];
}ZC_ClientQueryRsp;

/*find eq*/
typedef struct
{
    u8  u8Domain[ZC_DOMAIN_LEN];
}ZC_ClientQueryReq;


/*ZC_CODE_CLIENT_ACCESS_REQ*/
typedef struct
{
    u8 u8ClientNum;
    u8 u8Pad[3];
    //u8 DeviceId[0];
}ZC_ClientAccessInfo;

typedef struct
{
    u32 u32TraceSwitch;     //Trace data switch, 1:open, 0:close,default 0
    u32 u32SecSwitch;       //Sec data switch, 1:open, 0:close, 2:close RSA, default 1
    u32 u32WifiConfig;      //Use Config SSID,password,1:open, 0:close, default 0
    u32 u32ServerAddrConfig;  //connect with test url,0:dns, 1:use u32IpAddr, default 0
    u8  u8Ssid[ZC_SSID_MAX_LEN];
    u8  u8Password[ZC_PASSWORD_MAX_LEN];
    u32 u32IpAddr;
    u8  u8CloudAddr[ZC_CLOUD_ADDR_MAX_LEN];
    u8  u8CloudKey[ZC_CLOUD_KEY_MAX_LEN];
}ZC_Configuration;

/******************************* Option definition***********************************/

/*ZC_OPT_TRANSPORT*/
typedef struct
{
    u8 DomainId[ZC_DOMAIN_LEN];
    u8 DeviceId[ZC_HS_DEVICE_ID_LEN];
}ZC_TransportInfo;

/*ZC_OPT_APPDIRECT*/
typedef struct{
    u32 u32SsessionId;
}ZC_SsessionInfo;


#endif
/******************************* FILE END ***********************************/

