#ifndef _SHA1_H_
#define _SHA1_H_

#include "ssv_types.h"

/* MD5 context. */
typedef struct MD5_CTX{
  U32 state[4];                                   /* state (ABCD) */
  U32 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  U8 buffer[64];                         /* input buffer */
} MD5_CTX;

void MD5Init (MD5_CTX *context) ATTRIBUTE_SECTION_SRAM;
void MD5Update (MD5_CTX *context, unsigned char *input, unsigned int inputLen) ATTRIBUTE_SECTION_SRAM;
void MD5Final (unsigned char *digest, MD5_CTX *context) ATTRIBUTE_SECTION_SRAM;

#endif
