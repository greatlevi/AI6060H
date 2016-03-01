#ifndef _HW_TBL_H_
#define _HW_TBL_H_

#define PHY_INFO_TBL_SIZE          39
#define PHY_INDEX_TBL_SIZE         39
#define PHY_LL_LENGTH_TBL_SIZE     8

#define SET_PHY_INFO( _ctsdur, _ba_rate_idx,  				\
						_ack_rate_idx, _llength_idx, 		\
						_llength_enable) 					\
															\
									(_ctsdur<<16|			\
									_ba_rate_idx <<10|		\
									_ack_rate_idx<<4|		\
									_llength_idx<<1|		\
									_llength_enable)

#define SET_PHY_L_LENGTH( _l_ba, _l_rts, _l_cts_ack ) 		 (_l_ba<<12|_l_rts<<6 |_l_cts_ack)

/**
* ssv6xxx hardware security engine data structure.
*/
#define SECURITY_KEY_LEN                32
#define ME_NONE                         0
#define ME_WEP40                        1
#define ME_WEP104                       2
#define ME_TKIP                         3
#define ME_CCMP                         4

struct ssv6xxx_hw_key {
    U8     key[SECURITY_KEY_LEN];
    U32    tx_pn_l;
    U32    tx_pn_h;
    U32    rx_pn_l;
    U32    rx_pn_h;
}__attribute__((packed));

struct ssv6xxx_hw_sta_key {
	U8     pair_key_idx:4;    /* 0: pairwise key, 1-3: group key */
	U8     group_key_idx:4;   /* 0: pairwise key, 1-3: group key */
    U8     valid;             /* 0: invalid entry, 1: valid entry asic hw don't check this field*/
    U8     reserve[2]; 
    struct ssv6xxx_hw_key    pair;
}__attribute__((packed));

typedef struct ssv6xxx_hw_sec {
    struct ssv6xxx_hw_key group_key[3];    //mapping to protocol group key 1-3
    struct ssv6xxx_hw_sta_key sta_key[8];
//}ssv6xxx_hw_sec_t __attribute__((packed));
}ssv6xxx_hw_sec_t;

/**
* SSV6200 PHY Info Table for both AP & STA mode.
*/
U32 phy_info_tbl[] = {
	/* PHY Infor Table: */
	0x00000000, 0x00000100, 0x00000200, 0x00000300, 0x00000140, 
	0x00000240, 0x00000340, 0x00000001, 0x00000101, 0x00000201, 
	0x00000301, 0x00000401, 0x00000501, 0x00000601, 0x00000701, 
	0x00030002, 0x00030102, 0x00030202, 0x00030302, 0x00030402, 
	0x00030502, 0x00030602, 0x00030702, 0x00030082, 0x00030182, 
	0x00030282, 0x00030382, 0x00030482, 0x00030582, 0x00030682, 
	0x00030782, 0x00030042, 0x00030142, 0x00030242, 0x00030342, 
	0x00030442, 0x00030542, 0x00030642, 0x00030742,
	};
		
U32 phy_index_tbl[] = {
    /* PHY Index Table: */
	//B	
	SET_PHY_INFO(314, 0, 0, 0, 0),	//0x013A0000,
	SET_PHY_INFO(258, 0, 1, 0, 0),	//0x01020010,
	SET_PHY_INFO(223, 0, 1, 0, 0),  //0x00df0010, 
	SET_PHY_INFO(213, 0, 1, 0, 0),  //0x00d50010, 

	SET_PHY_INFO(162, 0, 4, 0, 0),	//0x00a20040,
	SET_PHY_INFO(127, 0, 4, 0, 0),	//0x007f0040,
	SET_PHY_INFO(117, 0, 4, 0, 0),  //0x00750040, 

	//G
	SET_PHY_INFO(60, 7,  7, 0, 0),  //0x003c0070, 
	SET_PHY_INFO(52, 7,  7, 0, 0),  //0x00340070, 
	SET_PHY_INFO(48, 9,  9, 0, 0),  //0x00300090, 
	SET_PHY_INFO(44, 9,  9, 0, 0),  //0x002c0090, 
 
	SET_PHY_INFO(44, 11, 11, 0, 0),  //0x002c00b0, 
	SET_PHY_INFO(40, 11, 11, 0, 0),  //0x002800b0, 
	SET_PHY_INFO(40, 11, 11, 0, 0),  //0x002800b0, 
	SET_PHY_INFO(40, 11, 11, 0, 0),  //0x002800b0, 

	//N
	SET_PHY_INFO(76,  7,  7, 0, 1),  //0x004c1c71, 
	SET_PHY_INFO(64,  9,  9, 1, 1),  //0x00402493, 
	SET_PHY_INFO(60,  9,  9, 2, 1),  //0x003c2495, 
	SET_PHY_INFO(60, 11, 11, 3, 1),  //0x003c2cb7, 
	SET_PHY_INFO(56, 11, 11, 4, 1),  //0x00382cb9, 
	SET_PHY_INFO(56, 11, 11, 5, 1),  //0x00382cbb, 
	SET_PHY_INFO(56, 11, 11, 5, 1),  //0x00382cbb, 
	SET_PHY_INFO(56, 11, 11, 5, 1),  //0x00382cbb, 


	SET_PHY_INFO(76,  7,  7, 6, 1),  //0x004c1c7d, 
	SET_PHY_INFO(64,  9,  9, 1, 1),  //0x00402493, 
	SET_PHY_INFO(60,  9,  9, 2, 1),  //0x003c2495, 
	SET_PHY_INFO(60, 11, 11, 3, 1),  //0x003c2cb7, 
	SET_PHY_INFO(56, 11, 11, 4, 1),  //0x00382cb9, 
	SET_PHY_INFO(56, 11, 11, 5, 1),  //0x00382cbb, 
	SET_PHY_INFO(56, 11, 11, 5, 1),  //0x00382cbb, 
	SET_PHY_INFO(56, 11, 11, 5, 1),  //0x00382cbb, 


	SET_PHY_INFO(64,  7,  7, 0, 0),  //0x00401c70, 
	SET_PHY_INFO(52,  9,  9, 0, 0),  //0x00342490, 
	SET_PHY_INFO(48,  9,  9, 0, 0),  //0x00302490, 
	SET_PHY_INFO(48, 11, 11, 0, 0),  //0x00302cb0, 
	SET_PHY_INFO(44, 11, 11, 0, 0),  //0x002c2cb0, 
	SET_PHY_INFO(44, 11, 11, 0, 0),  //0x002c2cb0, 
	SET_PHY_INFO(44, 11, 11, 0, 0),  //0x002c2cb0, 
	SET_PHY_INFO(44, 11, 11, 0, 0),  //0x002c2cb0, 
};

U32 phy_ll_length_tbl[] = {
	/* PHY LL-Length Table: */
	SET_PHY_L_LENGTH(50, 38, 35),//0x000329a3
	SET_PHY_L_LENGTH(35, 29, 26),//0x0002375a
	SET_PHY_L_LENGTH(29, 26, 23),//0x0001d697
	SET_PHY_L_LENGTH(26, 23, 23),//0x0001a5d7
	SET_PHY_L_LENGTH(23, 23, 20),//0x000175d4

	SET_PHY_L_LENGTH(23, 20, 20),//0x00017514
	SET_PHY_L_LENGTH(47, 38, 35),//0x0002f9a3
	SET_PHY_L_LENGTH( 0,  0,  0),//0x00000000
};

#endif /* _HW_TBL_H_ */
