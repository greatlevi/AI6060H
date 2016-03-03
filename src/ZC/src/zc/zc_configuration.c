/**
******************************************************************************
* @file     zc_configuration.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    configuration
******************************************************************************
*/
#include <zc_configuration.h>
#include <zc_protocol_controller.h>

ZC_ConfigDB g_struZcConfigDb;
ZC_RegisterInfo g_struRegisterInfo;
u8 g_u8DefaultCloudKey[ZC_CLOUD_KEY_MAX_LEN] = 
{
    0xb8, 0xd9, 0x35, 0xe4,
    0xd6, 0xd8, 0xf2, 0xd6,
    0xc8, 0x28, 0x2f, 0x9f,
    0xd9, 0x62, 0x48, 0xc7,
    0x96, 0xa9, 0xed, 0x5b,
    0x5a, 0x1a, 0x95, 0x59,
    0xd2, 0x3c, 0xbb, 0x5f,
    0x1b, 0x03, 0x07, 0x3f,
    0x01, 0x00, 0x01, 0x00
};
u8 g_u8DefaultTokenKey[ZC_HS_SESSION_KEY_LEN] = 
{
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05
};


/*************************************************
* Function: ZC_ConfigInitDefault
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_ConfigInitDefault(void)
{
    g_struZcConfigDb.struSwitchInfo.u32SecSwitch = 1;
    g_struZcConfigDb.struSwitchInfo.u32TraceSwitch = 0;
    g_struZcConfigDb.struSwitchInfo.u32WifiConfig = 0;
    g_struZcConfigDb.struSwitchInfo.u32ServerAddrConfig = 0;
    g_struZcConfigDb.struSwitchInfo.u16ServerPort = ZC_CLOUD_PORT;

    memcpy(g_struZcConfigDb.struCloudInfo.u8CloudAddr, "test.ablecloud.cn", ZC_CLOUD_ADDR_MAX_LEN);
    memcpy(g_struZcConfigDb.struCloudInfo.u8CloudKey, g_u8DefaultCloudKey, ZC_CLOUD_KEY_MAX_LEN);
    memcpy(g_struZcConfigDb.struCloudInfo.u8TokenKey, g_u8DefaultTokenKey, ZC_HS_SESSION_KEY_LEN);

    g_struZcConfigDb.struConnection.u32MagicFlag = 0xFFFFFFFF;
    g_struZcConfigDb.struDeviceInfo.u32UnBindFlag = 0xFFFFFFFF;
    g_struZcConfigDb.struDeviceInfo.u32UnBcFlag = 0xFFFFFFFF;    
}
/*************************************************
* Function: ZC_ConfigInitPara
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_ConfigInitPara(void)
{
    u32 u32Crc;
    g_struProtocolController.pstruMoudleFun->pfunReadFlash((u8 *)&g_struZcConfigDb,sizeof(g_struZcConfigDb));
    u32Crc = crc16_ccitt(((u8 *)&g_struZcConfigDb) + sizeof(u32), sizeof(g_struZcConfigDb) - sizeof(u32));
    if (u32Crc != g_struZcConfigDb.u32Crc)
    {
        ZC_ConfigInitDefault();
        ZC_Printf("no para, use default\n");
    }
    else
    {
        ZC_Printf("Crc check ok\n");
    }
}
/*************************************************
* Function: ZC_ConfigPara
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_ConfigPara(u8 *pu8Data)
{
    ZC_Configuration *pstruConfig;
    pstruConfig = (ZC_Configuration*)pu8Data;

    g_struZcConfigDb.struSwitchInfo.u32SecSwitch = ZC_HTONL(pstruConfig->u32SecSwitch);
    g_struZcConfigDb.struSwitchInfo.u32TraceSwitch = ZC_HTONL(pstruConfig->u32TraceSwitch);
    g_struZcConfigDb.struSwitchInfo.u32WifiConfig = ZC_HTONL(pstruConfig->u32WifiConfig);
    g_struZcConfigDb.struSwitchInfo.u32ServerAddrConfig = ZC_HTONL(pstruConfig->u32ServerAddrConfig);

    g_struZcConfigDb.struSwitchInfo.u32ServerIp = ZC_HTONL(pstruConfig->u32IpAddr);
    g_struZcConfigDb.struSwitchInfo.u16ServerPort = ZC_CLOUD_PORT;

    memcpy(g_struZcConfigDb.struSwitchInfo.u8Password, pstruConfig->u8Password, ZC_PASSWORD_MAX_LEN);
    memcpy(g_struZcConfigDb.struSwitchInfo.u8Ssid, pstruConfig->u8Ssid, ZC_SSID_MAX_LEN);
    
    memcpy(g_struZcConfigDb.struCloudInfo.u8CloudAddr, pstruConfig->u8CloudAddr, ZC_CLOUD_ADDR_MAX_LEN);
    memcpy(g_struZcConfigDb.struCloudInfo.u8CloudKey, pstruConfig->u8CloudKey, ZC_CLOUD_KEY_MAX_LEN);
    g_struZcConfigDb.u32Crc = crc16_ccitt(((u8 *)&g_struZcConfigDb) + 4, sizeof(g_struZcConfigDb) - 4);
    g_struProtocolController.pstruMoudleFun->pfunWriteFlash((u8*)&g_struZcConfigDb, sizeof(ZC_ConfigDB));
}


/*************************************************
* Function: ZC_StoreRegisterInfo
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_StoreRegisterInfo(u8 *pu8Data,u8 u8RegisterFlag)
{
    ZC_RegisterReq *pstruRegisterMsg;
    u8 u8Mac[ZC_SERVER_MAC_LEN];
    pstruRegisterMsg = (ZC_RegisterReq *)pu8Data;
    
    memcpy(g_struRegisterInfo.u8PrivateKey, pstruRegisterMsg->u8ModuleKey, ZC_MODULE_KEY_LEN);
    if(u8RegisterFlag)
	{
        memset(g_struRegisterInfo.u8DeviciId, '0', ZC_HS_DEVICE_ID_LEN);
		g_struProtocolController.pstruMoudleFun->pfunGetMac(u8Mac);
		memcpy(g_struRegisterInfo.u8DeviciId + (ZC_HS_DEVICE_ID_LEN - ZC_SERVER_MAC_LEN), u8Mac, ZC_SERVER_MAC_LEN);
	}
	else
	{
		memcpy(g_struRegisterInfo.u8DeviciId, pstruRegisterMsg->u8DeviceId, ZC_HS_DEVICE_ID_LEN);
	}
    memcpy(g_struRegisterInfo.u8DeviciId + ZC_HS_DEVICE_ID_LEN, pstruRegisterMsg->u8Domain, ZC_DOMAIN_LEN);
    memcpy(g_struRegisterInfo.u8EqVersion, pstruRegisterMsg->u8EqVersion, ZC_EQVERSION_LEN);
}


/*************************************************
* Function: ZC_StoreTokenKey
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_StoreTokenKey(u8 *pu8Data)
{
    g_struZcConfigDb.struDeviceInfo.u32UnBcFlag = ZC_MAGIC_FLAG;   
    memcpy(g_struZcConfigDb.struCloudInfo.u8TokenKey, pu8Data, ZC_HS_SESSION_KEY_LEN);
    g_struZcConfigDb.u32Crc = crc16_ccitt(((u8 *)&g_struZcConfigDb) + 4, sizeof(g_struZcConfigDb) - 4);
    g_struProtocolController.pstruMoudleFun->pfunWriteFlash((u8*)&g_struZcConfigDb, sizeof(ZC_ConfigDB));
}

/*************************************************
* Function: ZC_StoreConnectionInfo
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_StoreConnectionInfo(u8 *pu8Ssid, u8 *pu8Password)
{
    g_struZcConfigDb.struConnection.u32MagicFlag = ZC_MAGIC_FLAG;
    memcpy(g_struZcConfigDb.struConnection.u8Ssid, pu8Ssid, ZC_SSID_MAX_LEN);
    memcpy(g_struZcConfigDb.struConnection.u8Password, pu8Password, ZC_PASSWORD_MAX_LEN);
    g_struZcConfigDb.u32Crc = crc16_ccitt(((u8 *)&g_struZcConfigDb) + 4, sizeof(g_struZcConfigDb) - 4);
    g_struProtocolController.pstruMoudleFun->pfunWriteFlash((u8*)&g_struZcConfigDb, sizeof(ZC_ConfigDB));
}

/*************************************************
* Function: ZC_StoreConnectionInfo
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_StoreAccessInfo(u8 *pu8ServerIp, u8 *pu8ServerPort)
{
    g_struZcConfigDb.struConnection.u32MagicFlag = ZC_MAGIC_FLAG;
    g_struZcConfigDb.struSwitchInfo.u32ServerAddrConfig = 1;
    memcpy(&g_struZcConfigDb.struSwitchInfo.u32ServerIp, pu8ServerIp, ZC_SERVER_ADDR_MAX_LEN);
    memcpy(&g_struZcConfigDb.struSwitchInfo.u16ServerPort, pu8ServerPort, ZC_SERVER_PORT_MAX_LEN);
    g_struZcConfigDb.u32Crc = crc16_ccitt(((u8 *)&g_struZcConfigDb) + 4, sizeof(g_struZcConfigDb) - 4);
    g_struProtocolController.pstruMoudleFun->pfunWriteFlash((u8*)&g_struZcConfigDb, sizeof(ZC_ConfigDB));
}

/*************************************************
* Function: ZC_GetStoreInfor
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_GetStoreInfor(u8 u8Type, u8 **pu8Data)
{
    switch(u8Type)
    {
        case ZC_GET_TYPE_CLOUDKEY:
            *pu8Data = g_struZcConfigDb.struCloudInfo.u8CloudKey;
            break;
        case ZC_GET_TYPE_DEVICEID:
            *pu8Data = g_struRegisterInfo.u8DeviciId;
            break;
        case ZC_GET_TYPE_PRIVATEKEY:
            *pu8Data = g_struRegisterInfo.u8PrivateKey;
            break;
        case ZC_GET_TYPE_VESION:
            *pu8Data = g_struRegisterInfo.u8EqVersion;
            break;
        case ZC_GET_TYPE_TOKENKEY:
            *pu8Data = g_struZcConfigDb.struCloudInfo.u8TokenKey;
            break;
    }
}

/*************************************************
* Function: ZC_ConfigPara
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_ConfigUnBind(u32 u32UnBindFlag)
{
    g_struZcConfigDb.struDeviceInfo.u32UnBindFlag = u32UnBindFlag; 
    g_struZcConfigDb.u32Crc = crc16_ccitt(((u8 *)&g_struZcConfigDb) + 4, sizeof(g_struZcConfigDb) - 4);    
    g_struProtocolController.pstruMoudleFun->pfunWriteFlash((u8*)&g_struZcConfigDb, sizeof(ZC_ConfigDB));
    //sys_msleep(100);
}

/*************************************************
* Function: ZC_ConfigReset
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_ConfigReset()
{
    ZC_Printf("ZC_ConfigReset\n\r");
    g_struZcConfigDb.struSwitchInfo.u32ServerAddrConfig = 0;            
    g_struZcConfigDb.struDeviceInfo.u32UnBcFlag = 0xFFFFFFFF;
    g_struZcConfigDb.u32Crc = crc16_ccitt(((u8 *)&g_struZcConfigDb) + 4, sizeof(g_struZcConfigDb) - 4);    
    g_struProtocolController.pstruMoudleFun->pfunWriteFlash((u8 *)&g_struZcConfigDb, sizeof(ZC_ConfigDB));
    //sys_msleep(100);
    g_struProtocolController.pstruMoudleFun->pfunRest();
}

/******************************* FILE END ***********************************/

