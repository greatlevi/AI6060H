#include <string.h>
#include <stdio.h>
#include "httpstest.h"
#include <unistd.h>
#include <process.h>
#include "ssl_api.h"


/********************************** Globals ***********************************/
/************************* allpay *********************************/
static unsigned char g_httpRequestHdr1[] = "GET /certificate/index.htm HTTP/1.0\r\n"
	"Host: 110.75.236.154\r\n"
	"User-Agent: MatrixSSL/3.7.2-OPEN\r\n"
	"Accept: */*\r\n"
	"Content-Length: 0\r\n"
	"\r\n";

/************************* google *********************************/
static unsigned char g_httpRequestHdr2[] = "GET / HTTP/1.0\r\n"
	"Host: www.google.com.tw\r\n"
	"User-Agent: MatrixSSL/3.7.2-OPEN\r\n"
	"Accept: */*\r\n"
	"Content-Length: 0\r\n"
	"\r\n";

/************************* averipcam *********************************/
static unsigned char g_httpRequestHdr3[] = "GET /vb.htm?enableSSH=1 HTTP/1.0\r\n"
	"Host: 192.168.88.130\r\n"
	"User-Agent: MatrixSSL/3.7.2-OPEN\r\n"
	"Authorization: Basic YWRtaW46YWRtaW4=\r\n"
	"Accept: */*\r\n"
	"Content-Length: 0\r\n"
	"\r\n";

static unsigned short g_port;
static uip_ipaddr_t g_ipbyte1, g_ipbyte2, g_ipbyte3;
static uint32 g_bytes_requested;
static unsigned char *g_httpRequestHdr;
ssl_t *gsslctx;
int pr;
char httprsp[1000];

PROCESS(httpsClientConnection, "httpsClientConnection process");

PROCESS_THREAD(httpsClientConnection, ev, data)
{
	SSLMSG *msg;
    int rc;

    PROCESS_BEGIN();

    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG || ev == PROCESS_EVENT_EXIT);
		if( ev == PROCESS_EVENT_EXIT) 
		{
			break;
		} 
		else if(ev == PROCESS_EVENT_MSG) 
        {
            msg = data;
            if(msg->status == SSL_CONNECTED)
            {
                printf("SSL connect\n");
                printf("SSl write data len:%d\n", strlen(g_httpRequestHdr)+1);
                printf("%s\n", g_httpRequestHdr);
                SSL_write(gsslctx, g_httpRequestHdr, strlen(g_httpRequestHdr)+1);
            }
            else if(msg->status == SSL_CLOSED)
            {
                printf("SSL disconnect\n");
            }
            else if(msg->status == SSL_NEWDATA)
            {
                rc = SSL_read(gsslctx, httprsp, sizeof(httprsp));
                httprsp[rc] = 0;
                printf("recvlen:%d\n", rc);
                if(pr == 1)
                {
                    printf("rsp:%s\n", httprsp);
                    pr = 0;
                }
/*                rc = httpBasicParse(httprsp, msg->recvlen);
                if(rc == HTTPS_COMPLETE)
                {
                    printf("HTTPS response complete\n");
                    SSL_disconnect(gsslctx);
                }*/
            }
        }
    }

    PROCESS_END();
}

void httpstest()
{
    g_ipbyte1.u8[0] = 110;
    g_ipbyte1.u8[1] = 75;
    g_ipbyte1.u8[2] = 236;
    g_ipbyte1.u8[3] = 154;
    
    g_ipbyte2.u8[0] = 74;
    g_ipbyte2.u8[1] = 125;
    g_ipbyte2.u8[2] = 204;
    g_ipbyte2.u8[3] = 106;

    g_ipbyte3.u8[0] = 192;
    g_ipbyte3.u8[1] = 168;
    g_ipbyte3.u8[2] = 88;
    g_ipbyte3.u8[3] = 130;

	g_port               = 443;
    pr = 1;

    process_start(&httpsClientConnection, NULL);
    SSL_init();
    g_httpRequestHdr = g_httpRequestHdr2;
    gsslctx = SSL_connect(&httpsClientConnection, g_ipbyte2, g_port);
        
}

