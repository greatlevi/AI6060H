#ifndef _HMAC_MD5_H_
#define _HMAC_MD5_H_

#define HMAC_MD5_DIGEST_LENGTH	16
#define HMAC_MD5_BLOCK_LENGTH	64

/* The HMAC_SHA1 structure: */
typedef struct _HMAC_MD5_CTX {
	unsigned char	ipad[HMAC_MD5_BLOCK_LENGTH];
	unsigned char	opad[HMAC_MD5_BLOCK_LENGTH];
	unsigned char	key[HMAC_MD5_BLOCK_LENGTH];
	unsigned int	keylen;
} HMAC_MD5_CTX;

int hmac_md5_init(HMAC_MD5_CTX *hmac_ctx, unsigned char *key, int keylen) ATTRIBUTE_SECTION_SRAM;
int hmac_md5(HMAC_MD5_CTX *hmac_ctx, unsigned char *data, unsigned int datalen, unsigned char *digest, unsigned int digest_len) ATTRIBUTE_SECTION_SRAM;

#endif
