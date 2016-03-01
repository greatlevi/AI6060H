#ifndef _CRYPTHW_H_
#define _CRYPTHW_H_

#include "ssv_types.h"

#define TX_PKT_RES_BASE					16
#define HW_SECURITY_SUPPORT_WSID		8
#define SECURITY_KEY_LEN				32
#define GET_PKID_OFFSET 				16

#define		PHY_IDX_TBL_CNT		39		// phy info idx table count
#define		PHY_IDX_TBL_LEN		156		// phy info idx table length = 39 * sizeof(u32), in bytes
#define		PHY_LLEN_TBL_CNT	8		// phy l-length table count
#define		PHY_LLEN_TBL_LEN	32		// phy l-length table length = 8  * sizeof(u32), in bytes
#define		PHY_PBUF_LEN		(PHY_IDX_TBL_LEN + PHY_LLEN_TBL_LEN)

typedef struct PKT_Info_st
{
    /* The definition of WORD_1: */
    U32             len:16;
    U32             c_type:3;
    U32             f80211:1;
    U32             qos:1;          /* 0: without qos control field, 1: with qos control field */
    U32             ht:1;           /* 0: without ht control field, 1: with ht control field */
    U32             use_4addr:1;
    U32             RSVD_0:5;
    U32             more_data:1;
    U32             stype_b5b4:2;
    U32             extra_info:1;  

    /* The definition of WORD_2: */
    U32             fCmd;

    /* The definition of WORD_3: */
    U32             hdr_offset:8;
    U32             frag:1;
    U32             unicast:1;
    U32             hdr_len:6;
    U32             RSVD_1:10;
    U32             reason:6;

    /* The definition of WORD_4: */
    U32             payload_offset:8;
    U32             next_frag_pid:7;
    U32             RSVD_2:1;
    U32             fCmdIdx:3;
    U32             wsid:4;
    U32             RSVD_3:3;
	U32				drate_idx:6;		
    
} PKT_Info, *PPKT_Info;

typedef struct SECURITY_KEY_st
{
	//TK(0-15)MICRX(16-23)MICTX(24-31)
    U8          key[SECURITY_KEY_LEN];
	U64			tx_pn;
	U64        	rx_pn;
} SECURITY_KEY, *PSECURITY_KEY;

typedef struct WSID_SECURITY_st
{    
	U8         	pair_key_idx:4;		//0-3
	U8         	group_key_idx:4;	//0-3
	U8			reserve[3];
	//[0]	-pairwise key
	//[1-3]	-group key
	SECURITY_KEY	pair;
} __attribute__((packed))  WSID_SECURITY, *PWSID_SECURITY ;

typedef struct SRAM_KEY_st
{    
	SECURITY_KEY	group[3]; 	// 3*48
	WSID_SECURITY	wsid[HW_SECURITY_SUPPORT_WSID];	// (48+4)*HW_SECURITY_SUPPORT_WSID
} SRAM_KEY, *SRAM_PKEY;

struct soc_table_st {
    U8          sec_key_tbl[sizeof(struct SRAM_KEY_st)];
    U8          phy_idx_tbl[PHY_IDX_TBL_LEN];
    U8          phy_llen_tbl[PHY_LLEN_TBL_LEN];
	
};

typedef enum __PBuf_Type_E {
    NOTYPE_BUF  = 0,
    TX_BUF      = 1,
    RX_BUF      = 2    
} PBuf_Type_E;

typedef enum sec_type_en {
    SECURITY_NONE,
    SECURITY_WEP40,
    SECURITY_WEP104,
    SECURITY_TKIP,
    SECURITY_CCMP,
} sec_type;

enum wpa_alg {
	WPA_ALG_NONE,
	WPA_ALG_WEP,
	WPA_ALG_TKIP,
	WPA_ALG_CCMP,
	WPA_ALG_IGTK,
	WPA_ALG_PMK
};

enum key_type {
	KEY_NONE,
	KEY_PAIR,
	KEY_GROUP,
};

int wpa_drv_set_key(enum wpa_alg alg, enum key_type type, int key_idx, const U8 *key, size_t key_len, U8 wsid) ATTRIBUTE_SECTION_SRAM;
U8 sram_del_key(U8 wsid,U8 key_idx) ATTRIBUTE_SECTION_SRAM;
U8 sram_set_key(U8 wsid,int key_idx,const U8 *key,size_t key_len) ATTRIBUTE_SECTION_SRAM;
U8 sram_set_wep(U8 wsid,int key_idx,const U8 *key,size_t key_len) ATTRIBUTE_SECTION_SRAM;
S32 soft_mac_init(void);

#endif /* _CRYPTHW_H_ */
