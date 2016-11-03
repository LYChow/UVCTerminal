//
//  G711CodecImpl.h
//  cameratest
//
//  Created by hxf on 15/12/21.
//  Copyright © 2015年 Jsonmess. All rights reserved.
//

#ifndef G711CodecImpl_h
#define G711CodecImpl_h

#import <Foundation/Foundation.h>

@interface G711CodecImpl : NSObject

-(BOOL) encodeAlaw:(const char*)pInputBuf :(int)inputBufLen :(char*)pOutputBuf :(int)outputBufLen :(int*)pWriteLen;
-(BOOL) decodeAlaw:(const char*)pInputBuf :(int)inputBufLen :(char*)pOutputBuf :(int)outputBufLen :(int*)pWriteLen;
-(BOOL) encodeUlaw:(const char*)pInputBuf :(int)inputBufLen :(char*)pOutputBuf :(int)outputBufLen :(int*)pWriteLen;
-(BOOL) decodeUlaw:(const char*)pInputBuf :(int)inputBufLen :(char*)pOutputBuf :(int)outputBufLen :(int*)pWriteLen;

@end

#endif /*G711CodecImpl_h*/