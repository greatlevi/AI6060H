
#include "contiki.h"
#include "uip.h"
#include "dbg-printf.h"
#include "socket_driver.h"
#include "ssv_lib.h"
#include "uiplib.h"

#define NTP_LOCAL_PORT (1234)
#define NTP_SERVER_PORT (123)
#define MAX_BUFFER 	(48)
#define MAX_RETRYCNT 	(10)
#define NTP_TIMESTAMP_DELTA 0x83AA7E80
#define ENDIAN_SWAP32(data)  	((data >> 24) | /* right shift 3 bytes */ \
                                      ((data & 0x00ff0000) >> 8) | /* right shift 1 byte */ \
                                      ((data & 0x0000ff00) << 8) | /* left shift 1 byte */ \    
                                      ((data & 0x000000ff) << 24)) /* left shift 3 bytes */

/* some ntp server
time-a.nist.gov
time-b.nist.gov
time-a.timefreq.bldrdoc.gov
time-b.timefreq.bldrdoc.gov
time-c.timefreq.bldrdoc.gov
utcnist.colorado.edu
ntp.tuxfamily.net
time-nw.nist.gov
nist1.datum.com
nist1-ny.glassey.com
nist1.aol-ca.truetime.com
ntp2.belbone.be
ntp.doubleukay.com
ntp0.cs.mu.OZ.AU
time.apple.com
0.pool.ntp.org
1.pool.ntp.org
2.pool.ntp.org
2.europe.pool.ntp.org
3.europe.pool.ntp.org
1.north-america.pool.ntp.org
2.north-america.pool.ntp.org
0.oceania.pool.ntp.org
1.oceania.pool.ntp.org
0.au.pool.ntp.org
3.au.pool.ntp.org
*/

typedef struct {				
    U8 LiVnMode;
    U8 stratum; // Eight bits. Stratum level of the local clock.
    U8 poll; // Eight bits. Maximum interval between successive messages.
    U8 precision; // Eight bits. Precision of the local clock.
    U32 rootDelay; // 32 bits. Total round trip delay time.
    U32 rootDispersion; // 32 bits. Max error aloud from primary clock source.
    U8 refId[4]; // 32 bits. Reference clock identifier.
    U32 refTm_s; // 32 bits. Reference time-stamp seconds.
    U32 refTm_f; // 32 bits. Reference time-stamp fraction of a second.
    U32 origTm_s; // 32 bits. Originate time-stamp seconds.
    U32 origTm_f; // 32 bits. Originate time-stamp fraction of a second.
    U32 rxTm_s; // 32 bits. Received time-stamp seconds.
    U32 rxTm_f; // 32 bits. Received time-stamp fraction of a second.
    U32 txTm_s; // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    U32 txTm_f; // 32 bits. Transmit time-stamp fraction of a second.
} ntp_packet; // Total: 384 bits or 48 bytes.

typedef struct {
    U8 exit_process;
    U8 retry_cnt;
    U8 buf[MAX_BUFFER];
    S32 client_sock;
    uip_ip4addr_t server_ip;
} ntp_client;
