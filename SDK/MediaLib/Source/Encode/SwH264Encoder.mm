//
//  SwEncoder.mm
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/31.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "H264HwEncoderImpl.h"
#import "SwH264Encoder.h"

#pragma mark -  IH264HwEncoderImplDelegate
@interface IH264HwEncoderImplDelegate : NSObject <H264HwEncoderImplDelegate>
{
    @public CH264HwEncoderImpl* _cpp_delegate;
}

@end

@implementation IH264HwEncoderImplDelegate

- (void)gotSpsPps:(NSData*)sps pps:(NSData*)pps
{
    NSLog(@"gotSpsPps %d %d", (int)[sps length], (int)[pps length]);
    //    //[sps writeToFile:h264File atomically:YES];
    //    //[pps writeToFile:h264File atomically:YES];
    //    // write(fd, [sps bytes], [sps length]);
    //    //write(fd, [pps bytes], [pps length]);
    //    const char bytes[] = "\x00\x00\x00\x01";
    //    size_t length = (sizeof bytes) - 1; //string literals have implicit trailing '\0'
    //    NSData *ByteHeader = [NSData dataWithBytes:bytes length:length];
    //    [fileHandle writeData:ByteHeader];
    //    [fileHandle writeData:sps];
    //    [fileHandle writeData:ByteHeader];
    //    [fileHandle writeData:pps];

    if (_cpp_delegate) {
        _cpp_delegate->OnEncodedDataGot((const char*)sps.bytes, sps.length, 2);
        _cpp_delegate->OnEncodedDataGot((const char*)pps.bytes, pps.length, 3);
    }
}

- (void)gotEncodedData:(NSData*)data isKeyFrame:(BOOL)isKeyFrame
{
    NSLog(@"gotEncodedData %d", (int)[data length]);
    //    static int framecount = 1;
    //
    //    // [data writeToFile:h264File atomically:YES];
    //    //write(fd, [data bytes], [data length]);
    //    if (fileHandle != NULL)
    //    {
    //        const char bytes[] = "\x00\x00\x00\x01";
    //        size_t length = (sizeof bytes) - 1; //string literals have implicit trailing '\0'
    //        NSData *ByteHeader = [NSData dataWithBytes:bytes length:length];
    //
    //
    //        /*NSData *UnitHeader;
    //         if(isKeyFrame)
    //         {
    //         char header[2];
    //         header[0] = '\x65';
    //         UnitHeader = [NSData dataWithBytes:header length:1];
    //         framecount = 1;
    //         }
    //         else
    //         {
    //         char header[4];
    //         header[0] = '\x41';
    //         //header[1] = '\x9A';
    //         //header[2] = framecount;
    //         UnitHeader = [NSData dataWithBytes:header length:1];
    //         framecount++;
    //         }*/
    //        [fileHandle writeData:ByteHeader];
    //        //[fileHandle writeData:UnitHeader];
    //        [fileHandle writeData:data];
    //    }

//    if ((((char*)data.bytes)[0]&0x1f) == 6) {
//        return;
//    }
    if (_cpp_delegate) {
        _cpp_delegate->OnEncodedDataGot((const char*)data.bytes, data.length, isKeyFrame?1:0);
    }
}

@end

#pragma mark - class CH264HwEncoderImpl
const H264HwEncoderImpl* TransferH264HwEncoderImpl(void* _pH264HwEncoderImpl)
{
    H264HwEncoderImpl* pH264HwEncoderImpl = (__bridge H264HwEncoderImpl*)(_pH264HwEncoderImpl);
    return pH264HwEncoderImpl;
}

#pragma mark - public functions
CH264HwEncoderImpl::CH264HwEncoderImpl(PCH264HwEncoderImpl_DataCallback cb, long cbuser)
: m_nWidth(352)
, m_nHeight(288)
, m_nFrameduration_value(1)
, m_nFrameduration_timescale(30)
, m_nBitrate(1024)
, m_pH264HwEncoderImpl(NULL)
, m_pDataCallback(cb)
, m_lDataCallbackUser(cbuser)
, m_pFrameBuf(NULL)
, m_nFrameBufSize(0)
, m_nPreDataLen(0)
, m_bPreDataReady(false)
{
}

CH264HwEncoderImpl::~CH264HwEncoderImpl()
{
    Release();
    if (m_pFrameBuf) {
        delete [] m_pFrameBuf;
        m_pFrameBuf = NULL;
    }
    m_nFrameBufSize = 0;
}

