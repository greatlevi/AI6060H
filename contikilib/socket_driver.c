#include <stdlib.h>
#include "ssv_types.h"
#include "uipopt.h"
#include "uip.h"
#include "socket_driver.h"
#include "ssv_lib.h"
#include "uip-udp-packet.h"

extern struct uip_udpip_hdr *UIP_IP_BUF;
U16 recvoffset = 0;
PROCESS(tcp_driver_process, "Socket driver Process");
PROCESS(udp_driver_process, "Udp driver Process");

int getpeerIPv4addr(NETSOCKET socket, uip_ipaddr_t *ipv4addr)
{
	struct uip_conn *conn;
	
	conn = uip_gettcpconn(socket);

	if(conn)
	{
		memcpy(ipv4addr, &conn->ripaddr, sizeof(uip_ipaddr_t));
		return 0;
	}
	else
	    return -1;
}

void set_tcp_timeout(U16 interval) {
    change_periodic_tcp_timer(interval);
}

int tcpsend(NETSOCKET socket, char *data, U16 datalen)
{
	struct uip_conn *conn;
	
	if(data == NULL || (datalen == 0))
		return -1;

	conn = uip_gettcpconn(socket);
#if ENABLE_UIP_BUFFER
    if(conn)
    {
        if(datalen > (UIP_BUFFER_SIZE - conn->appstate.datalen))
            datalen = (UIP_BUFFER_SIZE - conn->appstate.datalen);

        dma_memcpy(conn->appstate.data + conn->appstate.datalen, data, datalen);
        conn->appstate.datalen += datalen;
            
        if(conn->appstate.sendlen == 0)
            tcpip_poll_tcp(conn);
        
        return datalen;
    }
#else
	if(conn && (conn->appstate.data == NULL))
	{
		conn->appstate.data = data;
		conn->appstate.datalen = datalen;
		conn->appstate.sendlen = 0;
		tcpip_poll_tcp(conn);
        return datalen;
    }
#endif
	else
		return -1;
}

int tcprecv(NETSOCKET socket, char *data, U16 datalen)
{
    int recvlen;
	if(data == NULL || (recvoffset >= uip_datalen()))
		return -1;

	if(uip_checktcpsocket(socket) == 0)
	{
        if(datalen >= uip_datalen())
            recvlen = uip_datalen();
        else
            recvlen = datalen;
        
		memcpy(data, (char*)uip_appdata + recvoffset, recvlen);
        recvoffset += recvlen;
		return recvlen;
	}
	else
		return -1;
}

void tcpevent_handle(struct uip_conn *conn)
{
	uip_tcp_appstate_t *appstate = &conn->appstate;
	static SOCKETMSG msg;

	msg.socket = appstate->soketnum;
	msg.lport = uip_htons(conn->lport);
	if(uip_connected()) 
	{
		if(appstate->tcpappflag == TCP_CONNECTING)
		{
			appstate->tcpappflag = TCP_CONNECTED;
			
			//process_post to notify process the TCP is connected.
			if(appstate->appp)
			{
				msg.status = SOCKET_CONNECTED;
				process_post_synch(appstate->appp, PROCESS_EVENT_MSG, &msg);
			}
		}
		else if(appstate->tcpappflag == TCP_CLOSE)
		{
			appstate->tcpappflag = TCP_CONNECTED;
            appstate->sendlen = 0;
#if ENABLE_UIP_BUFFER
           appstate->data = (char *)malloc(UIP_BUFFER_SIZE);
#endif
            
			//process_post to notify process the TCP connection is coming.
			if(appstate->appp)
			{
				msg.status = SOCKET_NEWCONNECTION;
				process_post_synch(appstate->appp, PROCESS_EVENT_MSG, &msg);
			}
		}
	}

	if(uip_acked() && (appstate->sendlen > 0)) 
	{
		appstate->datalen -= appstate->sendlen;
		if(appstate->datalen == 0) 
		{
#if !ENABLE_UIP_BUFFER
			appstate->data = NULL;
#endif			
			//process_post to notify process the TCP data is transmitted.
			if(appstate->appp)
			{
				msg.status = SOCKET_SENDACK;
				process_post_synch(appstate->appp, PROCESS_EVENT_MSG, &msg);
			}
		} 
		else 
		{
#if ENABLE_UIP_BUFFER
            memcpy(appstate->data, appstate->data + appstate->sendlen, appstate->datalen);
#else
			appstate->data += appstate->sendlen;
#endif
		}
		appstate->sendlen = 0;
	}
	if(uip_newdata()) 
	{
		//process_post to notify process there are incoming data.
		recvoffset = 0;
		if(appstate->appp)
		{
			msg.status = SOCKET_NEWDATA;
			process_post_synch(appstate->appp, PROCESS_EVENT_MSG, &msg);
		}
	}
  
	if(uip_rexmit()) 
	{
		uip_send(appstate->data, appstate->sendlen);
	} 
	else if(uip_newdata() || uip_acked() || uip_poll())
	{
		//Check is any data need to be send and there is no data transmission mow.
		if((appstate->data != NULL) && (appstate->sendlen == 0)) 
		{
			if(appstate->datalen > uip_mss()) 
			{
				appstate->sendlen = uip_mss();
			} 
			else 
			{
				appstate->sendlen = appstate->datalen;
			}
			uip_send(appstate->data, appstate->sendlen);
		}
	}
	
	if(uip_closed() || uip_aborted() || uip_timedout()) 
	{
#if ENABLE_UIP_BUFFER
        if(conn->appstate.data)
            free(conn->appstate.data);
#endif
		appstate->tcpappflag = TCP_CLOSE;
		appstate->datalen = 0;
		appstate->data = NULL;
	}

	if(appstate->tcpappflag == TCP_CLOSE) 
	{
		uip_close();
		//process_post to notify process the TCP conection is closed.
		if(((conn->tcpstateflags == UIP_ESTABLISHED) || (conn->tcpstateflags == UIP_SYN_SENT)) && appstate->appp)
		{
			msg.status = SOCKET_CLOSED;
			process_post_synch(appstate->appp, PROCESS_EVENT_MSG, &msg);
		}
	}
}

