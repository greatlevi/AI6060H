/* 
 * Copyright (c) 2010, Swedish Institute of Computer Science. 
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 * 
 */

/** 
 * \file 
 *         Common code between collect client and server 
 * \author 
 *         Niclas Finne <nfi@sics.se> 
 */

#ifndef __CABRIO_CONF_H__
#define __CABRIO_CONF_H__

#include "contiki.h"
#include "contiki-net.h"
#include "svn_ver.h"

#include "ssv_types.h"

/* Define debug flag */
/**
#define DEBUG_LOG	1
#if DEBUG_LOG
#define DEBUG_MSG	printf(const char * fmt,...)
#else
#define DEBUG_MSG
*/

/* Define if ATE mode involve into this process */
#define OPMODE_ATE			1
#define AT_ECHO				0
#define CURRENT_TEST		1

#define MAX_SEND_BUFFER 	1472

#define CABRIO_MANUFACTURE	"iComm Company"
//#define CABRIO_VERISON			"SSV6060.Z0.1001.150522"
#define CABRIO_VERISON			"SSV6060.Z0" SVN_VERSION ".0" 

/* ATE command definition section */
/* ATE command for receiving from host UART */
#define ATCMD_REBOOT			"AT+REBOOT"
#define ATCMD_VERSION			"AT+VERSION=?"
#define ATCMD_MINFO			"AT+MF_INFO=?"
#define ATCMD_GETCONFIG		"AT+GET_CONFIG=?"
#define ATCMD_SETIFCONFIG		"AT+SET_IFCONFIG="
#define ATCMD_SETWIFICONFIG	"AT+SET_WIFICONFIG="
#define ATCMD_NETSTATUS		"AT+NETSTATUS=?"
#define ATCMD_NETSCAN			"AT+SCAN=?"
#define ATCMD_CLEAN_LISTS			"AT+CLEAN_LISTS"
#define ATCMD_NETSCAN_CUSTOM	"AT+SCAN_CUSTOM=?"
#define ATCMD_SHOWCONNECTAP	"AT+SHOWCONNECTAP"
#define ATCMD_SHOWALLAP		"AT+SHOWALLAP"
#define ATCMD_CONNECT			"AT+WIFICONNECT"
#define ATCMD_SEND				"AT+SEND="
#define ATCMD_DISCONNECT		"AT+WIFIDISCONNECT"

//TCP
#define ATCMD_TCPCHANGETIMEOUT	"AT+TCPCHANGE_TIMEOUT="
#define ATCMD_TCPCONNECT		"AT+TCPCONNECT="
#define ATCMD_TCPSEND		       "AT+TCPSEND="
#define ATCMD_TCPDISCONNECT	"AT+TCPDISCONNECT="
#define ATCMD_TCPLISTEN	    "AT+TCPLISTEN="
#define ATCMD_TCPUNLISTEN	    "AT+TCPUNLISTEN="

#define ATCMD_UDPCREATE		"AT+UDPCREATE="
#define ATCMD_UDPSEND		"AT+UDPSEND="
#define ATCMD_UDPCLOSE  	"AT+UDPCLOSE="

#define ATCMD_HTTPTESTSTART		"AT+HTTPTESTSTART="
#define ATCMD_HTTPTESTSTOP		"AT+HTTPTESTSTOP"

#define ATCMD_SET_GPIO			"AT+SET_GPIO="
#define ATCMD_WRITE_GPIO		"AT+WRITE_GPIO="
#define ATCMD_READ_GPIO		"AT+READ_GPIO="
#define ATCMD_SET_PWM			"AT+SET_PWM="
#define ATCMD_POWERSAVE			"AT+POWERSAVE="

//OTA
#define ATCMD_OTASETSERVER		"AT+OTASETSERVER="
#define ATCMD_OTASETPARAM			"AT+OTASETPARAM="
#define ATCMD_OTASTART				"AT+OTASTART"

#define ATCMD_ENABLE_SMARTMODE		"AT+ENABLE_SMARTMODE="
#define ATCMD_ENABLE_SMARTREBOOT	"AT+ENABLE_SMARTREBOOT="

//### following are not AT CMD,they are for debugging ###
#define ATCMD_HELP                     "help"
#define ATCMD_WRITE_REG		"regw "
#define ATCMD_READ_REG		"regr "
#define ATCMD_SET_CHANNEL		"setchannel "
#define ATCMD_RF		             "rf"
#define ATCMD_APSTATUS		"ap"
#define ATCMD_WIFISTATUS		"wifi"
#define ATCMD_MRX                       "mrx"
#define ATCMD_PING		              "ping"
#define ATCMD_NETSTAT		       "netstat"
#define ATCMD_IPCONF		"ifconfig"
#define ATCMD_FLASH_WRITE             "flashw"
#define ATCMD_FLASH_READ             "flashr"
#define ATCMD_FLASH_ERASE             "flashe"

#define ATCMD_BENCH_FLASH_READ             "bflashr"
#define ATCMD_BENCH_FLASH_WRITE             "bflashw"


