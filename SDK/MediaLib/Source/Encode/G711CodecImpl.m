//
//  G711CodecImpl.m
//  cameratest
//
//  Created by hxf on 15/12/21.
//  Copyright © 2015年 Jsonmess. All rights reserved.
//

#import "g711.h"
#import "G711CodecImpl.h"

@implementation G711CodecImpl

-(BOOL) encodeAlaw:(const char*)pInputBuf :(int)inputBufLen :(char*)pOutputBuf :(int)outputBufLen :(int*)pWriteLen
{
    if (inputBufLen > outputBufLen*2)
        return false;
    
    union UNION_711_IN u711in;
    
    for (int i=0; i<inputBufLen/2; i++)
    {
        u711in.in_char[0] = pInputBuf[2*i];
        u711in.in_char[1] = pInputBuf[2*i+1];
        pOutputBuf[i] = linear2alaw(u711in.in_short);
    }
    if (0 != pWriteLen)
        *pWriteLen = inputBufLen/2;
    
    return true;
}

-(BOOL) decodeAlaw:(const char*)pInputBuf :(int)inputBufLen :(char*)pOutputBuf :(int)outputBufLen :(int*)pWriteLen
{
    if (inputBufLen*2 > outputBufLen)
        return false;
    
    union UNION_711_OUT u711out;
    
    for (int i=0; i<inputBufLen; i++)
    {
        u711out.out_short = alaw2linear(pInputBuf[i]);
        pOutputBuf[2*i] = u711out.out_char[0];
        pOutputBuf[2*i+1] = u711out.out_char[1];
    }
    if (0 != pWriteLen)
        *pWriteLen = 2*inputBufLen;
    
    return true;
}

-(BOOL) encodeUlaw:(const char*)pInputBuf :(int)inputBufLen :(char*)pOutputBuf :(int)outputBufLen :(int*)pWriteLen
{
    if (inputBufLen > outputBufLen*2)
        return false;
    
    union UNION_711_IN u711in;
    
    for (int i=0; i<inputBufLen/2; i++)
    {
        u711in.in_char[0] = pInputBuf[2*i];
        u711in.in_char[1] = pInputBuf[2*i+1];
        pOutputBuf[i] = linear2ulaw(u711in.in_short);
    }
    if (0 != pWriteLen)
        *pWriteLen = inputBufLen/2;
    
    return true;
}

-(BOOL) decodeUlaw:(const char*)pInputBuf :(int)inputBufLen :(char*)pOutputBuf :(int)outputBufLen :(int*)pWriteLen
{
    if (inputBufLen*2 > outputBufLen)
        return false;
    
    union UNION_711_OUT u711out;
    
    for (int i=0; i<inputBufLen; i++)
    {
        u711out.out_short = ulaw2linear(pInputBuf[i]);
        pOutputBuf[2*i] = u711out.out_char[0];
        pOutputBuf[2*i+1] = u711out.out_char[1];
    }
    if (0 != pWriteLen)
        *pWriteLen = 2*inputBufLen;
    
    return true;
}

@end
