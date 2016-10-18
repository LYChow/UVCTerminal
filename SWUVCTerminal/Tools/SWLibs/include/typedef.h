//
//  typedef.h
//  Common
//
//  Created by zhangyujun on 16/6/7.
//  Copyright © 2016年 sinowave. All rights reserved.
//

#ifndef typedef_h
#define typedef_h

#define TRUE 1
#define FALSE 0
#define NULL 0

#define VOID void
typedef void *LPVOID,*HOBJECT;
typedef void* HANDLE;
typedef char* LPTSTR;
typedef const char* LPCTSTR;
typedef unsigned char BYTE,*LPBYTE;
typedef unsigned short WORD,*LPWORD;
typedef unsigned int UINT,*LPINT,DWORD,*LPDWORD;
typedef long long LONGLONG,INT64,*LPLONGLONG,*LPINT64;
#ifndef time_t
typedef long time_t;
#endif

#ifndef LOWORD
#define LOWORD(x)	((WORD)((x)&0xffff))
#define HIWORD(x)	((WORD)((x>>16)&0xffff))
#endif

#define ZeroMemory(p,size) memset(p,0,size)
#define ASSERT(exp)
typedef int IBOOL;
typedef int IRESULT;

#define MAKEDWORD(l,h) ((DWORD)(((h)&0xffff)<<16)|((l)&0xffff))
#define INT_P(p) (*((int*)p))
#define PINT_P(p) ((int*)p)
#define _MMBOFFSET(mmb) (LPBYTE(&mmb)-LPBYTE(this))
#define _PTRMMBOFFSET(p,mmb) (LPBYTE(&p->mmb)-LPBYTE(p))
#define _OFFSETOF(t,m)	((int)&(((t *)0)->m))



#define ZeroVar(var) ZeroMemory(&(var),sizeof(var))

#define ZeroPtrFrom(p,mmb) ZeroMemory(&p->mmb,sizeof(*p)-_PTRMMBOFFSET(p,mmb));
#define ZeroThisFrom(mmb) ZeroPtrFrom(this,mmb)


#define BitMask(bit) (1<<bit)
#define BitSet(val,bit) val|=BitMask(bit)
#define BitClear(val,bit) val&=~BitMask(bit)
#define IsBitSet(val,bit) ((val&BitMask(bit))!=0)



#define _BEGIN_NAMESPACE(ns) namespace ns{
#define _END_NAMESPACE };


#endif /* typedef_h */
