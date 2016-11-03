#ifndef _SWVC3_TYPEDEFINE_H_20151216_
#define _SWVC3_TYPEDEFINE_H_20151216_

#include "string.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef _WINDEF_
typedef unsigned short		WORD;
typedef unsigned int		DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef long				LONG;
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define SWRGB(r,g,b) ((DWORD)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

#endif