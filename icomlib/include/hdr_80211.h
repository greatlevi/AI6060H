#ifndef _HDR80211_H_
#define _HDR80211_H_

//#include "ssv_pktdef.h"
#include "ssv_types.h"

#define MAC_ADDR_LEN            6

struct ETHER_ADDR_st {
    U8      addr[MAC_ADDR_LEN];
} STRUCT_PACKED;

typedef struct ETHER_ADDR_st ETHER_ADDR;

#define MAX_PACKET_SIZE		4096

#define IEEE80211_FRAME_CTL_LEN	4
#define FCS_LEN                 4
#define HDR80211_MGMT_LEN       24
#define IEEE80211_QOS_CTL_LEN   2
#define IEEE80211_HT_CTL_LEN    4

/**
 * IEEE 802.11 Address offset position. This defines the offset of Address1,
 * Address2, Address3 and Address4 if present.
 */
#define OFFSET_ADDR1            4
#define OFFSET_ADDR2            10
#define OFFSET_ADDR3            16
#define OFFSET_ADDR4            24

/* IEEE 802.11 Frame Control: */
#define M_FC_VER		0x0003
#define M_FC_FTYPE		0x000c
#define M_FC_STYPE		0x00f0
#define M_FC_TODS		0x0100
#define M_FC_FROMDS		0x0200
#define M_FC_MOREFRAGS	0x0400
#define M_FC_RETRY		0x0800
#define M_FC_PWRMGMT	0x1000
#define M_FC_MOREDATA	0x2000
#define M_FC_PROTECTED	0x4000
#define M_FC_ORDER		0x8000

#define M_SC_FRAG		0x000F
#define M_SC_SEQ		0xFFF0

#define M_QOS_TID		0x000F
#define M_QOS_TYPE		0x0010
#define M_QOS_ACK_POLICY	0x0060
#define M_QOS_TXOP		0xFF00

/* Frame Type: */
#define FT_MGMT		        0x0000
#define FT_CTRL		        0x0004
#define FT_DATA		        0x0008

/* Frame SubType: Management Frames */
#define FST_ASSOC_REQ	        0x0000
#define FST_ASSOC_RESP	        0x0010
#define FST_REASSOC_REQ	        0x0020
#define FST_REASSOC_RESP	0x0030
#define FST_PROBE_REQ	        0x0040
#define FST_PROBE_RESP	        0x0050
#define FST_BEACON		0x0080
#define FST_ATIM		0x0090
#define FST_DISASSOC	        0x00A0
#define FST_AUTH		0x00B0
#define FST_DEAUTH		0x00C0
#define FST_ACTION		0x00D0

/* Frame SubType: Control Frames */
#define FST_CONTROL_WRAPPER     0x0070
#define FST_BA_REQ	        0x0080
#define FST_BA	         	0x0090
#define FST_PSPOLL		0x00A0
#define FST_RTS		        0x00B0
#define FST_CTS		        0x00C0
#define FST_ACK		        0x00D0
#define FST_CFEND		0x00E0
#define FST_CFENDACK	        0x00F0

/* Frame SubType: Data Frames */
#define FST_DATA		0x0000
#define FST_DATA_CFACK		0x0010
#define FST_DATA_CFPOLL		0x0020
#define FST_DATA_CFACKPOLL	0x0030
#define FST_NULLFUNC		0x0040
#define FST_CFACK		0x0050
#define FST_CFPOLL		0x0060
#define FST_CFACKPOLL		0x0070
#define FST_QOS_DATA		0x0080
#define FST_QOS_DATA_CFACK	0x0090
#define FST_QOS_DATA_CFPOLL	0x00A0
#define FST_QOS_DATA_CFACKPOLL	0x00B0
#define FST_QOS_NULLFUNC	0x00C0
#define FST_QOS_CFACK		0x00D0
#define FST_QOS_CFPOLL		0x00E0
#define FST_QOS_CFACKPOLL	0x00F0


