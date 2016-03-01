#include "uip.h"
#include "socket_driver.h"
#include "ota_api.h"
#include "uiplib.h"
#include "irq.h"
#include "flash_api.h"
#include "wdog_api.h"
#include "socket_app.h"
#include "net/uip_arp.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------*/
//common process with two different down fw file method
PROCESS(auto_reboot_process, "Auto Reboot Process");
/*---------------------------------------------------------------------------*/
#ifdef ENABLE_DOWNLOAD_FORM_CLOUD_SERVER
//process with download fw file form cloud server
PROCESS(ota_connect_process, "OTA Connect Process");
#endif
/*---------------------------------------------------------------------------*/
//process with download fw file form smart phone
PROCESS(ota_updateProcess, "ota update Process");
PROCESS(tcp_client_otaProcess, "tcp_client_otaProcess");
/*---------------------------------------------------------------------------*/
//common global value with two different down fw file method
OTA_CONFIG ota_conf;
/*---------------------------------------------------------------------------*/
#ifdef ENABLE_DOWNLOAD_FORM_CLOUD_SERVER
//global value with download fw file form cloud server
static struct process* appcallbackproc = NULL;
static U32 addressImg = 0x0;
int gclientota = -1;

const char http_10[9] =
/* "HTTP/1.0" */
{0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x30, };
const char http_11[9] =
/* "HTTP/1.1" */
{0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x31, };
const char http_200[5] =
/* "200 " */
{0x32, 0x30, 0x30, 0x20, };
const char http_301[5] =
/* "301 " */
{0x33, 0x30, 0x31, 0x20, };
const char http_302[5] =
/* "302 " */
{0x33, 0x30, 0x32, 0x20, };
const char http_404[5] =
/* "404 " */
{0x34, 0x30, 0x34, 0x20, };
const char http_crnl[3] =
/* "\r\n" */
{0xd, 0xa, };
const char http_host[7] =
/* "host: " */
{0x68, 0x6f, 0x73, 0x74, 0x3a, 0x20, };
const char http_content_type[15] =
/* "content-type: " */
{0x63, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, };

const char http_content_length[17] =
/* "content-length: " */
{0x63, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x6c, 0x65, 0x6e, 0x67, 0x74, 0x6e, 0x3a, 0x20, };
	
const char http_content_type_binary[43] = 
/* "Content-type: application/octet-stream\r\n\r\n" */
{0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x61, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2f, 0x6f, 0x63, 0x74, 0x65, 0x74, 0x2d, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6d, 0xd, 0xa, 0xd, 0xa, };
const char http_location[11] =
/* "location: " */
{0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x3a, 0x20, };
const char http_http[8] =
/* "http://" */
{0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, };

static char* data = 0;
#endif
/*---------------------------------------------------------------------------*/
//common function api with two different down fw file method
int printf(const char *fmt, ...);

void cal_ota_checksum(int *data, int len) 
{
    int i = 0;
    U8 tmp = 0;
    
    if(len > WRITE_PAGE_SIZE) {
        printf("cal_ota_checksum len over buff\n");
        return;
    }
    
    tmp = (len % FOUR_BYTES == 0) ?  0 : 1;
    for(i = 0; i < (len >> 2) + tmp; i++) {
        ota_conf.check_sum = ota_conf.check_sum ^ *(data + i);
    }
}

