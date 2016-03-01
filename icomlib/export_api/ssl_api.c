#include "ssl_api.h"
#include "process.h"
#include "socket_driver.h"

#define MAX_SSL_CONNECTION 2

typedef struct t_SSLRECORD
{
	ssl_t 	*sslctx;
	int     sendlen;
	int     recvlen;
    unsigned char *recvbuf;
}SSLRECORD;

static SSLRECORD    gsslrecord[MAX_SSL_CONNECTION];

PROCESS(sslhandlerprocess, "ssl handler process");

/****************************** Local Functions *******************************/
static int32 insertSSLCTX(ssl_t *ssl)
{
	U8 i;

    for(i = 0; i < MAX_SSL_CONNECTION; i++)
    {
        if(gsslrecord[i].sslctx == NULL)
        {
            gsslrecord[i].sslctx = ssl;
            gsslrecord[i].sendlen = 0;
            gsslrecord[i].recvlen = 0;
            return PS_SUCCESS;
        }
    }
	return PS_FAILURE;
}
/****************************** Local Functions *******************************/
static int32 removeSSLCTX(ssl_t *ssl)
{
	U8 i;

    for(i = 0; i < MAX_SSL_CONNECTION; i++)
    {
        if(gsslrecord[i].sslctx == ssl)
        {
            gsslrecord[i].sslctx = NULL;
            gsslrecord[i].sendlen = 0;
            gsslrecord[i].recvlen = 0;
            return PS_SUCCESS;
        }
    }
	return PS_FAILURE;
}
/****************************** Local Functions *******************************/
static SSLRECORD *getSSLRECORD(NETSOCKET socket)
{
	U8 i;

    for(i = 0; i < MAX_SSL_CONNECTION; i++)
    {
        if(gsslrecord[i].sslctx->socket == socket)
        {
            return &gsslrecord[i];
        }
    }
	return NULL;
}
/****************************** Local Functions *******************************/
static int32 extensionCb(ssl_t *ssl, unsigned short extType,
							unsigned short extLen, void *e)
{
	unsigned char	*c;
	short			len;
	char			proto[128];

	c = (unsigned char*)e;

	if (extType == EXT_ALPN) {
		memset(proto, 0x0, 128);
		/* two byte proto list len, one byte proto len, then proto */
		c += 2; /* Skip proto list len */
		len = *c; c++;
		memcpy(proto, c, len);
		printf("Server agreed to use %s\n", proto);
	}
	return PS_SUCCESS;
}
/******************************************************************************/
/*
	Example callback to show possiblie outcomes of certificate validation.
	If this callback is not registered in matrixSslNewClientSession
	the connection will be accepted or closed based on the alert value.
 */