#define IS_EQUAL(a, b)                  ( (a) == (b) )

#define FC_TYPE(fc)           ((fc) & (M_FC_FTYPE|M_FC_STYPE))
#define FC_FTYPE(fc)          ((fc) & M_FC_FTYPE)
#define FC_STYPE(fc)          ((fc) & M_FC_STYPE)

#define IS_TODS_SET(fc)       ((fc) & M_FC_TODS)
#define IS_FROMDS_SET(fc)     ((fc) & M_FC_FROMDS)
#define IS_MOREFLAG_SET(fc)   ((fc) & M_FC_MOREFRAGS)
#define IS_RETRY_SET(fc)      ((fc) & M_FC_RETRY)
#define IS_PM_SET(fc)         ((fc) & M_FC_PWRMGMT)
#define IS_MOREDATA_SET(fc)   ((fc) & M_FC_MOREDATA)
#define IS_PROTECT_SET(fc)    ((fc) & M_FC_PROTECTED)
#define IS_ORDER_SET(fc)      ((fc) & M_FC_ORDER)
#define IS_4ADDR_FORMAT(fc)   IS_EQUAL(((fc)&(M_FC_TODS|M_FC_FROMDS)), (M_FC_TODS|M_FC_FROMDS))
#define IS_MGMT_FRAME(fc)     IS_EQUAL(FC_FTYPE(fc), FT_MGMT)
#define IS_CTRL_FRAME(fc)     IS_EQUAL(FC_FTYPE(fc), FT_CTRL)
#define IS_DATA_FRAME(fc)     IS_EQUAL(FC_FTYPE(fc), FT_DATA)
#define IS_QOS_DATA(fc)       IS_EQUAL(((fc)&(M_FC_FTYPE|FST_QOS_DATA)), (FT_DATA|FST_QOS_DATA))
#define IS_NULL_DATA(fc)      IS_EQUAL((fc)&(M_FC_FTYPE|FST_NULLFUNC), (FT_DATA|FST_NULLFUNC))
#define IS_QOS_NULL_DATA(fc)  IS_EQUAL((fc)&(M_FC_FTYPE|FST_NULLFUNC|FST_QOS_DATA), (FT_DATA|FST_NULLFUNC|FST_QOS_DATA)) 

#define IS_ASSOC_REQ(fc)      IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_ASSOC_REQ))
#define IS_ASSOC_RESP(fc)     IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_ASSOC_RESP))
#define IS_REASSOC_REQ(fc)    IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_REASSOC_REQ))
#define IS_REASSOC_RESP(fc)   IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_REASSOC_RESP))
#define IS_PROBE_REQ(fc)      IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_PROBE_REQ))
#define IS_PROBE_RESP(fc)     IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_PROBE_RESP))
#define IS_BEACON(fc)         IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_BEACON))
#define IS_ATIM(fc)           IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_ATIM))
#define IS_DISASSOC(fc)       IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_DISASSOC))
#define IS_AUTH(fc)           IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_AUTH))
#define IS_DEAUTH(fc)         IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_DEAUTH))
#define IS_ACTION(fc)         IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_ACTION))
#define IS_PSPOLL(fc)         IS_EQUAL(FC_TYPE(fc), (FT_CTRL|FST_PSPOLL))
#define IS_RTS(fc)            IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_RTS))
#define IS_CTS(fc)            IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_CTS))
#define IS_ACK(fc)            IS_EQUAL(FC_TYPE(fc), (FT_CTRL|FST_ACK))
#define IS_BAR(fc)            IS_EQUAL(FC_TYPE(fc), (FT_CTRL|FST_BA_REQ))
#define IS_BA(fc)             IS_EQUAL(FC_TYPE(fc), (FT_CTRL|FST_BA))

#define GET_FC_MOREFRAG(fc)	  (((fc) & 0x0400) >> 10)

//Frame Control - type
#define FC_TYPE_MGMT		        0
#define FC_TYPE_CTRL		        1
#define FC_TYPE_DATA		        2

