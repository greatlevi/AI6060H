#include <stdio.h>
#include <string.h>
#include "ssv_types.h"
#include "ssv_config.h"
//#include "systemconf.h"

#define MP_BIN_SIZE (8192)
#define MAC_STRING_LEN (17)
#define ETH_ALEN  (6)

#define SOFTAP_SSID "icomm-softap"


unsigned char buf_mp_org[]={0x11,0xAA,0x11,0x5E,0x04,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0xab,0x89,0x67,0x45,  
							0x23,0x01,0x00,0x00,0x04,0x00,0x00,0x01,0x11,0x11,0x11,0x11,0x04,0x00,0x00,0x02,	
							0x22,0x22,0x22,0x22,0x04,0x00,0x00,0x03,0x33,0x33,0x33,0x33};	

unsigned char local_mac[6] ;
char str_softap[32];
unsigned char buf_mp[MP_BIN_SIZE];

COMMON_CONFIG i_config;


void combine_ssid_mac(char *pSsid)
{
    S16 loop_i = 0;
    S16 loop_j = 0;
    U8 mac_ch;
    U8 mac_hex;

    *(pSsid++) = '-';   //seperator character between "ssid" and "MAC address"

    for(loop_i=0;loop_i<ETH_ALEN;loop_i++) {
        mac_ch = local_mac[loop_i];
        
        for(loop_j=0;loop_j<2;loop_j++){
            if(loop_j==0){
                mac_hex  = (mac_ch>>4) & 0xf;
            } else {
                mac_hex  = (mac_ch>>0) & 0xf;
            }

            if( (mac_hex >=0) && (mac_hex <=9) ){   //character 0~0
                *(pSsid++) = '0' + mac_hex;
            } else {            //must be character 'a'~'f'
                *(pSsid++) = 'a' + mac_hex - 10;
            }

        }
    }

    //printf("pssid=%s\n",pSsid-13);

}


U8* gen_ssid_mac_to_mem(int id,int size,U8 *p_src,U8 *p_des)
{
    U32 id_and_size;
    char ssid[33];

    id_and_size = (id<<24) | size;

    memcpy( p_des, &id_and_size, sizeof(id_and_size)); 
    p_des += sizeof(id_and_size);

    memset( &ssid[0], 0x0, sizeof(ssid));   //clear buffer
    strncpy( &ssid[0], SOFTAP_SSID, strlen(SOFTAP_SSID));
    combine_ssid_mac(ssid+strlen(SOFTAP_SSID));

    memcpy(p_des,&ssid[0],strlen(&ssid[0]));

#if 0
printf("<%s>p_des = %s\n", __func__, p_des);
#endif
    
    p_des += size;

    return p_des;
}



void set_softap_ssid(COMMON_CONFIG *ptr)
{
    //ste1: copy SOFTAP_SSID "icomm-softap"
    memset(&ptr->softap_ssid[0],0x0,sizeof(&ptr->softap_ssid));
    memcpy(&ptr->softap_ssid[0],SOFTAP_SSID,strlen(SOFTAP_SSID));

    //step2: copy MAC address ex: "00:23:45:67:89:16"
    combine_ssid_mac( (&ptr->softap_ssid[0]) + strlen(SOFTAP_SSID));

//    printf("<%s>ssid = %s\n", __func__,(&ptr->softap_ssid[0]));
}



