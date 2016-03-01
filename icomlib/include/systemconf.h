#ifndef __SYSTEMCONF_H__
#define __SYSTEMCONF_H__

#include "contiki-conf.h"
#include "uip.h"
#include "smartComm.h"

#include "ssv_config.h"

#define MAX_APRECORD 	1
#define MAX_AP_LIST		100
#define MAX_CONN_RTYCNT	10
#define CFG_SOFTAP_SSID_LENGTH (32)

typedef enum {
	WIFI_CONNECTION = 0,
	WIFI_DISCONNECTION,
} ieee80211_action;

typedef enum {
	IEEE80211_DISCONNECTED = 0,
	IEEE80211_PROBING_REQ,
	IEEE80211_AUTHENTICATING_SEQ1,
	IEEE80211_AUTHENTICATING_SEQ3,
	IEEE80211_ASSOCIATING_REQ,
	IEEE80211_REASSOCIATING_REQ,
	IEEE80211_EAPOL_HANDSHAKE1,
	IEEE80211_EAPOL_HANDSHAKE3,
	IEEE80211_GETIP,
	IEEE80211_SCANCHANNEL,
	IEEE80211_CONNECTED,
} ieee80211_state;

/* 802.11 authentication and  association Status code, refer to the following URL
https://supportforums.cisco.com/document/141136/80211-association-status-80211-deauth-reason-codes*/
typedef enum {
	IEEE80211_RSP_SUCCESS = 0,
	IEEE80211_RSP_FAILURE = 1,
	IEEE80211_RSP_CAPAB_UNSUPP = 10,
	IEEE80211_RSP_REASSOC_INVALID = 11,
	IEEE80211_RSP_ASSOC_DENIED = 12,
	IEEE80211_RSP_AUTH_ALGO_UNSUPP = 13,
	IEEE80211_RSP_AUTH_SEQ_INVALID = 14,
	IEEE80211_RSP_AUTH_CHALL_INVALID = 15,
	IEEE80211_RSP_AUTH_TIMEOUT = 16,
	IEEE80211_RSP_ASSOC_NO_ROOM = 17,
	IEEE80211_RSP_ASSOC_NEED_RATE = 18,
	IEEE80211_RSP_ASSOC_NEED_SHORT_PMBL = 19,
	IEEE80211_RSP_ASSOC_NEED_PBCC = 20,
	IEEE80211_RSP_ASSOC_NEED_CHAN_AGILITY = 21,
	IEEE80211_RSP_ASSOC_NEED_SPECTRUM_MGMT = 22,
	IEEE80211_RSP_ASSOC_BAD_POWER = 23,
	IEEE80211_RSP_ASSOC_BAD_CHANNELS = 24,
	IEEE80211_RSP_ASSOC_NEED_SHORT_SLOT = 25,
	IEEE80211_RSP_ASSOC_NEED_DSSS_OFDM = 26,
	IEEE80211_RSP_QOS_FAILURE = 32,
	IEEE80211_RSP_QOS_NO_ROOM = 33,
	IEEE80211_RSP_LINK_IS_HORRIBLE = 34,
	IEEE80211_RSP_ASSOC_NEED_QOS = 35,
	IEEE80211_RSP_REQUEST_DECLINED = 37,
	IEEE80211_RSP_REQUEST_INVALID = 38,
	IEEE80211_RSP_TS_NOT_CREATED_AGAIN = 39,
	IEEE80211_RSP_INVALID_IE = 40,
	IEEE80211_RSP_GROUP_CIPHER_INVALID = 41,
	IEEE80211_RSP_PAIR_CIPHER_INVALID  = 42,
	IEEE80211_RSP_AKMP_INVALID = 43,
	IEEE80211_RSP_RSN_VERSION_UNSUPP = 44,
	IEEE80211_RSP_RSN_CAPAB_INVALID = 45,
	IEEE80211_RSP_CIPHER_REJECTED = 46,
	IEEE80211_RSP_TS_NOT_CREATED_WAIT = 47,
	IEEE80211_RSP_DIRECT_LINK_FORBIDDEN = 48,
	IEEE80211_RSP_DEST_NOT_PRESENT = 49,
	IEEE80211_RSP_DEST_NOT_QOS = 50,
	IEEE80211_RSP_ASSOC_LISTEN_TOO_HIGH = 51,
} ieee80211_rsp_state;

typedef enum _MP_ID
{
    MP_MAC_ADDR=0,
    MP_RF_TABLE,
    MP_HT_TABLE,
    MP_LT_TABLE,
    MP_TEMP_BOUNDARY,
    MP_SERIAL_NO,
    MP_NUM_MAX
} MP_ID;


