#ifndef _DBG_ATCMD_API_H_
#define _DBG_ATCMD_API_H_

#include <ssvradio.h>
#include <ssv_types.h>
#include <console.h>
#include "clock.h"

extern global_radio_config_t g_tx_config;

#define ATCMD_RADIO_CMD	     		   	"AT+RADIO_"

//RADIO TEST
#define ATCMD_RADIO_CHANNEL	        	"AT+RADIO_CHANNEL="
#define ATCMD_RADIO_WIFI_OPMODE    	"AT+RADIO_WIFI_OPMODE="
#define ATCMD_RADIO_11NCONFIG	    		"AT+RADIO_11N_CONFIG="
#define ATCMD_RADIO_FIXDATARATE     	"AT+RADIO_FIX_DATA_RATE="
#define ATCMD_RADIO_AUTODATARATE    	"AT+RADIO_AUTO_DATA_RATE"
#define ATCMD_RADIO_SECURITY        		"AT+RADIO_SECURITY="
#define ATCMD_RADIO_TX_POWER    	    	"AT+RADIO_TX_POWER="
#define ATCMD_RADIO_AIFS            		"AT+RADIO_AIFS="
#define ATCMD_RADIO_APSTA           		"AT+RADIO_APSTA="
#define ATCMD_RADIO_SETMAC          		"AT+RADIO_SET_MAC="
#define ATCMD_RADIO_FRAMETYPE       	"AT+RADIO_FRAME_TYPE="
#define ATCMD_RADIO_FRAMESIZE       		"AT+RADIO_FRAME_SIZE="
#define ATCMD_RADIO_FRAMEDATA       	"AT+RADIO_FRAME_DATA="
#define ATCMD_RADIO_SEND_FRAME      	"AT+RADIO_SEND_FRAME="
#define ATCMD_RADIO_SET_BSSID       		"AT+RADIO_SET_BSSID="
#define ATCMD_RADIO_SET_PEERMAC     	"AT+RADIO_SET_PEERMAC="
#define ATCMD_RADIO_SET_SSID        		"AT+RADIO_SET_SSID="
#define ATCMD_RADIO_RX_CNT_CLR      	"AT+RADIO_RX_CNT_CLR="
#define ATCMD_RADIO_RX_DISABLE      	"AT+RADIO_RX_DISABLE="
#define ATCMD_RADIO_START_TX             	"AT+RADIO_START_TX="
#define ATCMD_RADIO_SET_COUNT         	"AT+RADIO_SET_COUNT="
#define ATCMD_RADIO_SET_DELAY        	"AT+RADIO_SET_DELAY="

//RF TEST
#define ATCMD_RADIO_RF_START                "AT+RADIO_RF_START="
#define ATCMD_RADIO_RF_RATE                  "AT+RADIO_RF_RATE="
#define ATCMD_RADIO_RF_BGAIN                 "AT+RADIO_RF_BGAIN="
#define ATCMD_RADIO_RF_GNGAIN               "AT+RADIO_RF_GNGAIN="
#define ATCMD_RADIO_RF_IQPHASE             "AT+RADIO_RF_IQPHASE="
#define ATCMD_RADIO_RF_IQAMP                 "AT+RADIO_RF_IQAMP="
#define ATCMD_RADIO_RF_STOP                   "AT+RADIO_RF_STOP"
#define ATCMD_RADIO_RF_RESET                   "AT+RADIO_RF_RESET"
#define ATCMD_RADIO_RF_COUNT                   "AT+RADIO_RF_COUNT="
#define ATCMD_RADIO_RF_FREQOFFSET                   "AT+RADIO_RF_FREQOFFSET="
#define ATCMD_RADIO_RF_TEMPCS_HT                   "AT+RADIO_RF_TEMPCS_HT="
#define ATCMD_RADIO_RF_TEMPCS_RT                   "AT+RADIO_RF_TEMPCS_RT="
#define ATCMD_RADIO_RF_TEMPCS_LT                   "AT+RADIO_RF_TEMPCS_LT="
#define ATCMD_RADIO_RF_READ_TEMPCS              "AT+RADIO_RF_READ_TEMPCS="
#define ATCMD_RADIO_RF_ENABLE_TCSR               "AT+RADIO_RF_ENABLE_TCSR="
#define ATCMD_RADIO_RF_DUMP                         "AT+RADIO_RF_DUMP"
#define ATCMD_RADIO_RF_SET_HT                       "AT+RADIO_RF_SET_HT="
#define ATCMD_RADIO_RF_SET_RT                        "AT+RADIO_RF_SET_RT="
#define ATCMD_RADIO_RF_SET_LT                          "AT+RADIO_RF_SET_LT="
#define ATCMD_RADIO_RF_LDO                          "AT+RADIO_RF_LDO="
#define ATCMD_RADIO_RF_PA1                          "AT+RADIO_RF_PA1="
#define ATCMD_RADIO_RF_PA2                          "AT+RADIO_RF_PA2="
#define ATCMD_RADIO_RF_PA3                          "AT+RADIO_RF_PA3="
#define ATCMD_RADIO_RF_XTAL                          "AT+RADIO_RF_XTAL="
#define ATCMD_RADIO_RF_FLASH_XTAL                          "AT+RADIO_RF_SET_XTAL="
#define ATCMD_RADIO_TEMP_BOUNDARY                          "AT+RADIO_TEMP_BOUNDARY="