int downloadFinish()
{
    struct OTA_CHECK ota_check;
    SSV_BOOT_CONFIG boot_config;
    U32 config_addr = CFG_CONFIG_START_ADDR - CFG_FLASH_START_ADDR;
    U32 ota_confg_offset = CFG_CONFIG_SIZE -sizeof(SSV_BOOT_CONFIG);
    U8 *flash_buf = drv_flash_get_cache_addr();
 
    memcpy(&ota_check, ota_conf.comb_buf+ ota_conf.comb_indx - sizeof(struct OTA_CHECK), sizeof(struct OTA_CHECK));

    printf("ota_check.magic_num: %x\n", ota_check.magic_num);
    printf("ota_check.module_id: %x\n", ota_check.module_id);
    printf("ota_check.file_check_sum: %x\n", ota_check.file_check_sum);
    
    if(ota_check.magic_num == OTA_MAGIC_NUM) {
        printf("ota_conf.check_sum: %x ota_check.file_check_sum: %x\n", ota_conf.check_sum, ota_check.file_check_sum);
        if(ota_check.file_check_sum == ota_conf.check_sum) {
            printf("MODULE_ID: %d ota_check.module_id: %d\n", MODULE_ID, ota_check.module_id);
            if(MODULE_ID == ota_check.module_id) {
                ota_conf.burn_flag = BURN_PASS;
                boot_config.ota_flag = HF_CONFIG_OTA_FLAG_REDAY;
                printf("check sum and module pass\n");
            } else {
                ota_conf.burn_flag = MODULE_FAIL;
                boot_config.ota_flag = HF_CONFIG_OTA_FLAG_OK;
                printf("module id fail\n");
            }
        } else {
            ota_conf.burn_flag = CHECKSUM_FAIL;
            boot_config.ota_flag = HF_CONFIG_OTA_FLAG_OK;
            printf("check sum fail\n");
        }
    } else {
        ota_conf.burn_flag = MAGICNUM_FAIL;
        boot_config.ota_flag = HF_CONFIG_OTA_FLAG_OK;
        printf("check ota magic num fail\n");
    }
    
    boot_config.magic_code = HF_BOOT_CONFIG_MAGIC_CODE;
    
    memset(flash_buf, 0, CFG_CONFIG_SIZE);
    spi_flash_read(config_addr, flash_buf, CFG_CONFIG_SIZE);
    memcpy(flash_buf + ota_confg_offset, &boot_config, sizeof(boot_config));
    spi_flash_sector_erase(config_addr);
    spi_flash_write(config_addr, flash_buf, CFG_CONFIG_SIZE);
    spi_flash_finalize(config_addr);
}
/*---------------------------------------------------------------------------*/
#ifdef ENABLE_DOWNLOAD_FORM_CLOUD_SERVER
//function api with download fw file form cloud server
U16 parser_data(char *buf, U16 len);

S32 ota_abort()
{
	//printf("OTA ABORT!!!\n");
	if(appcallbackproc != NULL)
		process_post(appcallbackproc, OTA_ABORT, NULL);
	return 0;
}

S32 ota_success()
{
	//printf("OTA SUCCESS!!!\n");
	if(appcallbackproc != NULL)
		process_post(appcallbackproc, OTA_SUCCESS, NULL);
	
	process_start(&auto_reboot_process, NULL);
	return 0;
}

S32 ota_download_file()
{	
	printf("start Download image file: %s\n", ota_conf.bin_filename);
	
	char strFile[200] = {0};
	
	sprintf(strFile, "GET /%s%s HTTP/1.0\r\nHost: \r\n\r\n", ota_conf.bin_filepath, ota_conf.bin_filename);
	if(gclientota == -1)
		return ERROR_TCP_CONNECTION;
	if(tcpsend(gclientota, strFile, strlen (strFile)) == -1)
	{
		return ERROR_TCP_CONNECTION;
	}
	else
	{
		if(appcallbackproc != NULL)
			process_post(appcallbackproc, OTA_DOWNLOAD_START, NULL);	
		return ERROR_SUCCESS;
	}
}

S32 ota_connected()
{
	//printf("OTA CONNECTED!!!\n");
	if(appcallbackproc != NULL)
		process_post(appcallbackproc, OTA_SERVER_CONNECTED, NULL);
	ota_download_file();
	
	return 0;
}

S32 ota_disconnected()
{
	//printf("OTA DISCONNECTED!!!\n");

	if(appcallbackproc != NULL)
		process_post(appcallbackproc, OTA_SERVER_DISCONNECTED, NULL);
	
	if(ota_conf.httpflag != HTTPFLAG_MOVED) 
	{
		data = NULL;
		if( parser_data(data, 0) == 0)
			ota_success();
		else
			ota_abort();
	}
	else
	{
		ota_abort();
	}

	return 0;
}

S32 ota_init()
{
	//printf("OTA INIT!!!\n");
	memset(&ota_conf, 0, sizeof(ota_conf));
	return 0;
}

char casecmp(char *str1, const char *str2, char len)
{
	static char c;
  
	while(len > 0) 
	{
		c = *str1;
		if(c & 0x40) 
		{
			c |= 0x20;
		}
		if(*str2 != c) 
		{
			return 1;
		}
		++str1;
		++str2;
		--len;
	}
	return 0;
}

