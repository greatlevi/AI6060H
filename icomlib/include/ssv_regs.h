#ifndef _SSV_REGS_H_
#define _SSV_REGS_H_

#include "ssv6200_reg.h"

#define REG32(addr)     (*(volatile unsigned int *)(addr))

#define SET_MIB_FULL_EN(_val_)       (REG32(ADR_MIB_EN)) = (_val_)

typedef volatile unsigned int	SSV_REG;

/***********************************************************************
 *        SOFTWARE REGISTER DEFINITIONS
 ***********************************************************************/

typedef struct CPU_UART_st {
    SSV_REG  DATA; 
    SSV_REG  IER;
    SSV_REG  FCR;
    SSV_REG  LCR;
    SSV_REG  MCR;
    SSV_REG  LSR;
    SSV_REG  MSR;
    SSV_REG  SPR;
    SSV_REG  RTHR;
    SSV_REG  ISR;
} CPU_UART, *P_CPU_UART ;

typedef struct REG_UART_st {
    SSV_REG     DATA;
    SSV_REG     IER;
    SSV_REG     FCR;
    SSV_REG     LCR;
    SSV_REG     MCR;
    SSV_REG     LSR;
    SSV_REG     MSR;
    SSV_REG     SPR;
    SSV_REG     RTHR;
    SSV_REG     ISR;
} REG_UART, *P_REG_UART ;


/**
 *  struct INTR_CTRL_st - Definition of hardware interrupt controller registers
 *
 */
typedef struct INTR_CTRL_st {
    SSV_REG     INT_MASK;
    SSV_REG     INT_MODE;
    SSV_REG     INT_IRQ_STS;
    SSV_REG     INT_FIQ_STS;
    SSV_REG     INT_IRQ_RAW;
    SSV_REG     INT_FIQ_RAW;
    SSV_REG     INT_PERI_MASK;
    SSV_REG     INT_PERI_STS;
    SSV_REG     INT_PERI_RAW;

} INTR_CTRL, *P_INTR_CTRL ;


/**
 *  struct HW_TIMER_st - Hardware timer register definition (ms/us)
 *
 *  @ TMR_CTRL[00:15] :  Counter value
 *  @ TMR_CTRL[16:16] :  Operation mode
 */
typedef struct HW_TIMER_st {
    SSV_REG     TMR_CTRL;
    SSV_REG     TMR_CURRENT;
    SSV_REG     DUMMY0;
    SSV_REG     DUMMY1;

} HW_TIMER, *P_HW_TIMER ;

/**
 * struct DMA_CTRL_st - DMA register definition
 *
 * @ DMA_SRC[31:00]: Source address
 * @ DMA_DST[31:00]: Destination address
 * @ DMA_CTRL: DMA Control
 */

typedef struct DMA_CTRL_st {
    SSV_REG     DMA_SRC;   // 0x00
    SSV_REG     DMA_DST;   // 0x04
    SSV_REG     DMA_CTRL; // 0x08
    SSV_REG     DMA_INT;   // 0x0C

} DMA_CTRL, *P_DMA_CTRL ;

/**
 * struct RTC_st - RTC register definition
 *
 * @ RTC_CTRL0[31:0]:
 * @ RTC_CTRL1[31:0]:
 * @ RTC_CNT[31:0]:
 * @ RTC_ALARM[31:0]:
 * @ RTC_RAM00[31:0] ~ RTC_RAM1f[31:0]:
 *
 */
typedef struct RTC_st {
    SSV_REG     RTC_CTRL0;
    SSV_REG     RTC_CTRL1;
    SSV_REG     RTC_CNT;
    SSV_REG     RTC_ALARM;
    SSV_REG     RTC_RAM00[1];
} RTC, *P_RTC ;


/**
 * struct WDT_st - Watch dog register definition
 *
 * @ WDT_TIMER[15:0]: Watch-dog timer value
 */
typedef struct WDT_st {
    SSV_REG     WDT_TIMER;

} WDT, *P_WDT ;

/**
* struct WSID_ENTRY_st - WSID Table Entry
*
* @ WINFO
* @ MAC00_31
*/
typedef struct WSID_ENTRY_st 
{
    SSV_REG     WINFO;
    SSV_REG     MAC00_31;
    SSV_REG     MAC32_47;

    struct {
    SSV_REG     UP_ACK_POLICY;
    SSV_REG     UP_TX_SEQCTL;
    } s[8];

    SSV_REG     RSVD;

} WSID_ENTRY, *P_WSID_ENTRY ;


