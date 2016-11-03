//
//  SwBaseWin.h
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/24.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#ifndef SwBaseWin_h
#define SwBaseWin_h

#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include "SWVC3TypeDefine.h"

typedef void* PVOID;
typedef void* LPVOID;
#define TRACE   //printf
#define TRACE0  printf
#define TRACE1  printf
#define TRACE2  printf
#define TRACE3  printf


#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
//#define LOWORD(l)           ((WORD)(l))
//#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))


DWORD GetTickCount();
//#define GetTickCount() 1

void Sleep(DWORD ms);
//#define Sleep(a) int _____slp=a

#endif /* SwBaseWin_h */