int filesave(char *data, int len)
{
	if(data == NULL)
	{
		//printf("\r\nDownload Finish %d Bytes\n", len_file);
		spi_flash_sector_erase(addressImg+CFG_IMAGE1_START_ADDR - CFG_FLASH_START_ADDR);
		spi_flash_write(addressImg+CFG_IMAGE1_START_ADDR - CFG_FLASH_START_ADDR, ota_conf.comb_buf, 0x1000);
		spi_flash_finalize(addressImg+CFG_IMAGE1_START_ADDR - CFG_FLASH_START_ADDR);
		cal_ota_checksum((int *)ota_conf.comb_buf, ota_conf.comb_indx - sizeof(struct OTA_CHECK));
		//printf("flash write over...%x\n", addressImg+CFG_IMAGE1_START_ADDR - CFG_FLASH_START_ADDR);	
		downloadFinish();
	}
	else
	{
		if( ota_conf.comb_indx+len>= WRITE_PAGE_SIZE)
		{        
			int copylen = WRITE_PAGE_SIZE-ota_conf.comb_indx;
			memcpy(ota_conf.comb_indx+ota_conf.comb_buf, data , copylen );      
	            ota_conf.comb_indx+=copylen;
	            //write buff to flash
			//printf("flash write start...%x\n", addressImg+CFG_IMAGE1_START_ADDR - CFG_FLASH_START_ADDR);
			spi_flash_sector_erase(addressImg+CFG_IMAGE1_START_ADDR - CFG_FLASH_START_ADDR);
			spi_flash_write(addressImg+CFG_IMAGE1_START_ADDR - CFG_FLASH_START_ADDR, ota_conf.comb_buf, 0x1000);
			spi_flash_finalize(addressImg+CFG_IMAGE1_START_ADDR - CFG_FLASH_START_ADDR);
			cal_ota_checksum(ota_conf.comb_buf, ota_conf.comb_indx);
			//printf("flash write over...%x\n", addressImg+CFG_IMAGE1_START_ADDR - CFG_FLASH_START_ADDR);    
			addressImg+=WRITE_PAGE_SIZE;
			//copy
			memset(ota_conf.comb_buf, 0, 5000);
			ota_conf.comb_indx = 0;
			memcpy(ota_conf.comb_indx+ota_conf.comb_buf , data+copylen,len-copylen );              
			ota_conf.comb_indx+=(len-copylen);
		}
		else
		{
			//copy         
			memcpy(ota_conf.comb_indx+ota_conf.comb_buf, data, len);           
			ota_conf.comb_indx+=len;            
		}

	}	

	return 0;
}

//porting code
U16 parse_statusline(char *buf, U16 len)
{
	char *cptr;
	while(len > 0 && ota_conf.httpheaderlineptr < sizeof(ota_conf.httpheaderline)) 
	{
		ota_conf.httpheaderline[ota_conf.httpheaderlineptr] = *(char *)data;	
		data = data + 1;
		--len;
		if(ota_conf.httpheaderline[ota_conf.httpheaderlineptr] == ISO_nl) 
		{
			if((strncmp(ota_conf.httpheaderline, http_10, sizeof(http_10) - 1) == 0) || (strncmp(ota_conf.httpheaderline, http_11, sizeof(http_11) - 1) == 0)) 
			{
				cptr = &(ota_conf.httpheaderline[9]);
				ota_conf.httpflag = HTTPFLAG_NONE;
				if(strncmp(cptr, http_200, sizeof(http_200) - 1) == 0) 
				{
					//printf("200 OK !!\n");
					ota_conf.httpflag = HTTPFLAG_OK;
				}
				else if(strncmp(cptr, http_301, sizeof(http_301) - 1) == 0 || strncmp(cptr, http_302, sizeof(http_302) - 1) == 0 ||strncmp(cptr, http_404, sizeof(http_404) - 1) == 0 ) 
				{
					//printf("301 or 302 or 404 Not Found !!\n");
					ota_conf.httpflag = HTTPFLAG_MOVED;
				} 
				else 
				{
					ota_conf.httpheaderline[ota_conf.httpheaderlineptr - 1] = 0;
				}
			
			} 
			else 
			{	
				ota_abort();
				return 0;
			}
      
			// We're done parsing the status line, so we reset the pointer and start parsing the HTTP headers.
			ota_conf.httpheaderlineptr = 0;
			ota_conf.parse_state = WEBCLIENT_STATE_HEADERS;
			break;
		
		} 
		else 
		{
			++ota_conf.httpheaderlineptr;
		}

	}
	
	return len;
}

