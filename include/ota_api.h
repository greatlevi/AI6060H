#ifndef OTA_API_H
#define OTA_API_H

#include "uip.h"

//#define ENABLE_DOWNLOAD_FORM_CLOUD_SERVER
/*---------------------------------------------------------------------------*/
//common define with two different down fw file method
#define HF_CONFIG_OTA_FLAG_REDAY			(0x0A0A55A0)
#define HF_CONFIG_OTA_FLAG_OK				(0x0505AA50)
#define HF_BOOT_CONFIG_MAGIC_CODE		(0x48464254)
#define OTA_MAGIC_NUM					(0x4D4D4F49)
#define MAX_PATH 256
#define APP_OTA_PORT (1026)
#define MAX_COMB_BUFFER (5120)
#define MAX_RECE_BUFFER (1024)
#define MAX_CNT (20)
#define WRITE_PAGE_SIZE (4*KB)
#define FOUR_BYTES 4

typedef enum 
{
	BURN_PASS = 0,
	MODULE_FAIL,
	CHECKSUM_FAIL,
	MAGICNUM_FAIL,
} BURN_FLAG;

typedef enum 
{
	DOWNLOAD_START = 0,
	DOWNLOAD_END,
} MSG_CMD;

/*---------------------------------------------------------------------------*/
#ifdef ENABLE_DOWNLOAD_FORM_CLOUD_SERVER
//define with down fw file form cloud server
#define WEBCLIENT_STATE_STATUSLINE 0
#define WEBCLIENT_STATE_HEADERS    1
#define WEBCLIENT_STATE_DATA       2
#define WEBCLIENT_STATE_CLOSE      3
#define HTTPFLAG_NONE   0
#define HTTPFLAG_OK     1
#define HTTPFLAG_MOVED  2
#define HTTPFLAG_ERROR  3
#define BOOT_CONFIG_ADDRESS				(0x000FF000)
#define UPGRADE_ADDRESS					(0x00180000)
#define SOFTWARE_CODE_ADDRESS			(0x00003000)
#define ISO_nl       0x0a
#define ISO_cr       0x0d
#define ISO_space    0x20
//typedef struct with down fw file form cloud server
typedef enum 
{
	OTA_SERVER_CONNECTED= 0x40,
	OTA_SERVER_DISCONNECTED,
	OTA_DOWNLOAD_START,
	OTA_DOWNLOAD_FINISH,
	OTA_SUCCESS,
	OTA_ABORT,
} OTA_MSG;
#endif

//common define with two different down fw file method
typedef struct
{
	uint32_t magic_code;
	uint32_t ver;
	uint32_t ota_flag;
	uint32_t dummy[5];
}SSV_BOOT_CONFIG;

struct OTA_CHECK
{
	U32			magic_num;
	U8			module_id;
	U32			file_check_sum;
}__attribute__ ((packed));

typedef struct
{
#ifdef ENABLE_DOWNLOAD_FORM_CLOUD_SERVER
	U8			download_protocol;	// 0:http 1:ftp 
	char			bin_filename[MAX_PATH];	
	char			bin_filepath[MAX_PATH];
	uip_ip4addr_t	server_ip_addr;
	U16			server_port;
	char			ftp_id[MAX_PATH];
	char			ftp_pw[MAX_PATH];
	U8			parse_state;
	char			httpheaderline[200];
	U16			httpheaderlineptr;
	U8			httpflag;
#endif
	U8			burn_flag;
	U8			exit_cnt;
	U8			cmd_num;
	U8			ota_status;
	U8			rece_buf[MAX_RECE_BUFFER];
	U8			comb_buf[MAX_COMB_BUFFER];
	U8			retry_cnt;
	U8			wait_ack;
	U32			comb_indx;
	U32			check_sum;
	U32			img_addr;
	S32			tcp_client;
	uip_ipaddr_t		phone_ip_addr;
}OTA_CONFIG;
    
#ifdef ENABLE_DOWNLOAD_FORM_CLOUD_SERVER
//function define with down fw file form cloud server
S32 ota_update(struct process *callbackproc);
S32 ota_set_server(U8 nProtocol, char *pIP, U16 port);
S32 ota_set_parameter(char* filename, char* filepath);
#endif
//function define with down fw file form smart phone
void setOtaRrmoteIp(uip_ipaddr_t peeraddr);

#endif

