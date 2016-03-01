#ifndef _SSV_CONFIG_H
#define _SSV_CONFIG_H

#include "ssv_types.h"
#include "ssv_module.h"

#define KB (1024)   // 1KB=1024 Bytes

//wifi,AP
#define CFG_MAX_AP_LISTS          (5)
#define CFG_AT_BUFFER_SIZE        (1500)	//UART for AT-command buffer size,udp max data size:1472
#define CFS_SMART_PHONE_LINK_MODE (1)		//Android <-> IOT final wifi-AP setting,0:no such function,1:icomm-smart-link 2:air-kiss
#define CFG_AUTO_CONNECT (1)    //if ssid being recorded in file system,iot will start to connect to AP.

//flash layout
#define CFG_OTA_ENABLE (1)  //if OTA being eanbled,it will cost a lot of flash space.


#if (MODULE_ID==MODULE_S092)    

#define CFG_FLASH_SIZE  (512 * KB)
#define CFG_BOOT_SECTOR_SIZE      (8 * KB)		//IOT coffee file system size(unit:KB)
#define CFG_MP_SIZE      (4 * KB)		//IOT coffee file system size(unit:KB)
#define CFG_CONFIG_SIZE      (4 * KB)		//IOT coffee file system size(unit:KB)

#elif (MODULE_ID==MODULE_6060Z)

#define CFG_FLASH_SIZE  (512 * KB)
#define CFG_BOOT_SECTOR_SIZE      (4 * KB)		//IOT coffee file system size(unit:KB)
#define CFG_MP_SIZE      (4 * KB)		//IOT coffee file system size(unit:KB)
#define CFG_CONFIG_SIZE      (4 * KB)		//IOT coffee file system size(unit:KB)

#elif (MODULE_ID==MODULE_6060P)

#define CFG_FLASH_SIZE  (512 * KB)
#define CFG_BOOT_SECTOR_SIZE      (4 * KB)		//IOT coffee file system size(unit:KB)
#define CFG_MP_SIZE      (4 * KB)		//IOT coffee file system size(unit:KB)
#define CFG_CONFIG_SIZE      (4 * KB)		//IOT coffee file system size(unit:KB)

#elif (MODULE_ID==MODULE_HIFLYING)

#define CFG_FLASH_SIZE  (2048 * KB)
#define CFG_BOOT_SECTOR_SIZE      (8 * KB)		//IOT coffee file system size(unit:KB)
#define CFG_MP_SIZE      (4 * KB)		//IOT coffee file system size(unit:KB)
#define CFG_CONFIG_SIZE      (4 * KB)		//IOT coffee file system size(unit:KB)

#else  //reserved!!

    #error "Please define MODULE_ID!!!"

#endif




#if (CFG_OTA_ENABLE==0)
#define CFG_IMAGE0_SIZE      (384 * KB)		//IOT coffee file system size(unit:KB)
#define CFG_IMAGE1_SIZE      (0)		//IOT coffee file system size(unit:KB)
#else
#define CFG_IMAGE0_SIZE      (240 * KB)		//IOT coffee file system size(unit:KB)
#define CFG_IMAGE1_SIZE      (CFG_IMAGE0_SIZE)		//IOT coffee file system size(unit:KB)
#endif


//#define CFG_FILE_SYSTEM_SIZE      (32)		//IOT coffee file system size(unit:KB)

#define CFG_CUSTOMER_SIZE      (4 * KB)		//IOT coffee file system size(unit:KB)

#if (CFG_OTA_ENABLE==0)
#define CFG_FILE_SYSTEM_SIZE      (CFG_FLASH_SIZE-CFG_BOOT_SECTOR_SIZE- CFG_MP_SIZE - CFG_CONFIG_SIZE -CFG_IMAGE0_SIZE - CFG_CUSTOMER_SIZE)		//IOT coffee file system size(unit:KB)
#else
#define CFG_FILE_SYSTEM_SIZE      (CFG_FLASH_SIZE - CFG_BOOT_SECTOR_SIZE - CFG_MP_SIZE - CFG_CONFIG_SIZE - CFG_IMAGE0_SIZE -CFG_IMAGE1_SIZE - CFG_CUSTOMER_SIZE)		//IOT coffee file system size(unit:KB)
#endif

#define CFG_FLASH_START_ADDR                   (0x03000000)
#define CFG_BOOT_SECTOR_START_ADDR     (CFG_FLASH_START_ADDR)
#define CFG_MP_START_ADDR                       (CFG_BOOT_SECTOR_START_ADDR + CFG_BOOT_SECTOR_SIZE)
#define CFG_CONFIG_START_ADDR                (CFG_MP_START_ADDR + CFG_MP_SIZE)
#define CFG_BOOT_INFO_START_ADDR                (CFG_CONFIG_START_ADDR + CFG_CONFIG_SIZE)


#define CFG_IMAGE0_START_ADDR                (CFG_CONFIG_START_ADDR + CFG_CONFIG_SIZE)


#if (CFG_OTA_ENABLE==0)
#define CFG_IMAGE1_START_ADDR                (CFG_IMAGE0_START_ADDR)    //image 1 size = 0
#else
#define CFG_IMAGE1_START_ADDR                (CFG_IMAGE0_START_ADDR + CFG_IMAGE0_SIZE)
#endif

#if (CFG_OTA_ENABLE==0)
#define CFG_CUSTOMER_START_ADDR                (CFG_IMAGE0_START_ADDR + CFG_IMAGE0_SIZE)
#else
#define CFG_CUSTOMER_START_ADDR                (CFG_IMAGE1_START_ADDR + CFG_IMAGE1_SIZE)
#endif

#define CFG_FILE_SYSTEM_ADDR                   (CFG_CUSTOMER_START_ADDR + CFG_CUSTOMER_SIZE)

//softap
//#define CFG_SOFTAP (1)    //now it's dynamic switchable by "gwifistatus.softap_enable"
#define CFG_SOFTAP_SSID ("icomm-softap")
#define CFG_SOFTAP_SSID_LENGTH (32)
#define CFG_SOFTAP_MAX_STA_NUM (4)

#define CFG_SOFTAP_ACK_TIMEOUT (10000)      //management frame idle counter,unit:ms(miliseconds)
#define CFG_SOFTAP_DATA_TIMEOUT (20000)      //data frame idle counter,reserved and not used yet!!

//#define CFG_SOFTAP_MAX_MNG_IDLE_COUNTER (200)      //management frame idle counter
//#define CFG_SOFTAP_MAX_DAT_IDLE_COUNTER (2000)      //data frame idle counter


#define CFG_SOFTAP_PASSPRASE ("1234567890")
#define CFG_SOFTAP_PASSPRASE_LENGTH (10)
#define CFG_SOFTAP_BEACON_INTERVAL (100)
#define CFG_SOFTAP_CHANNEL_NO (1)
#define CFG_SOFTAP_DHCP_LEASE_TIME (86400)

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


#endif //_SSV_CONFIG_H