NETSOCKET tcpconnect(uip_ipaddr_t *ripaddr, uint16_t rport, struct process *callbackproc)
{
	struct uip_conn *conn;

	if(ripaddr == NULL)
		return -1;
	
	conn = tcp_connect_icomm(ripaddr, uip_htons(rport), &tcp_driver_process, callbackproc);

	if(conn)
	{
#if ENABLE_UIP_BUFFER
        conn->appstate.data = (char *)malloc(UIP_BUFFER_SIZE);
#endif
		conn->appstate.tcpappflag = TCP_CONNECTING;
		conn->appstate.sendlen = 0;
		conn->appstate.datalen = 0;
		return conn->appstate.soketnum;
	}
	else
		return -1;
}

int tcpclose(NETSOCKET socket)
{
	struct uip_conn *conn;
	conn = uip_gettcpconn(socket);

	if(conn)
	{
#if ENABLE_UIP_BUFFER
        if(conn->appstate.data)
            free(conn->appstate.data);
#endif
		conn->appstate.tcpappflag = TCP_CLOSE;
		conn->appstate.datalen = 0;
		conn->appstate.data = NULL;
		return 0;
	}
	else
		return -1;
}

int tcplisten(U16 port, struct process *callbackproc)
{
	return tcp_listen_icomm(uip_htons(port), &tcp_driver_process, callbackproc);
}

void tcpunlisten(U16 port)
{
	tcp_unlisten(uip_htons(port));
}

int tcpattch(NETSOCKET socket, struct process *callbackproc)
{
	struct uip_conn *conn;
	conn = uip_gettcpconn(socket);

	if(conn)
	{
		conn->appstate.appp = callbackproc;
		return 0;
	}
	else
		return -1;
}

NETSOCKET udpcreate(U16 lport, struct process *callbackproc)
{
	struct uip_udp_conn *conn;
	conn = udp_new_icomm(&udp_driver_process, callbackproc);

	if(conn)
	{
		if(lport > 0)
			udp_bind(conn, uip_htons(lport));
		
		return conn->appstate.soketnum;
	}
	else
		return -1;
}

int udpclose(NETSOCKET socket)
{
	struct uip_udp_conn *conn;
	conn = uip_getudpconn(socket);

	if(conn)
	{
		conn->lport = 0;
		return 0;
	}
	else
		return -1;
}

int udpsendto(NETSOCKET socket, char *data, U16 datalen, uip_ipaddr_t *peeraddr, U16 peerport)
{
	struct uip_udp_conn *conn;
	int sendlen;
	
	if(peeraddr == NULL || peerport == 0)
		return -1;
	
	conn = uip_getudpconn(socket);
	if(conn)
	{
		while(0 < datalen)
		{
			if(datalen > MAX_UDP_SIZE)
			{
				sendlen = MAX_UDP_SIZE;
			}
			else
			{
				sendlen = datalen;
			}
			uip_udp_packet_sendto(conn, data, sendlen, peeraddr, uip_htons(peerport));
			datalen -= sendlen;
			data += sendlen;
		}
		return 0;
	}
	else
		return -1;
}

int udprecvfrom(NETSOCKET socket, char *data, U16 datalen, uip_ipaddr_t *peeraddr, U16 *peerport)
{
	if(data == NULL || (datalen < uip_datalen()))
		return -1;
	
	if(uip_checkudpsocket(socket) == 0)
	{
		memcpy(data, (char*)uip_appdata, uip_datalen());
		if(peeraddr != NULL)
			memcpy(peeraddr, &(UIP_IP_BUF->srcipaddr), sizeof(uip_ipaddr_t));
		if(peerport != NULL)
			*peerport = UIP_HTONS(UIP_IP_BUF->srcport);
		return uip_datalen();
	}
	else
		return -1;
}

PROCESS_THREAD(tcp_driver_process, ev, data)
{
	static struct uip_conn *currconn;
	
	PROCESS_BEGIN();
	while(1) 
	{
		PROCESS_WAIT_EVENT();

		if( ev == PROCESS_EVENT_EXIT) 
		{
			break;
		} 
		else if(ev == tcpip_event) 
		{
			currconn = uip_getcurrtcpconn();
			tcpevent_handle(currconn);
		}	
	} 
	PROCESS_END();
}

PROCESS_THREAD(udp_driver_process, ev, data)
{
	static struct uip_udp_conn *currconn;
	static SOCKETMSG msg;	
	
	PROCESS_BEGIN();
	while(1) 
	{
		PROCESS_WAIT_EVENT();

		if( ev == PROCESS_EVENT_EXIT) 
		{
			break;
		} 
		else if(ev == tcpip_event) 
		{
			currconn = uip_getcurrudpconn();

			if(uip_newdata())
			{
				msg.socket = currconn->appstate.soketnum;
				msg.lport = uip_htons(currconn->lport);
				msg.status = SOCKET_NEWDATA;

				if(currconn->appstate.appp)
					process_post_synch(currconn->appstate.appp, PROCESS_EVENT_MSG, &msg);
			}
		}	
	} 
	PROCESS_END();
}


