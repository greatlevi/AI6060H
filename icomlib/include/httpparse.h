#ifndef __HTTPPARSE_H__
#define __HTTPPARSE_H__

#include "socket_driver.h"

#define HTTPCMD_MAX 256
#define HTTPRSP_MAX 512

enum{
    HTTPREQ_SUCC        = 0,
    HTTPREQ_STILL_RUN,
    HTTPREQ_CMD_ERROR,
    HTTPREQ_CONN_ERROR,
    HTTPREQ_RSP_DATA,
    HTTPREQ_RSP_TIMEOUT,
    HTTPREQ_RSP_ERROR,
};

enum{
    HTTP_IDLE        = 0,
    HTTP_CONNECTING,
    HTTP_SENDDATA,
    HTTP_WAITRESPONSE,
};

typedef struct t_HTTPMSG
{
    U8 msgtype;
    U8 *rsp;
    U16 rsplen;
}HTTPMSG;

typedef struct t_HTTP_REQ
{
    NETSOCKET httpsock;
    U8 httpstatus;
    void (*callbackfn)(void *);
    U8 ipstr[16];
    uip_ip4addr_t ip_addr;
    U16 port;
    U8 httpcmd[HTTPCMD_MAX];
    U16 cmdlen;
    U8 httprsp[HTTPRSP_MAX];
    U16 rsplen;
}HTTP_REQ;

void httpparse_init();
void httpparse_deinit();
int httprequest_send(U8 *httpcmd, U16 cmdlen, void (*fn)(void *));
void httprsp_parse(U8 *httprsp, U16 rsplen);

#endif /* __HTTPPARSE_H__ */
