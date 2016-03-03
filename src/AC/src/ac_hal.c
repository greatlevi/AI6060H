/**
******************************************************************************
* @file     ac_app_main.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief   
******************************************************************************
*/

#include <zc_common.h>
#include <zc_protocol_interface.h>
#include <zc_protocol_controller.h>
#include <ac_api.h>
#include <ac_hal.h>
#include <ac_cfg.h>
#include <zc_configuration.h>
#include <zc_module_interface.h>
u32 g_u32CloudStatus = CLOUDDISCONNECT;
typedef struct tag_STRU_LED_ONOFF
{		
    u8	     u8LedOnOff ; // 0:关，1：开，2：获取当前开关状态
    u8	     u8Pad[3];		 
}STRU_LED_ONOFF;

u32 g_u32WifiPowerStatus = WIFIPOWEROFF;

u8 g_u8DevMsgBuildBuffer[512];
u8  g_u8EqVersion[]={0,0,0,0};      
u8  g_u8ModuleKey[ZC_MODULE_KEY_LEN] = DEFAULT_IOT_PRIVATE_KEY;
u64  g_u64Domain = ((((u64)((SUB_DOMAIN_ID & 0xff00) >> 8)) << 48) + (((u64)(SUB_DOMAIN_ID & 0xff)) << 56) + (((u64)MAJOR_DOMAIN_ID & 0xff) << 40) + ((((u64)MAJOR_DOMAIN_ID & 0xff00) >> 8) << 32)
	+ ((((u64)MAJOR_DOMAIN_ID & 0xff0000) >> 16) << 24)
	+ ((((u64)MAJOR_DOMAIN_ID & 0xff000000) >> 24) << 16)
	+ ((((u64)MAJOR_DOMAIN_ID & 0xff00000000) >> 32) << 8)
	+ ((((u64)MAJOR_DOMAIN_ID & 0xff0000000000) >> 40) << 0));
u8  g_u8DeviceId[ZC_HS_DEVICE_ID_LEN] = DEVICE_ID;
#ifdef TEST_ADDR	
#define CLOUD_ADDR "test.ablecloud.cn"
#else
#define CLOUD_ADDR "device.ablecloud.cn"
#endif
typedef enum {
    PKT_UNKNOWN,
    PKT_PUREDATA,
} PKT_TYPE;