#define ATCMD_PHY_COUNT		             "phycount"
#define ATCMD_MEM_READ             "memr"
#define ATCMD_MBOX                     "mbox"
#define ATCMD_WIFI_RESET            "wifi_reset"
#define ATCMD_NTP_TIME            "ntptime"


//watch dog 

#define ATCMD_WDOG_START		             "wdog_start"
#define ATCMD_WDOG_STOP		             "wdog_stop"


#if CURRENT_TEST
#define ATCMD_POWERON				"AT+POWERON"
#define ATCMD_RX					"AT+RX"
#define ATCMD_TX					"AT+TX"
#define ATCMD_POWERSAVING			"AT+POWERSAVING="
#endif

#define ATCMD_REMOVE_CONF			"AT+REMOVE_CONF"
#define ATCMD_GET_LOCALMAC			"AT+GET_LOCALMAC=?"
#define ATCMD_DEBUG_PRINTF			"log="
#define ATCMD_PROCESS_DUMP			"ppd"
#define ATCMD_PROCESS_KILL			"ppk="
#define ATCMD_PROCESS_QUE_DUMP		"pqd"
#define ATCMD_PROCESS_QUE_KILL		"pqk="

//
#define ATCMD_BMODE_RESET			"breset"
#define ATCMD_BMODE_COUNT			"bcount"
#define ATCMD_GMODE_RESET			"greset"
#define ATCMD_GMODE_COUNT		"gcount"
#define ATCMD_FILTER_RESET			"freset"
#define ATCMD_FILTER_COUNT		"fcount"

#define ATCMD_NSLOOKUP			"AT+NSLOOKUP="
/* ATE response command for sending to host uart */
#define	ATRSP_OK                    	"+OK\r\n"
#define	ATRSP_ERROR                 	"+ERROR:%d\r\n"
#define	ATRSP_ERROR_STR           	"+ERROR:%s\r\n"
#define	ATRSP_VERSION               "+VERSION:%s\r\n"
#define	ATRSP_MANUFACTURE	"+MF_INFO:%s\r\n"
#define	ATRSP_CONFIGURE		"+GET_CONFIG=%d,%s,%s,%d,%d,%d.%d.%d.%d,%d.%d.%d.%d,%d.%d.%d.%d\r\n"
#define	ATRSP_NETSTATUS		"+NETSTATUS=%d,%d,%d\r\n"
#define   ATRSP_GPIO				"+GPIO=%d\r\n"

/*ATE indication command for sending to host uart */
#define	ATIND_SCAN			"+SCAN:%s\r\n"
#define	ATIND_RECV			"+RECV:%s\r\n"
#define	ATIND_CONNECT		"+CONNECT:%s\r\n"
#define	ATIND_DISCONNECT	"+DISCONNECT:%s\r\n"
#define	ATIND_GPIO			"+GPIO:%d\r\n"

//ATE phy calibration command
#define ATCMD_PHY_INI				"AT+PHYINI"
#define ATCMD_PHY_CALI				"AT+PHYCALI="
#define ATCMD_EVM					"AT+EVM="
#define ATCMD_TONE					"AT+TONE="
#define ATCMD_SETCH					"AT+SETCH="

//SOFT_AP TEST
#define ATCMD_AP			                 ("AT+AP")
#define ATCMD_SET_APCONFIG		    ("AT+SET_APCONFIG")
#define ATCMD_AP_EXIT			          ("AT+AP_EXIT")

#define ATCMD_SET_AP_SSID			   ("AT+SET_AP_SSID")      //set ssid
#define ATCMD_SET_AUTO_CONNECT	   ("AT+SET_AUTO_CONNECT")      //auto-connect
#define ATCMD_SAVE_CONFIG	   ("AT+SAVE_CONFIG")      //save config


typedef enum t_TAG_WIFIOPMODE
{
	WIFI_B 				= 0,
	WIFI_G,
	WIFI_N,
	WIFI_GREENFIELD
} TAG_WIFIOPMODE;

typedef enum t_TAG_11N
{
	GI 		= 0,
	AMPDU,
	HT20,
	HT40
} TAG_11N;

typedef enum t_TAG_AIFS
{
	VO = 0,
	VI,
	BE,
	BK
} TAG_AIFS;

typedef enum t_TAG_APSTA
{
	AP 	= 0,
	STA
} TAG_APSTA;

typedef enum t_TAG_FRAMETYPE
{
	DATA = 0,
	CTRL,
	MGMT
} TAG_FRAMETYPE;

typedef enum t_TAG_TXDATA
{
	NORMAL = 0,
	CW_WAVE
} TAG_TXDATA;

typedef struct t_TAG_NET_STATUS
{
	u8_t tcp_connect;
	u8_t udp_connect;
	uip_ip4addr_t	remote_ip_addr;
	u16_t			port;
	u16_t			listenport;
	u16_t			udpport;
} TAG_NET_STATUS;

#endif /* __CABRIO_CONF_H__ */