U16 parse_headers(char *buf, U16 len)
{
	char *cptr;
	static unsigned char i;

	while(len > 0 && ota_conf.httpheaderlineptr < sizeof(ota_conf.httpheaderline)) 
	{
		ota_conf.httpheaderline[ota_conf.httpheaderlineptr] = *(char *)data;
		data = data + 1;
		--len;
		if(ota_conf.httpheaderline[ota_conf.httpheaderlineptr] == ISO_nl) 
		{
			//We have an entire HTTP header line in httpheaderline, so we parse it.
			if(ota_conf.httpheaderline[0] == ISO_cr) 
			{
				//This was the last header line (i.e., and empty "\r\n"), so we are done with the headers and proceed with the actual data.
				ota_conf.parse_state = WEBCLIENT_STATE_DATA;
				return len;
			}

			ota_conf.httpheaderline[ota_conf.httpheaderlineptr - 1] = 0;
			//Check for specific HTTP header fields.
			if(casecmp(ota_conf.httpheaderline, http_content_type, sizeof(http_content_type) - 1) == 0) 
			{
			
			} 	
			else if(casecmp(ota_conf.httpheaderline, http_content_length, sizeof(http_content_length) - 1) == 0) 
			{
			
			}
			else if(casecmp(ota_conf.httpheaderline, http_location, sizeof(http_location) - 1) == 0) 
			{

			}
			else if(casecmp(ota_conf.httpheaderline, http_location, sizeof(http_location) - 1) == 0) 
			{
			}
	 		//We're done parsing, so we reset the pointer and start the next line.
	 		ota_conf.httpheaderlineptr = 0;
	
 		} 
 		else 
 		{
			++ota_conf.httpheaderlineptr;
		}
		
	}

	return len;
}

U16 parser_data(char *buf, U16 len)
{
	//printf("parser_data %d \n", len);
	//printf("%x %x %x %x\n", *data, *(data+1), *(data+2), *(data+3));
	static unsigned long dload_bytes;
	int ret;

	if(len > 0) 
	{
		dload_bytes += len;
		//printf("Downloading (%lu bytes)\n", dload_bytes);
		//filesave(buf, len);
		filesave(data, len);
	}

	if(data == NULL) 
	{
		if(dload_bytes == 0 )
			return 1;
		printf("\nFinished downloading %lu bytes.\n", dload_bytes);
		filesave(data, len);
	}
	
	return 0;
}

void ota_newdata(char* buf, U16 rece_len)
{
	U16 len;
	char* pData;
	len = rece_len;
	data = buf;
	printf(".");	
	if(ota_conf.parse_state == WEBCLIENT_STATE_STATUSLINE) 
	{
		len = parse_statusline(buf, len);
	}
  
	if(ota_conf.parse_state == WEBCLIENT_STATE_HEADERS && len > 0) 
	{
		len = parse_headers(buf, len);
	}

	if(len > 0 && ota_conf.parse_state == WEBCLIENT_STATE_DATA && ota_conf.httpflag != HTTPFLAG_MOVED) 
	{
		parser_data(buf, len);
	}	
}