PKT_TYPE g_CurType = PKT_UNKNOWN;
u8 g_u8RecvDataLen = 0;
u8 g_u8CurPktLen = 0;
extern ZC_UartBuffer g_struUartBuffer;
extern void AC_UartSend(u8* inBuf, u32 datalen);
/*************************************************
* Function: AC_UartRecv
* Description:============================================
             |  1B  |1B|1B|1B| 1B | 1B |  nB  | 1B |  1B |
             ============================================
            | 0x5A | len |id|resv|code|payload|sum|0x5B |
            ============================================
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_UartProcess(u8* inBuf, u32 datalen) 
{ 
    if(ZC_RET_ERROR==AC_CheckSum(inBuf,datalen))
    {
        return;
    }
    switch(inBuf[5])
    {
        case ZC_CODE_REST: 
        {
            if((0xD8==inBuf[6])&&(0x8D==inBuf[7]))
            {
                AC_SendRestMsg();
            }
            break;
        } 
        
        case ZC_CODE_UNBIND:         
        {  
            if((0xA6==inBuf[6])&&(0x6A==inBuf[7]))
            {
                AC_SendUbindMsg();
            }
            break;
        }
        case ZC_CODE_REGSITER:
        {
            AC_SendDeviceRegsiterWithMac(inBuf+6,inBuf+18,inBuf+10);
            break;
        }
        default:
        {
            u16 u16DataLen = 0;
            if(0==inBuf[4])
            {
                AC_BuildMessage(inBuf[5],inBuf[3],
                                inBuf+6, datalen-8,
                                NULL, 
                                g_u8DevMsgBuildBuffer, &u16DataLen);
                AC_SendMessage(g_u8DevMsgBuildBuffer, u16DataLen);
            }
            else
            {
                AC_OptList struOptList;
                struOptList.pstruTransportInfo=NULL;
                struOptList.pstruSsession = (ZC_SsessionInfo *)(inBuf+6);
                AC_BuildMessage(inBuf[5],inBuf[3],
                                inBuf+10, datalen-12,
                                &struOptList, 
                                g_u8DevMsgBuildBuffer, &u16DataLen);
                AC_SendMessage(g_u8DevMsgBuildBuffer, u16DataLen);
                
            }
            break;
        }
        
    }
}

/*************************************************
* Function: AC_UartRecv
* Description: 
              ============================================
             |  1B  |1B|1B|1B| 1B | 1B |  nB  | 1B |  1B |
             ============================================
            | 0x5A | len |id|resv|code|payload|sum|0x5B |
            ============================================
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_UartRecv(u8 *pu8Data, u32 u32DataLen)
{
    u32 u32i = 0;
    u8 data = 0;
    for(u32i=0;u32i < u32DataLen;u32i++)
    {
        data = pu8Data[u32i];
        switch(g_CurType)
        {
            case PKT_UNKNOWN:
            {
                if(data==0x5A)
                {
                    g_CurType = PKT_PUREDATA;
                    g_u8RecvDataLen = 0;
                    g_struUartBuffer.u8UartBuffer[g_u8RecvDataLen++] = data; 
                    g_u8CurPktLen = 0;
                }
                break;
            }
            case PKT_PUREDATA:
            {
                g_struUartBuffer.u8UartBuffer[g_u8RecvDataLen++] = data;
                if (2 == g_u8RecvDataLen)
                {
                    g_u8CurPktLen = data;
                }
                else if(3 == g_u8RecvDataLen)
                {
                    g_u8CurPktLen = (g_u8CurPktLen<<8) + data;
                } 
                
                if (g_u8RecvDataLen == g_u8CurPktLen)
                {
                    if (data==0x5B)
                    {
                        AC_UartProcess(g_struUartBuffer.u8UartBuffer,g_u8CurPktLen);
                    }
                    g_CurType = PKT_UNKNOWN;
                    g_u8RecvDataLen = 0; 
                    g_u8CurPktLen = 0;
                    break;
                }
            }
            
        }
    }
    return;
}

/*************************************************
* Function: AC_SendDevStatus2Server
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_SendLedStatus2Server()
{
    STRU_LED_ONOFF struRsp;
    u16 u16DataLen;
    //struRsp.u8LedOnOff = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2);
    struRsp.u8LedOnOff = struRsp.u8LedOnOff>>2;
    AC_BuildMessage(MSG_SERVER_CLIENT_GET_LED_STATUS_RSP,0,
                    (u8*)&struRsp, sizeof(STRU_LED_ONOFF),
                    NULL, 
                    g_u8DevMsgBuildBuffer, &u16DataLen);
    AC_SendMessage(g_u8DevMsgBuildBuffer, u16DataLen);
}

/*************************************************
* Function: AC_ConfigWifi
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_ConfigWifi()
{
#ifdef TEST_ADDR	
    g_struZcConfigDb.struSwitchInfo.u32SecSwitch = 0;
#else
    g_struZcConfigDb.struSwitchInfo.u32SecSwitch = 1;  
#endif
    g_struZcConfigDb.struSwitchInfo.u32TraceSwitch = 0;
    g_struZcConfigDb.struSwitchInfo.u32WifiConfig = 0;
    memcpy(g_struZcConfigDb.struCloudInfo.u8CloudAddr, CLOUD_ADDR, ZC_CLOUD_ADDR_MAX_LEN);
}

#if ZC_EASY_UART
/*************************************************
* Function: AC_DealNotifyMessage
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealNotifyMessage(ZC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{
    u8 u8SendBuff[] = {0x5a,0x00,0x0a,0x01,0x00,0x00,0x00,0x00,00,0x5b};
    u8SendBuff[5] = pstruMsg->MsgCode;
    //处理wifi模块的通知类消息
    switch(pstruMsg->MsgCode)
    {
        case ZC_CODE_WIFI_CONNECTED://wifi模块启动通知
        //AC_StoreStatus(WIFIPOWERSTATUS , WIFIPOWERON);
        u8SendBuff[6] = 0xf0;
        u8SendBuff[7] = 0x0f;
        break;
        case ZC_CODE_WIFI_DISCONNECTED://wifi连接成功通知
        //AC_SendDeviceRegsiterWithMac(g_u8EqVersion,g_u8ModuleKey,g_u64Domain);
        u8SendBuff[6] = 0xf1;
        u8SendBuff[7] = 0x1f;
        break;
        case ZC_CODE_CLOUD_CONNECTED://云端连接通知
        //AC_StoreStatus(CLOUDSTATUS,CLOUDCONNECT);
        u8SendBuff[6] = 0xf2;
        u8SendBuff[7] = 0x2f;
        break;
        case ZC_CODE_CLOUD_DISCONNECTED://云端断链通知
        //AC_StoreStatus(CLOUDSTATUS,CLOUDDISCONNECT);
        u8SendBuff[6] = 0xf3;
        u8SendBuff[7] = 0x3f;
        break;
    }
    u8SendBuff[8] = AC_CalcSum(u8SendBuff +1,7);
    AC_UartSend(u8SendBuff,10);
}
#else
/*************************************************
* Function: AC_DealNotifyMessage
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealNotifyMessage(ZC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{
    //处理wifi模块的通知类消息
    switch(pstruMsg->MsgCode)
    {
        case ZC_CODE_EQ_DONE://wifi模块启动通知
        AC_ConfigWifi();
        AC_StoreStatus(WIFIPOWERSTATUS , WIFIPOWERON);
        break;
        case ZC_CODE_WIFI_CONNECTED://wifi连接成功通知
        AC_SendDeviceRegsiterWithMac(g_u8EqVersion,g_u8ModuleKey,(u8*)&g_u64Domain);
        break;
        case ZC_CODE_CLOUD_CONNECTED://云端连接通知
        AC_StoreStatus(CLOUDSTATUS,CLOUDCONNECT);
        break;
        case ZC_CODE_CLOUD_DISCONNECTED://云端断链通知
        AC_StoreStatus(CLOUDSTATUS,CLOUDDISCONNECT);
        break;
    }
}
#endif
#if ZC_EASY_UART    
/*************************************************
* Function: AC_DealEvent
* Description:============================================
             |  1B  |1B|1B|1B| 1B | 1B |  nB  | 1B |  1B |
             ============================================
            | 0x5A | len |id|resv|code|payload|sum|0x5B |
            ============================================
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealEvent(ZC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{   
    ZC_MessageHead struMsgHead;
    u8 *pu8SendData = (u8 *)pstruMsg;
    u16 Packetlen=0;
    struMsgHead = *pstruMsg;
    
    AC_BuildEasyMessage(struMsgHead.MsgCode,struMsgHead.MsgId,
                          pu8Playload,ZC_HTONS(struMsgHead.Payloadlen),
                          pstruOptList,pu8SendData,&Packetlen);
    AC_UartSend(pu8SendData,Packetlen);

}
#else
/*************************************************
* Function: AC_DealEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealEvent(ZC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{   
    //处理设备自定义控制消息
    switch(pstruMsg->MsgCode)
    {
        case MSG_SERVER_CLIENT_SET_LED_ONOFF_REQ:
        {
            AC_DealLed(pstruMsg, pstruOptList, pu8Playload);
        }
        break;
    }
}
#endif
/*************************************************
* Function: AC_DealEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_StoreStatus(u32 u32Type , u32 u32Data)
{
    
    switch(u32Type)
    {
        case CLOUDSTATUS:
        g_u32CloudStatus = u32Data;
        break;
        case WIFIPOWERSTATUS:
        g_u32WifiPowerStatus = u32Data;
        break;
    }
}
/*************************************************
* Function: AC_BlinkLed
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_BlinkLed(unsigned char blink)
{
    if(blink)
    {

    }
    else
    {

    }

}
/*************************************************
* Function: AC_DealLed
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealLed(ZC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{
    u16 u16DataLen;
    u8 test[] = {1,0,0,0};

    switch (((STRU_LED_ONOFF *)pu8Playload)->u8LedOnOff)
    {
        case 0://处理开关消息
        case 1:        
            AC_BlinkLed(((STRU_LED_ONOFF *)pu8Playload)->u8LedOnOff);
            AC_BuildMessage(CLIENT_SERVER_OK,pstruMsg->MsgId,
                    (u8*)test, sizeof(test),
                    pstruOptList, 
                    g_u8DevMsgBuildBuffer, &u16DataLen);
            AC_SendMessage(g_u8DevMsgBuildBuffer, u16DataLen);
            break;       
    }
    
}
/*************************************************
* Function: AC_DealEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 AC_GetStoreStatus(u32 u32Type)
{
    switch(u32Type)
    {
        case CLOUDSTATUS:
        return g_u32CloudStatus;
        case WIFIPOWERSTATUS:
        return g_u32WifiPowerStatus;
    }
   return ZC_RET_ERROR;
}

/*************************************************
* Function: AC_Init
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_Init()
{

    AC_ConfigWifi();
}
