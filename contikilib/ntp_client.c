
#include "contiki.h"
#include "uip.h"
#include "dbg-printf.h"
#include "socket_driver.h"
#include "ssv_lib.h"
#include "uiplib.h"
#include "ntp_client.h"
#include "sys/clock.h"

/*---------------------------------------------------------------------------*/
//struct data
static ntp_packet packet, *pPacket;
static ntp_client ntp;
/*---------------------------------------------------------------------------*/
//ntp client process name
PROCESS(ntp_client_Process, "ntp client process");
PROCESS(ntp_connectPprocess, "ntp ConnectProcess");
/*---------------------------------------------------------------------------*/
//function
void showReceNtpMsg(ntp_packet * msg)
{
    printf("LiVnMode: %d\n", msg->LiVnMode);
    printf("stratum: %d\n", msg->stratum);
    printf("poll: %d\n", msg->poll);
    printf("precision: %d\n", msg->precision);
    printf("rootDelay: %u\n", ENDIAN_SWAP32(msg->rootDelay));
    printf("rootDispersion: %u\n", ENDIAN_SWAP32(msg->rootDispersion));
    printf("refId: %u.", msg->refId[0]);
    printf("%u", msg->refId[1]);
    printf("%u", msg->refId[2]);
    printf("%u\n", msg->refId[3]);
    printf("refTm_s: %u\n", ENDIAN_SWAP32(msg->refTm_s));
    printf("refTm_f: %u\n", ENDIAN_SWAP32(msg->refTm_f));
    printf("origTm_s: %u\n", ENDIAN_SWAP32(msg->origTm_s));
    printf("origTm_f: %u\n", ENDIAN_SWAP32(msg->origTm_f));
    printf("rxTm_s: %u\n", ENDIAN_SWAP32(msg->rxTm_s));
    printf("rxTm_f: %u\n", ENDIAN_SWAP32(msg->rxTm_f));
    printf("txTm_s: %u\n", ENDIAN_SWAP32(msg->txTm_s));
    printf("txTm_f: %u\n", ENDIAN_SWAP32(msg->txTm_f));
}
/*---------------------------------------------------------------------------*/
//process
PROCESS_THREAD(ntp_connectPprocess, ev, data)
{
    SOCKETMSG msg;
    int recvlen;
    uip_ipaddr_t peeraddr;
    U16 peerport;
    unsigned long now;
    
    PROCESS_BEGIN();

    //printf("ntp_connectPprocess begin\n");
    while(1) {
        PROCESS_WAIT_EVENT();
        if( ev == PROCESS_EVENT_EXIT) {
            break;
        }  else if(ev == PROCESS_EVENT_MSG) {
            msg = *(SOCKETMSG *)data;
            //The TCP socket is connected, This socket can send data after now.
            if(msg.status == SOCKET_NEWDATA) {
                if(UIP_CONNS <= msg.socket && msg.socket < UIP_CONNS + UIP_UDP_CONNS) {
                    recvlen = udprecvfrom(msg.socket, ntp.buf, MAX_BUFFER, &peeraddr, &peerport);
                    pPacket = (ntp_packet *)ntp.buf;
                    //printf("UDP socked:%d recvdata:%s from %d.%d.%d.%d:%d\n", msg.socket, ntp.buf, peeraddr.u8[0], peeraddr.u8[1], peeraddr.u8[2], peeraddr.u8[3], peerport);
                    //showReceNtpMsg(pPacket);
                    now = (ENDIAN_SWAP32(pPacket->txTm_s) - NTP_TIMESTAMP_DELTA);
                    //printf( "Now Time: %d\n", now);
                    update_ntp_time(now);
                    ntp.exit_process = 1;
                } else {
                    printf("Illegal socket:%d\n", msg.socket);
                } 
            }else {
                printf("unknow message type\n");
            }
        }
    }
    //printf("ntp_connectPprocess end\n");
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
//process
PROCESS_THREAD(ntp_client_Process, ev, data)
{
    static struct etimer ntp_timer;
    PROCESS_BEGIN();
    //printf("ntp_client_Process begin\n");
    
    process_start(&ntp_connectPprocess, NULL);

    ntp.client_sock = -1;
    ntp.exit_process = 0;
    ntp.retry_cnt = 0;
    
    ntp.client_sock = udpcreate(NTP_LOCAL_PORT, &ntp_connectPprocess);
    if(ntp.client_sock == -1) {
        printf("create udp socket fail");
        ntp.exit_process = 1;
    } else {
        //printf("create socket:%d\n", ntp.client_sock);
        //us.pool.ntp.org(108.61.56.35)
        if( uiplib_ipaddrconv("108.61.56.35", &ntp.server_ip)  == 0) {
            printf("erro ip format\n");
            ntp.exit_process = 1;
        }
    }
    
    memset( &packet, 0, sizeof( ntp_packet ) );
    *( ( char * ) &packet + 0 ) = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.
    
    while(!ntp.exit_process) {
        if(ntp.retry_cnt > MAX_RETRYCNT) {
            printf("ntp client can not rece ntp server data, please check network\n");
            break;
        }
        etimer_set(&ntp_timer, 1 * CLOCK_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

        if(udpsendto(ntp.client_sock, (char *)&packet, sizeof(packet), &ntp.server_ip, NTP_SERVER_PORT) == -1) {
            printf("udpsendto fail\n");
        }
        ntp.retry_cnt++;
    }

    if(udpclose(ntp.client_sock) == -1) {
        printf("udpclose fail\n");
    }
    process_post_synch(&ntp_connectPprocess, PROCESS_EVENT_EXIT, NULL);
    //printf("ntp_client_Process end\n");
    PROCESS_END();
}