typedef struct WSID_RX_ENTRY_st 
{
   SSV_REG UP_RX_SEQCTL[8];

} WSID_RX_ENTRY, *P_WSID_RX_ENTRY ;


/**
* struct MRX_DECISION_st - MAC Rx Decision Table Entry.
*
* @ DECI_VAL
*/
typedef struct MRX_DECISION_st {
    SSV_REG     DECI_VAL[16];

} MRX_DECISION, *P_MRX_DECISION ;

/**
* struct MRX_DECISION_MASK_st - MAC Rx Decision Mask Table Entry.
*
* @ DECI_MSK
*/
typedef struct MRX_DECISION_MASK_st {
    SSV_REG      DECI_MSK[9];

} MRX_DECI_MASK, *P_MRX_DECI_MASK ;


/**
* struct GMFLT_ENTRY_st - Group MAC address filtering Table Entry.
* 
* @ 
*/
typedef struct GMFLT_ENTRY_st {
    SSV_REG     TB_MAC00_31;
    SSV_REG     TB_MAC32_47;
    SSV_REG     MK_MAC00_31;
    SSV_REG     MK_MAC32_47;
	SSV_REG     control;
} MRX_GMFLT_TBL, *P_MRX_GMFLT_TBL ;



/**
* struct PHYINFO_TBL_st - PHY Info Table (register table)
*
*/
typedef struct PHYINFO_TBL_st {
    SSV_REG     INFO;
} PHYINFO_TBL, *P_PHYINFO_TBL;

/****************************************************************************
 *  Definition of Base Register Address
 ****************************************************************************/

#define timer_ms_0                    (10)
#define timer_ms_1                    (1)
#define timer_ms_2                    (1)
#define timer_ms_3                    (1)
 
#define REG_BASE0                     (SYS_REG_BASE)

// UART Base Address
#define REG_CPU_UART                  ( (P_CPU_UART) (UART_REG_BASE) )
#define REG_HCI_UART                  ( (P_REG_UART) (DAT_UART_REG_BASE) )

// Interrupt Base Address
#define REG_INTR_CTRL                 ( (P_INTR_CTRL)(INT_REG_BASE) )

// HW TIMER Base Address
#define REG_TIMER_BASE                ( (P_HW_TIMER) (TU0_US_REG_BASE) )
#define REG_TIMER_US_0                ( (P_HW_TIMER) (TU0_US_REG_BASE) )
#define REG_TIMER_US_1                ( (P_HW_TIMER) (TU1_US_REG_BASE) )
#define REG_TIMER_US_2                ( (P_HW_TIMER) (TU2_US_REG_BASE) )
#define REG_TIMER_US_3                ( (P_HW_TIMER) (TU3_US_REG_BASE) )
#define REG_TIMER_MS_0                ( (P_HW_TIMER) (TM0_MS_REG_BASE) )
#define REG_TIMER_MS_1                ( (P_HW_TIMER) (TM1_MS_REG_BASE) )
#define REG_TIMER_MS_2                ( (P_HW_TIMER) (TM2_MS_REG_BASE) )
#define REG_TIMER_MS_3                ( (P_HW_TIMER) (TM3_MS_REG_BASE) )

#define REG_WSID_ENTRY          ( (P_WSID_ENTRY    ) (ADR_WSID0             ) )
#define REG_MRX_DECTBL          ( (P_MRX_DECISION  ) (ADR_MRX_FLT_TB0       ) )
#define REG_MRX_DECMSK          ( (P_MRX_DECI_MASK ) (ADR_MRX_FLT_EN0       ) )
#define REG_MRX_GMFLT           ( (P_MRX_GMFLT_TBL ) (ADR_MRX_MCAST_TB0_0   ) )
#define REG_WSID_RX_ENTRY       ( (P_WSID_RX_ENTRY ) (ADR_WSID0_TID0_RX_SEQ ) )
#define REG_PHYINFO_ENTRY       ( (P_PHYINFO_TBL   ) (ADR_INFO0             ) )

#endif	/* _SSV_REGS_H_ */
