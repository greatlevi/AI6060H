
#ifndef _WEP_H_
#define _WEP_H_

#include "ssv_types.h"
#include "ieee80211.h"
#include "arc4.h"

#define AP5_80211_ADDR {0x14, 0x75, 0x90, 0x6F, 0x0B, 0x3D}
#define AP6_80211_ADDR {0x00, 0x02, 0x6F, 0x00, 0x00, 0x00}

#define WEP_ENABLE         1

#define CRCPOLY		0xedb88320
#define ChallengeLen         128
#define StaticBufferSize     1514
#define IVBYTE                     3
#define KEYINDEX               1
#define ICVBYTE                  4
#define ALGBYTE                 2
#define SEQBYTE                 2
#define STATUSBYTE          2
#define EIDBYTE                   1
#define EIDLENBYTE           1
#define PASSMAXBYTE     16

typedef enum _WEP_TYPE
{
    ENCRYPTION = 0,
    DECRYPTION,
} WEP_TYPE;

typedef struct t_TAG_WEP_INFO
{
    U8 iv[IVBYTE];
    U8 keyIndex;
    U8 AuthAlg;
    U8 AuthSeq;
    U8 AuthStatus;
    U16 NcrDecTextLen;
    U8 NcrDecText[StaticBufferSize];
    U8 totalKey[IVBYTE + PASSMAXBYTE];
    U8 totalKeyLen;
    U8 sonkey[StaticBufferSize];
} TAG_WEP_INFO;

U8 getAuthChallengeTextLen();
U8 getAuthAlgNum();
U8 getAuthSeqNum();
U8 getAuthStatus();
void SetAuthChallengeText(U8 *src);
void SetAuthChallengeTextLen(U8 len);
void SetAuthAlgNum(U8 algNum);
void setAuthSeqNum(U8 seqNum);
void setAuthStatus(U8 status);
void setKeyAndIV(U8 *src, U8 size);
void setBefEncData();
void setBefEncData();
U16 getEncDecData(U8 *dst, WEP_TYPE type);
void encDecSrcData(U8 *src, U16 len , WEP_TYPE type);
void initKeyAndIV();
void randIV();
void savePassToTotalKey();
void initWEP(U16 len, U8 othLen);
void clearParaWithWEP();
U16 saveAftEncData(U8 *dst);

#endif

