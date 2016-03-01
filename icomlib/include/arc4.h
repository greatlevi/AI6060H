#ifndef _IPXE_ARC4_H
#define _IPXE_ARC4_H

struct arc4_ctx {
	int i, j;
	U8 state[256];
};

#define ARC4_CTX_SIZE sizeof ( struct arc4_ctx )

void arc4_skip ( const void *key, size_t keylen, size_t skip, const void *src, void *dst, size_t msglen ) ATTRIBUTE_SECTION_SRAM;
int arc4_setkey ( void *ctxv, const void *keyv, size_t keylen ) ATTRIBUTE_SECTION_SRAM;
void arc4_xor ( void *ctxv, const void *srcv, void *dstv, size_t len ) ATTRIBUTE_SECTION_SRAM;

#endif /* _IPXE_ARC4_H */
