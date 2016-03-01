#ifndef _DRV_MBOX_H_
#define _DRV_MBOX_H_

#include <ssv_types.h>

/* Hardware Offload Engine ID */
#define M_ENG_CPU                       0x00
#define M_ENG_HWHCI                     0x01
#define M_ENG_EMPTY	                    0x02
#define M_ENG_ENCRYPT                   0x03
#define M_ENG_MACRX                     0x04  
#define M_ENG_MIC	                    0x05
#define M_ENG_TX_EDCA0                  0x06
#define M_ENG_TX_EDCA1                  0x07
#define M_ENG_TX_EDCA2                  0x08
#define M_ENG_TX_EDCA3                  0x09
#define M_ENG_TX_MNG                    0x0A
#define M_ENG_ENCRYPT_SEC               0x0B
#define M_ENG_MIC_SEC                   0x0C
#define M_ENG_RESERVED_1                0x0D
#define M_ENG_RESERVED_2                0x0E
#define M_ENG_TRASH_CAN                 0x0F
#define M_ENG_MAX                      (M_ENG_TRASH_CAN+1)

/* Software Engine ID: */
#define M_CPU_HWENG                     0x00
#define M_CPU_TXL34CS                   0x01
#define M_CPU_RXL34CS                   0x02
#define M_CPU_DEFRAG                    0x03
#define M_CPU_EDCATX                    0x04
#define M_CPU_RXDATA                    0x05
#define M_CPU_RXMGMT                    0x06
#define M_CPU_RXCTRL                    0x07
#define M_CPU_FRAG	                    0x08

#define HW_ID_OFFSET                    7
#define ADDRESS_OFFSET	16

S32 drv_mailbox_cpu_ff(U32 pktmsg, U32 hw_number) ATTRIBUTE_SECTION_SRAM;

// Yan Wu, back up! 20141007
#if 0
extern inline S32 ENG_MBOX_NEXT(u32 pktmsg);
extern inline S32 ENG_MBOX_SEND(u32 hw_number,u32 pktmsg);
extern inline S32 TX_FRAME(u32 pktmsg);
#endif


#endif /* _DRV_MBOX_H_ */
