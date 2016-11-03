//
//  SwBaseWin.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/15.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#include "SwBaseWin.h"


DWORD GetTickCount()
{
    struct timeval tv;
    if(gettimeofday(&tv, 0))
        return 0;

    return (tv.tv_sec*1000)+(tv.tv_usec/1000);
}

void Sleep(DWORD ms)
{
    usleep(ms*1000);
}