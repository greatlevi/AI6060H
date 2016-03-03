/**
******************************************************************************
* @file     zc_common.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief   
******************************************************************************
*/

#include <zc_common.h>
#include <zc_protocol_controller.h>
#include <zc_sec_engine.h>

/*************************************************
* Function: ZC_TraceData
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_TraceData(u8* pData, u32 Len)
{
    u32 Index;
    if (0 == g_struZcConfigDb.struSwitchInfo.u32TraceSwitch)
    {
        return;
    }
    if (0 == Len)
    {
        return;
    }
    
    ZC_Printf("++++++++++++++++++++++++++++++++++++++++++++++++\n");
    for (Index = 0; Index + 4 < Len; Index = Index + 4)
    {
        ZC_Printf("0x%02x, 0x%02x, 0x%02x, 0x%02x,\n",
            pData[Index],
            pData[Index + 1],
            pData[Index + 2],
            pData[Index + 3]);
    }
    
    for (; Index < Len - 1; Index++)
    {
        ZC_Printf("0x%02x, ", pData[Index]);
    }
    ZC_Printf("0x%02x", pData[Index]);

    ZC_Printf("\n++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

/*************************************************
* Function: ZC_Rand
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_Rand(u8 *pu8Rand)
{
    u32 u32Rand;
    u32 u32Index;  
    for (u32Index = 0; u32Index < (ZC_HS_MSG_LEN/4); u32Index++)
    {
        u32Rand = rand();
        //ZC_Printf("rand is %u\n\r", u32Rand);
        pu8Rand[u32Index * 4] = ((u8)u32Rand % 26) + 65;
        pu8Rand[u32Index * 4 + 1] = ((u8)(u32Rand >> 8) % 26) + 65;
        pu8Rand[u32Index * 4 + 2] = ((u8)(u32Rand >> 16) % 26) + 65;
        pu8Rand[u32Index * 4 + 3] = ((u8)(u32Rand >> 24) % 26) + 65;        
    }
}

/*************************************************
* Function: ZC_HexToString
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_HexToString(u8 *StringBuf,u8* HexBuf,u8 len)
{
    u8 i;
    u8 *xad;

    // Display the extended address.
    xad = HexBuf;

    for (i = 0; i < len*2; xad++)
    {
        u8 ch;
        ch = (*xad >> 4) & 0x0F;
        StringBuf[i++] = ch + (( ch < 10 ) ? '0' : '7');
        ch = *xad & 0x0F;
        StringBuf[i++] = ch + (( ch < 10 ) ? '0' : '7');
    }
}
/******************************* FILE END ***********************************/