//Frame Control - sub type
//Management
#define FC_SUBTYPE_ASSOC_REQ		0	
#define FC_SUBTYPE_ASSOC_RESP		1	
#define FC_SUBTYPE_REASSOC_REQ		2	
#define FC_SUBTYPE_REASSOC_RESP		3	
#define FC_SUBTYPE_PROBE_REQ		4	
#define FC_SUBTYPE_PROBE_RESP		5	
#define FC_SUBTYPE_BEACON		8	
#define FC_SUBTYPE_ATIM			9	
#define FC_SUBTYPE_DISASSOC		10	
#define FC_SUBTYPE_AUTH			11	
#define FC_SUBTYPE_DEAUTH		12	
#define FC_SUBTYPE_ACTION		13	
#define FC_SUBTYPE_ACTION_NOACK		14	
//Control
#define FC_SUBTYPE_BLOCKACK_REQ		8	
#define FC_SUBTYPE_BLOCKACK		9	
#define FC_SUBTYPE_PS_POLL		10	
#define FC_SUBTYPE_RTS			11	
#define FC_SUBTYPE_CTS			12	
#define FC_SUBTYPE_ACK			13	
#define FC_SUBTYPE_CF_END		14	
#define FC_SUBTYPE_CF_END_ACK		15	
//Data
#define FC_SUBTYPE_DATA			0	
#define FC_SUBTYPE_DATA_CF_ACK		1	
#define FC_SUBTYPE_DATA_CF_POLL		2	
#define FC_SUBTYPE_DATA_CF_ACK_POLL	3	
#define FC_SUBTYPE_NULL			4	
#define FC_SUBTYPE_CF_ACK		5	
#define FC_SUBTYPE_CF_POLL		6	
#define FC_SUBTYPE_CF_ACK_POLL		7	
#define FC_SUBTYPE_QOS_DATA		8	
#define FC_SUBTYPE_QOS_DATA_CF_ACK	9	
#define FC_SUBTYPE_QOS_DATA_CF_POLL	10	
#define FC_SUBTYPE_QOS_DATA_CF_ACK_POLL	11	
#define FC_SUBTYPE_QOS_NULL		12	
#define FC_SUBTYPE_QOS_CF_POLL		14	
#define FC_SUBTYPE_QOS_CF_ACK_POLL	15
//To DS and From DS fields
#define FC_DS_IBSS			0
#define FC_DS_TOAP			1
#define FC_DS_FROMAP			2
#define FC_DS_WDS			3

/* modify 80211 frame macro */
#define SET_FRAMECTRL(hdr, _fctrl)		(hdr = (U16) _fctrl)

#define SET_FC_TYPE_SUBTYPE(hdr, _fc)	(hdr = ((_fc<<2)  | ((hdr->fc) & (~(M_FC_FTYPE|M_FC_STYPE)))))

#define SET_FC_TYPE(hdr, _fc)		(hdr = ((_fc<<2)  | ((hdr) & (~M_FC_FTYPE))))
#define SET_FC_SUBTYPE(hdr, _fc)	(hdr = ((_fc<<4)  | ((hdr) & (~M_FC_STYPE))))
#define SET_FC_TODS(hdr, _fc)		(hdr = ((_fc<<8)  | ((hdr->fc) & (~M_FC_TODS))))
#define SET_FC_FROMDS(hdr, _fc)		(hdr = ((_fc<<9)  | ((hdr->fc) & (~M_FC_FROMDS))))
#define SET_FC_MOREFLAG(hdr, _fc)	(hdr = ((_fc<<10) | ((hdr->fc) & (~M_FC_MOREFRAGS))))
#define SET_FC_RETRY(hdr, _fc)		(hdr = ((_fc<<11) | ((hdr->fc) & (~M_FC_RETRY))))
#define SET_FC_PM(hdr, _fc)		(hdr = ((_fc<<12) | ((hdr->fc) & (~M_FC_PWRMGMT))))
#define SET_FC_MOREDATA(hdr, _fc)	(hdr = ((_fc<<13) | ((hdr->fc) & (~M_FC_MOREDATA))))
#define SET_FC_PROTECT(hdr, _fc)	(hdr = ((_fc<<14) | ((hdr->fc) & (~M_FC_PROTECTED))))
#define SET_FC_ORDER(hdr, _fc)		(hdr = ((_fc<<15) | ((hdr->fc) & (~M_FC_ORDER))))

