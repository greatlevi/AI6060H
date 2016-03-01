/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
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
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A MAC protocol implementation that does not do anything.
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "process.h"
#include "ssv_types.h"
#include "ieee80211.h"

#ifndef __IEEE80211_MGMT_H__
#define __IEEE80211_MGMT_H__

#define MIN_CHANNEL 	1
#define MAX_CHANNEL	13
#define MAX_SSID_LEN	32
#define RSSI_LEVEL		5
#define MIN_RSSI		(-100)
#define MAX_RSSI		(-55)
#define RSSI_CHALEV_CNT 3

typedef enum RSSILEVEL_MIN
{
    RSSILEVEL_1 = -100,
    RSSILEVEL_2 = -85,
    RSSILEVEL_3 = -70,
    RSSILEVEL_4 = -55,
} RSSILEVEL_MIN;
//void rx_beacon () ATTRIBUTE_SECTION_SRAM;
/*---------------------------------------------------------------------------*/
void startrecordAP();
void stoprecordAP();
void startscanchannel(U32 timeperiod);
void stopscanchannel();
void addAvailableIndex();
u8_t getAvailableIndex() ATTRIBUTE_SECTION_SRAM;
#if 1 //obsolete moved to drv_wdog.c and wdog_api.c
void bss_mgmt_reboot ();
#endif
void bss_mgmt_init ();
void bss_mgmt_scan (struct process *p);
void bss_mgmt_scan_custom (struct process *p,U8 min_channel_num,U8 max_channel_num,U16 delay_in_ms);
void bss_mgmt_connect (struct process *p);
void bss_mgmt_disconnect ();
void bss_nslookup(char* dnsname);
void startrecordAP();
void stoprecordAP();
void startscanchannel(U32 timeperiod);
void stopscanchannel();
void pre_set () ATTRIBUTE_SECTION_SRAM;
void post_set () ATTRIBUTE_SECTION_SRAM;
int is_probe_req(le16_t fc);
int is_auth(le16_t fc);
int is_deauth(le16_t fc);
int is_disassoc(le16_t fc);
int is_assoc_req(le16_t fc);
int is_reassoc_req(le16_t fc);
int is_beacon(le16_t fc); //used in AP mode.
/*---------------------------------------------------------------------------*/
void rx_process_mgmt() ATTRIBUTE_SECTION_SRAM;
void rx_process_check_ap() ATTRIBUTE_SECTION_SRAM;
U8 find_eid_channel (enum ieee80211_eid eid, U8 *buf, int buf_len) ATTRIBUTE_SECTION_SRAM;
void updateRssiAndLevel(U8 litNum, U8 rssi) ATTRIBUTE_SECTION_SRAM;
void updateRssiAndLevel(U8 litNum, U8 rssi) ATTRIBUTE_SECTION_SRAM;
void removeAP_fromlist(U8 *apmac);
void reset_AvailableIndex();
/*---------------------------------------------------------------------------*/
//void onprobe_rsp ();
//void onauthentication_rsp ();
//void onassociation_rsp ();
/*---------------------------------------------------------------------------*/
//static enum mgmt_states gState;
/*---------------------------------------------------------------------------*/

#endif /* __IEEE80211_MGMT_H__ */