void init_config(COMMON_CONFIG *ptr)
{
//    memset(&ptr->softap_ssid[0],0x0,sizeof(&ptr->softap_ssid));
//    memcpy(&ptr->softap_ssid[0],SOFTAP_SSID,strlen(SOFTAP_SSID));
    set_softap_ssid(ptr);

    ptr->softap_channel = 1;
    ptr->softap_ack_timeout = 10000;
    ptr->softap_data_timeout = 20000;

    ptr->auto_connect = 1;

    ptr->region_code = 0x00009c;    //China
//    ptr->region_code = 0x00009e;    //Taiwan
       
    ptr->slink_mode = 0;

#if 1
    ptr->softap_ssid_length = strlen(&ptr->softap_ssid[0]);
#else
    ptr->softap_ssid_length = 0;    //hack to test sofap_init()
#endif
    
    ptr->watch_dog_count = 5000; //unit: (mini-seconds)

    ptr->softap_encryt_mode = 0;  //0: open, 1:WPA2-tkip 2:WPA2:CCMP
    memcpy( &ptr->softap_key[0], "1234567890",sizeof("1234567890") );
    ptr->softap_keylen = strlen("1234567890");

//    printf("ptr->softap_ssid_length = %d\n", ptr->softap_ssid_length);

#if 0    
    ptr->region_code = 1;
    ptr->debug_level = 2;
    ptr->debug_group = 0x0000ffff;

    U8 local_ip_addr[] = {0x11,0x22,0x33,0x44}; 
    U8 net_mask[] = {0x55,0x66,0x77,0x88}; 
    U8 gateway_ip_addr[] = {0x99,0xaa,0xbb,0xcc}; 

    ptr->wifi_mode = 0x9;
    ptr->dhcp_enable = 1;

    ptr->region_code = 1;

    ptr->intoSmartLink = 1;
    ptr->debugLevel = 2;
    ptr->debugGroup = 0x12345678;

    memcpy(&ptr->local_ip_addr,&local_ip_addr[0],sizeof(local_ip_addr));
    memcpy(&ptr->net_mask,&net_mask[0],sizeof(net_mask));
    memcpy(&ptr->gateway_ip_addr,&gateway_ip_addr[0],sizeof(gateway_ip_addr));

    ptr->APcount = 2;
    ptr->APrecord[0].channel = 1;
    ptr->APrecord[0].key_len= 16;
    strcpy(&ptr->APrecord[0].ssid[0],"jonathan");
    
    ptr->APrecord[1].channel = 2;
    ptr->APrecord[1].key_len= 32;
    strcpy(&ptr->APrecord[1].ssid[0],"jimyang");
#endif
}



U8* gen_header_to_mem(U32 magic_num,U32 entry_num,U8 *p_des){
    U32 tmp_val=0x0;

    memcpy( p_des, &magic_num, sizeof(magic_num)); 
    p_des += sizeof(magic_num);

    memcpy( p_des, &entry_num, sizeof(entry_num)); 
    p_des += sizeof(entry_num);

    return p_des;
}


U8* gen_config_to_mem(int id,int size,U8 *p_src,U8 *p_des){
    U32 id_and_size;
    //printf("<%s>id = %d\n",__func__,id);

    id_and_size = (id<<24) | size;

    memcpy( p_des, &id_and_size, sizeof(id_and_size)); 
    p_des += sizeof(id_and_size);

    memcpy(p_des,p_src,size);
    p_des += size;

    return p_des;
}




void gen_config_table(U8 *pConfig){
    U8 *p_des = NULL;
    
    init_config(&i_config);

    //initial pointer
    p_des = pConfig;
    
    p_des = gen_header_to_mem(0x5e11aa11, CONFIG_NUM_MAX, p_des);    // only 3 configs now!!

    p_des = gen_ssid_mac_to_mem(CONFIG_SOFTAP_SSID,sizeof(i_config.softap_ssid),(U8*)(&i_config.softap_ssid), p_des);
    
    p_des = gen_config_to_mem(CONFIG_SOFTAP_CHANNEL,sizeof(i_config.softap_channel),(U8*)(&i_config.softap_channel), p_des);
    p_des = gen_config_to_mem(CONFIG_SOFTAP_ACK_TIMEOUT,sizeof(i_config.softap_ack_timeout),(U8*)(&i_config.softap_ack_timeout), p_des);
    p_des = gen_config_to_mem(CONFIG_SOFTAP_DATA_TIMEOUT,sizeof(i_config.softap_data_timeout),(U8*)(&i_config.softap_data_timeout), p_des);
    p_des = gen_config_to_mem(CONFIG_AUTO_CONNECT, sizeof(i_config.auto_connect),(U8*)(&i_config.auto_connect), p_des);

    p_des = gen_config_to_mem(CONFIG_REGION_CODE, sizeof(i_config.region_code),(U8*)(&i_config.region_code), p_des);
    p_des = gen_config_to_mem(CONFIG_SLINK_MODE, sizeof(i_config.slink_mode),(U8*)(&i_config.slink_mode), p_des);
    p_des = gen_config_to_mem(CONFIG_SOFTAP_SSID_LENGTH, sizeof(i_config.softap_ssid_length),(U8*)(&i_config.softap_ssid_length), p_des);
    p_des = gen_config_to_mem(CONFIG_WATCH_DOG_COUNT, sizeof(i_config.watch_dog_count),(U8*)(&i_config.watch_dog_count), p_des);


    p_des = gen_config_to_mem(CONFIG_SOFTAP_ENCRYPT_MODE, sizeof(i_config.softap_encryt_mode),(U8*)(&i_config.softap_encryt_mode), p_des);
    p_des = gen_config_to_mem(CONFIG_SOFTAP_KEY, sizeof(i_config.softap_key),(U8*)(&i_config.softap_key), p_des);
    p_des = gen_config_to_mem(CONFIG_SOFTAP_KEYLEN, sizeof(i_config.softap_keylen),(U8*)(&i_config.softap_keylen), p_des);


#if 0
    p_des = gen_config_to_mem(CONFIG_REGION_CODE,sizeof(i_config.region_code),(U8*)(&i_config.region_code), p_des);
    p_des = gen_config_to_mem(CONFIG_DEBUG_LEVEL,sizeof(i_config.debug_level),(U8*)(&i_config.debug_level), p_des);
    p_des = gen_config_to_mem(CONFIG_DEBUG_GROUP,sizeof(i_config.debug_group),(U8*)(&i_config.debug_group), p_des);
#endif


#if 0
    //----------------1:channel-----------------
    element_id = 1;
    element_len = 1;
    channel_num = 1;

    memcpy( &pConfig[idx], &element_len, ELEMENT_LENGTH_SIZEOF);
    idx+= ELEMENT_LENGTH_SIZEOF;
    memcpy( &pConfig[idx], &element_id, ELEMENT_ID_SIZEOF);
    idx+= ELEMENT_ID_SIZEOF;
    
    memcpy( &pConfig[idx], &channel_num, element_len);
    idx+= element_len;

    //----------------2:ack timeout-----------------
    element_id = 2;
    element_len = 4;
    ack_timeout = 100;
    memcpy( &pConfig[idx], &element_len, ELEMENT_LENGTH_SIZEOF);
    idx+= ELEMENT_LENGTH_SIZEOF;
    memcpy( &pConfig[idx], &element_id, ELEMENT_ID_SIZEOF);
    idx+= ELEMENT_ID_SIZEOF;

    
    memcpy( &pConfig[idx], &ack_timeout, element_len);
    idx+= element_len;

    //----------------2:data timeout-----------------
    element_id = 3;
    element_len = 4;
    data_timeout = 10000;
    memcpy( &pConfig[idx], &element_len, ELEMENT_LENGTH_SIZEOF);
    idx+= ELEMENT_LENGTH_SIZEOF;
    memcpy( &pConfig[idx], &element_id, ELEMENT_ID_SIZEOF);
    idx+= ELEMENT_ID_SIZEOF;
    
    memcpy( &pConfig[idx], &data_timeout, element_len);
    idx+= element_len;

#endif

#if 0
    printf("ssid = %s\n",ssid);
    printf("channel_num =%d\n",channel_num);
    printf("ack_timeout = %d\n",ack_timeout);
    printf("data_timeout = %d\n",data_timeout);
#endif


}


