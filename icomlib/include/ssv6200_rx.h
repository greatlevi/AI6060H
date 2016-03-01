#ifndef _SSV6200_RX_H_
#define _SSV6200_RX_H_

#include <ssv_types.h>
#include "clock.h"

#define RX_QUEUE_LEN (16)   //it must be power of 2
#define RX_QUEUE_MASK (RX_QUEUE_LEN-1)

typedef struct rx_info
{
  U8* pbuf_addr;
  U32 pktlen;
  U8 rssi;
  U8 snr;
  U8  isoccupied;
  clock_time_t packettime;
} rx_info_t;
 
 typedef struct rx_queue
 {
   rx_info_t queue[RX_QUEUE_LEN];
   U8 save_index;
   U8 proc_index;
 } rx_queue_t;

#define RETX_QUEUE_MAX 15
typedef struct t_RETX_INFO{
    U32 pktbufaddr;
    U8  retrycnt;
}RETX_INFO;

typedef struct t_RETX_QUE{
    RETX_INFO retx_queue[RETX_QUEUE_MAX];
    U8  queuecnt;
}RETX_QUE;

void clearRxBuffer() ATTRIBUTE_SECTION_SRAM;
U32 rate_index_parsing(void *data);
void rx_data_clear();
void drop_packet(U32 pbuf_addr);
//void mac_rx_handler();
void STA_mac_rx_handler();
void AP_mac_rx_handler();
U8 dropDataInSmartLink(U32 pbuf_addr);
U8 retx_add_queue(U32 bufaddr);
U8 is_tx_active(void);

#endif /* _SSV6200_RX_H_ */
