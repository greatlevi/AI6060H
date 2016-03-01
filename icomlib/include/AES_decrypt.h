#include "ssv_types.h"

#define rot1(x) (((x) << 24) | ((x) >> 8))
#define rot2(x) (((x) << 16) | ((x) >> 16))
#define rot3(x) (((x) <<  8) | ((x) >> 24))

#define AES_BLOCKSIZE 16
#define AES_MAXROUNDS			14
#define AES_BLOCKSIZE           16
#define AES_IV_SIZE             16

#define mt  0x80808080
#define ml  0x7f7f7f7f
#define mh  0xfefefefe
#define mm  0x1b1b1b1b
#define mul2(x,t)	((t)=((x)&mt), \
			((((x)+(x))&mh)^(((t)-((t)>>7))&mm)))
#define inv_mix_col(x,f2,f4,f8,f9) (\
			(f2)=mul2(x,f2), \
			(f4)=mul2(f2,f4), \
			(f8)=mul2(f4,f8), \
			(f9)=(x)^(f8), \
			(f8)=((f2)^(f4)^(f8)), \
			(f2)^=(f9), \
			(f4)^=(f9), \
			(f8)^=rot3(f2), \
			(f8)^=rot2(f4), \
			(f8)^rot1(f9))

typedef struct aes_key_st 
{
    U16 rounds;
    U16 key_size;
    U32 ks[(AES_MAXROUNDS+1)*8];
    U8 iv[AES_IV_SIZE];
} AES_CTX;

typedef enum
{
    AES_MODE_128,
    AES_MODE_256
} AES_MODE;

/** AES context size */
#define AES_CTX_SIZE sizeof(AES_CTX)

void AES_set_key(AES_CTX *ctx, const U8 *key, const U8 *iv, AES_MODE mode) ATTRIBUTE_SECTION_SRAM;
void AES_convert_key(AES_CTX *ctx) ATTRIBUTE_SECTION_SRAM;
void AES_decrypt(const AES_CTX *ctx, U32 *data) ATTRIBUTE_SECTION_SRAM;
int aes_setkey ( void *ctx, const void *key, size_t keylen ) ATTRIBUTE_SECTION_SRAM;
int aes_wrap ( const void *kek, const void *src, void *dest, int nblk )  ATTRIBUTE_SECTION_SRAM;
int aes_unwrap ( const void *kek, const void *src, void *dest, int nblk ) ATTRIBUTE_SECTION_SRAM;
void aes_decrypt ( void *ctx, const void *src, void *dst, size_t len ) ATTRIBUTE_SECTION_SRAM;
void aes_encrypt ( void *ctx, const void *src, void *dst, size_t len );