////Export API
S32 ota_update(struct process *callbackproc)
{

	printf("OTA START:\n");
	printf("Server %d.%d.%d.%d port = %d\n", ota_conf.server_ip_addr.u8[0], ota_conf.server_ip_addr.u8[1], ota_conf.server_ip_addr.u8[2], ota_conf.server_ip_addr.u8[3], ota_conf.server_port);
	printf("BIN Filename: %s\n", ota_conf.bin_filename);

	appcallbackproc = callbackproc;
	//ota_conf.ota_state = OTA_INIT;
	ota_conf.parse_state = WEBCLIENT_STATE_STATUSLINE;
	ota_conf.httpheaderlineptr = 0;
	ota_conf.check_sum = 0;
	
	process_start(&ota_connect_process, NULL);
	//process_start(&ota_update_process, NULL);

	gclientota = tcpconnect( &ota_conf.server_ip_addr , ota_conf.server_port, &ota_connect_process);
	
	return 0;
}
S32 ota_set_server(U8 nProtocol, char *pIP, U16 port)
{
	printf("ota_set_server ip:%s, port:%d\n", pIP, port);
	ota_conf.download_protocol = nProtocol;
	ota_conf.server_port = port;
	if( uiplib_ipaddrconv(pIP, &ota_conf.server_ip_addr)  == 0)
	{
		printf("erro ip format\n");
		return 1;
	}
	return 0;
}
S32 ota_set_parameter(char* filename, char* filepath)
{
	//printf("ota_set_parameter file=[%s], path=[%s]\n", filename, filepath);
	memset(ota_conf.bin_filename, 0, MAX_PATH);
	memset(ota_conf.bin_filepath, 0, MAX_PATH);
	if(filename != NULL && (strlen(filename) < MAX_PATH && strlen(filename) > 0))
	{
		memcpy(ota_conf.bin_filename, filename, strlen(filename));
	}
	else
		return 1;
	
	if(filepath != NULL && (strlen(filepath) < MAX_PATH && strlen(filepath) > 0))
	{
		if( *filepath == '/')
			memcpy(ota_conf.bin_filepath, filepath+1, strlen(filepath)-1);
		else
		memcpy(ota_conf.bin_filepath, filepath, strlen(filepath));

		if(ota_conf.bin_filepath[strlen(ota_conf.bin_filepath)-1] != '/' )
		{
			ota_conf.bin_filepath[strlen(ota_conf.bin_filepath)] = '/';
			ota_conf.bin_filepath[strlen(ota_conf.bin_filepath)+1] = '0';	
		}	
	}
	printf("filename:%s\n", ota_conf.bin_filename);
	printf("filenpath:%s\n", ota_conf.bin_filepath);
	
}
#endif
/*---------------------------------------------------------------------------*/
//function api with download fw file form smart phone
int filesave_from_phone(char *data, int len)
{
    int i = 0;
    U32 now_addr = 0;

    now_addr =ota_conf.img_addr + CFG_IMAGE1_START_ADDR - CFG_FLASH_START_ADDR;
    if((now_addr <  (CFG_IMAGE1_START_ADDR - CFG_FLASH_START_ADDR)) || 
        (now_addr > (CFG_CUSTOMER_START_ADDR  - CFG_FLASH_START_ADDR))) {
        printf("#####flash write over image1 range: %x, please check#####\n", now_addr);
        return -1;
    }
    
    if(data == NULL) {
        printf("Finish flash write addr:%x, write size: %d\n", now_addr, ota_conf.comb_indx);
        spi_flash_sector_erase(now_addr);
        spi_flash_write(now_addr, ota_conf.comb_buf, WRITE_PAGE_SIZE);
        spi_flash_finalize(now_addr);
        cal_ota_checksum(ota_conf.comb_buf, ota_conf.comb_indx - sizeof(struct OTA_CHECK));
        downloadFinish();
    } else {
        if( ota_conf.comb_indx+len>= WRITE_PAGE_SIZE) {        
            int copylen = WRITE_PAGE_SIZE-ota_conf.comb_indx;
            memcpy(ota_conf.comb_indx+ota_conf.comb_buf, data , copylen );      
            ota_conf.comb_indx+=copylen;
            //write buff to flash
            printf("Start flash write addr:%x, write size: %d\n", now_addr, ota_conf.comb_indx);
            spi_flash_sector_erase(now_addr);
            spi_flash_write(now_addr, ota_conf.comb_buf, WRITE_PAGE_SIZE);
            spi_flash_finalize(now_addr);
            cal_ota_checksum(ota_conf.comb_buf, ota_conf.comb_indx);
            ota_conf.img_addr+=WRITE_PAGE_SIZE;
            //copy
            memset(ota_conf.comb_buf, 0, MAX_COMB_BUFFER);
            ota_conf.comb_indx = 0;
            memcpy(ota_conf.comb_buf , data+copylen,len-copylen );              
            ota_conf.comb_indx+=(len-copylen);
        } else {
            //copy         
            memcpy(ota_conf.comb_indx+ota_conf.comb_buf, data, len);           
            ota_conf.comb_indx+=len;
        }
    }	

    return 0;
}
/*---------------------------------------------------------------------------*/
int OtaTcpConnect()
{
    ota_conf.ota_status = SOCKET_CREATE;

    ota_conf.tcp_client = tcpconnect(&ota_conf.phone_ip_addr, APP_OTA_PORT, &tcp_client_otaProcess);
    printf("create tcp ota socket:%d\n", ota_conf.tcp_client);
    
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int OtaTcpDisConnect()
{
    ota_conf.ota_status = SOCKET_CLOSED;
    tcpclose(ota_conf.tcp_client);
    printf("close tcp ota socket:%d\n", ota_conf.tcp_client);
    
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
void setOtaRrmoteIp(uip_ipaddr_t peeraddr)
{
    memcpy(&ota_conf.phone_ip_addr, &peeraddr, sizeof(uip_ipaddr_t));
}
/*---------------------------------------------------------------------------*/
void clearOtaParameters()
{
    ota_conf.comb_indx = 0;
    ota_conf.ota_status = SOCKET_CREATE;
    ota_conf.exit_cnt = 0;
    ota_conf.tcp_client = -1;
    ota_conf.img_addr = 0;
}
/*---------------------------------------------------------------------------*/
//common process with two different down fw file method
PROCESS_THREAD(auto_reboot_process, ev, data)
{
	static struct etimer ota_timer;
	static S8 count;
	
	PROCESS_BEGIN();
	count = 5;
	while(count > 0)
	{
		etimer_set(&ota_timer, 1 * CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
		printf("\nSystem will auto reboot after %d second\n", count);
		count--;
	}
	
	api_wdog_reboot();
	PROCESS_END();
}	
/*---------------------------------------------------------------------------*/
#ifdef ENABLE_DOWNLOAD_FORM_CLOUD_SERVER
//process with download fw file form cloud server
PROCESS_THREAD(ota_connect_process, ev, data)
{
	PROCESS_BEGIN();
	SOCKETMSG msg;
	int recvlen;
	uip_ipaddr_t peeraddr;
	U16 peerport;

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
				if(msg.socket == gclientota)
				{	
					ota_connected();
					//process_post (&ota_update_process, OTA_SERVER_CONNECTED, &ota_conf);
				}
			}
			//TCP connection is closed. Clear the socket number.
			else if(msg.status == SOCKET_CLOSED)
			{
				ota_disconnected();
				//process_post (&ota_update_process, OTA_SERVER_DISCONNECTED, &ota_conf);
				if(gclientota == msg.socket)
					gclientota = -1;
			}
			//Get ack, the data trasmission is done. We can send data after now.
			else if(msg.status == SOCKET_SENDACK)
			{
				//printf("socked:%d send data ack\n", msg.socket);
			}
			//There is new data coming. Receive data now.
			else if(msg.status == SOCKET_NEWDATA)
			{
				if(0 <= msg.socket && msg.socket < UIP_CONNS)
				{
					//if(ota_conf.ota_state == OTA_CONNECT)
						//process_post (&ota_update_process, OTA_DOWNLOAD_START, &ota_conf);
					
					recvlen = tcprecv(msg.socket, ota_conf.rece_buf, MAX_RECE_BUFFER);
					ota_conf.rece_buf[recvlen] = 0;
					//printf("TCP socked:%d recvdata:%d bytes\n", msg.socket, recvlen);
					ota_newdata(ota_conf.rece_buf, recvlen);	

				}
				else
				{
					printf("Illegal socket:%d\n", msg.socket);
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
#endif
/*---------------------------------------------------------------------------*/
//process with download fw file form smart phone
PROCESS_THREAD(tcp_client_otaProcess, ev, data)
{

    static struct etimer periodic_timer;
    SOCKETMSG msg;
    int recvlen;
    
    PROCESS_BEGIN();
    
    printf("tcp_client_otaProcess begin\n");
    while(1)  {
        PROCESS_WAIT_EVENT();
        if(ev == PROCESS_EVENT_EXIT) {
            break;
        } else if(ev == PROCESS_EVENT_MSG) {
            msg = *(SOCKETMSG *)data;
            //The TCP socket is connected, This socket can send data after now.
            if(msg.status == SOCKET_CONNECTED) {
                ota_conf.exit_cnt = 0;
                ota_conf.retry_cnt = 0;
                printf("socked:%d connected\n", (int)msg.socket);
                ota_conf.ota_status = SOCKET_CONNECTED;
                if(ota_conf.cmd_num == DOWNLOAD_START) {
                    strcpy(ota_conf.comb_buf,"download start");
                } else if(ota_conf.cmd_num == DOWNLOAD_END) {
                    if(ota_conf.burn_flag == BURN_PASS) {
                        strcpy(ota_conf.comb_buf,"download end");
                    } else {
                        strcpy(ota_conf.comb_buf,"download fail");
                    }
                }
                if(tcpsend(msg.socket, ota_conf.comb_buf, strlen(ota_conf.comb_buf)) == -1) {
                    printf(" send data fail\n");                    
                } else {
                    ota_conf.wait_ack = 1;
                    printf(" send text (%s) to smart phone\n", ota_conf.comb_buf);
                }
            } else if(msg.status == SOCKET_CLOSED) {
                if(ota_conf.tcp_client == msg.socket)
                    ota_conf.tcp_client = -1;
                 if(ota_conf.ota_status== SOCKET_CREATE) {
                    printf("ota socked:%d create fail and retry\n", (int)msg.socket);
                    ota_conf.retry_cnt++;
                    etimer_set(&periodic_timer, 1 * CLOCK_SECOND);
                    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
                    OtaTcpConnect();
                } else {
                    printf("ota socked:%d closed\n", (int)msg.socket);
                    ota_conf.ota_status = SOCKET_CLOSED;
                    if(ota_conf.cmd_num == DOWNLOAD_START)
                        filesave_from_phone(NULL, 0);
                    process_post_synch(&ota_updateProcess, PROCESS_EVENT_CONTINUE, NULL);
                }
            //Get ack, the data trasmission is done. We can send data after now.
            } else if(msg.status == SOCKET_SENDACK) {
                printf("socked:%d send data ack\n", (int)msg.socket);
                ota_conf.wait_ack = 0;
                ota_conf.ota_status = SOCKET_SENDACK;
            //There is new data coming. Receive data now.
            } else if(msg.status == SOCKET_NEWDATA) {
                if(0 <= msg.socket && msg.socket < UIP_CONNS) {
                    ota_conf.exit_cnt = 0;
                    recvlen = tcprecv(msg.socket, (char *)ota_conf.rece_buf, MAX_RECE_BUFFER);
                    if(ota_conf.cmd_num == DOWNLOAD_START)
                        filesave_from_phone(ota_conf.rece_buf, recvlen);
                } else {
                    printf("Illegal socket:%d\n", (int)msg.socket);
                }
            } else {
                printf("unknow message type: %d\n", msg.status);
            }
        }	
    }
    printf("tcp_client_otaProcess end\n");
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ota_updateProcess, ev, data)
{
    static struct etimer periodic_timer;
    
    PROCESS_BEGIN();
    
    printf("ota_updateProcess begin\n");

    ota_conf.cmd_num = DOWNLOAD_START;
    clearOtaParameters();
    process_start(&tcp_client_otaProcess, NULL);
    
    send_needip_arp_request(ota_conf.phone_ip_addr);
    
    while(1) {
        etimer_set(&periodic_timer, 1 * CLOCK_SECOND);        
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER ||ev == PROCESS_EVENT_CONTINUE);

        if(ev == PROCESS_EVENT_TIMER) {
            if((check_ip_in_arptable(ota_conf.phone_ip_addr) == 0) && ota_conf.tcp_client == -1) {
                printf("OtaTcpConnect to %d. %d.%d.%d \n", ota_conf.phone_ip_addr.u8[0],  
                              ota_conf.phone_ip_addr.u8[1],  ota_conf.phone_ip_addr.u8[2],  ota_conf.phone_ip_addr.u8[3]);
                OtaTcpConnect();
            }
            if(ota_conf.wait_ack == 1) {
                ota_conf.exit_cnt = 0;
            } else {
                ota_conf.exit_cnt++;
            }
            if(ota_conf.exit_cnt > MAX_CNT ||  ota_conf.retry_cnt > MAX_CNT) {
                printf("can not update fw with ota, timout and exit ota process\n");
                break;
            }
        } else if(ev == PROCESS_EVENT_CONTINUE) {
            if(ota_conf.ota_status == SOCKET_CLOSED) {
                if(ota_conf.cmd_num == DOWNLOAD_START) {
                    ota_conf.cmd_num = DOWNLOAD_END;
                    clearOtaParameters();
                } else if(ota_conf.cmd_num == DOWNLOAD_END) {
                    break;
                }
            }
        } else {
            printf("ota_updateProcess rece unkonwn event\n");
        }
    }
    
    process_post_synch(&tcp_client_otaProcess, PROCESS_EVENT_EXIT, NULL);
    otaProcessDisable();
    printf("ota_updateProcess end\n");
    process_start(&auto_reboot_process, NULL);
    PROCESS_END();
}