#define SET_DURID(hdr, _durid)		(hdr = (U16) _durid)

#define SET_SEQCTRL_FRAG(hdr, _seq)	(hdr = ((_seq<<0) | ((hdr) & (~M_SC_FRAG))))
#define SET_SEQCTRL_SEQ(hdr, _seq)	(hdr = ((_seq<<4) | ((hdr) & (~M_SC_SEQ))))

#define SET_QOS_TID(hdr, _qos)		(hdr = ((_qos<<0) | ((hdr) & (~M_QOS_TID))))
#define SET_QOS_TYPE(hdr, _qos)		(hdr = ((_qos<<4) | ((hdr) & (~M_QOS_TYOE))))
#define SET_QOS_ACK_POLICY(hdr, _qos)	(hdr = ((_qos<<5) | ((hdr) & (~M_QOS_ACK_POLICY))))
#define SET_QOS_TXOP(hdr, _qos)		(hdr = ((_qos<<8) | ((hdr) & (~M_QOS_TXOP))))

#define GET_FC_TYPE_SUBTYPE(hdr)    (((hdr) & (M_FC_FTYPE|M_FC_STYPE)) >>2)
#define GET_FC_TYPE(hdr)		    (((hdr) & (M_FC_FTYPE)) >>2)
#define GET_FC_SUBTYPE(hdr)		    (((hdr) & (M_FC_STYPE)) >>4)

#define GET_FC_RETRY(hdr)		    (((hdr) & (M_FC_RETRY)) >>11)

#define GET_SEQCTRL_FRAG(hdr)	    (((hdr) & (M_SC_FRAG)) >>0)
#define GET_SEQCTRL_SEQ(hdr)	    (((hdr) & (M_SC_SEQ)) >>4)

#define GET_QOS_TID(hdr)		    (((hdr) & (M_QOS_TID))>>0)
#define GET_QOS_TYPE(hdr)		    (((hdr) & (M_QOS_TYOE))>>4)
#define GET_QOS_ACK_POLICY(hdr)	    (((hdr) & (M_QOS_ACK_POLICY))>>5)
#define GET_QOS_TXOP(hdr)		    (((hdr) & (M_QOS_TXOP))>>8)

#define GET_QOS_BIT(hdr)		    (((hdr) & (FST_QOS_DATA)) >>7)

#define ST_SEQ_MASK	0xfff0          
#define GET_BAR_ST_SEQ(hdr)		    (((hdr) & (ST_SEQ_MASK)) >> 4)


typedef struct FRAME_CONTROL {
    U16 version		:2;
    U16 type		:2;
    U16 sub_type	:4;
    U16 ds		    :2;
    U16 more_frag 	:1;
    U16 retry		:1;
    U16 pwr_mgt		:1;
    U16 more_date 	:1;
    U16 protect		:1;
    U16 order		:1;
} FRAME_CONTROL;

//Sequence
typedef struct SEQUENCE_CONTROL {
    U16 fragment	:4;
    U16 sequence	:12;
} SEQUENCE_CONTROL;

//QOS
typedef struct QOS_HEADER {
    U16 tid		:4;
    U16 type		:1;
    U16 ack_policy	:2;
    U16 reserved	:1;
    U16 txop		:8;			
} QOS_HEADER;
#define QOS_HDR_LEN	2

//802.1h & RFC1042 logical link control
typedef struct DOT11_LLC_HEADER {
    U8 dsap;
    U8 ssap;
    U8 control;
    U8 encap[3];			
} DOT11_LLC_HEADER;
#define DOT11_LLC_HDR_LEN	6