int main(int argc,char **argv)
{
	char fn_out[] = "ssv6060_mac.hex";
	char *pch=NULL;
	FILE *FOUT_BIN=NULL;
	int mac_index = 17;	
	char buf_tmp[10]="";
	const char delimiters[] = ":-";
       S16 local_mac_idx=0;
    
	//printf ("argc=%d,argv=%s,len=%d\n", argc, argv[1], strlen(argv[1]));	
	if( (argc!=2) || (strlen(argv[1]) != MAC_STRING_LEN ) ){
		printf("\nexample: %s 11:22:33:44:55:66  (ie:11:22:33:44:55:66 is a stupid MAC suggestion,plz do not follow!!!)\n\n",argv[0]);
		return -1;		
	}
   
	memset( buf_mp, 0x0, MP_BIN_SIZE);

       memcpy(&str_softap[0],"icomm-softap",strlen("icomm-softap"));
    

	pch = strtok (argv[1],delimiters);
 	while (pch != NULL)
	{
		//printf ("%s\n",pch);
		memset(buf_tmp,0x0,sizeof(buf_tmp));
		memcpy(buf_tmp,pch,2);

             local_mac[local_mac_idx++] = strtoul(buf_tmp,NULL,16);
		buf_mp_org[mac_index--] = strtoul(buf_tmp,NULL,16);
		//sscanf(buf_tmp,"%2x",&buf_mp_org[mac_index--]);
		pch = strtok (NULL, delimiters);
	}

	printf("\nMAC addr:%2x-%2x-%2x-%2x-%2x-%2x\n",buf_mp_org[17],buf_mp_org[16],buf_mp_org[15],buf_mp_org[14],buf_mp_org[13],buf_mp_org[12]);

	memcpy( buf_mp,buf_mp_org,sizeof(buf_mp_org));

        //generate config tables  0x2000~0x2ffff
        gen_config_table(buf_mp+ 4*KB);



	FOUT_BIN = fopen(fn_out,"wb");

	fwrite(buf_mp,1,MP_BIN_SIZE,FOUT_BIN);

	if(FOUT_BIN != NULL){
		fclose(FOUT_BIN);
	}

	return 0;
}
