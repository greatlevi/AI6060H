#ifndef _HMAC_SHA1_H_
#define _HMAC_SHA1_H_

#include "sha1.h"
#include "ssv_types.h"

#define HMAC_SHA1_DIGEST_LENGTH	20
#define HMAC_SHA1_BLOCK_LENGTH	64

/* The SHA1 structure: */
typedef struct _SHA_CTX {
	unsigned int	state[5];
	unsigned int	count[2];
	unsigned char	buffer[HMAC_SHA1_BLOCK_LENGTH];
} SHA_CTX;

/* The HMAC_SHA1 structure: */
typedef struct _HMAC_SHA1_CTX {
	unsigned char	ipad[HMAC_SHA1_BLOCK_LENGTH];
	unsigned char	opad[HMAC_SHA1_BLOCK_LENGTH];
	unsigned char	key[HMAC_SHA1_BLOCK_LENGTH];
	unsigned int	keylen;
} HMAC_SHA1_CTX;

int hmac_sha1_init(HMAC_SHA1_CTX *hmac_ctx, unsigned char *key, int keylen) ATTRIBUTE_SECTION_SRAM;
//int hmac_sha1(unsigned char *key, unsigned int keylen, unsigned char *data, unsigned int datalen, unsigned char *digest, unsigned int digest_len);
int hmac_sha1_end(HMAC_SHA1_CTX *hmac_ctx, unsigned char *data, int datalen, unsigned char *digest, int digest_len) ATTRIBUTE_SECTION_SRAM;

#endif
