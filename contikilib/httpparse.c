#include "contiki-net.h"
#include "httpparse.h"
#include "process.h"
#include "etimer.h"
#include "ssv_lib.h"

HTTP_REQ httpreqdata;
HTTPMSG  httpmsg;

PROCESS(http_req_process, "http req process");

void httpparse_init()
{
    memset(&httpreqdata, 0, sizeof(httpreqdata));
    httpreqdata.httpsock != -1;
    process_start(&http_req_process, NULL);
}
/*---------------------------------------------------------------------------*/
void httpparse_deinit()
{
    memset(&httpreqdata, 0, sizeof(httpreqdata));
    process_exit(&http_req_process);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(http_req_process, ev, data)
{
    static struct etimer httptimeout;
	SOCKETMSG msg;

    PROCESS_BEGIN();

    while(1)
    {
		//wait for TCP connected or uip_timeout.
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG || ev == PROCESS_EVENT_TIMER || ev == PROCESS_EVENT_EXIT);
        if(ev == PROCESS_EVENT_MSG)
        {
    		msg = *(SOCKETMSG *)data;
    		if(msg.status == SOCKET_CONNECTED)
    		{
    		    //send out the http request.
    		    if(httpreqdata.cmdlen)
                    tcpsend(httpreqdata.httpsock, httpreqdata.httpcmd, httpreqdata.cmdlen);
    		}
    		else if(msg.status == SOCKET_SENDACK)
    		{
    		    //set timeot for http response.
    		    etimer_set(&httptimeout, 10 * CLOCK_CONF_SECOND);
    		}
    		else if(msg.status == SOCKET_NEWDATA)
    		{
    			//Get http response, parse response and close socket.
    			etimer_stop(&httptimeout);
                httpreqdata.rsplen = tcprecv(httpreqdata.httpsock, httpreqdata.httprsp, HTTPRSP_MAX);
                tcpclose(httpreqdata.httpsock);
                httpreqdata.httpsock = -1;
                httpreqdata.httpstatus = HTTP_IDLE;
                if(httpreqdata.rsplen > 0 && httpreqdata.callbackfn)
                {
                    httpreqdata.httprsp[httpreqdata.rsplen] = 0;
                    httprsp_parse(httpreqdata.httprsp, httpreqdata.rsplen);
                }
    		}
    		else if(msg.status == SOCKET_CLOSED)
    		{
    			//socket closed, if it is unnormal case, notify upper layer.
    			if(httpreqdata.httpstatus != HTTP_IDLE)
                {
                    if(httpreqdata.callbackfn)
                    {
                        httpmsg.msgtype = HTTPREQ_CONN_ERROR;
                        httpmsg.rsp = NULL;
                        httpreqdata.callbackfn(&httpmsg);
                    }
                    httpreqdata.httpsock = -1;
                    httpreqdata.httpstatus = HTTP_IDLE;
                }
    		}
        }
        else if(ev == PROCESS_EVENT_TIMER)
        {
            //http response timeout, close socket and notify upper layer.
            tcpclose(httpreqdata.httpsock);
            httpreqdata.httpsock = -1;
            httpreqdata.httpstatus = HTTP_IDLE;
            if(httpreqdata.callbackfn)
            {
                httpmsg.msgtype = HTTPREQ_RSP_TIMEOUT;
                httpmsg.rsp = NULL;
                httpreqdata.callbackfn(&httpmsg);
            }
        }
        else if(ev == PROCESS_EVENT_EXIT)
        {
            break;
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
int httprequest_send(U8 *httpcmd, U16 cmdlen, void (*fn)(void *))
{
    int tmpint;
    U8 *ptr1, *ptr2, *end = httpcmd + cmdlen;

    if(httpreqdata.httpstatus != HTTP_IDLE)
    {
        printf("httpstatus:%d\n", httpreqdata.httpstatus);
        return HTTPREQ_STILL_RUN;
    }

    ptr1 = httpcmd;
    httpreqdata.rsplen = 0;
    httpreqdata.httprsp[0] = 0;
    if(memcmp(ptr1, "http://", 7) != 0)
    {
        printf("1:%s\n", ptr1);
        return HTTPREQ_CMD_ERROR;
    }
 
    //Go to get remote ip information
    ptr1 += 7;
    ptr2 = ptr1;
    while(ptr2 < end)
    {
        if((*ptr2 == ':') || (*ptr2 == '/'))
        {
            if(ptr2 - ptr1 > 16)
                return HTTPREQ_STILL_RUN;
            else
            {
                memcpy(httpreqdata.ipstr, ptr1, ptr2 - ptr1);
                httpreqdata.ipstr[ptr2 - ptr1] = 0;
                ptr1 = ptr2;
                if(uiplib_ipaddrconv(httpreqdata.ipstr, &httpreqdata.ip_addr) == 0)
                    return HTTPREQ_STILL_RUN;
            }
            break;
        }
        ptr2++;
    }

    //Go to check there's port information in command 
    if(ptr1[0] == ':')
    {
        ptr1++;
        tmpint = 0;
        while(ptr1 < end)
        {
            if('0' <= ptr1[0] && ptr1[0] <= '9')
            {
                tmpint = tmpint * 10 + ptr1[0] - '0';
                ptr1++;
            }
            else
                break;
        }

        if((ptr1 >= end) || (ptr1[0] != '/') || (tmpint <= 0) || (tmpint >= 65536))
        {
            printf("2:%s, port:%d\n", ptr1 < end, tmpint);
            return HTTPREQ_CMD_ERROR;
        }
        else
            httpreqdata.port = tmpint;
    }
    else if(ptr1[0] == '/')
    {
        httpreqdata.port = 80;
    }
    else
    {
        printf("3:%s\n", ptr1);
        return HTTPREQ_CMD_ERROR;
    }

    httpreqdata.callbackfn = fn;
    //generate http get command
    ptr2 = httpreqdata.httpcmd;
    memcpy(ptr2 , "GET ", 4); ptr2 += 4;
    memcpy(ptr2, ptr1, (end - ptr1)); ptr2 += (end - ptr1);
    tmpint = strlen(" HTTP/1.1\r\nHOST: ");
    memcpy(ptr2 , " HTTP/1.1\r\nHOST: ", tmpint); ptr2 += tmpint;
    memcpy(ptr2 , httpreqdata.ipstr, strlen(httpreqdata.ipstr)); ptr2 += strlen(httpreqdata.ipstr);
    tmpint = strlen("\r\nAuthorization: Basic YWRtaW46YWRtaW4=");
    memcpy(ptr2 , "\r\nAuthorization: Basic YWRtaW46YWRtaW4=", tmpint); ptr2 += tmpint;
    tmpint = strlen("\r\nUser-Agent: ICOMMHTTP/1.0\r\n\r\n");
    memcpy(ptr2 , "\r\nUser-Agent: ICOMMHTTP/1.0\r\n\r\n", tmpint); ptr2 += tmpint;
    httpreqdata.cmdlen = ptr2 - httpreqdata.httpcmd;
    ptr2 = 0;

    printf("ip:%s, port:%d, cmd:%s\n", httpreqdata.ipstr, httpreqdata.port, ptr1);
    printf("len:%d, cmd:%s\n", httpreqdata.cmdlen, httpreqdata.httpcmd);
    httpreqdata.httpsock = tcpconnect( &httpreqdata.ip_addr, httpreqdata.port, &http_req_process);

    return HTTPREQ_SUCC;
}
/*---------------------------------------------------------------------------*/
void httprsp_parse(U8 *httprsp, U16 rsplen)
{
    U8 *ptr1, *ptr2, *end = httprsp + rsplen;
    int contentlen = 0;

    printf("rsp:%s\n\n\n", httprsp);
    ptr1 = httprsp;
    if(memcmp(ptr1, "HTTP/1.1 200 OK\r\n", 17) != 0)
    {
        //notigy upper layer response error
        while(ptr1 < end)
        {
            if(ptr1[0] == '\r' && ptr1[1] == '\n')
            {
                ptr1[0] = 0;
                httpmsg.msgtype = HTTPREQ_RSP_ERROR;
                httpmsg.rsp = httprsp;
                httpmsg.rsplen = ptr1 - httprsp;
                httpreqdata.callbackfn(&httpmsg);
                break;
            }
            ptr1++;
        }
        return;
    }

    //get content length
    ptr1 += 17;
    while(ptr1 < end)
    {
        if(*ptr1 == 'C' && (memcmp(ptr1, "Content-Length: ", 16) == 0))
        {
            ptr1 += 16;
            while(ptr1 < end)
            {
                if('0' <= ptr1[0] && ptr1[0] <= '9')
                {
                    contentlen = contentlen * 10 + ptr1[0] - '0';
                    ptr1++;
                }
                else
                    break;
            }
            //printf("contentlen:%d\n", contentlen);
            break;
        }
        ptr1++;
    }

    //find the content start position
    while(ptr1 < end)
    {
        if(ptr1[0] == '\r' && ptr1[1] == '\n' && ptr1[2] == '\r' && ptr1[3] == '\n')
        {
            ptr1 += 4;
            ptr1[contentlen] = 0;
            httpmsg.msgtype = HTTPREQ_RSP_DATA;
            httpmsg.rsp = ptr1;
            httpmsg.rsplen = end - ptr1;
            httpreqdata.callbackfn(&httpmsg);
            //printf("rsp:%s\n", ptr1);
            break;
        }
        ptr1++;
    }

    if(ptr1 = end)
    {
        httpmsg.msgtype = HTTPREQ_RSP_ERROR;
        httpmsg.rsp = NULL;
        httpreqdata.callbackfn(&httpmsg);
    }
    //printf("end\n");
}
/*---------------------------------------------------------------------------*/
