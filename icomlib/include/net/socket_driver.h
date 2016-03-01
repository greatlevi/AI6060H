#ifndef _SOCKET_DRIVER_H_
#define _SOCKET_DRIVER_H_

#define ENABLE_UIP_BUFFER 0
#define UIP_BUFFER_SIZE 1500

enum{
	TCP_CLOSE,
	TCP_CONNECTING,
	TCP_CONNECTED,
};

typedef enum t_SOCKETSTATE{
	SOCKET_CREATE,			//TCP socket is create.
	SOCKET_CONNECTED,		//TCP socket is connected.
	SOCKET_CLOSED,			//TCP connection is closed.
	SOCKET_SENDACK,			//The send data procedure is completed.
	SOCKET_NEWDATA,			//There is new data coming.
	SOCKET_NEWCONNECTION,	//A new connection from listening port is created.
}SOCKETSTATE;

typedef struct t_SOCKETMSG
{
	NETSOCKET 	socket;
	U16		  	lport;
	SOCKETSTATE status;
}SOCKETMSG;

void set_tcp_timeout(U16 interval);
    
/**
 * @brief Get IP address of the peer side. 
 * @param socket		[IN] Socket number of the connection.
 * @param ripaddr		[OUT] The IP address of the peer side.
 * @return -1 : Get IP address fail. 0 : Get IP address successful.
 */
int getpeerIPv4addr(NETSOCKET socket, uip_ipaddr_t *ipv4addr);

/**
 * @brief Connect device to the peer side with TCP protocol. 
 * @param ripaddr		[IN] The IP address of the receiver.
 * @param rport	    	[IN] The TCP port of the receiver, in host byte order.
 * @param callbackproc	[IN] The process of this connection to receive massge from TCPIP layer.
 * @return Socket number(0 ~ 9). -1 : Create socket fail, uip only allow ten
 *                                    connections at the same time.
 *
 * This function only create the TCP socket and start the three-way handshake procedure.
 * The connection is actually established when callback process get message SOCKET_CONNECTED .
 */
NETSOCKET tcpconnect(uip_ipaddr_t *ripaddr, uint16_t rport, struct process *callbackproc);

/**
 * @brief Close the TCP connection. 
 * @param socket		[IN] Socket number of the connection.
 * @return -1 : Close connection fail. 0 : Close connection successful.
 */
int tcpclose(NETSOCKET socket);

/**
 * @brief Open a TCP port for listening. 
 * @param port			[IN] The TCP port for listening, in host byte order.
 * @param callbackproc	[IN] The process to receive massge from TCPIP layer.
 * @return -1 : Open listening port fail. 0 :  Open listening port successful.
 *
 * When a new connection established, the callback process will get message SOCKET_NEWCONNECTION.
 * Application can get socket number of the connection from the message (SOCKETMSG).
 * The callback process of the new connection will bind with the process of the tcplisten function.
 * If the new connection want to bind another proess, please call tcpattach function. 
 */
int tcplisten(U16 port, struct process *callbackproc);

/**
 * @brief Close a TCP port for listening. 
 * @param port		[IN] The TCP port for listening, in host byte order.
 * @returnnone.
 */
void tcpunlisten(U16 port);

/**
 * @brief Send data to the peer side. 
 * @param socket	[IN] The socket number of the connection.
 * @param data		[IN] the data buffer.
 * @param datalen	[IN] the size of the data buffer.
 * @return -1 : Send data fail. 0 : Send data successful.
 *
 * This function only start the send data procedure. The send procedure is done when the callback
 * process get message SOCKET_SENDACK. Please do not modify the data in the data buffer before the
 * process get message SOCKET_SENDACK.
 */
int tcpsend(NETSOCKET socket, char *data, U16 datalen);

/**
 * @brief Receive data to the peer side. 
 * @param socket	[IN] The socket number of the connection.
 * @param data		[OUT] the data buffer.
 * @param datalen	[IN] the size of the data buffer.
 * @return -1 : Receive data fail. >= 0 : Receive data successful and return value is the number of bytes received.
 *
 * When the process get message SOCKET_NEWDATA means there's data coming. Please get the data from
 * TCPIP layer right now.
 */
int tcprecv(NETSOCKET socket, char *data, U16 datalen);

/**
 * @brief Bind the another callback process. 
 * @param socket		[IN] The socket number of the connection.
 * @param callbackproc	[IN] The process to receive massge from TCPIP layer.
 * @return -1 : Bind process fail. 0 : Bind process successful.
 */
int tcpattch(NETSOCKET socket, struct process *callbackproc);

/**
 * @brief Creat a UDP socket. 
 * @param lport	    	[IN] The local UDP port, in host byte order.
 * @param callbackproc	[IN] The process of this connection to receive massge from TCPIP layer.
 * @return Socket number(10 ~ 19). -1 : Create socket fail, uip only allow ten
 *                                      connections at the same time.
 *
 * This function create a UDP socket. Driver will random assign a port to this socket if the lport is zero.
 */
NETSOCKET udpcreate(U16 lport, struct process *callbackproc);

/**
 * @brief Close the UDP connection. 
 * @param socket		[IN] Socket number of the connection.
 * @return -1 : Close connection fail. 0 : Close connection successful.
 */
int udpclose(NETSOCKET socket);

/**
 * @brief Send data to the peer side. 
 * @param socket	[IN] The socket number of the connection.
 * @param data		[IN] the data buffer.
 * @param datalen	[IN] the size of the data buffer.
 * @param peeraddr	[IN] the ip address of peer side.
 * @param peerport	[IN] the port number of peer side, in host byte order.
 * @return -1 : Send data fail. 0 : Send data successful.
 */
int udpsendto(NETSOCKET socket, char *data, U16 datalen, uip_ipaddr_t *peeraddr, U16 peerport);

/**
 * @brief Receive data to the peer side. 
 * @param socket	[IN] The socket number of the connection.
 * @param data		[OUT] the data buffer.
 * @param datalen	[IN] the size of the data buffer.
 * @param peeraddr	[OUT] the ip address of peer side.
 * @param peerport	[OUT] the port number of peer side, in host byte order.
 * @return -1 : Receive data fail. >= 0 : Receive data successful and return value is the number of bytes received.
 *
 * When the process get message SOCKET_NEWDATA means there's data coming. Please get the data from
 * TCPIP layer right now.
 */
int udprecvfrom(NETSOCKET socket, char *data, U16 datalen, uip_ipaddr_t *peeraddr, U16 *peerport);

#endif
