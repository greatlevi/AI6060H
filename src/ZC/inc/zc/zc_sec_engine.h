/**
******************************************************************************
* @file     zc_sec_engine.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Security module
******************************************************************************
*/

#ifndef  __ZC_SEC_ENGINE_H__ 
#define  __ZC_SEC_ENGINE_H__
#include <zc_common.h>
#include <zc_protocol_controller.h>
#include <rsa.h>
#include <bignum.h>

#define  ZC_SEC_RSA_KEY_LEN        (256) /*bit len*/
#define  ZC_SEC_AES_BLOCK_SIZE     (16)


#ifdef __cplusplus
extern "C" {
#endif

u32 SEC_EncryptTextByRsa(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len, u16 *pu16CiperLen);
u32 SEC_DecryptTextByRsa(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len, u16 *pu16PlainLen);
u32 SEC_AesEncrypt(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len, u16 *pu16CiperLen);
u32 SEC_AesDecrypt(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len, u16 *pu16PlainLen);
u32 SEC_Encrypt(ZC_SecHead *pstruSecHead, u8 *pu8CiperBuf, u8 *pu8PlainBuf, u16 *pu16CiperLen);
u32 SEC_Decrypt(ZC_SecHead *pstruSecHead, u8 *pu8CiperBuf, u8 *pu8PlainBuf, u16 *pu16PlainLen);
void SEC_InitRsaContextWithPublicKey(rsa_context *pstruRsa, const u8 *pu8Pubkey);
void SEC_InitRsaContextWithPrivateKey(rsa_context *pstrRsa, const u8 *pu8PrivateKey);
u32 SEC_PaddingCheck(u8 u8SecType, u16 u16PlainLen, u16 *u16PaddingLen);
void AES_CBC_Decrypt(
    unsigned char *CipherText,
    unsigned int CipherTextLength,
    unsigned char *Key,
    unsigned int KeyLength,
    unsigned char *IV,
    unsigned int IVLength,
    unsigned char *PlainText,
    unsigned int *PlainTextLength);

void AES_CBC_Encrypt(
    unsigned char *PlainText,
    unsigned int PlainTextLength,
    unsigned char *Key,
    unsigned int KeyLength,
    unsigned char *IV,
    unsigned int IVLength,
    unsigned char *CipherText,
    unsigned int *CipherTextLength);
    
#ifdef __cplusplus
}
#endif

#endif
/******************************* FILE END ***********************************/