int CH264HwEncoderImpl::Init(int width, int height, int frameduration_value, int frameduration_timescale, int bitrate)
{
    Release();

    int ir = -1;

    m_nWidth = width;
    m_nHeight = height;
    m_nFrameduration_value = frameduration_value;
    m_nFrameduration_timescale = frameduration_timescale;
    m_nBitrate = bitrate;

    do {
        if (m_pH264HwEncoderImpl) {
            ir = 0;
            break;
        }

        IH264HwEncoderImplDelegate* pH264HwEncoderImplDelegate = [[IH264HwEncoderImplDelegate alloc] init];
        if (!pH264HwEncoderImplDelegate) {
            printf("[[IH264HwEncoderImplDelegate alloc] init] Failed.\n");
            ir = -2;
            break;
        }
        else
        {
            pH264HwEncoderImplDelegate->_cpp_delegate = this;
        }

        H264HwEncoderImpl* pH264HwEncoderImpl = [H264HwEncoderImpl alloc];
        if (!pH264HwEncoderImpl) {
            pH264HwEncoderImplDelegate = nil;
            printf("[[H264HwEncoderImpl alloc] init] Failed.\n");
            ir = -3;
            break;
        }
        else
        {
            [pH264HwEncoderImpl initWithMembers];
            pH264HwEncoderImpl._delegate = pH264HwEncoderImplDelegate;
            [pH264HwEncoderImpl start:width height:height frameduration:CMTimeMake(m_nFrameduration_value, m_nFrameduration_timescale) bitrate:bitrate];
            m_pH264HwEncoderImpl = (__bridge_retained void*)pH264HwEncoderImpl;

            ir = 0;
        }
    } while (0);

    return ir;
}

int CH264HwEncoderImpl::PutData(const char* data, int frameduration_value, int frameduration_timescale, int flags)
{
    int ir = -1;
    if (!data) {
        return ir;
    }

//#ifdef DEBUGjgggg
//    {
//        CVImageBufferRef pixelBuffer = (CVImageBufferRef)data;
//        if(CVPixelBufferLockBaseAddress(pixelBuffer, 0) == kCVReturnSuccess) {
//            //        UInt8 *bufferPtr = (UInt8 *)CVPixelBufferGetBaseAddress(pixelBuffer);
//            //        size_t buffeSize = CVPixelBufferGetDataSize(pixelBuffer);
//            size_t bufWidth  = CVPixelBufferGetWidth(pixelBuffer);
//            size_t bufHeight = CVPixelBufferGetHeight(pixelBuffer);
//
//            if (h264Encoder) {
//                if (w!=bufWidth || h!=bufHeight) {
//                    h264Encoder._delegate = nil;
//                    h264Encoder = nil;
//                }
//            }
//            if (!h264Encoder) {
//                w = bufWidth;
//                h = bufHeight;
//                h264Encoder = [H264HwEncoderImpl alloc];
//                [h264Encoder initWithMembers];
//                h264Encoder._delegate = [[IH264HwEncoderImplDelegate alloc] init];
//                [h264Encoder start:w height:h framerate:30 bitrate:1024];
//            }
//            if (h264Encoder) {
//                [h264Encoder encode:pixelBuffer];
//            }
//            
//            CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
//        }
//        return 0;
//    }
//#endif //#ifdef DEBUG

    CVImageBufferRef pixelBuffer = (CVImageBufferRef)data;
    if(CVPixelBufferLockBaseAddress(pixelBuffer, 0) == kCVReturnSuccess) {
//        UInt8 *bufferPtr = (UInt8 *)CVPixelBufferGetBaseAddress(pixelBuffer);
//        size_t buffeSize = CVPixelBufferGetDataSize(pixelBuffer);
        size_t bufWidth  = CVPixelBufferGetWidth(pixelBuffer);
        size_t bufHeight = CVPixelBufferGetHeight(pixelBuffer);

        if (!m_pH264HwEncoderImpl || m_nWidth!=bufWidth || m_nHeight!=bufHeight
            || m_nFrameduration_value!=frameduration_value || m_nFrameduration_timescale!=frameduration_timescale) {
            Release();
            ir = Init(bufWidth, bufHeight, frameduration_value, frameduration_timescale, m_nBitrate);
            if (ir != 0) {
                return ir;
            }
        }

        if (!m_pH264HwEncoderImpl) {
            return ir;
        }

        [TransferH264HwEncoderImpl(m_pH264HwEncoderImpl) encode:pixelBuffer];

        CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
        ir = 0;
    }

    return ir;
}