static int32 certCb(ssl_t *ssl, psX509Cert_t *cert, int32 alert)
{
#ifndef USE_ONLY_PSK_CIPHER_SUITE
	psX509Cert_t	*next;

	/* Did we even find a CA that issued the certificate? */
	if (alert == SSL_ALERT_UNKNOWN_CA) {
			/* Example to allow anonymous connections based on a define */
//		_psTraceStr("Allowing anonymous connection for: %s.\n",
//						cert->subject.commonName);
		return SSL_ALLOW_ANON_CONNECTION;
	}

	/*
 		If the expectedName passed to matrixSslNewClientSession does not
		match any of the server subject name or subjAltNames, we will have
		the alert below.
		For security, the expected name (typically a domain name) _must_
		match one of the certificate subject names, or the connection
		should not continue.
		The default MatrixSSL certificates use localhost and 127.0.0.1 as
		the subjects, so unless the server IP matches one of those, this
		alert will happen.
		To temporarily disable the subjet name validation, NULL can be passed
		as expectedName to matrixNewClientSession.
	*/
	if (alert == SSL_ALERT_CERTIFICATE_UNKNOWN) {
		_psTraceStr("ERROR: %s not found in cert subject names\n",
			ssl->expectedName);
	}

/*	if (alert == SSL_ALERT_CERTIFICATE_EXPIRED) {
		_psTrace("ERROR: A cert did not fall within the notBefore/notAfter window\n");
        alert = 0;
	}*/

	if (alert == SSL_ALERT_ILLEGAL_PARAMETER) {
		_psTrace("ERROR: Found correct CA but X.509 extension details are wrong\n");
	}

	/* Key usage related problems on chain */
	for (next = cert; next != NULL; next = next->next) {
		if (next->authStatus == PS_CERT_AUTH_FAIL_EXTENSION) {
			if (next->authFailFlags & PS_CERT_AUTH_FAIL_KEY_USAGE_FLAG) {
				_psTrace("CA keyUsage extension doesn't allow cert signing\n");
			}
			if (next->authFailFlags & PS_CERT_AUTH_FAIL_EKU_FLAG) {
				_psTrace("Cert extendedKeyUsage extension doesn't allow TLS\n");
			}
		}
	}

	if (alert == SSL_ALERT_BAD_CERTIFICATE) {
		/* Should never let a connection happen if this is set.  There was
			either a problem in the presented chain or in the final CA test */
		_psTrace("ERROR: Problem in certificate validation.  Exiting.\n");
	}


/*	if (g_trace && alert == 0) _psTraceStr("SUCCESS: Validated cert for: %s.\n",
		cert->subject.commonName);*/

#endif /* !USE_ONLY_PSK_CIPHER_SUITE */
	return alert;
}
/******************************************************************************/
static void SSL_rspdisconnect(ssl_t *sslctx)
{
    SSLMSG  sslmsg;

    sslmsg.sslctx = sslctx;
    sslmsg.status = SSL_CLOSED;
    process_post_synch(sslctx->callbackproc, PROCESS_EVENT_MSG, &sslmsg);
    SSL_disconnect(sslctx);
}
/******************************************************************************/
static void SSL_send(SSLRECORD *sslrecd)
{
    unsigned char *buf;
    
    sslrecd->sendlen = matrixSslGetOutdata(sslrecd->sslctx, &buf);
//    psTraceBytes("SEND", buf, sslrecd->sendlen);
    if(sslrecd->sendlen > 0)
    {
        tcpsend(sslrecd->sslctx->socket, buf, sslrecd->sendlen);
    }
}
/******************************************************************************/
static void SSL_recv(SSLRECORD *sslrecd)
{
    unsigned char *buf;
    int sslinlen, recvlen, rc;
    SSLMSG  sslmsg;
    ssl_t *sslctx = sslrecd->sslctx;
    
    if ((sslinlen = matrixSslGetReadbuf(sslctx, &buf)) <= 0) {
        return;
    }
    
    recvlen = tcprecv(sslctx->socket, buf, sslinlen);
    rc = matrixSslReceivedData(sslctx, (int)recvlen, &buf, (U32*)&sslinlen);
PROCESS_RSP:
    if(rc == MATRIXSSL_HANDSHAKE_COMPLETE)
    {
        //Notify APP the ssl handshake complete
        sslmsg.sslctx = sslctx;
        sslmsg.status = SSL_CONNECTED;
        process_post_synch(sslctx->callbackproc, PROCESS_EVENT_MSG, &sslmsg);
    }
    else if(rc == MATRIXSSL_APP_DATA || rc == MATRIXSSL_APP_DATA_COMPRESSED)
    {
        //Notify APP there's new data incoming
        if(sslinlen)
        {
            sslmsg.sslctx = sslctx;
            sslmsg.status = SSL_NEWDATA;
            sslmsg.recvlen = sslinlen;
            sslrecd->recvlen = sslinlen;
            sslrecd->recvbuf = buf;
            process_post_synch(sslctx->callbackproc, PROCESS_EVENT_MSG, &sslmsg);

            //clear ssl input buffer
            sslrecd->recvlen = 0;
        }
        
        rc = matrixSslProcessedData(sslctx, &buf, (uint32*)&sslinlen);
        goto PROCESS_RSP;
    }
    else if(rc == MATRIXSSL_REQUEST_SEND)
    {
        SSL_send(sslrecd);
    }
    else if(rc == MATRIXSSL_REQUEST_RECV)
    {
        //Do nothing, wait for others data coming.
        
    }
    else if(rc == MATRIXSSL_RECEIVED_ALERT)
    {
        if (*buf == SSL_ALERT_LEVEL_FATAL) {
            psTraceIntInfo("Fatal alert: %d, closing connection.\n", *(buf + 1));
            SSL_rspdisconnect(sslctx);
            return;
        }
        
        /* Closure alert is normal (and best) way to close */
        if (*(buf + 1) == SSL_ALERT_CLOSE_NOTIFY) {
            SSL_rspdisconnect(sslctx);
            return;
        }
        
        if ((rc = matrixSslProcessedData(sslctx, &buf, (uint32*)&sslinlen)) == 0) {
            /* No more data in buffer. Might as well read for more. */
        }
        else
            goto PROCESS_RSP;
    }
    else if(rc < 0)
    {
        SSL_rspdisconnect(sslctx);
    }
}

