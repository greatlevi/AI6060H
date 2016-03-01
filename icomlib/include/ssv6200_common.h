#ifndef _SSV6200_COMMON_H_
#define _SSV6200_COMMON_H_

#include <ssv_types.h>

typedef struct ssv6200_tx_desc ssv6200_tx_desc_t;
typedef struct ssv6200_rx_desc ssv6200_rx_desc_t;

typedef struct ssv6200_rxphy_info ssv6200_rxphy_info_t;


/**
 *  The flag definition for c_type (command type) field of PKTInfo:
 *
 *      @ M0_TXREQ:         
 *      @ M1_TXREQ
 *      @ M2_TXREQ
 *      @ M0_RXEVENT
 *      @ M1_RXEVENT
 *      @ HOST_CMD
 *      @ HOST_EVENT
 *
 */
#define M0_TXREQ                            0
#define M1_TXREQ                            1
#define M2_TXREQ                            2
#define M0_RXEVENT                          3
#define M2_RXEVENT                          4
#define HOST_CMD                            5
#define HOST_EVENT                          6
#define TEST_CMD                            7

#define SSV6XXX_RX_DESC_LEN                     \
        (sizeof(struct ssv6200_rx_desc) +       \
         sizeof(struct ssv6200_rxphy_info))
#define SSV6XXX_TX_DESC_LEN                     \
        (sizeof(struct ssv6200_tx_desc) + 0)    \


#define SSV62XX_TX_MAX_RATES    3
struct fw_rc_retry_params {
    U32 count:4;
    U32 drate:6;
    U32 crate:6;
    U32 rts_cts_nav:16;
    U32 frame_consume_time:10;
    U32 dl_length:12;
    U32 RSVD:10;
} STRUCT_PACKED;

/**
* struct ssv6200_tx_desc - ssv6200 tx frame descriptor.
* This descriptor is shared with ssv6200 hardware and driver.
*/
struct ssv6200_tx_desc
{
    /* The definition of WORD_1: */
    U32             len:16;
    U32             c_type:3;
    U32             f80211:1;
    U32             qos:1;          /* 0: without qos control field, 1: with qos control field */
    U32             ht:1;           /* 0: without ht control field, 1: with ht control field */
    U32             use_4addr:1;
    U32             RSVD_0:3;//used for rate control report event.
    U32             bc_que:1;
    U32             security:1;
    U32             more_data:1;
    U32             stype_b5b4:2;
    U32             extra_info:1;   /* 0: don't trap to cpu after parsing, 1: trap to cpu after parsing */

    /* The definition of WORD_2: */
    U32             fCmd;

    /* The definition of WORD_3: */
    U32             hdr_offset:8;
    U32             frag:1;
    U32             unicast:1;
    U32             hdr_len:6;
    U32             tx_report:1;
    U32             tx_burst:1;     /* 0: normal, 1: burst tx */
    U32             ack_policy:2;   /* See Table 8-6, IEEE 802.11 Spec. 2012 */
    U32             aggregation:1;
    U32             RSVD_1:3;//Used for AMPDU retry counter
    U32             do_rts_cts:2;   /* 0: no RTS/CTS, 1: need RTS/CTS */
                                    /* 2: CTS protection, 3: RSVD */
    U32             reason:6;

    /* The definition of WORD_4: */
    U32             payload_offset:8;
    U32             next_frag_pid:7;
    U32             RSVD_2:1;
    U32             fCmdIdx:3;
    U32             wsid:4;
    U32             txq_idx:3;
    U32             TxF_ID:6;

    /* The definition of WORD_5: */
    U32             rts_cts_nav:16;
    U32             frame_consume_time:10;  //32 units
    U32             crate_idx:6;

    /* The definition of WORD_6: */
    U32             drate_idx:6;
    U32             dl_length:12;
    U32             RSVD_3:14;
    /* The definition of WORD_7~15: */
    U32             RESERVED[8];
    /* The definition of WORD_16~20: */
    struct fw_rc_retry_params rc_params[SSV62XX_TX_MAX_RATES];
} STRUCT_PACKED;

/**
* struct ssv6200_rx_desc - ssv6200 rx frame descriptor.
* This descriptor is shared with ssv6200 hardware and driver.
*/
struct ssv6200_rx_desc
{
    /* The definition of WORD_1: */
    U32             len:16;
    U32             c_type:3;
    U32             f80211:1;
    U32             qos:1;          /* 0: without qos control field, 1: with qos control field */
    U32             ht:1;           /* 0: without ht control field, 1: with ht control field */
    U32             use_4addr:1;
    U32             l3cs_err:1;
    U32             l4cs_err:1;
    U32             align2:1;
    U32             RSVD_0:2;
    U32             psm:1;
    U32             stype_b5b4:2;
    U32             extra_info:1;  