int CH264HwEncoderImpl::SetBitrate(int bitrate)
{
    int ir = -1;

    if (m_pH264HwEncoderImpl) {
        [TransferH264HwEncoderImpl(m_pH264HwEncoderImpl) setBitrate:bitrate];
        ir = 0;
    }

    return ir;
}

int CH264HwEncoderImpl::ForceKeyFrame()
{
    int ir = -1;

    if (m_pH264HwEncoderImpl) {
        [TransferH264HwEncoderImpl(m_pH264HwEncoderImpl) forceKeyFrame];
        ir = 0;
    }

    return ir;
}

void CH264HwEncoderImpl::CH264HwEncoderImpl::Release()
{
    if (m_pH264HwEncoderImpl) {
        H264HwEncoderImpl* pH264HwEncoderImpl = (__bridge_transfer H264HwEncoderImpl*)(m_pH264HwEncoderImpl);
        m_pH264HwEncoderImpl = NULL;

        [pH264HwEncoderImpl stop];
        pH264HwEncoderImpl._delegate = nil;
        pH264HwEncoderImpl = nil;
    }
}

void CH264HwEncoderImpl::OnEncodedDataGot(const char* data, int len, int type) // type: 0 P, 1 IDR, 2 sps, 3 pps
{
    static const char nalu_sp[4] = { '\0', '\0', '\0', '\1' };
    AsureFrameBufSize(len+4);

    switch (type) {
        case 0: // P
            if (m_bPreDataReady && m_pDataCallback && m_pFrameBuf) {
                memcpy(m_pFrameBuf+m_nPreDataLen, nalu_sp, 4);
                memcpy(m_pFrameBuf+m_nPreDataLen+4, data, len);

                H264HE_DataCallbackFrame frame;
                frame.buf = (unsigned char*)(m_pFrameBuf+m_nPreDataLen);
                frame.size = len+4;
                frame.keyframe = false;

                m_pDataCallback(H264HE_DCC_FRAME, (long)&frame, m_lDataCallbackUser);
            }
            break;
        case 1: // IDR
            if (m_bPreDataReady && m_pDataCallback && m_pFrameBuf) {
                memcpy(m_pFrameBuf+m_nPreDataLen, nalu_sp, 4);
                memcpy(m_pFrameBuf+m_nPreDataLen+4, data, len);

                H264HE_DataCallbackFrame frame;
                frame.buf = (unsigned char*)m_pFrameBuf;
                frame.size = m_nPreDataLen+len+4;
                frame.keyframe = true;

                m_pDataCallback(H264HE_DCC_FRAME, (long)&frame, m_lDataCallbackUser);
            }
            break;
        case 2: // sps
            m_nPreDataLen = 0;
            if (m_pFrameBuf) {
                memcpy(m_pFrameBuf, nalu_sp, 4);
                memcpy(m_pFrameBuf+4, data, len);
                m_nPreDataLen = len+4;
            }
            m_bPreDataReady = false;
            break;
        case 3: // pps
            if (!m_bPreDataReady && m_nPreDataLen>0 && m_pFrameBuf) {
                memcpy(m_pFrameBuf+m_nPreDataLen, nalu_sp, 4);
                memcpy(m_pFrameBuf+m_nPreDataLen+4, data, len);
                m_nPreDataLen += (len+4);

                m_bPreDataReady = true;
            }
            break;

        default:
            break;
    }
}

#pragma mark - protected functions
void CH264HwEncoderImpl::AsureFrameBufSize(int newdatalen)
{
    if (!m_pFrameBuf) {
        m_nFrameBufSize = 0;
        m_nPreDataLen = 0;
    }

    if (m_nPreDataLen+newdatalen > m_nFrameBufSize) {
        char* pTmp = m_pFrameBuf;

        m_nFrameBufSize = m_nPreDataLen+newdatalen;
        m_pFrameBuf = new char[m_nFrameBufSize];
        if (m_pFrameBuf && pTmp && m_nPreDataLen>0) {
            memcpy(m_pFrameBuf, pTmp, m_nPreDataLen);
        }

        delete [] pTmp;
    }
}