//MAC header
typedef struct MAC_HEADER{
    U16	fc;		
    U16	durid;		
    U8	addr1[MAC_ADDR_LEN];
    U8	addr2[MAC_ADDR_LEN];
    U8	addr3[MAC_ADDR_LEN];
    U16	seq;	
    U8	addr4[MAC_ADDR_LEN];
} MAC_HEADER;

//Control frames
//RTS
typedef struct RTS_FRAME {
    U16	fc;
    U16	durid;
    U8	addr1[MAC_ADDR_LEN];
    U8	addr2[MAC_ADDR_LEN];
} RTS_FRAME;
#define	RTS_LEN		16

//CTS
typedef struct CTS_FRAME {
    U16	fc;		
    U16	durid;		
    U8	addr1[MAC_ADDR_LEN];		
} CTS_FRAME;
#define	CTS_LEN		10

//ACK
typedef struct ACK_FRAME {
    U16	fc;		
    U16	durid;		
    U8	addr1[MAC_ADDR_LEN];		
} ACK_FRAME;
#define	ACK_LEN		10	

//PS-Poll
typedef struct PS_POLL_FRAME {
    U16	fc;
    U16	durid;		
    U8	addr1[MAC_ADDR_LEN];
    U8	addr2[MAC_ADDR_LEN];
} PS_POLL_FRAME;
#define	PS_POLL_LEN	16

//CF-END
typedef struct CF_END_FRAME {
    U16	fc;
    U16	durid;
    U8	addr1[MAC_ADDR_LEN];		
    U8	addr2[MAC_ADDR_LEN];
} CF_END_FRAME;
#define	CF_END_LEN	16

//CF-End + CF-Ack
typedef struct CF_END_ACK_FRAME {
    U16	fc;
    U16	durid;		
    U8	addr1[MAC_ADDR_LEN];
    U8	addr2[MAC_ADDR_LEN];
} CF_END_ACK_FRAME;
#define	CF_END_ACK_LEN	16		

//Block Ack Request
typedef struct BLOCK_ACK_REQ {
    U16	fc;
    U16	durid;
    U8	addr1[MAC_ADDR_LEN];
    U8	addr2[MAC_ADDR_LEN];
    U16	bar_control;	
    U16	seqnum;		
} BLOCK_ACK_REQ;
#define BAR_LEN		20		

//Block Ack
#define BA_BITMAP_LEN	128	
typedef struct BLOCK_ACK {
    U16	fc;
    U16	durid;		
    U8	addr1[MAC_ADDR_LEN];		
    U8	addr2[MAC_ADDR_LEN];
    U16	ba_control;	
    U16	seqnum;		
    U8	bitmap[BA_BITMAP_LEN];	
} BLOCK_ACK;
#define BA_LEN	148
//Control header
typedef struct CONTROL_MAC_HEADER {
    U16	fc;
    U16	durid;		
    U8	addr1[MAC_ADDR_LEN];
    U8	addr2[MAC_ADDR_LEN];
} CONTROL_MAC_HEADER;
#define CTL_HDR_LEN	16

//Data frames
//Common data header
typedef struct COMMON_DATA_MAC_HEADER {
    U16    fc;
    U16    durid;
    U8     addr1[MAC_ADDR_LEN];
    U8     addr2[MAC_ADDR_LEN];
    U8     addr3[MAC_ADDR_LEN];
    U16    seqnum;
} COMMON_DATA_MAC_HEADER;
#define COMMON_DATA_HDR_LEN	24

//Qos data header
typedef struct QOS_DATA_MAC_HEADER {
    U16    fc;
    U16    durid;
    U8     addr1[MAC_ADDR_LEN];
    U8     addr2[MAC_ADDR_LEN];
    U8     addr3[MAC_ADDR_LEN];
    U16    seqnum;
    U16    qos;
} QOS_DATA_MAC_HEADER;
#define QOS_DATA_HDR_LEN	26

