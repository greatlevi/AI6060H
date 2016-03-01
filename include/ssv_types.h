#ifndef _SSV_TYPES_H_
#define _SSV_TYPES_H_

//#include <ssv_config.h>
#include <ssv_module.h>

#ifdef __GNUC__
#define STRUCT_PACKED __attribute__ ((packed))
#else
//#define STRUCT_PACKED
#endif

#define ATTRIBUTE_SECTION_SRAM  __attribute__((section("keep_in_sram")))
#define SECTION_SRAM section("keep_in_sram")

#ifndef NULL
#define NULL		((void *)0)
#endif

#define TRUE        (1)
#define FALSE       (0)

#define ENABLE      (0x1)
#define DISABLE     (0x0)



typedef unsigned int        	size_t;

#if 0 //obsolete and do not use it!! 2015/07/27 jim.yang
typedef unsigned char       	u8;
typedef unsigned short      	u16;
typedef unsigned int        	u32;
typedef unsigned long long  u64;
typedef char            		s8;
typedef short           		s16;
typedef int             		s32;
typedef long long       		s64;
typedef unsigned int		bool;
#endif

#if 1
typedef unsigned char       	U8;
typedef unsigned short      	U16;
typedef unsigned int        	U32;
typedef unsigned long long  U64;

typedef char            		S8;
typedef short           		S16;
typedef int             		S32;
typedef long long       		S64;

typedef unsigned int		BOOL;
#endif

//---------------------merged from original compiler.h,this kind of defintion is from older SSH source code----------------
typedef unsigned short		le16_t;
typedef unsigned long		le32_t;

#define cpu_to_le16(x)	Le16(x)
#define le16_to_cpu(x)	Le16(x)

// Endian converters
#define Le16(b)                        \
   (  ((U16)(     (b) &   0xFF) << 8)  \
   |  (     ((U16)(b) & 0xFF00) >> 8)  \
   )
#define Le32(b)                             \
   (  ((U32)(     (b) &       0xFF) << 24)  \
   |  ((U32)((U16)(b) &     0xFF00) <<  8)  \
   |  (     ((U32)(b) &   0xFF0000) >>  8)  \
   |  (     ((U32)(b) & 0xFF000000) >> 24)  \
   )


#endif /* _SSV_TYPES_H_ */