#if 0 //move to ssv_confg.h
typedef enum _CONFIG_ID
{
    CONFIG_SOFTAP_SSID=0,
    CONFIG_SOFTAP_CHANNEL,
    CONFIG_SOFTAP_ACK_TIMEOUT,
    CONFIG_SOFTAP_DATA_TIMEOUT,
    CONFIG_AUTO_CONNECT,
    CONFIG_REGION_CODE,
    CONFIG_SLINK_MODE,
    CONFIG_SOFTAP_SSID_LENGTH,
    CONFIG_WATCH_DOG_COUNT,     //unit: mini-seconds!

    CONFIG_SOFTAP_ENCRYPT_MODE,     //unit: mini-seconds!
    CONFIG_SOFTAP_KEY,
    CONFIG_SOFTAP_KEYLEN,
    
#if 0
    CONFIG_REGION_CODE = 0,
    CONFIG_DEBUG_LEVEL,
    CONFIG_DEBUG_GROUP

    CONFIG_WIFI_MODE,
    CONFIG_DHCP_ENABLE,
    CONFIG_SOFT_AP,
    CONFIG_INTO_SMART_LINK,
    CONFIG_LOCAL_IP_ADDR,
    CONFIG_NET_MASK,
    CONFIG_GATEWAY_IP_ADDR,
    CONFIG_AP_COUNT,
    CONFIG_AP_RECORD
#endif
    CONFIG_NUM_MAX
} CONFIG_ID;

#endif


typedef struct t_AP_DETAIL_INFO
{
	u8_t 		ssid[32];
	u8_t		ssid_len;
	u8_t		key[64];
	u8_t		key_len;
	u8_t 		mac[6];
	char		pmk[32];
	u8_t		channel;
	u8_t		security_type;
	u8_t		security_subType;
} AP_DETAIL_INFO;

typedef struct t_TAG_AP_INFO
{
	u8_t 		name[32];
	u8_t		name_len;
	u8_t 		mac[6];
	u8_t		channel;
	u8_t		security_type;
	u8_t		security_subType;
	u8_t		rssi;
	u8_t		rssi_level;
	u8_t		snr;
} TAG_AP_INFO;
TAG_AP_INFO ap_list[MAX_AP_LIST];

typedef enum t_TAG_SECURITY
{
	NONE = 0,
	WEP,
	WPA,
	WPA2,
	WPAWPA2,
} TAG_SECURITY;

typedef enum t_POWER_MODE
{
	NORMAL_MODE = 0,
	SAVEING_MODE,
} POWER_MODE;

typedef enum t_CLOCK_MODE
{
	CLOCK_40M = 0,
	CLOCK_26M,
} CLOCK_MODE;

typedef enum t_TAG_SECURITY_TYPE
{
	NOSECTYPE = 0,
	TKIP,
	CCMP,
	TKIP_CCMP,
} TAG_SECURITY_TYPE;

#if 0//move to ssv_config.h 2015/12/24
typedef struct _COMMON_CONFIG
{
    char softap_ssid[32];
    U8 softap_channel;
    U32 softap_ack_timeout;
    U32 softap_data_timeout;
    U8 auto_connect;

    //added in 2015/11/27
    U32 region_code;
    U8 slink_mode;
    U8 softap_ssid_length;
    U32 watch_dog_count; //unit: (mini-seconds)

    U8 softap_encryt_mode;  //0: open, 1:WPA2-tkip 2:WPA2:CCMP
    U8 softap_key[64];
    U8 softap_keylen;

    //U32         debug_level;
    //U32         debug_group;
    //U32 all kinds time_out setting 
    //other wifi attributes
    //ps-poll
    //wifi_mode  b-only/bg/bgn
} COMMON_CONFIG;
#endif

typedef struct t_TAG_CABRIO_CONFIGURATION
{
       U8                       debugLevel;
	U8				wifi_mode;			// 1: AP mode, 0: STA-mode, IBSS, Infrastructure
	U8				dhcp_enable;		// 1: Enable 0: Fix IP
	uip_ip4addr_t		local_ip_addr;
	uip_ip4addr_t		net_mask;
	uip_ip4addr_t		gateway_ip_addr;
	U8				enableSmartSoftFilter;
	U8				APcount;
	AP_DETAIL_INFO  	APrecord[MAX_APRECORD];
} TAG_CABRIO_CONFIGURATION;

typedef enum _ssv_rc_rate_type {
    RC_TYPE_B_ONLY=0,
    RC_TYPE_LEGACY_GB,
    RC_TYPE_HT_SGI_20,
    RC_TYPE_HT_LGI_20,
    RC_TYPE_HT_GF,
    RC_TYPE_MAX,
} ssv_rc_rate_type;

