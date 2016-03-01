/*
 * userSmart.h
 *
 *  Created on: 2015-09-16
 *      Author: Tim Li
 */

#ifndef USERSMAT_H_
#define USERSMAT_H_

#include "ssv_types.h"
#include "smartComm.h"
#include "process.h"

#if defined(SMART_USER)

//callback define
typedef void (*callback)(U8*, U32);

#define TOTAL_FILTER_NUM 4
#define CHECK_FILTER_TYPE(a, b) ((U16)((a >> b) & 0x0001))
#define SET_FILTER_TABLE(num) SET_FILTER_TABLE##num()

/* filter type */
#define RECE_ONLY_BEACON__MANAG	               0x0001
#define RECE_ONLY_BROADCAST_DATA               0x0002
#define RECE_ALL_MANAG                                         0x0004
#define RECE_ALL_DATA                                              0x0008

typedef struct _userSmart {
    U8 userSmartEnable;
    U8 callBackFrameEnable;
    U8 filterTable[TOTAL_FILTER_NUM];
    void (* callBackFrame)(U8 *data, U32 num);
} userSmart;

void AutoScanAP(struct process *p);
void userSmartSetChannel(U8 ch);
void SetUserReceFilter(U16 type);
U8 EnableUserReceFilter(void);
void DisableUserReceFilter(void);
void SetCallBackFunction(callback function);
U8 EnableCallBackFrame(void);
void DisableCallBackFrame(void);
void EnableUserSmartMode(void);
U8 DisableUserSmartMode(void);
void rx_process_userSmart(U8 *data, U32 len) ATTRIBUTE_SECTION_SRAM;
#endif

#endif /* AIRKISS_H_ */
