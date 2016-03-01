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

#include "net/mac/mac.h"
#include "uip.h"
#include "uipopt.h"
#include "dev/radio.h"
#include "ssv_types.h"

#ifndef __IEEE80211_MAC_H__
#define __IEEE80211_MAC_H__

extern const struct mac_driver ieee80211_mac;

#define ETH_ALEN 6

void rx_process_data () ATTRIBUTE_SECTION_SRAM;
int is_protectframe(le16_t fc) ATTRIBUTE_SECTION_SRAM;
int is_data(le16_t fc) ATTRIBUTE_SECTION_SRAM;
int is_mgmt(le16_t fc) ATTRIBUTE_SECTION_SRAM;
S32 tx_nulldata_req (int pwmode) ATTRIBUTE_SECTION_SRAM;
void tx_pspoll () ATTRIBUTE_SECTION_SRAM;
static inline unsigned compare_ether_addr(const U8 *addr1, const U8 *addr2) ATTRIBUTE_SECTION_SRAM;
static inline BOOL ether_addr_equal(const U8 *addr1, const U8 *addr2) ATTRIBUTE_SECTION_SRAM;

static inline unsigned compare_ether_addr(const U8 *addr1, const U8*addr2)
{
         const U16 *a = (const U16 *) addr1;
         const U16 *b = (const U16 *) addr2;

         return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2])) != 0;
}

static inline BOOL ether_addr_equal(const U8 *addr1, const U8 *addr2)
{
         return !compare_ether_addr(addr1, addr2);
}

typedef struct ethernet_snap_llc {
	U8 dsap;
	U8 ssap;
	U8 cfld;
	U8 ocode[3];
	U16 etype;
} ethernet_snap_llc;

typedef struct eth_header {
  U8 dest_addr[6];
  U8 src_addr[6];
  U16 type;
} eth_header;

#define BROADCAST_80211_ADDR {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}

int is_data(le16_t fc);

#endif /* __IEEE80211_MAC_H__ */
