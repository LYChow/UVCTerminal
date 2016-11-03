//
//  SwH264Decoder.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 16/2/3.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "SwH264Decoder.h"
#import "H264HwDecoderImpl.h"

#pragma mark -  IH264HwEncoderImplDelegate
@interface IH264HwDecoderImplDelegate : NSObject <H264HwDecoderImplDelegate>
{
    @public CH264HwDecoderImpl* _cpp_delegate;
}

@end

@implementation IH264HwDecoderImplDelegate
-(void) gotDecodedData:(CVImageBufferRef) imageBuffer
{
    if (_cpp_delegate) {
        _cpp_delegate->OnGotDecodeData((void*)imageBuffer);
    }
}

@end

#pragma mark - class CH264HwEncoderImpl
const H264HwDecoderImpl* TransferH264HwDecoderImpl(void* _pH264HwDecoderImpl)
{
    H264HwDecoderImpl* pH264HwDecoderImpl = (__bridge H264HwDecoderImpl*)(_pH264HwDecoderImpl);
    return pH264HwDecoderImpl;
}

#pragma mark - public functions constructor/destructor destructor
CH264HwDecoderImpl::CH264HwDecoderImpl(PH264HwDecoderImpl_DataCallback pDataCallback, long lDataCallbackUser)
: m_pH264HwDecoderImpl(NULL)
, m_pDataCallback(pDataCallback)
, m_lDataCallbackUser(lDataCallbackUser)
{
}

CH264HwDecoderImpl::~CH264HwDecoderImpl()
{
    stop();
}

#pragma mark - public functions own
void CH264HwDecoderImpl::start()
{
    if (!m_pH264HwDecoderImpl) {
        IH264HwDecoderImplDelegate* pH264HwDecoderImplDelegate = [[IH264HwDecoderImplDelegate alloc] init];
        if (!pH264HwDecoderImplDelegate) {
            printf("[[IH264HwDecoderImplDelegate alloc] init] Failed.\n");
        }
        else
        {
            pH264HwDecoderImplDelegate->_cpp_delegate = this;
        }
        H264HwDecoderImpl* pH264HwDecoderImpl = [H264HwDecoderImpl alloc];
        if (!pH264HwDecoderImpl) {
            printf("[[H264HwDecoderImpl alloc] init] Failed.\n");
        }
        else
        {
            [pH264HwDecoderImpl initWithMembers];
            pH264HwDecoderImpl._delegate = pH264HwDecoderImplDelegate;
            m_pH264HwDecoderImpl = (__bridge_retained void*)pH264HwDecoderImpl;
        }
    }
    if (m_pH264HwDecoderImpl) {
        [TransferH264HwDecoderImpl(m_pH264HwDecoderImpl) start];
    }
}

void CH264HwDecoderImpl::decode(int width, int height, const char* data, int len)
{
#ifdef DEBUG33456
    static FILE* fileH264=NULL;

    if (!fileH264) {
        NSString* nspath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject] stringByAppendingString:@"/decode_____term_vc3.h264"];
        if (nspath) {
           // TRACE("will write data to %s\n", szBuf);
            fileH264 = fopen([nspath UTF8String], "w");
        }
    }
    if (fileH264) {
        fwrite((const void*)data, len, 1, fileH264);
    }
#endif //#ifdef DEBUG

    if (m_pH264HwDecoderImpl) {
//        NSData* nsData = [NSData dataWithBytes:data length:len];
        [TransferH264HwDecoderImpl(m_pH264HwDecoderImpl) decode:width height:height inData:data inDataLen:len];
    }
}

void CH264HwDecoderImpl::stop()
{
    if (m_pH264HwDecoderImpl) {
        H264HwDecoderImpl* pH264HwDecoderImpl = (__bridge_transfer H264HwDecoderImpl*)(m_pH264HwDecoderImpl);
        m_pH264HwDecoderImpl = NULL;
        [pH264HwDecoderImpl stop];
        pH264HwDecoderImpl._delegate = nil;
        pH264HwDecoderImpl = nil;
    }
}

void CH264HwDecoderImpl::OnGotDecodeData(void* _CVImageBufferRef)
{
    if (m_pDataCallback) {
        m_pDataCallback(H264HD_DCC_DECODEFRAME, (long)_CVImageBufferRef, m_lDataCallbackUser);
    }
}

#pragma mark - protected functions