//Init ssl library
int SSL_init(void)
{
    int rc, i;
    
	if ((rc = matrixSslOpen()) < 0) {
		_psTrace("MatrixSSL library init failure.  Exiting\n");
		return PS_FAILURE;
	}

    for(i = 0; i < MAX_SSL_CONNECTION; i++)
    {
        gsslrecord[i].sslctx = NULL;
    }

    process_start(&sslhandlerprocess, NULL);

    return 0;
}

//load public keys from cetificate
int SSL_loadkeys(void)
{
    return 0;
}

PROCESS_THREAD(sslhandlerprocess, ev, data)
{
	SOCKETMSG   socketmsg;
    SSLMSG      sslmsg;
    SSLRECORD	*sslrecd;
    int rc;
    
    PROCESS_BEGIN();

    while(1)
    {
		PROCESS_WAIT_EVENT();
        
		if( ev == PROCESS_EVENT_EXIT) 
		{
			break;
		} 
		else if(ev == PROCESS_EVENT_MSG) 
		{
		    socketmsg = *(SOCKETMSG *)data;
            sslrecd = getSSLRECORD(socketmsg.socket);

            if(sslrecd == NULL)
                continue;
            
			//The TCP socket is connected, This socket can send data after now.
			if(socketmsg.status == SOCKET_CONNECTED)
			{
//				printf("socked:%d connected\n", socketmsg.socket);
                SSL_send(sslrecd);
			}
			//TCP connection is closed. Clear the socket number.
			else if(socketmsg.status == SOCKET_CLOSED)
			{
			    SSL_rspdisconnect(sslrecd->sslctx);
			}
			//Get ack, the data trasmission is done. We can send data after now.
			else if(socketmsg.status == SOCKET_SENDACK)
			{
                if ((rc = matrixSslSentData(sslrecd->sslctx, sslrecd->sendlen)) < 0) {
                    SSL_rspdisconnect(sslrecd->sslctx);
                }

                sslrecd->sendlen = 0;
                if (rc == MATRIXSSL_REQUEST_CLOSE) {
                    SSL_rspdisconnect(sslrecd->sslctx);
                }
                else if (rc == MATRIXSSL_HANDSHAKE_COMPLETE) {
                    //Notify APP the ssl handshake complete
                    sslmsg.sslctx = sslrecd->sslctx;
                    sslmsg.status = SSL_CONNECTED;
                    process_post_synch(sslrecd->sslctx->callbackproc, PROCESS_EVENT_MSG, &sslmsg);
                }
                else if (rc == MATRIXSSL_REQUEST_SEND) {
                    SSL_send(sslrecd);
                }
			}
			//There is new data coming. Receive data now.
			else if(socketmsg.status == SOCKET_NEWDATA)
			{
			    SSL_recv(sslrecd);
			}
			else
			{
				printf("unexpect message type:%d\n", socketmsg.status);
			}
        }
    }

    PROCESS_END();
}

