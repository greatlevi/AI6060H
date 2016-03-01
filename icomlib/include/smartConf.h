
#ifndef _SMARTCONF_H_
#define _SMARTCONF_H_

#include "ssv_types.h"
#include "smartComm.h"
#include "arc4.h"

#if defined(SMART_ICOMM)

#define SEQ_BASE_CMD 256 //0 ~ 255 SSID&PASS_DATA_CMD, 256 ~ 512 SEQ_CMD
#define SSID_DATA 1 // 1BYTE
#define PASS_DATA 1 // 1BYTE
#define CRC8_DATA 1 // 1BYTE
#define IP_DATA 4 // 4BYTE
#define MAC_DATA 6 // 6BYTE
#define CMD_DATA_MAX_LEN 256
#define MAX_BASE_LEN_BUF 5
#define SMARTLINK_SCAN_PERIOD 10 * CLOCK_MINI_SECOND

#define TIM_NOTE2_80211_ADDR {0x38, 0xAA, 0x3C, 0xE1, 0xDC, 0xC9}

typedef struct smart_control {
    U8 stopScan;
    U16 backData[2];//0xffff data is clear data
    U8 crcData;
    U8 decrypValue;
    //ssid & pass len
    U8 ssidLen[2];
    U8 passLen[2];
    //seq
    U8 seqData[2];
    U8 NumSeq[2];
    U8 backSeq[2];
    //cmd
    U8 cmdNum; // strat cmd number 1
    U8 CmdNumLen[2]; //every cmd have diff len
    //get current packet len
    U8 baseLenBufCount[2];
    U8 checkBaseLenBuf[2][5];
    U8 turnBaseLenBufCount[2];
    U8 turnCheckBaseLenBuf[2][5];
    //rc4 & cmd  buf
    U8 sonkey[CMD_DATA_MAX_LEN];//rc4 buf
    U8 packet[2][CMD_DATA_MAX_LEN];//cmd buf
    U8 phoneIP[2][IP_DATA];//
    U8 phoneMAC[2][MAC_DATA];//
    U8 buf[SEND_MAX_BUF];
    U8 stage1Timer;
    U8 stage2Timer;
} smart_control;

void getPhoneMac(U8 *src);
void initSmartLink();
void rx_process_smartConf() ATTRIBUTE_SECTION_SRAM;
void clearSmartLinkBuf();
void initSmartLink();
#endif

#endif
