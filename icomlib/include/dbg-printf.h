#include "ssv_types.h"
#ifndef _DBG_PRINTF_H_
#define  _DBG_PRINTF_H_

#define MAX_LOG_GROUP_NUM (32)

#define LOG_DISABLE  (0)      // 0: don't print
#define LOG_ENABLE  (1)      // 1: print 

#define MAX_LOG_TYPE_NUM     (4)      //basic setup it's (LOG_INFO+1)
#define LOG_DEBUG  (3)
#define LOG_INFO     (2)
#define LOG_WARN   (1)
#define LOG_ERROR  (0)    //will always print

typedef enum _eGROUP_ID    //GROUP ID
{
//GROUP_ALL,            //print all
//GROUP_NONE,         //print none
GROUP_RF,               //print group of RF
GROUP_BSP,             //printf group of BSP
GROUP_WDOG,         //printf group of watch dog
GROUP_FLASH,         //printf group of flash
GROUP_GPIO,            //printf group of gpio
GROUP_PMU,            //printf group of PMU
GROUP_ENCPT,          //printf group of encrypt
GROUP_MGM,            //printf group of 80211_mgm packet
GROUP_NET,            //printf group of 80211_net packet
GROUP_STA,              //printf group of STA
GROUP_SOFTAP,            //printf group of SOFTAP
GROUP_SLINK,         //printf group of SMARTLINK
GROUP_AKISS,
GROUP_TCPIP,
GROUP_RX,
GROUP_USER0,  //printf group of USER0
GROUP_USER1,  //printf group of USER1
GROUP_USER2,  //printf group of USER2
GROUP_MAX
} eGROUP_ID;

void set_log_all_group_enable();     // all enable
void set_log_all_group_disable();     //  all disable
void set_log_all_group(U32 bitmap);     //bit 0:means GROUP_RF,bit1:means GROUP_BSP 
void set_log_all_level(U32 group_31_24_level,U32 group_23_16_level,U32 group_15_08_level,U32 group_07_00_level);     //bit 0:means GROUP_RF,bit1:means GROUP_BSP 
S32 set_log_single_group_level(eGROUP_ID group,BOOL enable,U8 level);     //  1:means enable

void dump_log_control_info();
int icomprintf(eGROUP_ID group,U8 level, const char *fmt, ...) ATTRIBUTE_SECTION_SRAM;

#endif // _DBG_PRINTF_H_
