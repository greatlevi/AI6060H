#ifndef __RATE_CONTROL_H__
#define __RATE_CONTROL_H__

#include "ssv_types.h"

#define MAX_RATECNT 10
#define SHIFTBITS 16
#define CCK_SIFS_TIME  10
#define OFDM_SIFS_TIME 16
#define HT_SIFS_TIME   10
#define HT_SIGNAL_EXT  6

#define ACKLEN			14
#define RTSLEN			20
#define CTSLEN			14
#define FCSLEN          4

typedef struct t_DURATION_TABLE
{
    U32 multiplevalue;
    U8  ctrl_rate_idx;
}DURATION_TABLE;

void init_ratecontrol();
void datarate_down();
void datarate_up();
U8 getctrlrate(U8 rate) ATTRIBUTE_SECTION_SRAM;
U32 navcalculate(int pktlen, U8 rate) ATTRIBUTE_SECTION_SRAM;

#endif
