#ifndef _ATCMD_ICOMM_H_
#define _ATCMD_ICOMM_H_

#include "uip.h"

int ip2int (char *pStr, uip_ip4addr_t *ipaddr);
void At_RespOK (char* pStr);
int At_GetLocalMac (void);
int At_GetVersion (void);
int At_GetManufactureInfo (void);
int At_SetWifiConfig (char *buff);
int At_GetConfigure (void);
int At_SetIfConfigure (char *buff);
int At_GetNetStatus (void);
int At_Ping(char *buff);
int At_Help(void);
int At_GetApStatus(void);
int At_GetWifiStatus(void);
int At_GetMRX();
int At_TestMemRead(char *buff);
int At_MboxDump();
int At_WifiReset();
int At_GetIPconf();
int At_NetScan (void);
int At_NetScan_Custom (char *buff);
int At_Connect (void);
int At_Disconnect (void);
int At_POWERSAVE (char *buff);
int At_WriteReg32 (char *buff);
int At_ReadReg32 (char *buff);
int At_SetChannel (char *buff);
int At_GetRfStatus (char *buff);
int At_ShowRfCommand (char *buff);
int AT_RemoveCfsConf();
int At_bmode_count ();
int At_bmode_reset ();
int At_gmode_count ();
int At_gmode_reset ();
int At_filter_reset(char *buff);
int At_filter_count(char *buff);

S32 at_cmd_start();
S32 at_cmd_end();

#endif
