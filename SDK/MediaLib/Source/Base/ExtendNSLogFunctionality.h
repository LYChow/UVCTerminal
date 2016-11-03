//
//  ExtendNSLogFunctionality.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/3/31.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef ExtendNSLogFunctionality_h
#define ExtendNSLogFunctionality_h

//#ifdef DEBUG
//#define NSLog(args...)  ExtendNSLog(__FILE__,__LINE__,__PRETTY_FUNCTION__,args);
//#else
#define NSLog(x...)
//#endif
#define Log(args...)  //Log2File(__FILE__,__LINE__,__PRETTY_FUNCTION__,args);

void ExtendNSLog(const char *file, int lineNumber, const char *functionName, NSString *format, ...);
void Log2File(const char *file, int lineNumber, const char *functionName, NSString *format, ...);

#endif /* ExtendNSLogFunctionality_h */
