#ifndef _DRV_PACKETBUFFER_H_
#define _DRV_PACKETBUFFER_H_

#include <ssv_types.h>
//#include <ssv_lib.h>
#include <skbuff.h>

U32 ssv6xxx_pbuf_alloc(int size) ATTRIBUTE_SECTION_SRAM;
BOOL ssv6xxx_pbuf_free(U32 pbuf_addr) ATTRIBUTE_SECTION_SRAM;
S32 getpacketbuffer(struct tx_pktbuf *buf, int payload_len) ATTRIBUTE_SECTION_SRAM;
void setpacketbuf_len(U32 *buf, int payload_len) ATTRIBUTE_SECTION_SRAM;
void ssv6xxx_pbuf_id_free() ATTRIBUTE_SECTION_SRAM;
void ssv6xxx_pbuf_dump_reg();

#endif /* _DRV_PACKETBUFFER_H_ */
