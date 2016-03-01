#ifndef _HASJ_ALG_H_
#define _HASJ_ALG_H_

#include "ssv_types.h"

int hmac_md5_cal(unsigned char *key, int keylen, unsigned char *salt, int saltlen, unsigned char *digest) ATTRIBUTE_SECTION_SRAM;
int hmac_sha1_cal(unsigned char *key, int keylen, unsigned char *salt, int saltlen, unsigned char *digest) ATTRIBUTE_SECTION_SRAM;
int prf_sha1(unsigned char *key, int keylen, const char *prefix, unsigned char *salt, int saltlen, unsigned char *ptk, int ptklen) ATTRIBUTE_SECTION_SRAM;
int pbkdf2_sha1_f(unsigned char *key, int keylen, unsigned char *salt, int saltlen, unsigned int iterc, unsigned int blk, unsigned char *digest) ATTRIBUTE_SECTION_SRAM;
int pbkdf2_sha1(unsigned char *key, int keylen, unsigned char *salt, int saltlen, unsigned int iterc, unsigned char *pmk, int pmklen) ATTRIBUTE_SECTION_SRAM;

#endif /* _HASJ_ALG_H_ */

