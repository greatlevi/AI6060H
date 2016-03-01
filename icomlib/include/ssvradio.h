#ifndef _SSVRADIO_H_
#define _SSVRADIO_H_

#include <ssv_types.h>
#include "dev/radio.h"

#define SSV6200_MAX_PACKET_LEN     16

extern struct global_radio_config g_tx_config;

extern const struct radio_driver ssvradio_driver;

typedef struct global_radio_config {
  U8   rate_index;
  U8   aifs;
  U8   frame_data;
  U32  frame_size;
  U32  frame_type;
} global_radio_config_t;

#endif /* _SSVRADIO_H_ */
