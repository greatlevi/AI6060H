/**
******************************************************************************
* @file     zc_sec_engine.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Security module
******************************************************************************
*/
#include <zc_sec_engine.h>
#include <zc_protocol_controller.h>

/*************************************************
* Function: SEC_EncryptTextByRsa
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
u32 SEC_EncryptTextByRsa(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len, u16 *pu16CiperLen)
{
    s32 s32Ret;
    u8 *pu8PublicKey;
    u16 u16ReadLen;
    rsa_context *rsa;


    rsa = (rsa_context *)ZC_malloc(sizeof(rsa_context));
    ZC_GetStoreInfor(ZC_GET_TYPE_CLOUDKEY, &pu8PublicKey);

    SEC_InitRsaContextWithPublicKey(rsa, pu8PublicKey);

    if (u16Len + 11 > rsa->len)
    {
        *pu16CiperLen = 0;
        u16ReadLen = 0;
        while (u16ReadLen + (rsa->len - 11) <= u16Len)
        {
            s32Ret = rsa_pkcs1_encrypt(rsa, RSA_PUBLIC, (rsa->len - 11), 
                pu8Plainbuf + u16ReadLen, pu8CiperBuf + *pu16CiperLen);
            if (s32Ret)
            {
                break;
            }
            *pu16CiperLen += rsa->len;
            u16ReadLen += rsa->len - 11;
        }

        if (u16ReadLen < u16Len)
        {
            s32Ret = rsa_pkcs1_encrypt(rsa, RSA_PUBLIC, (u16Len - u16ReadLen),
                pu8Plainbuf + u16ReadLen, pu8CiperBuf + *pu16CiperLen);
            *pu16CiperLen += rsa->len;
        }
    }
    else
    {
        s32Ret = rsa_pkcs1_encrypt(rsa, RSA_PUBLIC, u16Len, pu8Plainbuf, pu8CiperBuf);
        *pu16CiperLen = rsa->len;
    }
   
    rsa_free(rsa);
    ZC_free(rsa);

    if (s32Ret)
    {
        return ZC_RET_ERROR;
    }
    else
    {
        return ZC_RET_OK;
    }
    
}

/*************************************************
* Function: SEC_DecryptTextByRsa
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 SEC_DecryptTextByRsa(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len, u16 *pu16PlainLen)
{
    rsa_context *pstruRsa;
    s32 s32len;
    s32 s32Ret;
    u16 u16ReadLen;
    u8 *pu8PrivateKey;
    
    pstruRsa = (rsa_context *)ZC_malloc(sizeof(rsa_context));

    ZC_GetStoreInfor(ZC_GET_TYPE_PRIVATEKEY, &pu8PrivateKey);


    SEC_InitRsaContextWithPrivateKey(pstruRsa, pu8PrivateKey);

    if (u16Len > pstruRsa->len)
    {
        *pu16PlainLen = 0;
        u16ReadLen = 0;
        while (u16ReadLen + pstruRsa->len <= u16Len)
        {
            s32Ret = rsa_pkcs1_decrypt(pstruRsa, RSA_PRIVATE, &s32len, (pu8CiperBuf + u16ReadLen),
                pu8Plainbuf + *pu16PlainLen, u16Len);
            if (s32Ret)
            {
                break;
            }
            
            u16ReadLen += pstruRsa->len;
            *pu16PlainLen += s32len;
        }

        if (u16ReadLen < u16Len)
        {
            s32Ret = rsa_pkcs1_decrypt(pstruRsa, RSA_PRIVATE, &s32len, (pu8CiperBuf + u16ReadLen),
                pu8Plainbuf + *pu16PlainLen, u16Len);
            *pu16PlainLen += s32len;
        }
        
    }
    else
    {
        s32Ret = rsa_pkcs1_decrypt(pstruRsa, RSA_PRIVATE, &s32len, pu8CiperBuf,
            pu8Plainbuf, u16Len);
        *pu16PlainLen = (u16)s32len;
    }
    
    ZC_Printf("rsa_pkcs1_decrypt = %d, u32RetVal = %d\n", s32len, s32Ret);
    rsa_free(pstruRsa);
    
    ZC_free(pstruRsa);

    if (s32Ret)
    {
        return ZC_RET_ERROR;
    }
    else
    {
        return ZC_RET_OK;
    }
}
/*************************************************
* Function: SEC_InitRsaContextWithPublicKey
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void SEC_InitRsaContextWithPublicKey(rsa_context *pstruRsa, const u8 *pu8Pubkey)
{
    rsa_init(pstruRsa, RSA_PKCS_V15, RSA_RAW, NULL, NULL);

    pstruRsa->len = ZC_SEC_RSA_KEY_LEN >> 3;
    mpi_read_binary(&pstruRsa->N, pu8Pubkey, pstruRsa->len);
    mpi_read_binary(&pstruRsa->E, pu8Pubkey + pstruRsa->len, 3);
}

/*************************************************
* Function: SEC_InitRsaContextWithPrivateKey
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void SEC_InitRsaContextWithPrivateKey(rsa_context *pstrRsa, const u8 *pu8PrivateKey)
{
    u8 u8Index;
    u16 u16StartPos;
    u8 u8BufLen[6] = {ZC_SEC_RSA_KEY_LEN >> 3,ZC_SEC_RSA_KEY_LEN >> 4,ZC_SEC_RSA_KEY_LEN >> 4,ZC_SEC_RSA_KEY_LEN >> 4,ZC_SEC_RSA_KEY_LEN >> 4,ZC_SEC_RSA_KEY_LEN >> 4};
    mpi *pstruMpi[6];
    rsa_init(pstrRsa, RSA_PKCS_V15, RSA_RAW, NULL, NULL);

    pstrRsa->len = ZC_SEC_RSA_KEY_LEN >> 3;

    pstruMpi[0] = &pstrRsa->N;
    pstruMpi[1] = &pstrRsa->P;
    pstruMpi[2] = &pstrRsa->Q;
    pstruMpi[3] = &pstrRsa->DP;
    pstruMpi[4] = &pstrRsa->DQ;    
    pstruMpi[5] = &pstrRsa->QP;  

    u16StartPos = 0;
    for (u8Index = 0; u8Index < 6; u8Index++)
    {
        mpi_read_binary(pstruMpi[u8Index], pu8PrivateKey + u16StartPos, u8BufLen[u8Index]);
        u16StartPos += (u16)u8BufLen[u8Index];
    }
    
    ZC_Printf("pstrRsa->Q.s = %d, pstrRsa->Q.n = %d, pstrRsa->Q.p[0] = %d, pstrRsa->Q.p = %d\n",
        pstrRsa->Q.s,pstrRsa->Q.n, pstrRsa->Q.p[0], pstrRsa->Q.p);
}
/*************************************************
* Function: SEC_AesEncrypt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 SEC_AesEncrypt(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len, u16 *pu16CiperLen)
{
    u32 u32OutLen;
    PTC_ProtocolCon *pstruCon;

    /*must assign the outlen*/
    u32OutLen = *pu16CiperLen;

    pstruCon = &g_struProtocolController;
    
    if (PCT_KEY_RECVED != pstruCon->u8keyRecv)
    {
        return ZC_RET_ERROR;            
    }
    
    AES_CBC_Encrypt(pu8Plainbuf, u16Len, 
        pstruCon->u8SessionKey, ZC_HS_SESSION_KEY_LEN, 
        pstruCon->IvSend, 16, 
        pu8CiperBuf, &u32OutLen);
    *pu16CiperLen = (u16)u32OutLen;  
    //memcpy(pstruCon->IvSend, pu8CiperBuf, 16);
    return ZC_RET_OK;
}
/*************************************************
* Function: SEC_AesDecrypt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:PTC_ProtocolCon *pstruCon, u8 *pu8Key, u8 *pu8IvRecv,
*************************************************/
u32 SEC_AesDecrypt(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len, u16 *pu16PlainLen)
{
    u8 *pu8Key;
    u8 *pu8IvRecv;
    //u8 u8NextIv[16];
    u32 u32OutLen;
    PTC_ProtocolCon *pstruCon;
    
    /*must assign the outlen*/
    u32OutLen = u16Len;
    pstruCon = &g_struProtocolController;
    
    if (PCT_KEY_RECVED != pstruCon->u8keyRecv)
    {
        return ZC_RET_ERROR;            
    }
    
    pu8Key = pstruCon->u8SessionKey;
    pu8IvRecv = pstruCon->IvRecv;
#if 0    
    if (u16Len > 16)
    {
        memcpy(u8NextIv, pu8CiperBuf, 16);
    }
    else
    {
        /*padding if small than 16*/
        memcpy(u8NextIv, pu8CiperBuf, u16Len);
        memset(u8NextIv + u16Len, 0, 16 - u16Len);
    }
#endif    
    AES_CBC_Decrypt(pu8CiperBuf, u16Len, pu8Key, ZC_HS_SESSION_KEY_LEN, pu8IvRecv, 16, pu8Plainbuf, &u32OutLen);
    *pu16PlainLen = (u16)u32OutLen;
    //memcpy(pstruCon->IvRecv, u8NextIv, 16);
    return ZC_RET_OK;
}
/*************************************************
* Function: SEC_PaddingCheck
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 SEC_PaddingCheck(u8 u8SecType, u16 u16PlainLen, u16 *u16PaddingLen)
{
    u16 LastBlockSize = 0;
    u8 u8SecFlag;
    *u16PaddingLen = 0;

    if (0 == g_struZcConfigDb.struSwitchInfo.u32SecSwitch)
    {
        u8SecFlag = ZC_SEC_ALG_NONE;
    }
    else if (2 == g_struZcConfigDb.struSwitchInfo.u32SecSwitch)
    {
        if (ZC_SEC_ALG_RSA == u8SecType)
        {
            u8SecFlag = ZC_SEC_ALG_NONE;    
        }
        else
        {
            u8SecFlag = u8SecType;
        }
    }else
    {
        u8SecFlag = u8SecType;
    }
    
    switch(u8SecFlag)
    {
        case ZC_SEC_ALG_AES:
        {
            LastBlockSize = u16PlainLen % ZC_SEC_AES_BLOCK_SIZE;
            if(LastBlockSize > 0)
                *u16PaddingLen = ZC_SEC_AES_BLOCK_SIZE - LastBlockSize;
            else
                *u16PaddingLen = ZC_SEC_AES_BLOCK_SIZE;
            break;
        }
        case ZC_SEC_ALG_RSA:
        {
            LastBlockSize = u16PlainLen % (ZC_SEC_RSA_KEY_LEN >> 3);
            if (LastBlockSize > 0)
                *u16PaddingLen = (ZC_SEC_RSA_KEY_LEN >> 3) - LastBlockSize;
            else
                *u16PaddingLen = 0;
            break;
        }
    }

    return ZC_RET_OK;
}


/*************************************************
* Function: SEC_Encrypt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 SEC_Encrypt(ZC_SecHead *pstruSecHead, u8 *pu8CiperBuf, u8 *pu8PlainBuf, u16 *pu16CiperLen)
{
    u32 u32RetVal = ZC_RET_ERROR;
    
    if (0 == g_struZcConfigDb.struSwitchInfo.u32SecSwitch)
    {
        pstruSecHead->u8SecType = ZC_SEC_ALG_NONE;
    }
    
    switch (pstruSecHead->u8SecType)
    {
        case ZC_SEC_ALG_NONE:
            memcpy(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg));
            u32RetVal = ZC_RET_OK;
            *pu16CiperLen = ZC_HTONS(pstruSecHead->u16TotalMsg);
            break;
        case ZC_SEC_ALG_RSA:
            u32RetVal = SEC_EncryptTextByRsa(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg), pu16CiperLen);
            break;
        case ZC_SEC_ALG_AES:
            u32RetVal = SEC_AesEncrypt(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg), pu16CiperLen);
            break;                        
    }
    return u32RetVal;
}
/*************************************************
* Function: SEC_Decrypt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 SEC_Decrypt(ZC_SecHead *pstruSecHead, u8 *pu8CiperBuf, u8 *pu8PlainBuf, u16 *pu16PlainLen)
{
    u32 u32RetVal = ZC_RET_ERROR;

    u8 u8SecFlag;
    if (0 == g_struZcConfigDb.struSwitchInfo.u32SecSwitch)
    {
        u8SecFlag = ZC_SEC_ALG_NONE;
    }
    else if (2 == g_struZcConfigDb.struSwitchInfo.u32SecSwitch)
    {
        if (ZC_SEC_ALG_RSA == pstruSecHead->u8SecType)
        {
            u8SecFlag = ZC_SEC_ALG_NONE;    
        }
        else
        {
            u8SecFlag = pstruSecHead->u8SecType;
        }
    }else
    {
        u8SecFlag = pstruSecHead->u8SecType;
    }

    switch (u8SecFlag)
    {
        case ZC_SEC_ALG_NONE:
            memcpy(pu8PlainBuf, pu8CiperBuf, ZC_HTONS(pstruSecHead->u16TotalMsg));
            u32RetVal = ZC_RET_OK;
            *pu16PlainLen = ZC_HTONS(pstruSecHead->u16TotalMsg);
            break;
        case ZC_SEC_ALG_RSA:
            u32RetVal = SEC_DecryptTextByRsa(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg), pu16PlainLen);
            break;
        case ZC_SEC_ALG_AES:
            u32RetVal = SEC_AesDecrypt(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg), pu16PlainLen);
            break;                        
    }
    return u32RetVal;
}
/******************************* FILE END ***********************************/
