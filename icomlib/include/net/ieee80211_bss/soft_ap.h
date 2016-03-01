#ifndef _SOFT_AP_H
#define _SOFT_AP_H

#include <ssv_types.h>
#include "soft_ap_def.h"

#include "ieee80211.h"


void softap_disable_autobeacon();

//S32 softap_init();  //moved to soft_ap_init.h
S32 softap_start();
S32 softap_exit();
//void softap_reset_all();
//void softap_reset_sta_all();
void softap_set_state_dhcp_rack();
void softap_set_ip_info(U8 *p_mac,U8 *p_ip);

S32 softap_rx_process(struct ieee80211_hdr_3addr * mgmt) ATTRIBUTE_SECTION_SRAM; 
S32 softap_rx_process_data(struct ieee80211_hdr_3addr * mgmt) ATTRIBUTE_SECTION_SRAM;
S32 softap_rx_probe_request(struct ieee80211_hdr_3addr * mgmt) ATTRIBUTE_SECTION_SRAM;
S32 softap_rx_authenticate(struct ieee80211_hdr_3addr * mgmt) ATTRIBUTE_SECTION_SRAM;
S32 softap_rx_assoc_req(struct ieee80211_hdr_3addr * mgmt) ATTRIBUTE_SECTION_SRAM;

void softap_set_peer_mac(U8 index_sta,U8 *p_mac) ATTRIBUTE_SECTION_SRAM;
S32 softap_tx_beacon(U8 channel,S32 seq_no,U64 timestamp) ATTRIBUTE_SECTION_SRAM;
S32 softap_tx_probe_response(char *softap_ssid,S16 seq_and_freg_no,U64 timestamp,S16 _beacon_interval,U8 channel) ATTRIBUTE_SECTION_SRAM;
S32 softap_tx_authenticate(U16 seq_and_freg_no) ATTRIBUTE_SECTION_SRAM;
S32 softap_tx_assoc_response(U16 seq_and_freg_no,U8 *p_mac) ATTRIBUTE_SECTION_SRAM;
S32 softap_tx_deassoc() ATTRIBUTE_SECTION_SRAM;
void dbg_dump_mac(S8 *msg,U8 *pmac) ATTRIBUTE_SECTION_SRAM;

//S32 softap_reset_sta_idle_timer(U8 *p_mac) ; //if null sta being ACKed,reset it
S32 softap_reset_sta_idle_timer(S8 index_sta);
S32 softap_reset_sta_idle_timer_by_mac(U8 *p_mac);

void softap_sta_assoc_resp_acked(U8 *p_mac_acked) ;

S32 softap_get_sta_connected();
S32 softap_tx_deauth_unknown_sta(U16 seq_and_freg_no,U8 *p_mac_unknown);

#endif // _SOFT_AP_H