typedef struct t_DATARATE_INFO
{
	U8 datarate;
	U8 index;
	ssv_rc_rate_type ratetype;
	U8 succfulcnt;
    U8 failcnt;
}DATARATE_INFO;


typedef enum {
   eSOFTAP_STA_ID0=0,      //client 0
   eSOFTAP_STA_ID1,        //client 1
   eSOFTAP_STA_ID2,        //client 2 
   eSOFTAP_STA_ID3,        //client 3 
   eSOFTAP_STA_MAX_ID        //client 1
} eSOFTAP_STA_ID;

typedef struct _STA_INFO    //0:use peermac0,1:use peermac1
{
    U8 connected;    
//    U8 state;
    U8 mac[6];    //only one sta available now!!
    U8 ipaddr[4];
    S32 idle_timer;
    S32 softap_tx_null_data_idle_time;
} STA_INFO;


typedef struct t_PS_STATUS
{
    U8 pwsave_en:1;
    U8 pwsave_run:1;
    U8 txsend:1;
    U8 pwmode:1;
    U8 clockmode:1;
    U8 pollnorsp:1;
    U8 reserve:2;
}PS_STATUS;

typedef struct t_IEEE80211STATUS
{
	AP_DETAIL_INFO		connAP;
	u8_t				local_mac[6];
	
    ieee80211_action 	action;
    ieee80211_state 	status;
    U32	 		 	 	scantime;
    U8	 		 	 	recordAP;      
    U8	 		 	 	wifi_connect;
    U8	 		 	 	connectmethod;
    U8	 		 	 	checkbeacon;
    U8					retrycnt;
    U16					capability_info;
    U32					txengflow;
    U32					engstart;
    TAG_SECURITY		wifi_security;
    U8 					ht_support;
    u16_t				ht_capab;
    DATARATE_INFO		rateinfo;
	U16 				beacon_interval;
    U8 					association_id;
    U8 					aplist_index;
    U16                 unicastseq;
    struct process 	    *callbackproc;
    //mac_rx_handler execute counter!! used to measure input packets counter!!! If it didn't change any more,it means something wrong!!
    U32 mac_rx_cnt;
    PS_STATUS psstate;

    U8 auto_connect;

    //added in 2015/11/27
    U32 region_code;
    U8 slink_mode;
    U32 watch_dog_count; //unit: (mini-seconds)    

    //softap status!!
    U8 softap_enable;
    U8 softap_state;

#if 0
    S8 softap_state_cnt;
    U8 softap_sta_mac_tmp[6];    //previous STA ,this variable is used for tx-disassociation
    U8 softap_sta_mac_connected[6];    //only one sta available now!!
    U8 softap_sta_connected;
    S32 softap_sta_idle_timer;
#endif

    S16 softap_sta_connected_number;                //0 or 1 or 2
    //S8 softap_state_cnt;                //0 or 1
    U8 softap_sta_mac_tmp[6];    //previous STA ,this variable is used for tx-disassociation   ??? to-do we should have more softap_sta_mac_tmp ???

#if 1
//   S32 softap_tx_null_data_idle_time;
//    U8 softap_sta_mac_connected[6];    //only one sta available now!!
//    U8 softap_sta_connected;
#endif

    STA_INFO softap_sta_info[CFG_SOFTAP_MAX_STA_NUM];

    U8 softap_key[64];
    U8 softap_keylen;
    U8 softap_pmk[32];
    U8 softap_encryt_mode;  //0: open, 1:WPA2-tkip 2:WPA2:CCMP

//<<config start
    S8 softap_ssid[CFG_SOFTAP_SSID_LENGTH+1];   //+1 to prevent printf core dump!!
    S8 softap_ssid_length;   //+1 to prevent printf core dump!!
    U8 softap_channel;
    U32 softap_ack_timeout;
    U32 softap_data_timeout;
//>>config end
    
    U16 softap_seq_no;    //sequence number!!
    U16 softap_freg_no;   //fregment number!!
    U16 softap_seq_and_freg_no;
    U64 softap_timestamp;   //time stamp
    S16 softap_beacon_interval;

    //smart&airkiss status
    U8 smart_state[2];
    U8 smart_tmerSec;
}IEEE80211STATUS;

int remove_sysconfig(void);
int write_wifi_config(void);
int read_wifi_config(void);
void reset_global_conf();
void init_global_conf();

S32 gconfig_set_softap_ssid(char *p_ssid);
S32 gconfig_set_enable_smart(SMARK_LINK_TYPE val, U8 change_now);
S32 gconfig_set_auto_connect(U8 val);
S32 gconfig_save_config();
void clear_ap_lists();
#endif