    /* The definition of WORD_2: */
    U32             edca0_used:4;
    U32             edca1_used:5;
    U32             edca2_used:5;
    U32             edca3_used:5;
    U32             mng_used:4;
    U32             tx_page_used:9;

    /* The definition of WORD_3: */
    U32             hdr_offset:8;
    U32             frag:1;
    U32             unicast:1;
    U32             hdr_len:6;
    U32             RxResult:8;
    U32             wildcard_bssid:1;
    U32             RSVD_1:1;
    U32             reason:6;

    /* The definition of WORD_4: */
    U32             payload_offset:8;
    U32             tx_id_used:8;
    U32             fCmdIdx:3;
    U32             wsid:4;
    U32             RSVD_3:3;
    U32             rate_idx:6;

} STRUCT_PACKED;



struct ssv6200_rxphy_info {
    /* WORD 1: */
    U32             len:16;
    U32             rsvd0:16;

    /* WORD 2: */
    U32             mode:3;
    U32             ch_bw:3;
    U32             preamble:1;
    U32             ht_short_gi:1;
    U32             rate:7;
    U32             rsvd1:1;
    U32             smoothing:1;
    U32             no_sounding:1;
    U32             aggregate:1;
    U32             stbc:2;
    U32             fec:1;
    U32             n_ess:2;
    U32             rsvd2:8;

    /* WORD 3: */
    U32             l_length:12;
    U32             l_rate:3;
    U32             rsvd3:17;

    /* WORD 4: */
    U32             rsvd4;

    /* WORD 5: G, N mode only */
    U32             rpci:8;     /* RSSI */
    U32             snr:8;
    U32             service:16;

} STRUCT_PACKED;




struct ssv6200_rxphy_info_padding {

/* WORD 1: for B, G, N mode */
U32             rpci:8;     /* RSSI */
U32             snr:8;
U32             RSVD:16;
} STRUCT_PACKED;



struct ssv6200_txphy_info {
    U32             rsvd[7];

} STRUCT_PACKED;


#define MAX_AGGR_NUM   (24)

#if 0

struct ssv62xx_tx_rate {
    s8 data_rate;
    u8 count;
} __attribute__((packed));

struct ampdu_ba_notify_data {
    //u16 retry_count;
    u8  wsid;
    struct ssv62xx_tx_rate tried_rates[SSV62XX_TX_MAX_RATES];
    u16 seq_no[MAX_AGGR_NUM];    
} __attribute__((packed));

struct firmware_rate_control_report_data{
    u8 wsid;
    struct ssv62xx_tx_rate rates[SSV62XX_TX_MAX_RATES];
    u16 ampdu_len;
    u16 ampdu_ack_len;
    int ack_signal;
    /* 15 bytes free */
} __attribute__((packed));

#define RC_RETRY_PARAM_OFFSET  ((sizeof(struct fw_rc_retry_params))*SSV62XX_TX_MAX_RATES)
#define SSV_RC_RATE_MAX                     39

#ifdef FW_WSID_WATCH_LIST
enum SSV6XXXX_WSID_OPS
{
    SSV6XXXX_WSID_OPS_ADD,
    SSV6XXXX_WSID_OPS_DEL,
    SSV6XXXX_WSID_OPS_RESETALL,
    SSV6XXXX_WSID_OPS_MAX
};

struct ssv6xxx_wsid_params
{
    u8 cmd;
    u8 wsid_idx;
    u8 target_wsid[6];
}; 
#endif //#ifdef FW_WSID_WATCH_LIST

struct ssv6xxx_iqk_cfg {
    u32 cfg_xtal:8;
    u32 cfg_pa:8;
    u32 cfg_tssi_trgt:8;
    u32 cfg_tssi_div:8;
    u32 cfg_def_tx_scale_11b:8;
    u32 cfg_def_tx_scale_11b_p0d5:8;
    u32 cfg_def_tx_scale_11g:8;
    u32 cfg_def_tx_scale_11g_p0d5:8;
    u32 cfg_papd_tx_scale_11b:8;
    u32 cfg_papd_tx_scale_11b_p0d5:8;
    u32 cfg_papd_tx_scale_11g:8;
    u32 cfg_papd_tx_scale_11g_p0d5:8;
    u32 cmd_sel;
    union {
        u32 fx_sel;
        u32 argv;
    };
    u32 phy_tbl_size;
    u32 rf_tbl_size;
};
#define PHY_SETTING_SIZE sizeof(phy_setting)

#ifdef CONFIG_SSV_CABRIO_E
#define IQK_CFG_LEN         (sizeof(struct ssv6xxx_iqk_cfg)+sizeof(phy_setting)+sizeof(asic_rf_setting))
#define RF_SETTING_SIZE     (sizeof(asic_rf_setting))
#endif

/*
    If change defallt value .please recompiler firmware image.
*/
#define MAX_PHY_SETTING_TABLE_SIZE    1920
#define MAX_RF_SETTING_TABLE_SIZE    512

#endif

#endif /* _SSV6200_H_ */
