#include <stdio.h>
#include <string.h>

#include "systemconf.h"

#define CONFIG_BIN_SIZE (8192)


COMMON_CONFIG i_config;
unsigned char buf_config[CONFIG_BIN_SIZE];



void init_config(COMMON_CONFIG *ptr){

    memset(&ptr->softap_ssid[0],0x0,sizeof(&ptr->softap_ssid));
    memcpy(&ptr->softap_ssid[0],
    ptr->softap_channel = 1;
    


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



int main(int argc,char **argv){
    char fn_out[] = "ssv6060_config.hex";
    FILE *FOUT_BIN=NULL;
    U8 *p_des = &buf_config[0];
	
//    printf ("argc=%d,argv=%s,len=%d\n", argc, argv[1], strlen(argv[1]));

    memset(buf_config, 0x0, CONFIG_BIN_SIZE);

    FOUT_BIN = fopen(fn_out,"wb");

    //printf("init_config start\n");
    init_config(&i_config);
    //printf("init_config end\n");
    
    p_des = gen_header_to_mem(0x5e11aa11, 3, p_des);    // only 3 configs now!!
    p_des = gen_config_to_mem(CONFIG_REGION_CODE,sizeof(i_config.region_code),(U8*)(&i_config.region_code), p_des);
    p_des = gen_config_to_mem(CONFIG_DEBUG_LEVEL,sizeof(i_config.debug_level),(U8*)(&i_config.debug_level), p_des);
    p_des = gen_config_to_mem(CONFIG_DEBUG_GROUP,sizeof(i_config.debug_group),(U8*)(&i_config.debug_group), p_des);

#if 0
    p_des = gen_config_to_mem(CONFIG_WIFI_MODE,sizeof(i_config.wifi_mode),&i_config.wifi_mode, p_des);
    p_des = gen_config_to_mem(CONFIG_DHCP_ENABLE,sizeof(i_config.dhcp_enable),&i_config.dhcp_enable, p_des);
    p_des = gen_config_to_mem(CONFIG_INTO_SMART_LINK,sizeof(i_config.intoSmartLink),&i_config.intoSmartLink, p_des);
    p_des = gen_config_to_mem(CONFIG_LOCAL_IP_ADDR,sizeof(i_config.local_ip_addr),&i_config.local_ip_addr.u8[0], p_des);
    p_des = gen_config_to_mem(CONFIG_NET_MASK,sizeof(i_config.net_mask),&i_config.net_mask.u8[0], p_des);
    p_des = gen_config_to_mem(CONFIG_GATEWAY_IP_ADDR,sizeof(i_config.gateway_ip_addr),&i_config.gateway_ip_addr.u8[0], p_des);
    p_des = gen_config_to_mem(CONFIG_AP_COUNT,sizeof(i_config.APcount),(U8*)(&i_config.APcount), p_des);
    p_des = gen_config_to_mem(CONFIG_AP_RECORD,sizeof(i_config.APrecord) * MAX_APRECORD,(U8*)(&i_config.APrecord[0]), p_des);
#endif

    fwrite( buf_config, 1,CONFIG_BIN_SIZE,FOUT_BIN);


    if(FOUT_BIN != NULL){
        fclose(FOUT_BIN);
    }

    
#if 0
    if( (argc!=2) || (strlen(argv[1]) != MAC_STRING_LEN ) ){
        printf("\nexample: %s 11:22:33:44:55:66  (ie:11:22:33:44:55:66 is a stupid MAC suggestion,plz do not follow!!!)\n\n",argv[0]);
        return -1;
    }
#endif        
    
    return 0;
}
