#ifndef _HW_CTRL_H_
#define _HW_CTRL_H_

#include <ssv_types.h>

#define TX_NO_RTS_CTS    0
#define TX_RTS_CTS       1
#define TX_CTS           2

int ssv6200_transmit(U32 addr, U32 txqueue, U8 enrx) ATTRIBUTE_SECTION_SRAM;
void ssv6200_set_nav(U32 *addr) ATTRIBUTE_SECTION_SRAM;
int ssv6200_set_descriptor(U32 *addr, U8 cmdtype) ATTRIBUTE_SECTION_SRAM;
void ssv6200_force_1M(U32 *addr) ATTRIBUTE_SECTION_SRAM;

#endif /* _HW_CTRL_H_ */
