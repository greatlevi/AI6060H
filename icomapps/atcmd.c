
//standard libary include
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ssv_config.h"

//contiki include
#include "contiki.h"
#include "contiki-net.h"
#include "contiki-lib.h"

#include "sys/process.h"
#include "sys/etimer.h"
#include "sys/clock.h"
//#include "sys/log.h"

#include "uip.h"
#include "tcpip.h"
#include "dhcpc.h"
#include "uip_arp.h"
#include "mac.h"

//icomm low level include (????????????)
#include <drv_uart.h>
#include "drv_phy.h"
#include <drv_pmu.h>

#include <gpio_api.h>
#include "pwm_api.h"

#include "sys_status_api.h"
#include "console.h"
#include "dbg-atcmd-api.h"
#include "systemconf.h"

//icomm high level include
#include "socket_app.h"
#include "socket_driver.h"
#include "flash_api.h"
#include "wdog_api.h"
#include "soft_ap_init.h"
#include "soft_ap.h"
#include "ota_api.h"
#include "smartComm.h"
#include "test_entry.h"

#include "Cabrio-conf.h"
#include "atcmd.h"
#include "atcmd_icomm.h"  //these are all icomm-semi only debug command!!

extern void dbg_dump_binary(U32 dump_addr,S32 length);


extern TAG_CABRIO_CONFIGURATION	gCabrioConf;
extern TAG_NET_STATUS				gNetStatus;
static struct uip_conn * conn;
//extern struct wpa_common_ctx gwpa_ctx;
extern IEEE80211STATUS gwifistatus;

static uint8_t bTXRun = 0;
static uint8_t ghttprun = 0;
//static char buffer_in[MAX_SEND_BUFFER];
//static char buffer_out[MAX_SEND_BUFFER] = {0};   

static char *buffer_in_dynamic = NULL;
static char *buffer_out_dynamic = NULL;



U32 testcnt = 0;
U32 successcnt = 0;
int gclientsock = -1;
int gserversock = -1;
int gudpsock = -1;


#define RATE_NUM 39
static char *dataRateMapping[RATE_NUM] = 
{
"11b 1M",
"11b 2M",
"11b 5.5M",
"11b 11M",
"11b 2M SP",
"11b 5.5M SP",
"11b 11M SP",
"NON-HT 6M",
"NON-HT 9M",
"NON-HT 12M",
"NON-HT 18M",
"NON-HT 24M",
"NON-HT 36M",
"NON-HT 48M",
"NON-HT 54M",
"HT-MM MCS0",
"HT-MM MCS1",
"HT-MM MCS2",
"HT-MM MCS3",
"HT-MM MCS4",
"HT-MM MCS5",
"HT-MM MCS6",
"HT-MM MCS7",
"HT-MM SGI MCS0",
"HT-MM SGI MCS1",
"HT-MM SGI MCS2",
"HT-MM SGI MCS3",
"HT-MM SGI MCS4",
"HT-MM SGI MCS5",
"HT-MM SGI MCS6",
"HT-MM SGI MCS7",
"HT-GF MCS0",
"HT-GF MCS1",
"HT-GF MCS2",
"HT-GF MCS3",
"HT-GF MCS4",
"HT-GF MCS5",
"HT-GF MCS6",
"HT-GF MCS7"
};


//dns
static char hostname[128] = {0};
extern at_cmd_info atcmd_info_tbl[];

/*---------------------------------------------------------------------------*/
//PROCESS(Cabrio_ate_process, "Cabrio_ate_process");
/*---------------------------------------------------------------------------*/
PROCESS_NAME(Cabrio_ate_process);   //This process is used for uart receiver!!
/*---------------------------------------------------------------------------*/
PROCESS(tcp_connect_process, "Tcp Connect Process");
PROCESS(poll_tx_process, "Poll TX Process");
PROCESS(nslookup_process, "NSLookup Process");
PROCESS(http_request_process, "http requset Process");
/*---------------------------------------------------------------------------*/
PROCESS_NAME(bss_reconnect_process2);
/*---------------------------------------------------------------------------*/

S32 allocate_buffer_in()
{
    S32 rlt = -1;
    if(NULL==buffer_in_dynamic)
    {
        buffer_in_dynamic = malloc(MAX_SEND_BUFFER);
        memset(buffer_in_dynamic, 0x0, MAX_SEND_BUFFER);
        rlt = 0;
    }

    return rlt;
}

S32 release_buffer_in()     //not necessary to call this  yet!!
{
    S32 rlt = -1;
    if(buffer_in_dynamic != NULL)
    {
        free(buffer_in_dynamic);
        buffer_in_dynamic = NULL;
        rlt  = 0;
    }
    return rlt;
}


S32 allocate_buffer_out()
{
    S32 rlt = -1;
    if(NULL==buffer_out_dynamic)
    {
        buffer_out_dynamic = malloc(MAX_SEND_BUFFER);
        memset(buffer_out_dynamic, 0x0, MAX_SEND_BUFFER);
        rlt = 0;
    }

    return rlt;
}

S32 release_buffer_out()        //not necessary to call this  yet!!
{
    S32 rlt = -1;
    if(buffer_out_dynamic != NULL)
    {
        free(buffer_out_dynamic);
        buffer_out_dynamic = NULL;
        rlt  = 0;
    }
    return rlt;
}