//Full data header
typedef struct FULL_DATA_MAC_HEADER {
    U16    fc;
    U16    durid;
    U8     addr1[MAC_ADDR_LEN];
    U8     addr2[MAC_ADDR_LEN];
    U8     addr3[MAC_ADDR_LEN];
    U16    seqnum;    
    U8     addr4[MAC_ADDR_LEN];
    U16    qos;
    //u8     llc[6]; /* DOT11_LLC_HEADER llc ??*/
} FULL_DATA_MAC_HEADER;
#define FULL_DATA_HDR_LEN	38

//Management frames
//Management header
typedef struct MANAGEMENT_MAC_HEADER {
	U16	fc;		
	U16	durid;		
	U8	addr1[MAC_ADDR_LEN];		
	U8	addr2[MAC_ADDR_LEN];
	U8	addr3[MAC_ADDR_LEN];
	U16	seqnum;		
} MANAGEMENT_MAC_HEADER;
#define	MGMT_HDR_LEN	24

#define AMPDU_SIGNATURE		0x4E
#define DELIMITER_LEN		0x4
#define	ALIGNMENT_OFFSET	0x4

typedef struct DELIMITER_st
{
    U16	 reserved:4; 	 //0-3
    U16	 length:12; 	 //4-15
    U8	 crc;
    U8	 signature;
} DELIMITER;//, *pDELIMITER;


#if 0

typedef struct FC_Field_st {
    u16         ver:2;
    u16         type:2;
    u16         subtype:4;
    u16         toDS:1;
    u16         fromDS:1;
    u16         MoreFlag:1;
    u16         Retry:1;
    u16         PwrMgmt:1;
    u16         MoreData:1;
    u16         Protected:1;
    u16         order:1;
} FC_Field, *PFC_Field;

typedef struct QOS_Ctrl_st {
    u16                 tid:4;
    u16                 bit4:1;
    u16                 ack_policy:2;
    u16                 rsvd:1;
    u16                 bit8_15:8;
} QOSCtrl_Field, *PQOSCtrl_Field;

typedef struct HDR80211_Data_st {
	FC_Field   fc;
	u16        dur;
	u8      addr1[MAC_ADDR_LEN];
	u8      addr2[MAC_ADDR_LEN];
	u8      addr3[MAC_ADDR_LEN];
	u16     seq_ctrl;    
} HDR80211_Data, *PHDR80211_Data;

typedef struct HDR80211_Data_4addr_st {
	FC_Field        fc;
	u16             dur;
	u8      addr1[MAC_ADDR_LEN];
	u8      addr2[MAC_ADDR_LEN]; 
	u8      addr3[MAC_ADDR_LEN];
	u16     seq_ctrl;        
	u8      addr4[MAC_ADDR_LEN];
} HDR80211_Data_4addr, *PHDR80211_Data_4addr;

#define BASE_LEN(PTR2variable,PTR)			(PTR2variable - (u8 *) PTR)