#define ATCMD_RADIO_WRITE_MAC                          "AT+RADIO_WRITE_MAC="
#define ATCMD_RADIO_READ_MAC                          "AT+RADIO_READ_MAC="
#define ATCMD_RADIO_WRITE_SERIAL                          "AT+RADIO_WRITE_SERIAL="
#define ATCMD_RADIO_READ_SERIAL                          "AT+RADIO_READ_SERIAL="
typedef enum t_TAG_RADIOOP
{
	CHANNEL 		= 0,
	WIFI_OPMODE,
	CONFIG11N,
	FIXDATARATE,
	AUTODATARATE,
	SECURITY,
	TX_POWER,
	AIFS,
	APSTA,
	SETMAC,
	FRAMETYPE,
	FRAMESIZE,
	FRAMEDATA,
    SEND_FRAME,
    BSSID,
	PEERMAC,
    SSID,
    RX_CNT_CLR,
    RX_DISABLE,
	START_TX,
	SET_COUNT,
	SET_DELAY,
	RF_START,       //22
	RF_RATE,
	RF_BGAIN,
	RF_GNGAIN,
	RF_IQPHASE,
	RF_IQAMP,
	RF_STOP,
	RF_RESET,
	RF_COUNT,
	RF_FREQOFFSET,
	RF_TEMPCS_HT,
	RF_TEMPCS_RT,
	RF_TEMPCS_LT,
      RF_READ_TEMPCS,
      RF_ENABLE_TCSR,
      RF_DUMP,
      RF_SET_HT,
      RF_SET_RT,
      RF_SET_LT,
      RF_LDO,
      RF_PA1,
      RF_PA2,  
      RF_PA3,  
	RF_XTAL,
	RF_FLASH_XTAL,
      TEMP_BOUNDARY,
      WRITE_MAC,
      READ_MAC,
      WRITE_SERIAL,
      READ_SERIAL
} TAG_RADIOOP;

#define _DBG_AT_SET_DATA_RATE(_rate)        g_tx_config.rate_index = _rate
#define _DBG_AT_SET_AIFS(_aifs)             g_tx_config.aifs = _aifs
#define _DBG_AT_SET_FRAME_TYPE(_type)       g_tx_config.frame_type = _type
#define _DBG_AT_SET_FRAME_SIZE(_size)       g_tx_config.frame_size = _size
#define _DBG_AT_SET_FRAME_DATA(_data)       g_tx_config.frame_data = _data
void _DBG_AT_SET_PEERMAC(U8 *mac, U8 num);
void _DBG_AT_SET_PEERMAC_SOFTAP(U8 *mac, U8 num);
void _DBG_AT_RESET_PEERMAC_SOFTAP(U8 *mac, U8 num);
void _DBG_AT_SET_BSSID(U8 *mac);
int _DBG_AT_SET_CHANNEL(U16 channel);

int Radio_Command(TAG_RADIOOP op, char *buff);
int Parse_Radio_Command(char *buff);
void _DBG_AT_SET_MAC(U8 *mac);

void UpdateConf() ;

//RF test API
void RFStart();
void RFRate(U16 rate_index);
void RFBGain(S32 gainBMode, BOOL bWriteFlash);
void RFGNGain(S32 gainGNMode, BOOL bWriteFlash);
void RFIQPhase(S8 value, BOOL bWriteFlash);
void RFIQAmp(S8 value, BOOL bWriteFlash);
void RFStop();
void RFReset();
void RFCount(U8 mode, U32 *pCount, U32 *pCrc);
int RFFreqOffset(S8 valueP, S8 valueN);
void RFTempcsHT(S8 value1, S8 value2);
void RFTempcsRT(S8 value1, S8 value2);
void RFTempcsLT(S8 value1, S8 value2);
S8 ReadTemperature (int *pTemperature);
void EnableTemperatureCompensator(U32 nMsec);


#endif /* _DBG_ATCMD_API_H_ */
