//
//  itzLog.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 16/2/2.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#include "itzLog.h"
#import <Foundation/Foundation.h>

int GetDocumentDirectory(char* buf, int bufsize)
{
    if (!bufsize || bufsize<1) {
        return -1;
    }

    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    if (!paths) {
        return -1;
    }
    NSString *docDir = [paths objectAtIndex:0];
    if (!docDir) {
        return -1;
    }

    int ir = [docDir length];
    if (ir >= bufsize) {
        ir = bufsize-1;
    }
    strncpy(buf, [docDir UTF8String], ir);

    return ir;
}