int parseBuff2Param(char* bufCmd, stParam* pParam)
{
	int idx = 0;
	const char delimiters[] = " ,";

	if (strlen (bufCmd) == 0) 
		return ERROR_INVALID_PARAMETER;
	
	strcpy (pParam->atCmdBuf, bufCmd);
	while(1)
	{
		if(idx == 0)
			pParam->argv[idx] = strtok (pParam->atCmdBuf, delimiters);
		else
			pParam->argv[idx] = strtok (NULL, delimiters);

		if( pParam->argv[idx]== NULL )
			break;
		idx++;
	}
	pParam->argc = idx;
	
	return 0;
}
int At_EnableSmartReboot(char* buff)
{
    S32 rlt = 0;
    
    printf ("[%s] : +++\n",__func__);
    if(*buff == '0') {
        gconfig_set_enable_smart(NOSMART, 0);
    } else if(*buff == '1'){
        gconfig_set_enable_smart(ICOMM, 0);
    } else if(*buff == '2'){
        gconfig_set_enable_smart(WECHAT, 0);
    } else if(*buff == '3'){
        gconfig_set_enable_smart(USER, 0);
    } else {
        printf("please enter range 0 to 3\n");
    }
    
    rlt = gconfig_save_config();
    if(rlt!=0){
        printf("<Error>gconfig_save_config failed!!\n");
    }
    rlt = remove_sysconfig();
    if(rlt!=0){
        printf("<Error>systemconf remove failed!!\n");
    }
    api_wdog_reboot(); 
    
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_EnableSmartMode(char* buff)
{
    S32 rlt = 0;
    
    printf ("[%s] : +++\n",__func__);

    if(*buff == '0') {
        gconfig_set_enable_smart(NOSMART, 1);
    } else if(*buff == '1'){
        gconfig_set_enable_smart(ICOMM, 1);
    } else if(*buff == '2'){
        gconfig_set_enable_smart(WECHAT, 1);
    } else if(*buff == '3'){
        gconfig_set_enable_smart(USER, 1);
    } else {
        printf("please enter range 0 to 3\n");
    }
    
    rlt = gconfig_save_config();
    if(rlt!=0){
        printf("<Error>gconfig_save_config failed!!\n");
    }
    
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
char* showDataRate(int idx)
{
	if(idx >=0 && idx <= 38 )
		return dataRateMapping[idx];
	else
		return "Unknown";
}
/*---------------------------------------------------------------------------*/
const char *securityString(U8 ver)
{
    switch(ver) {
    case NONE:
        return "OPEN";
        break;
    case WEP:
        return "WEP";
        break;
    case WPA:
        return "WPA";
        break;
    case WPA2:
        return "WPA2";
        break;
    case WPAWPA2:
        return "WPAWPA2";
        break;
    default:
        return "unknown security type";
    }
    return NULL;
}
/*---------------------------------------------------------------------------*/
const char *securitySubString(U8 ver)
{
    switch(ver) {
    case NOSECTYPE:
        return "NONE";
        break;
    case TKIP:
        return "TKIP";
        break;
    case CCMP:
        return "AES";
        break;
    case TKIP_CCMP:
        return "TKIP_CCMP";
        break;
    default:
        return "unknown security type";
    }
    return NULL;
}
/*---------------------------------------------------------------------------*/
int At_Reboot (void)
{	
	//printf ("[At_Reboot] : +++\n");
	//bss_mgmt_reboot ();   //move to wdog_api.c drv_wdog.c
       api_wdog_reboot(); 
	//printf (ATRSP_OK);
	At_RespOK(ATCMD_REBOOT);
	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_WatchDOG_Start(void)
{
    printf("<%s> continue to kick watch dog in 5000ms or will be rebooted\n",__func__);

    api_wdog_period_5000ms();
    //api_wdog_period_10000ms();
    //api_wdog_period_cutommed(????);

    At_RespOK(ATCMD_WDOG_START);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_WatchDOG_Stop(void)
{
    printf("<%s> stop watch dog\n",__func__);

    api_wdog_stop();


    At_RespOK(ATCMD_WDOG_STOP);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ProcessQueKill(char *buff)
{

    if(*buff == 'a') {
        process_para.event = PROCESS_ALL_QUE_KILL;
        process_para.num = 0;
    } else {
        process_para.event = PROCESS_SIGNAL_QUE_KILL;
        *(buff + 1) = '\0';
        process_para.num = atoi(buff);
    }
    printf("process event: %d num: %d\n", process_para.event, process_para.num);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ProcessKill(char *buff)
{

    if(*buff == 'a') {
        process_para.event = PROCESS_ALL_KILL;
        process_para.num = 0;
    } else {
        process_para.event = PROCESS_SIGNAL_KILL;
        *(buff + 1) = '\0';
        process_para.num = atoi(buff);
    }
    printf("process event: %d num: %d\n", process_para.event, process_para.num);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ProcessAllQueDump(void)
{	
    process_para.event = PROCESS_ALL_QUE_DUMP;
    process_para.num = 0;
    printf("process event: %d num: %d\n", process_para.event, process_para.num);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ProcessAllDump(void)
{	
    process_para.event = PROCESS_ALL_DUMP;
    process_para.num = 0;
    printf("process event: %d num: %d\n", process_para.event, process_para.num);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_ShowNtpTime()
{
    unsigned long time;
    time = get_ntp_time();
    printf("get time: %d sec\n", time);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ShowAllAP ()
{	
    int i = 0;
    if (get_ap_lsit_total_num() > 0) {
        for (i=0; i<get_ap_lsit_total_num(); i++) {
            printf ("[%d]%s\n",i, &(ap_list[i]).name[0]);
            printf ("        ch: %2d", ap_list[i].channel);
            printf (", rssi: -%3d dBm", ap_list[i].rssi);
            printf (", rssiLevel: %d, ", ap_list[i].rssi_level);
            printf ("security_type = %s", securityString(ap_list[i].security_type));
            printf ("/%s\r\n", securitySubString(ap_list[i].security_subType));
        }
    } else {
        printf ("please scan ap then you can show all ap info\r\n");
    }

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ShowConnectAP ()
{	
    int i = 0;
    if (get_wifi_connected() == 1) {
        for (i=0; i<get_ap_lsit_total_num(); i++) {
            if (memcmp (gwifistatus.connAP.ssid, &(ap_list[i]).name[0], ap_list[i].name_len) == 0) {
                printf ("[%d]%s", i, &(ap_list[i]).name[0]);
                printf (", ch: %d", ap_list[i].channel);
                printf (", rssi: -%d dBm", ap_list[i].rssi);
                printf (", rssiLevel: %d, ", ap_list[i].rssi_level);
                printf ("security_type = %s", securityString(ap_list[i].security_type));
                printf ("/%s ", securitySubString(ap_list[i].security_subType));
                printf (", %s\r\n", showDataRate(gwifistatus.rateinfo.datarate) );
                break;
            }
        }
    } else {
        printf ("please connect ap\r\n");
    }

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_SetGpio (char *buff)
{
	char *pid = 0, *pmode = 0;
	char cp[MAX_LEN]={0};
	const char delimiters[] = " ,";
	GPIO_CONF conf;

	//printf ("At_SetGpio +++\n");
	if (strlen (buff) == 0) {
		//printf (ATRSP_ERROR, ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}

	strcpy (cp, buff);
	pid = strtok (cp, delimiters);
	pmode = strtok (NULL, delimiters);


	if (atoi(pid) > GPIO_20 || atoi(pmode) > OUTPUT) {
		//printf (ATRSP_ERROR, ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}

	conf.dirt = atoi(pmode);
	conf.driving_en = 0;
	conf.pull_en = 0;
	
	pinMode(atoi(pid), conf);

	printf (ATRSP_OK);
	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_WriteGpio (char *buff)
{
	char *pid = 0, *pdata = 0;
	char cp[MAX_LEN]={0};
	const char delimiters[] = " ,";

	//printf ("At_WriteGpio +++\n");
	if (strlen (buff) == 0) {
		//printf (ATRSP_ERROR, ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}

	strcpy (cp, buff);
	pid = strtok (cp, delimiters);
	pdata = strtok (NULL, delimiters);


	if (atoi(pid) > GPIO_20) {
		//printf (ATRSP_ERROR, ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}

	digitalWrite(atoi(pid), atoi(pdata));
	
	printf (ATRSP_OK);
	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ReadGpio (char *buff)
{
	char *pid = 0;
	char cp[MAX_LEN]={0};
	const char delimiters[] = " ,";
	U32 data;

	//printf ("At_ReadGpio +++\n");
	if (strlen (buff) == 0) {
		//printf (ATRSP_ERROR, ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}

	strcpy (cp, buff);
	pid = strtok (cp, delimiters);

	if (atoi(pid) > GPIO_20) {
		//printf (ATRSP_ERROR, ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}

	data = digitalRead(atoi(pid));

	printf (ATIND_GPIO, data);
	
	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
/* Receive RX data from UART */
int 
At_ParserRecv (unsigned char * buff, int len) 
{
	EVENTMSG_DATA	msg;

	/* Handle received data with null end*/
	*(buff+len) = 0;

	msg.msgid = MSG_ATE_RXRECV;
	msg.data_len = len;
	memcpy ((unsigned char *)msg.data,  buff, len);
	
	process_post (&Cabrio_ate_process, PROCESS_EVENT_CONTINUE, &msg);

	return ERROR_SUCCESS;
}
#if CURRENT_TEST
int
At_PowerOn (void)
{	
	bTXRun = 0;
	Radio_Command(RX_DISABLE, "1");
	printf (ATRSP_OK);
	return ERROR_SUCCESS;
}
int
At_RX (void)
{	
	bTXRun = 0;
	Radio_Command(RX_DISABLE, "0");
	printf (ATRSP_OK);
	return ERROR_SUCCESS;
}
int
At_TX (void)
{	
	process_start(&poll_tx_process, NULL);
	bTXRun = 1;
	process_post (&poll_tx_process, PROCESS_EVENT_CONTINUE, NULL);
	printf (ATRSP_OK);
	return ERROR_SUCCESS;
}
int
At_PowerSaving (char *buff)
{	
	char *nSec = 0;
	char cp[MAX_LEN]={0};
	const char delimiters[] = " ,";
	
	bTXRun = 0;

	if (strlen (buff) == 0) 
	{
		//printf (ATRSP_ERROR, ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}

	strcpy (cp, buff);
	nSec = strtok (cp, delimiters);

	if(nSec == 0)
		return ERROR_INVALID_PARAMETER;
	printf ("Sleep %d sec\n", atoi(nSec));
	//sleep
	drv_pmu_setwake_cnt(atoi(nSec)*100000);
	drv_pmu_init();
	drv_pmu_sleep();
	drv_pmu_chk();
	if(drv_pmu_check_interupt_event() == 1)
		printf("pmu check interupt wakeup!\n");

	printf (ATRSP_OK);
	return ERROR_SUCCESS;
}		
#endif
/*---------------------------------------------------------------------------*/
int At_TCPChange_Timeout(char *buff)
{
	char  *pport = 0;
	char cp[MAX_LEN]={0};

	u16_t time_val;
	if (strlen (buff) == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	strcpy (cp, buff);
	time_val = strtoul(cp,NULL,10);
	if(time_val > 0 && time_val <= 1000) {
		printf("time_val: %d\n", time_val);
		set_tcp_timeout(time_val);
	} else {
		printf("over tcp time out range\n");
	}
	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_TCPConnect (char *buff)
{
	char *pIp = 0, *pport = 0;
	char cp[MAX_LEN]={0};
	const char delimiters[] = " ,";

	if (get_wifi_connected() != 1)
		return ERROR_WIFI_CONNECTION;

	if (gclientsock >= 0)
	{
		printf("TCP already Connect\n");
		return ERROR_TCP_CONNECTION;
	}
	
	if (strlen (buff) == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	strcpy (cp, buff);
	pIp = strtok (cp, delimiters);
	pport = strtok (NULL, delimiters);
	
	gNetStatus.port = atoi (pport);
	if (gNetStatus.port == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	if( uiplib_ipaddrconv(pIp, &gNetStatus.remote_ip_addr)  == 0 )	
	{
		printf("DNS format : %s\n", pIp);
		set_nslookup(pIp);  
	}
	else
	{
		printf("IP format : %s\n", pIp);
             gclientsock = tcpconnect(&gNetStatus.remote_ip_addr, gNetStatus.port, &tcp_connect_process);
		printf("create socket:%d\n", gclientsock);
	}

	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_TCPSend (char *buff)
{	
	char *psock = 0, *pdata = 0;
	char cp[MAX_LEN]={0};
	const char delimiters[] = " ,";
	int sock;
	
	if (strlen (buff) >= MAX_SEND_BUFFER || strlen (buff) == 0)
		return ERROR_INVALID_PARAMETER;


	strcpy (cp, buff);
	psock = strtok (cp, delimiters);
	pdata = strtok (NULL, delimiters);
	sock = atoi(psock);
	
	if (sock < 0)
	{
		printf("socket error:%d\n", sock);
		return ERROR_TCP_CONNECTION;	
	}	

    allocate_buffer_out();


    memcpy(buffer_out_dynamic, pdata, strlen (pdata));
	if(tcpsend(sock, buffer_out_dynamic, strlen (pdata)) == -1)
	{
		return ERROR_TCP_CONNECTION;
	}
	else
	{
		return ERROR_SUCCESS;
	}
}
/*---------------------------------------------------------------------------*/
int At_TCPDisconnect (char *buff)
{
	int sock;
    
	if (strlen (buff) >= MAX_SEND_BUFFER || strlen (buff) == 0)
		return ERROR_INVALID_PARAMETER;
    
	sock = atoi(buff);
	
	if(tcpclose(sock) == -1)
	{
		return ERROR_TCP_CONNECTION;
	}
	else
	{
		return ERROR_SUCCESS;
	}
}
/*---------------------------------------------------------------------------*/
int At_TCPListen (char *buff)
{
	char  *pport = 0;
	char cp[MAX_LEN]={0};
	const char delimiters[] = " ,";

	if (strlen (buff) == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	strcpy (cp, buff);
	pport = strtok (cp, delimiters);

	gNetStatus.listenport = strtoul(pport,NULL,10);

    
	if (gNetStatus.listenport == 0) {
		return ERROR_INVALID_PARAMETER;
	}


	if(tcplisten(gNetStatus.listenport, &tcp_connect_process) == -1)
	{
		printf("listen tcp fail\n");
		return ERROR_TCP_CONNECTION;
	}
	else
	{
		At_RespOK(ATCMD_TCPLISTEN);
		return ERROR_SUCCESS;
	}

}
/*---------------------------------------------------------------------------*/
int At_TCPUnlisten (char *buff)
{
	char  *pport = 0;
	char cp[MAX_LEN]={0};
	const char delimiters[] = " ,";

	if (get_wifi_connected() != 1)
		return ERROR_WIFI_CONNECTION;
	
	if (strlen (buff) == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	strcpy (cp, buff);
	pport = strtok (cp, delimiters);
	
	gNetStatus.listenport = atoi (pport);
	if (gNetStatus.listenport == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	tcpunlisten(gNetStatus.listenport);
	At_RespOK(ATCMD_TCPUNLISTEN);
	
	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_UDPCreate (char *buff)
{
	char  *pport = 0;
	const char delimiters[] = " ,";

	if ( get_wifi_connected() != 1)
		return ERROR_WIFI_CONNECTION;
	
	if (strlen (buff) == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	pport = strtok (buff, delimiters);
	
	gNetStatus.udpport = atoi (pport);
	if (gNetStatus.udpport == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	gudpsock = udpcreate(gNetStatus.udpport, &tcp_connect_process);

	if(gudpsock == -1)
	{
		printf("create udp socket fail\n");
		return ERROR_UDP_CONNECTION;
	}
	else
	{
		printf("create socket:%d\n", gudpsock);
		At_RespOK(ATCMD_UDPCREATE);
		return ERROR_SUCCESS;
	}

}
/*---------------------------------------------------------------------------*/
int At_UDPSend (char *buff)
{
	const char delimiters[] = " ,";
	int sock;
	char *pIp = 0, *pport = 0, *psock = 0, *pdata = 0;
	char cp[MAX_LEN]={0};
	
	strcpy (cp, buff);
	psock = strtok (cp, delimiters);
	pIp = strtok (NULL, delimiters);
	pport = strtok (NULL, delimiters);
	pdata = strtok (NULL, delimiters);

	if (ERROR_SUCCESS != ip2int (pIp, &gNetStatus.remote_ip_addr)) {
		return ERROR_INVALID_PARAMETER;
	}
	
	gNetStatus.port = atoi (pport);
	if (gNetStatus.port == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	sock = atoi(psock);
	
	if(udpsendto(sock, pdata, strlen(pdata), &gNetStatus.remote_ip_addr, gNetStatus.port) == -1)
	{
		return ERROR_UDP_CONNECTION;
	}
	else
	{
		At_RespOK(ATCMD_UDPSEND);
		return ERROR_SUCCESS;
	}
}
/*---------------------------------------------------------------------------*/
int At_UDPClose (char *buff)
{
	int sock;
	
	if (strlen (buff) >= MAX_SEND_BUFFER || strlen (buff) == 0)
		return ERROR_INVALID_PARAMETER;
	
	sock = atoi(buff);
	
	if(udpclose(sock) == -1)
	{
		return ERROR_UDP_CONNECTION;
	}
	else
	{
		gudpsock = -1;
		At_RespOK(ATCMD_UDPCLOSE);
		return ERROR_SUCCESS;
	}

}
/*---------------------------------------------------------------------------*/
int At_HTTPTest (char *buff)
{	
	char *pIp = 0, *pport = 0, *pbuff = 0;
	char cp[MAX_LEN]={0};
	const char delimiters[] = " ,";

	if (get_wifi_connected() != 1)
		return ERROR_WIFI_CONNECTION;

	if (strlen (buff) >= MAX_SEND_BUFFER || strlen (buff) == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	strcpy (cp, buff);
	pIp = strtok (cp, delimiters);
	pport = strtok (NULL, delimiters);
	pbuff = strtok (NULL, delimiters);
	
	gNetStatus.port = atoi (pport);
	if (gNetStatus.port == 0) {
		return ERROR_INVALID_PARAMETER;
	}

      allocate_buffer_out();  

	if( uiplib_ipaddrconv(pIp, &gNetStatus.remote_ip_addr)  == 0 )	
	{
		printf("DNS format : %s\n", pIp);
		set_nslookup(pIp);
	}
	else
	{
		printf("IP format : %s\n", pIp);
	}

      memset(buffer_out_dynamic, 0, 256);
      strcat(buffer_out_dynamic, "GET /cgi-bin/test.pl?param1=\"");
//	strcat(buffer_out, "GET /cgi-bin/test.cgi?param1=\"");
    strcat(buffer_out_dynamic, pbuff);
	strcat(buffer_out_dynamic, "\" HTTP/1.0\r\nHost:\r\n\r\n");

	ghttprun = 1;
	process_start(&http_request_process, NULL);
	
	return ERROR_SUCCESS;    
}
/*---------------------------------------------------------------------------*/
int At_HTTPTestStop(void)
{
	ghttprun = 0;
	return ERROR_SUCCESS;    
}
/*---------------------------------------------------------------------------*/
int At_NSLookup(char *buff)
{
	memset(hostname, 0, 128);
	memcpy(hostname, buff, strlen(buff));

	process_start(&nslookup_process, NULL);
	set_nslookup(hostname);
	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_SET_AP_SSID (char *buff)
{
    printf ("[%s] : +++\n",__func__);
    const char delimiters[] = " ,=\"";
    char *p_ssid = NULL;

    buff = strtok ( buff, delimiters);
    p_ssid = strtok ( NULL, delimiters);

    //printf("p_ssid = %s\n",p_ssid);

    gconfig_set_softap_ssid(p_ssid);

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_SET_AUTO_CONNECT (char *buff)
{
    printf ("[%s] : +++\n",__func__);
    const char delimiters[] = " ,";
    char *p_val = NULL;
    U32 auto_connect = 0;
    S32 rlt =0;

    buff = strtok ( buff, delimiters);
    p_val = strtok ( NULL, delimiters);

    if( p_val == NULL ){
        return ERROR_INVALID_PARAMETER;
    }

    auto_connect = atoi(p_val);

    if( (auto_connect !=0) &&  (auto_connect !=1))
    {
        rlt = ERROR_INVALID_PARAMETER;
        goto error_exit;
    }

    gconfig_set_auto_connect(auto_connect);

exit:
    return rlt;
error_exit:
    return rlt;
}
/*---------------------------------------------------------------------------*/
S32 At_SAVE_CONFIG (void)
{
    S32 rlt = 0;
    printf ("[%s] : +++\n",__func__);

    rlt = gconfig_save_config();

    if(rlt!=0){
        printf("<Error>gconfig_save_config failed!!\n");
    }

exit:
    return rlt;
error_exit:
    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_Clean_Lists (void)
{
    printf ("[At_Clean_Lists] : +++\n");
    clear_ap_lists();
}
/*---------------------------------------------------------------------------*/
int At_AP (void)
{
    printf ("[At_AP] : +++\n");
    softap_start();

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_SET_APCONFIG (char *buff)
{
    char *token;
    const char delimiters[] = " .";    //ip address being split in xxx.xxx.xxx.xxx
    uip_ipaddr_t ip_addr;
    S32 rlt = ERROR_INVALID_PARAMETER;
    S8 loop_i = 0; 
    unsigned long int toul_val;
    U8 a_gw_digit[3];
       
    printf ("[At_AP_EX] : +++\n");

    if (strlen (buff) == 0) 
        return ERROR_INVALID_PARAMETER;

//    printf("buff = %s\n",buff);

    token = strtok (buff, delimiters);
    token = strtok (NULL, delimiters);
    
    for(loop_i=0;loop_i<3;loop_i++)
    {
        if(token==NULL){
            rlt = ERROR_INVALID_PARAMETER;
            goto error_exit;
        }

        toul_val =  strtoul(token, NULL, 10);
        a_gw_digit[loop_i] = (U8)toul_val;

        //printf(" a_gw_digit[%d] = %d token=%s\n",loop_i,a_gw_digit[loop_i], token);
        //printf( "<%d>str=%s ,val=%d,org=%d\n", loop_i,token,ip_addr.u8[loop_i],toul_val);
        token = strtok(NULL, delimiters);
    }

    //for example :  softap_init_ex(192,168,0);     //it means to set gateway as 192.168.0.1
    //for example :  softap_init_ex(192,168,10);    //it means to set gateway as 192.168.10.1
    softap_init_ex(a_gw_digit[0],a_gw_digit[1],a_gw_digit[2]); 
    rlt = ERROR_SUCCESS;
error_exit:
    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_AP_EXIT (void)
{
    printf ("[At_AP_EXIT] : +++\n");
    softap_exit();
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_TestFlashErase(char *buff)
{
    //step1:split cmd into ip first!!
	char *token;
	const char delimiters[] = " ";    //ip address being split in xxx.xxx.xxx.xxx
	uip_ipaddr_t ip_addr;
       S32 rlt = 0;
       S8 loop_i = ERROR_SUCCESS; 
       unsigned long int toul_val;
       U32 au32val[2];
       
	if (strlen (buff) == 0) 
		return ERROR_INVALID_PARAMETER;

       token = strtok (buff, delimiters);
	 for(loop_i=0;loop_i<2;loop_i++)
        {
            if(token==NULL){
                rlt = ERROR_INVALID_PARAMETER;
                goto error_exit;
            }

            toul_val =  strtoul(token, NULL, 16);
            //ip_addr.u8[loop_i] = (U8)toul_val; 
            au32val[loop_i] = toul_val;
            
            //printf( "<%d>str=%s ,val=%d,org=%d\n", loop_i,token,ip_addr.u8[loop_i],toul_val);
            token = strtok(NULL, delimiters);
	 }

    //step2:ping!!
    //ping_start(&ip_addr);
    printf("(addr,length)=(0x%x,0x%x)\n",au32val[0],au32val[1]);
    if( (au32val[0] & 0xfff != 0x0) || (au32val[1] & 0xfff != 0x0))
    {
        printf("addr and length must all be 4KB aligned\n"); 
        rlt = ERROR_INVALID_PARAMETER;
        goto error_exit;
    }

    if( au32val[0] < 12*KB)
    {
        printf("addr must >= 12KB\n"); 
        rlt = ERROR_INVALID_PARAMETER;
        goto error_exit;
    }

//spi_flash_test(au32val[0],au32val[1]);
    spi_flash_sector_erase(au32val[0]);
    
exit:
    return rlt;
error_exit:
    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_TestFlashWrite(char *buff){
    //step1:split cmd into ip first!!
	char *token;
	const char delimiters[] = " ";    //ip address being split in xxx.xxx.xxx.xxx
	uip_ipaddr_t ip_addr;
       S32 rlt = 0;
       S8 loop_i = ERROR_SUCCESS; 
       unsigned long int toul_val;
       U32 au32val[2];
       
	if (strlen (buff) == 0) 
		return ERROR_INVALID_PARAMETER;

       token = strtok (buff, delimiters);
	 for(loop_i=0;loop_i<2;loop_i++)
        {
            if(token==NULL){
                rlt = ERROR_INVALID_PARAMETER;
                goto error_exit;
            }

            toul_val =  strtoul(token, NULL, 16);
            //ip_addr.u8[loop_i] = (U8)toul_val; 
            au32val[loop_i] = toul_val;
            
            //printf( "<%d>str=%s ,val=%d,org=%d\n", loop_i,token,ip_addr.u8[loop_i],toul_val);
            token = strtok(NULL, delimiters);
	 }

    //step2:ping!!
    //ping_start(&ip_addr);
    printf("(addr,length)=(0x%x,0x%x)\n",au32val[0],au32val[1]);
#if 0
    if( (au32val[0] & 0xfff != 0x0) || (au32val[1] & 0xfff != 0x0))
    {
        printf("addr and length must all be 4KB aligned\n"); 
        rlt = ERROR_INVALID_PARAMETER;
        goto error_exit;
    }
#endif    

    if( au32val[0] < 12*KB)
    {
        printf("addr must >= 12KB\n"); 
        rlt = ERROR_INVALID_PARAMETER;
        goto error_exit;
    }

    spi_flash_test(au32val[0],au32val[1]);
exit:
    return rlt;
error_exit:
    return rlt;
}

/*---------------------------------------------------------------------------*/
int At_BenchFlashRead(char *buff)
{
#define BENCH_BLOCK_SIZE (4096) 
    //step1:split cmd into ip first!!
	char *token;
	const char delimiters[] = " ";    //ip address being split in xxx.xxx.xxx.xxx
	uip_ipaddr_t ip_addr;
       S32 rlt = 0;
       S8 loop_i = ERROR_SUCCESS; 
       unsigned long int toul_val;
       U32 au32val[3];
       void *buf_dyn = NULL;
       S32 read_addr;
       S32 read_length;
       BOOL dump_flag=0;
       S32 rsize = 0;   //current read size

        //printf("<%s>strlen = %d\n", __func__,strlen(buff));
       
	if (strlen (buff) == 0) 
		return ERROR_INVALID_PARAMETER;

       token = strtok (buff, delimiters);
	 for(loop_i=0;loop_i<3;loop_i++)
        {
            if(token==NULL){
                rlt = ERROR_INVALID_PARAMETER;
                goto error_exit;
            }

            toul_val =  strtoul(token, NULL, 16);
            //ip_addr.u8[loop_i] = (U8)toul_val; 
            au32val[loop_i] = toul_val;
            
            //printf( "<%d>str=%s ,val=%d,org=%d\n", loop_i,token,ip_addr.u8[loop_i],toul_val);
            token = strtok(NULL, delimiters);
	 }
    
    read_addr = au32val[0];
    read_length = au32val[1];
    dump_flag = au32val[2]; 

    //step2:ping!!
    //ping_start(&ip_addr);
    printf("(addr,length,dump_flag)=(0x%x,0x%x,%d)\n",read_addr,read_length,dump_flag);
    //dbg_dump_binary((au32val[0] |SRAM_BOOT_FLASH_BASE_ADDR) ,au32val[1]);

    buf_dyn = malloc(BENCH_BLOCK_SIZE);

    while( read_length > 0 )
    {
        printf("length = 0x%x,rsize=0x%d\n", read_length,rsize);
        
        if(read_length >= BENCH_BLOCK_SIZE)
        {
            rsize = BENCH_BLOCK_SIZE;
        } else {
            rsize = read_length;
        }

        spi_flash_read( read_addr, buf_dyn, rsize);
        if(1==dump_flag)
        {
            dbg_dump_binary(buf_dyn,rsize);
        }
    
        read_addr += rsize;
        read_length -= rsize;

        
    }

    if(buf_dyn != NULL)
    {
        free(buf_dyn);
    }
    
exit:
    return rlt;
error_exit:
    return rlt;
}

/*---------------------------------------------------------------------------*/
int At_BenchFlashWrite(char *buff)
{
#define BENCH_BLOCK_SIZE (4096) 
    //step1:split cmd into ip first!!
	char *token;
	const char delimiters[] = " ";    //ip address being split in xxx.xxx.xxx.xxx
	uip_ipaddr_t ip_addr;
       S32 rlt = 0;
       S32 loop_i = 0; 
       unsigned long int toul_val;
       U32 au32val[3];
       void *buf_dyn = NULL;

       S32 write_addr;
       S32 write_length;
       S32 loop_no=0;

       S32 tmp_write_addr;
       S32 tmp_write_length;
       S32 wsize = 0;   //current write size
       
	if (strlen (buff) == 0) 
		return ERROR_INVALID_PARAMETER;

       token = strtok (buff, delimiters);
	 for(loop_i=0;loop_i<3;loop_i++)
        {
            if(token==NULL){
                rlt = ERROR_INVALID_PARAMETER;
                goto error_exit;
            }

            toul_val =  strtoul(token, NULL, 16);
            //ip_addr.u8[loop_i] = (U8)toul_val; 
            au32val[loop_i] = toul_val;
            
            //printf( "<%d>str=%s ,val=%d,org=%d\n", loop_i,token,ip_addr.u8[loop_i],toul_val);
            token = strtok(NULL, delimiters);
	 }

    
    write_addr = au32val[0];
    write_length = au32val[1];
    loop_no = au32val[2]; 

    //step2:ping!!
    //ping_start(&ip_addr);
    printf("(write_addr,write_length,loop_no)=(0x%x,0x%x,%d)\n", write_addr,write_length,loop_no);
    //dbg_dump_binary((au32val[0] |SRAM_BOOT_FLASH_BASE_ADDR) ,au32val[1]);

    buf_dyn = malloc(BENCH_BLOCK_SIZE);

    for(loop_i=0;loop_i<loop_no;loop_i++)
    {
        tmp_write_addr  = write_addr;
        tmp_write_length = write_length;
    
        while( tmp_write_length > 0 )
        {
            printf("<loop_i=%d>tmp_write_length = 0x%x,wsize=0x%d\n",loop_i, tmp_write_length, wsize);
            
            if(tmp_write_length >= BENCH_BLOCK_SIZE)
            {
                wsize = BENCH_BLOCK_SIZE;
            } else {
                wsize = tmp_write_length;
            }

            //spi_flash_write( tmp_write_addr, buf_dyn, wsize);
            spi_flash_test(tmp_write_addr,wsize);
        
            tmp_write_addr += wsize;
            tmp_write_length -= wsize;
            
        }

    }


    if(buf_dyn != NULL)
    {
        free(buf_dyn);
    }
    
exit:
    return rlt;
error_exit:
    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_TestFlashRead(char *buff)
{
    //step1:split cmd into ip first!!
	char *token;
	const char delimiters[] = " ";    //ip address being split in xxx.xxx.xxx.xxx
	uip_ipaddr_t ip_addr;
       S32 rlt = 0;
       S8 loop_i = ERROR_SUCCESS; 
       unsigned long int toul_val;
       U32 au32val[2];

        //printf("<%s>strlen = %d\n", __func__,strlen(buff));
       
	if (strlen (buff) == 0) 
		return ERROR_INVALID_PARAMETER;

       token = strtok (buff, delimiters);
	 for(loop_i=0;loop_i<2;loop_i++)
        {
            if(token==NULL){
                rlt = ERROR_INVALID_PARAMETER;
                goto error_exit;
            }

            toul_val =  strtoul(token, NULL, 16);
            //ip_addr.u8[loop_i] = (U8)toul_val; 
            au32val[loop_i] = toul_val;
            
            //printf( "<%d>str=%s ,val=%d,org=%d\n", loop_i,token,ip_addr.u8[loop_i],toul_val);
            token = strtok(NULL, delimiters);
	 }

    //step2:ping!!
    //ping_start(&ip_addr);
    printf("(addr,length)=(0x%x,0x%x)\n",au32val[0],au32val[1]);
    dbg_dump_binary((au32val[0] |SRAM_BOOT_FLASH_BASE_ADDR) ,au32val[1]);
exit:
    return rlt;
error_exit:
    return rlt;
}
/*---------------------------------------------------------------------------*/
#ifdef ENABLE_DOWNLOAD_FORM_CLOUD_SERVER
int At_OTASetServer (char *buff)
{
	char *pIp = 0, *pport = 0;
	char cp[MAX_LEN]={0};
	const char delimiters[] = " ,";

	if (get_wifi_connected() != 1)
		return ERROR_WIFI_CONNECTION;
	
	if (strlen (buff) == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	strcpy (cp, buff);
	pIp = strtok (cp, delimiters);
	pport = strtok (NULL, delimiters);
	
	if (atoi(pport) == 0) 
	{
		return ERROR_INVALID_PARAMETER;
	}

	ota_set_server(1, pIp, atoi(pport));

	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_OTASetParam (char *buff)
{
	char *pFilename = 0, *pPathname = 0;
	char cp[MAX_LEN]={0};
	const char delimiters[] = " ,";

	strcpy (cp, buff);
	pFilename = strtok (cp, delimiters);
	pPathname = strtok (NULL, delimiters);

	if(pFilename == 0)
		return 1;
	if(pPathname == NULL)
		ota_set_parameter(pFilename, "");
	else
		ota_set_parameter(pFilename, pPathname);
	return ERROR_SUCCESS;	
}
/*---------------------------------------------------------------------------*/
int At_OTAStart (char *buff)
{
	ota_update(NULL);	
	return ERROR_SUCCESS;
}
#endif
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(http_request_process, ev, data)
{
    static struct etimer httptimeout;
	static NETSOCKET httpsock;
	int recvlen;
	SOCKETMSG msg;
   // static struct etimer send_timer;

      allocate_buffer_in();  
      allocate_buffer_out();

	PROCESS_BEGIN();

	while(ghttprun)
	{
		//tcp connection
		if(get_wifi_connected() == 0)
	{
			printf("******WIFI disconnected, sleep one second\n");
			etimer_set(&httptimeout, 1 * CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
			continue;
	}
		
		testcnt++;
		httpsock = tcpconnect( &gNetStatus.remote_ip_addr, gNetStatus.port, &http_request_process);

		//wait for TCP connected or uip_timeout.
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG);
		msg = *(SOCKETMSG *)data;
		if(msg.status != SOCKET_CONNECTED)
	{
			printf("TCP connect fail! Post message type:%d\n", msg.status);
			goto dissconnect;
	}
	
		//Send http request to server
		tcpsend(httpsock, buffer_out_dynamic, strlen (buffer_out_dynamic));
	
		//Wait for data is transmitted or uip_timeout.
		while(1)
		{
			PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG);
			msg = *(SOCKETMSG *)data;
			if(msg.status == SOCKET_SENDACK)
			{
				break;
			}
			else if(msg.status == SOCKET_CLOSED)
			{
				printf("TCP send fail! Post message type:%d\n", msg.status);
				goto dissconnect;
			}
		}

		//Wait for response or timeout
		etimer_set(&httptimeout, 15 * CLOCK_SECOND);
		while(1)
		{
			PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG || ev == PROCESS_EVENT_TIMER);
			if(ev == PROCESS_EVENT_TIMER)
			{
				printf("Recv data timeout\n");
				goto dissconnect;
			}
			else if(ev == PROCESS_EVENT_MSG)
			{
				msg = *(SOCKETMSG *)data;
				if(msg.status == SOCKET_NEWDATA)
				{
					successcnt++;
					recvlen = tcprecv(httpsock, buffer_in_dynamic, MAX_SEND_BUFFER);
					buffer_in_dynamic[recvlen] = 0;
					printf("recv:%s\n", buffer_in_dynamic);
					break;
				}
				else if(msg.status == SOCKET_CLOSED)
	{
					printf("Recv data fail! Post message type:%d\n", msg.status);
					goto dissconnect;
				}
			}
	}
  
dissconnect:
		//dissconnect
		tcpclose(httpsock);
		printf("Testcnt:%d failcnt:%d\n", testcnt, (testcnt-successcnt));
		//Wait for one second to close the connection.
		etimer_set(&httptimeout, 1 * CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
	}

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(tcp_connect_process, ev, data)
{
	PROCESS_BEGIN();
	SOCKETMSG msg;
	int recvlen;
	uip_ipaddr_t peeraddr;
	U16 peerport;

      allocate_buffer_in();

	while(1) 
	{
		PROCESS_WAIT_EVENT();

		if( ev == PROCESS_EVENT_EXIT) 
		{
			break;
		} 
		else if(ev == PROCESS_EVENT_MSG) 
		{
			msg = *(SOCKETMSG *)data;
			//The TCP socket is connected, This socket can send data after now.
			if(msg.status == SOCKET_CONNECTED)
			{
				printf("socked:%d connected\n", msg.socket);
				if(msg.socket == gclientsock)
					At_RespOK(ATCMD_TCPCONNECT);
			}
			//TCP connection is closed. Clear the socket number.
			else if(msg.status == SOCKET_CLOSED)
			{
				printf("socked:%d closed\n", msg.socket);
				At_RespOK(ATCMD_TCPDISCONNECT);
				if(gclientsock == msg.socket)
					gclientsock = -1;
				if(gserversock == msg.socket)
					gserversock = -1;
			}
			//Get ack, the data trasmission is done. We can send data after now.
			else if(msg.status == SOCKET_SENDACK)
			{
				printf("socked:%d send data ack\n", msg.socket);
				At_RespOK(ATCMD_TCPSEND);
			}
			//There is new data coming. Receive data now.
			else if(msg.status == SOCKET_NEWDATA)
			{
				if(0 <= msg.socket && msg.socket < UIP_CONNS)
				{
					recvlen = tcprecv(msg.socket, buffer_in_dynamic, MAX_SEND_BUFFER);
					buffer_in_dynamic[recvlen] = 0;
      #if 0              
					printf("TCP socked:%d recvdata:%s\n", msg.socket, buffer_in_dynamic);
      #endif
      //printf("TCP-%d:%c-%c-%c\n", msg.socket, buffer_in_dynamic[0],buffer_in_dynamic[1],buffer_in_dynamic[2]);
      printf("%d:%c%c\n", msg.socket, buffer_in_dynamic[0],buffer_in_dynamic[1]);
	
				}
				else if(UIP_CONNS <= msg.socket && msg.socket < UIP_CONNS + UIP_UDP_CONNS)
				{
					recvlen = udprecvfrom(msg.socket, buffer_in_dynamic, MAX_SEND_BUFFER, &peeraddr, &peerport);
					buffer_in_dynamic[recvlen] = 0;
					printf("UDP socked:%d recvdata:%s from %d.%d.%d.%d:%d\n", msg.socket, buffer_in_dynamic, peeraddr.u8[0], peeraddr.u8[1], peeraddr.u8[2], peeraddr.u8[3], peerport);
				}
				else
		{
					printf("Illegal socket:%d\n", msg.socket);
				}
			}
			//A new connection is created. Get the socket number and attach the calback process if needed.
			else if(msg.status == SOCKET_NEWCONNECTION)
			{
				if(gserversock == -1)
				{
					gserversock = msg.socket;
					printf("new connected to listen port(%d), socket:%d\n", msg.lport, msg.socket);
				}
				else
				{
					//Only allow one client connection for this application.
					//tcpclose(msg.socket);
				}
			}
			else
			{
				printf("unknow message type\n");
			}
		}	
		}	

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(poll_tx_process, ev, data)
{
	PROCESS_BEGIN();
	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
		while(bTXRun)
		{
			Radio_Command(SEND_FRAME, "");
		}
	}
	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nslookup_process, ev, data)
{
	PROCESS_BEGIN();
	PROCESS_WAIT_EVENT_UNTIL(ev == resolv_event_found);
	{
		uip_ipaddr_t addr;
		uip_ipaddr_t *addrptr;
		addrptr = &addr;
	
		char *pHostName = (char*)data;
		if(ev==resolv_event_found)
		{
			resolv_lookup(pHostName, &addrptr);
			uip_ipaddr_copy(&addr, addrptr);
			printf("AT+NSLOOKUP=%d.%d.%d.%d\n", addr.u8[0] , addr.u8[1] , addr.u8[2] , addr.u8[3] );
		}
	}
	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
int At_ParserInd (EVENTMSG_ID msgid, unsigned char *buff, int len) 
{
	int i = 0;

	switch (msgid) {
		case MSG_ATE_RECV:
			printf (ATIND_RECV, buff);
			break;
		case MSG_ATE_SCAN:
			printf ("+SCAN:");
			for (i=0; i<get_ap_lsit_total_num(); i++) {
				printf ("%s, ", &(ap_list[i]).name[0]);
			}
			printf ("\r\n");
			At_RespOK(ATCMD_NETSCAN);
			break;
		case MSG_ATE_DISCONNECT:
			printf (ATIND_DISCONNECT, "SUCCESS");
			At_RespOK(ATCMD_DISCONNECT);
			break;
		case MSG_ATE_ERROR:
			printf (ATRSP_ERROR_STR, buff);
			break;
            default:
                break;
	}

	//printf ("[Ate_ParserInd] : %d---\n",msgid);
	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_Parser (char *buff, int len)
{
	int i = 0;
	int	nRet = ERROR_UNKNOWN_COMMAND;
	
        if( (1==len) && (buff[0]=='\r' || buff[0]=='\n')){
            nRet = ERROR_SUCCESS;
            goto exit;
        }
    
	if (0 == len) {
		printf (ATRSP_ERROR, ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}

	buff[len-1] = 0x0;  //chomp!! replace \r\n with null string

	if (!strcmp ((char *)buff, ATCMD_REBOOT)) 
		nRet = At_Reboot ();
      else if (!strcmp ((char *)buff, ATCMD_WDOG_START))
		nRet = At_WatchDOG_Start();
      else if (!strcmp ((char *)buff, ATCMD_WDOG_STOP))
		nRet = At_WatchDOG_Stop();
      else if (!strcmp ((char *)buff, ATCMD_VERSION))
		nRet = At_GetVersion ();
	else if (!strcmp ((char *)buff, ATCMD_MINFO))
		nRet = At_GetManufactureInfo ();
	else if (0 == strncmp ((char *)buff, ATCMD_SETWIFICONFIG, sizeof (ATCMD_SETWIFICONFIG)-1))
		nRet = At_SetWifiConfig (buff+sizeof(ATCMD_SETWIFICONFIG)-1);
	else if (!strcmp ((char *)buff, ATCMD_GETCONFIG)) 
		nRet = At_GetConfigure ();
	else if (0 == strncmp ((char *)buff, ATCMD_SETIFCONFIG, sizeof (ATCMD_SETIFCONFIG)-1)) 
		nRet = At_SetIfConfigure (buff+sizeof(ATCMD_SETIFCONFIG)-1);
	else if (!strcmp ((char *)buff, ATCMD_NETSTATUS))
		nRet = At_GetNetStatus ();
	else if (!strcmp ((char *)buff, ATCMD_IPCONF))
		nRet = At_GetIPconf();
       else if (!strncmp ((char *)buff, ATCMD_HELP, sizeof (ATCMD_HELP)-1))
		nRet = At_Help ();
      else if (!strcmp ((char *)buff, ATCMD_APSTATUS))
		nRet = At_GetApStatus ();
      else if (!strcmp ((char *)buff, ATCMD_WIFISTATUS))
		nRet = At_GetWifiStatus ();
      else if (!strcmp ((char *)buff, ATCMD_MRX))
		nRet = At_GetMRX();
      	else if (!strncmp ((char *)buff, ATCMD_FLASH_WRITE, sizeof (ATCMD_FLASH_WRITE)-1))
		nRet = At_TestFlashWrite (buff+sizeof(ATCMD_FLASH_WRITE)-1);
      	else if (!strncmp ((char *)buff, ATCMD_FLASH_READ, sizeof (ATCMD_FLASH_READ)-1))
		nRet = At_TestFlashRead (buff+sizeof(ATCMD_FLASH_READ)-1);
      	else if (!strncmp ((char *)buff, ATCMD_BENCH_FLASH_READ, sizeof (ATCMD_BENCH_FLASH_READ)-1))
		nRet = At_BenchFlashRead (buff+sizeof(ATCMD_BENCH_FLASH_READ)-1);
      else if (!strncmp ((char *)buff, ATCMD_BENCH_FLASH_WRITE, sizeof (ATCMD_BENCH_FLASH_WRITE)-1))
		nRet = At_BenchFlashWrite (buff+sizeof(ATCMD_BENCH_FLASH_WRITE)-1);
       else if (!strncmp ((char *)buff, ATCMD_MEM_READ, sizeof (ATCMD_MEM_READ)-1))
		nRet = At_TestMemRead (buff+sizeof(ATCMD_MEM_READ)-1);
       else if (!strcmp ((char *)buff, ATCMD_MBOX))
		nRet = At_MboxDump (buff+sizeof(ATCMD_MBOX)-1);
       else if (!strcmp ((char *)buff, ATCMD_WIFI_RESET))
		nRet = At_WifiReset (buff+sizeof(ATCMD_WIFI_RESET)-1);
       else if (!strcmp ((char *)buff, ATCMD_NTP_TIME))
		nRet = At_ShowNtpTime (buff+sizeof(ATCMD_NTP_TIME)-1);
      	else if (!strncmp ((char *)buff, ATCMD_FLASH_ERASE, sizeof (ATCMD_FLASH_ERASE)-1))
		nRet = At_TestFlashErase (buff+sizeof(ATCMD_FLASH_ERASE)-1);
      else if (!strncmp ((char *)buff, ATCMD_PING, sizeof (ATCMD_PING)-1))
		nRet = At_Ping (buff+sizeof(ATCMD_PING)-1);
	else if (!strcmp ((char *)buff, ATCMD_NETSCAN))
		nRet = At_NetScan ();
      else if (!strcmp ((char *)buff, ATCMD_CLEAN_LISTS))
		nRet = At_Clean_Lists ();
      else if (!strncmp ((char *)buff, ATCMD_NETSCAN_CUSTOM,sizeof (ATCMD_NETSCAN_CUSTOM)-1))
            nRet = At_NetScan_Custom (buff+sizeof(ATCMD_NETSCAN_CUSTOM)-1);
	else if (!strcmp ((char *)buff, ATCMD_CONNECT))
		nRet = At_Connect ();
	else if (!strcmp ((char *)buff, ATCMD_DISCONNECT)) 
		nRet = At_Disconnect ();
	else if (!strncmp ((char *)buff, ATCMD_POWERSAVE, sizeof (ATCMD_POWERSAVE)-1)) 
		nRet = At_POWERSAVE(buff+sizeof(ATCMD_POWERSAVE)-1);
	else if (!strncmp ((char *)buff, ATCMD_SET_GPIO, sizeof (ATCMD_SET_GPIO)-1))
		nRet = At_SetGpio (buff+sizeof(ATCMD_SET_GPIO)-1);
	else if (!strncmp ((char *)buff, ATCMD_WRITE_GPIO, sizeof (ATCMD_WRITE_GPIO)-1))
		nRet = At_WriteGpio (buff+sizeof(ATCMD_WRITE_GPIO)-1);
	else if (!strncmp ((char *)buff, ATCMD_READ_GPIO, sizeof (ATCMD_READ_GPIO)-1))
		nRet = At_ReadGpio (buff+sizeof(ATCMD_READ_GPIO)-1);
	else if (!strncmp ((char *)buff, ATCMD_WRITE_REG, sizeof (ATCMD_WRITE_REG)-1))
		nRet = At_WriteReg32 (buff+sizeof(ATCMD_WRITE_REG)-1);
	else if (!strncmp ((char *)buff, ATCMD_READ_REG, sizeof (ATCMD_READ_REG)-1))
		nRet = At_ReadReg32 (buff+sizeof(ATCMD_READ_REG)-1);
	else if (!strncmp ((char *)buff, ATCMD_SET_CHANNEL, sizeof (ATCMD_SET_CHANNEL)-1))
		nRet = At_SetChannel (buff+sizeof(ATCMD_SET_CHANNEL)-1);
      else if (!strncmp ((char *)buff, ATCMD_RF, sizeof (ATCMD_RF)-1))
		nRet = At_ShowRfCommand (buff+sizeof(ATCMD_RF)-1);
      else if (!strncmp ((char *)buff, ATCMD_PHY_COUNT, sizeof (ATCMD_PHY_COUNT)-1))
		nRet = At_GetRfStatus (buff+sizeof(ATCMD_PHY_COUNT)-1);      
	else if (!strncmp ((char *)buff, ATCMD_REMOVE_CONF, sizeof (ATCMD_REMOVE_CONF)-1))
		nRet = AT_RemoveCfsConf (buff+sizeof(ATCMD_REMOVE_CONF)-1);
/*	else if (!strncmp ((char *)buff, ATCMD_PHY_INI, sizeof (ATCMD_PHY_INI)-1))
		nRet = AT_Phyinit ();
	else if (!strncmp ((char *)buff, ATCMD_PHY_CALI, sizeof (ATCMD_PHY_CALI)-1))
		nRet = AT_Phycali (buff+sizeof(ATCMD_PHY_CALI)-1);
	else if (!strncmp ((char *)buff, ATCMD_EVM, sizeof (ATCMD_EVM)-1))
		nRet = AT_Evm (buff+sizeof(ATCMD_EVM)-1);
	else if (!strncmp ((char *)buff, ATCMD_TONE, sizeof (ATCMD_TONE)-1))
		nRet = AT_Tone (buff+sizeof(ATCMD_TONE)-1);
	else if (!strncmp ((char *)buff, ATCMD_SETCH, sizeof (ATCMD_SETCH)-1))
		nRet = AT_ChangeCH (buff+sizeof(ATCMD_SETCH)-1);*/
	else if (!strncmp ((char *)buff, ATCMD_TCPCHANGETIMEOUT, sizeof (ATCMD_TCPCHANGETIMEOUT)-1))
		nRet = At_TCPChange_Timeout (buff+sizeof(ATCMD_TCPCHANGETIMEOUT)-1);	
	else if (!strncmp ((char *)buff, ATCMD_TCPCONNECT, sizeof (ATCMD_TCPCONNECT)-1))
		nRet = At_TCPConnect (buff+sizeof(ATCMD_TCPCONNECT)-1);	
	else if (!strncmp ((char *)buff, ATCMD_TCPSEND, sizeof (ATCMD_TCPSEND)-1)) 
		nRet = At_TCPSend (buff+sizeof(ATCMD_TCPSEND)-1);
	else if (!strncmp ((char *)buff, ATCMD_TCPDISCONNECT, sizeof (ATCMD_TCPDISCONNECT)-1)) 
		nRet = At_TCPDisconnect (buff+sizeof(ATCMD_TCPDISCONNECT)-1);
	else if (!strncmp ((char *)buff, ATCMD_UDPCREATE, sizeof (ATCMD_UDPCREATE)-1))
		nRet = At_UDPCreate (buff+sizeof(ATCMD_UDPCREATE)-1);	
	else if (!strncmp ((char *)buff, ATCMD_UDPSEND, sizeof (ATCMD_UDPSEND)-1)) 
		nRet = At_UDPSend (buff+sizeof(ATCMD_UDPSEND)-1);
	else if (!strncmp ((char *)buff, ATCMD_UDPCLOSE, sizeof (ATCMD_UDPCLOSE)-1)) 
		nRet = At_UDPClose (buff+sizeof(ATCMD_UDPCLOSE)-1);
	else if (!strncmp ((char *)buff, ATCMD_TCPLISTEN, sizeof (ATCMD_TCPLISTEN)-1))		
		nRet = At_TCPListen (buff+sizeof(ATCMD_TCPLISTEN)-1);	
	else if (!strncmp ((char *)buff, ATCMD_TCPUNLISTEN, sizeof (ATCMD_TCPUNLISTEN)-1))		
		nRet = At_TCPUnlisten (buff+sizeof(ATCMD_TCPUNLISTEN)-1);	
	else if (!strncmp ((char *)buff, ATCMD_HTTPTESTSTART, sizeof (ATCMD_HTTPTESTSTART)-1)) 
		nRet = At_HTTPTest (buff+sizeof(ATCMD_HTTPTESTSTART)-1);
	else if (!strncmp ((char *)buff, ATCMD_HTTPTESTSTOP, sizeof (ATCMD_HTTPTESTSTOP)-1)) 
		nRet = At_HTTPTestStop ();
	else if (!strcmp ((char *)buff, ATCMD_GET_LOCALMAC)) 
		nRet = At_GetLocalMac ();
	else if (!strncmp ((char *)buff, ATCMD_PROCESS_DUMP, sizeof (ATCMD_PROCESS_DUMP)-1))
		nRet = At_ProcessAllDump();
	else if (!strncmp ((char *)buff, ATCMD_PROCESS_KILL, sizeof (ATCMD_PROCESS_KILL)-1))
		nRet = At_ProcessKill(buff+sizeof(ATCMD_PROCESS_KILL)-1);
	else if (!strncmp ((char *)buff, ATCMD_PROCESS_QUE_DUMP, sizeof (ATCMD_PROCESS_QUE_DUMP)-1))
		nRet = At_ProcessAllQueDump();
	else if (!strncmp ((char *)buff, ATCMD_PROCESS_QUE_KILL, sizeof (ATCMD_PROCESS_QUE_KILL)-1))
		nRet = At_ProcessQueKill(buff+sizeof(ATCMD_PROCESS_QUE_KILL)-1);
	else if (!strncmp ((char *)buff, ATCMD_SHOWCONNECTAP, sizeof (ATCMD_SHOWCONNECTAP)-1))
		nRet = At_ShowConnectAP();
	else if (!strncmp ((char *)buff, ATCMD_SHOWALLAP, sizeof (ATCMD_SHOWALLAP)-1))
		nRet = At_ShowAllAP();
//phy count
	else if (!strcmp ((char *)buff, ATCMD_BMODE_COUNT))
		nRet = At_bmode_count();
	else if (!strcmp ((char *)buff, ATCMD_BMODE_RESET))
		nRet = At_bmode_reset ();
	else if (!strcmp ((char *)buff, ATCMD_GMODE_COUNT))
		nRet = At_gmode_count ();
	else if (!strcmp ((char *)buff, ATCMD_GMODE_RESET))
		nRet = At_gmode_reset ();
	else if (!strncmp ((char *)buff, ATCMD_FILTER_COUNT, sizeof (ATCMD_FILTER_COUNT)-1)) 
		nRet = At_filter_count (buff+sizeof(ATCMD_FILTER_COUNT)-1);		
	else if (!strncmp ((char *)buff, ATCMD_FILTER_RESET, sizeof (ATCMD_FILTER_RESET)-1)) 
		nRet = At_filter_reset (buff+sizeof(ATCMD_FILTER_RESET)-1);		
//resolv
	else if (!strncmp ((char *)buff, ATCMD_NSLOOKUP, sizeof (ATCMD_NSLOOKUP)-1)) 
		nRet = At_NSLookup (buff+sizeof(ATCMD_NSLOOKUP)-1);		
#ifdef ENABLE_DOWNLOAD_FORM_CLOUD_SERVER	//ota
	else if (!strncmp ((char *)buff, ATCMD_OTASETSERVER, sizeof (ATCMD_OTASETSERVER)-1))
		nRet = At_OTASetServer (buff+sizeof(ATCMD_OTASETSERVER)-1);	
	else if (!strncmp ((char *)buff, ATCMD_OTASETPARAM, sizeof (ATCMD_OTASETPARAM)-1)) 
		nRet = At_OTASetParam (buff+sizeof(ATCMD_OTASETPARAM)-1);
	else if (!strncmp ((char *)buff, ATCMD_OTASTART, sizeof (ATCMD_OTASTART)-1)) 
		nRet = At_OTAStart(buff+sizeof(ATCMD_OTASTART)-1);
#endif 
	else if (!strncmp ((char *)buff, ATCMD_ENABLE_SMARTMODE, sizeof (ATCMD_ENABLE_SMARTMODE)-1)) 
		nRet = At_EnableSmartMode(buff+sizeof(ATCMD_ENABLE_SMARTMODE)-1);
	else if (!strncmp ((char *)buff, ATCMD_ENABLE_SMARTREBOOT, sizeof (ATCMD_ENABLE_SMARTREBOOT)-1)) 
		nRet = At_EnableSmartReboot(buff+sizeof(ATCMD_ENABLE_SMARTREBOOT)-1);

#if CURRENT_TEST
	else if (!strcmp ((char *)buff, ATCMD_POWERON)) 
		nRet = At_PowerOn ();
	else if (!strcmp ((char *)buff,ATCMD_RX))
		nRet = At_RX ();
	else if (!strcmp ((char *)buff,ATCMD_TX))
		nRet = At_TX ();
	else if (!strncmp ((char *)buff, ATCMD_POWERSAVING, sizeof (ATCMD_POWERSAVING)-1))
		nRet = At_PowerSaving (buff+sizeof(ATCMD_POWERSAVING)-1);
    	else if (!strcmp ((char *)buff, ATCMD_AP))
		nRet = At_AP ();
        else if (!strncmp ((char *)buff, ATCMD_SET_APCONFIG, sizeof (ATCMD_SET_APCONFIG)-1))        
		nRet = At_SET_APCONFIG (buff);
    	else if (!strcmp ((char *)buff, ATCMD_AP_EXIT))
		nRet = At_AP_EXIT ();
	else if (!strncmp ((char *)buff, ATCMD_SET_AP_SSID, sizeof (ATCMD_SET_AP_SSID)-1))        
		nRet = At_SET_AP_SSID (buff);
#if 1        
	else if (!strncmp ((char *)buff, ATCMD_SET_AUTO_CONNECT, sizeof (ATCMD_SET_AUTO_CONNECT)-1))        
		nRet = At_SET_AUTO_CONNECT(buff);
    	else if (!strcmp ((char *)buff, ATCMD_SAVE_CONFIG))
		nRet = At_SAVE_CONFIG ();
#endif
        
#endif

	else if (0 == strncmp ((char *)buff, ATCMD_RADIO_CMD, sizeof (ATCMD_RADIO_CMD)-1)) 
		nRet = Parse_Radio_Command(buff+sizeof(ATCMD_RADIO_CMD)-1);	
	else 
	{
	
		//Handle Customer AT Command
		while(1)
		{
			if(atcmd_info_tbl[i].pfHandle == NULL || atcmd_info_tbl[i].atCmd == NULL || strlen(atcmd_info_tbl[i].atCmd) < 0)
			{
				nRet = ERROR_UNKNOWN_COMMAND;
				break;
			}

			//local variable
			stParam param = {0};
			if( atcmd_info_tbl[i].atCmd[strlen(atcmd_info_tbl[i].atCmd)-1] == '=' )
			{
				if (!strncmp ((char *)buff, atcmd_info_tbl[i].atCmd, strlen (atcmd_info_tbl[i].atCmd)-1))
				{
					parseBuff2Param(buff+strlen(atcmd_info_tbl[i].atCmd), &param);
					nRet = atcmd_info_tbl[i].pfHandle(&param);
					break;
				}
			}
			else
			{
				if (!strcmp ((char *)buff, atcmd_info_tbl[i].atCmd))
				{
					nRet = atcmd_info_tbl[i].pfHandle(&param);
					break;
				}
			}
			i++;
		}
	}
	
	if (ERROR_SUCCESS != nRet)
		printf (ATRSP_ERROR, nRet);

exit:
	return nRet;
}