typedef struct HDR80211_Mgmt_st {
    FC_Field    fc;
    u16         dur;
    u8          da[MAC_ADDR_LEN];
    u8          sa[MAC_ADDR_LEN];
    u8          bssid[MAC_ADDR_LEN];
    u16         seq_ctrl;

    union {
        struct {
            u16         auth_algo;
            u16         trans_id;
            u16         status_code;
            /* possibly followed by Challenge text */
            u8          variable[0];    
        } auth;
        struct {
            u16         reason_code;
        } deauth;
        struct {
            u16         capab_info;
            u16         listen_interval;
            /* followed by SSID and Supported rates */
            u8          variable[0];
        } assoc_req;
        struct {
	    u16         capab_info;
	    u16         status_code;
	    u16         aid;
	    /* followed by Supported rates */
	    u8 variable[0];
	} assoc_resp, reassoc_resp;
        struct {
	    u16         capab_info;
	    u16         listen_interval;
	    u8          current_ap[6];
	    /* followed by SSID and Supported rates */
	    u8 variable[0];
	} reassoc_req;
        struct {
	    u16         reason_code;
	} disassoc;
        struct {
	    u64         timestamp;
	    u16         beacon_int;
	    u16         capab_info;
	    /* followed by some of SSID, Supported rates,
	     * FH Params, DS Params, CF Params, IBSS Params, TIM */
	    u8 variable[0];
	} beacon;
	struct { /*lint -save -e43 */
		 /* only variable items: SSID, Supported rates */
	    u8 variable[0];
	} probe_req; /*lint -restore */
	struct {
	    u64         timestamp;
	    u16         beacon_int;
	    u16         capab_info;
	    /* followed by some of SSID, Supported rates,
	     * FH Params, DS Params, CF Params, IBSS Params */
	    u8 variable[0];
	} probe_resp;
        struct {
            u8          category;
            union {
                struct {
			u8  action_code;
			u8  dialog_token;
			u8  status_code;
			u8  variable[0];
		} wme_action;
		struct {
			u8  action_code;
			u8  element_id;
			u8  length;
//			struct ieee80211_channel_sw_ie sw_elem;
		} chan_switch;
                struct {
			u8  action_code;
			u8  dialog_token;
			u8  element_id;
			u8  length;
//			struct ieee80211_msrment_ie msr_elem;
		} measurement;
		struct{
			u8  action_code;
			u8  dialog_token;
			u16 capab;
			u16 timeout;
			u16 start_seq_num;
		} STRUCT_PACKED addba_req;
		struct{
			u8  action_code;
			u8  dialog_token;
			u16 status;
			u16 capab;
			u16 timeout;
		} STRUCT_PACKED addba_resp;
                struct {
			u8  action_code;
			u16 params;
			u16 reason_code;
		} STRUCT_PACKED delba;
		struct {
			u8  action_code;
			/* capab_info for open and confirm,
			 * reason for close
			 */
			u16 aux;
			/* Followed in plink_confirm by status
			 * code, AID and supported rates,
			 * and directly by supported rates in
			 * plink_open and plink_close
			 */
			u8  variable[0];
		} plink_action;
                struct{
			u8  action_code;
			u8  variable[0];
		} mesh_action;
		struct {
			u8  action;
//			u8  trans_id[WLAN_SA_QUERY_TR_ID_LEN];
		} sa_query;
		struct {
			u8  action;
			u8  smps_control;
		} ht_smps;
            } u;           
        } STRUCT_PACKED action;
    } u;
    
} STRUCT_PACKED HDR80211_Mgmt, *PHDR80211_Mgmt;

typedef struct HDR80211_Ctrl_st 
{
    FC_Field    fc;
    u16         dur;
    u8          ra[MAC_ADDR_LEN];
    u8          ta[MAC_ADDR_LEN];
} HDR80211_Ctrl, *PHDR80211_Ctrl;


typedef struct HDR8023_Data_st {
	u8	dest[MAC_ADDR_LEN];
	u8	src[MAC_ADDR_LEN];
	u16     protocol;
} HDR8023_Data, *PHDR8023_Data;

#define HDR80211_MGMT(x)    /*lint -save -e740 */ (HDR80211_Mgmt *)((u8 *)(x)+((PKT_Info *)(x))->hdr_offset) /*lint -restore */
#define HDR80211_CTRL(x)
#define HDR80211_DATA(x)

#endif 

/**
 * WLAN Operation Mode Definition (for wlan_mode field):
 *
 * @ WLAN_STA: operate as STA (infrastructure) mode
 * @ WLAN_AP: operate as AP (infrastructure) mode
 * @ WLAN_IBSS: operate as IBSS (ad-hoc) mode
 * @ WLAN_WDS: Wireless Distribution System mode (Wireless Bridge)
 */
#define WLAN_STA                            0
#define WLAN_AP                             1
#define WLAN_IBSS                           2
#define WLAN_WDS                            3

#endif /* _HDR80211_H_ */