//Do the SSL handshake
ssl_t *SSL_connect(struct process *callbackproc, uip_ipaddr_t peeraddr, U16 port)
{
	sslSessOpts_t	options;
    sslSessionId_t *sid = NULL;
    unsigned char	*buf, *ext;
	tlsExtension_t	*extension;
	int			    rc, extLen;
    unsigned char   peeraddrstr[16];
    U32             ciphers[4];
    int             ciphercnt;
    ssl_t           *sslctx;

    if(callbackproc == NULL)
        return;
    
	memset(&options, 0x0, sizeof(sslSessOpts_t));
	options.versionFlag = SSL_FLAGS_TLS_1_2;
	options.userPtr = NULL;

    memset(peeraddrstr, 0, sizeof(peeraddrstr));
    sprintf(peeraddrstr, "%d.%d.%d.%d", peeraddr.u8[0], peeraddr.u8[1], peeraddr.u8[2], peeraddr.u8[3]);

	matrixSslNewHelloExtension(&extension, NULL);
	matrixSslCreateSNIext(NULL, (unsigned char*)peeraddrstr, (uint32)strlen(peeraddrstr),	&ext, &extLen);
	matrixSslLoadHelloExtension(extension, ext, extLen, EXT_SNI);
	free(ext);

    ciphercnt = 4;
    ciphers[0] = TLS_RSA_WITH_AES_128_CBC_SHA;
    ciphers[1] = TLS_RSA_WITH_AES_256_CBC_SHA;
    ciphers[2] = TLS_RSA_WITH_AES_128_CBC_SHA256;
    ciphers[3] = TLS_RSA_WITH_AES_256_CBC_SHA256;
    
    rc = matrixSslNewClientSession(&sslctx, NULL, sid, ciphers, ciphercnt,
        certCb, peeraddrstr, extension, extensionCb, &options);

	matrixSslDeleteHelloExtension(extension);
	if (rc != MATRIXSSL_REQUEST_SEND) {
		_psTraceInt("New Client Session Failed: %d.  Exiting\n", rc);
        SSL_rspdisconnect(sslctx);
		return NULL;
	}

    if(insertSSLCTX(sslctx) == PS_FAILURE)
        return NULL;

    sslctx->callbackproc = callbackproc;
    sslctx->socket = tcpconnect(&peeraddr, port, &sslhandlerprocess);

    return sslctx;
}

//Send data into https connection
int SSL_write(ssl_t *sslctx, U8 *data, int datalen)
{
    int tmplen;
    unsigned char *buf;
    SSLRECORD	*sslrecd;

    if(sslctx == NULL && sslctx->hsState != SSL_HS_DONE)
        return -1;
    
    sslrecd = getSSLRECORD(sslctx->socket);
    if(sslrecd == NULL)
        return -1;
    
	if ((tmplen = matrixSslGetWritebuf(sslctx, &buf, datalen)) < 0) {
		return PS_FAILURE;
	}

    if(tmplen < datalen)
	    datalen = tmplen;
    
    memcpy(buf, data, datalen);
    
	if((matrixSslEncodeWritebuf(sslctx, datalen)) < 0) {
		return -1;
	}

    sslrecd = getSSLRECORD(sslctx->socket);
    SSL_send(sslrecd);

    return datalen;
}

//Clear SSL context buffer after tcp ack
int SSL_Handle_writeAck(ssl_t *sslctx)
{
    return matrixSslSentData(sslctx, sslctx->outlen);
}

//Read data from https connection
int SSL_read(ssl_t *sslctx, char *buf, int buflen)
{
    SSLRECORD	*sslrecd;

    sslrecd = getSSLRECORD(sslctx->socket);

//    printf("read len:%d\n", sslrecd->recvlen);
    if(sslrecd->recvlen <= buflen)
    {
        memcpy(buf, sslrecd->recvbuf, sslrecd->recvlen);
    }
    else
        return -1;

    return sslrecd->recvlen;
}

//Close the SSL connection
void SSL_disconnect(ssl_t *sslctx)
{
    if(removeSSLCTX(sslctx) == PS_SUCCESS)
    {
        tcpclose(sslctx->socket);
        sslctx->socket = -1;
        sslctx->callbackproc = NULL;
    	matrixSslDeleteSessionId(sslctx->sid);
    	matrixSslDeleteSession(sslctx);
        sslctx = NULL;
    }
}

//Deinit ssl library
void SSL_deinit(void)
{
    U8 i;
    
	matrixSslClose();
    
    for(i = 0; i < MAX_SSL_CONNECTION; i++)
    {
        gsslrecord[i].sslctx = NULL;
    }

    process_exit(&sslhandlerprocess);
}

