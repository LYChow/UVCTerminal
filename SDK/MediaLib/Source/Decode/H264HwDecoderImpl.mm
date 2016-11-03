//
//  H264HwDecoderImpl.m
//  cameratest
//
//  Created by hxf on 12/18/15.
//  Copyright (c) 2015 hxf. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>
#import "H264HwDecoderImpl.h"
#import <VideoToolbox/VideoToolbox.h>
//#include "itzLog.h"

@interface H264HwDecoderImpl()
{
    CMVideoFormatDescriptionRef _videoFormatDescr;
    VTDecompressionSessionRef _encodingSession;
    NSData *_spsData;
    NSData *_ppsData;
}
@end

@implementation H264HwDecoderImpl
@synthesize _error;
@synthesize _delegate;

-(void) initWithMembers
{
    _spsData = nil;
    _ppsData = nil;
}

-(void) start
{
    NSLog(@"start decode h264 data ");
}

-(void) decode:(int)width height:(int)height inData:(const char*)in inDataLen:(int)len
{
    NSLog(@"decode h264 data %d", len);

    if (!in || len<1) {
        return;
    }

    uint8_t* data = (uint8_t*)in;
    int size = len;

#ifdef DEBUG333
    static FILE* fileH264=NULL;

    if (!fileH264) {
        char szBuf[512] = {0};
        int ir = GetDocumentDirectory(szBuf, sizeof(szBuf)-1);
        if (ir > 0) {
            snprintf(szBuf+ir, sizeof(szBuf)-ir-1, "/decode_oc.h264");
            fileH264 = fopen(szBuf, "w");
        }
    }
    if (fileH264) {
        fwrite((const void*)data, size, 1, fileH264);
    }
#endif //#ifdef DEBUG
    OSStatus status;
    // 1. get SPS,PPS form stream data, and create CMFormatDescription and VTDecompressionSession
    if (_spsData == nil || _ppsData == nil) {
//        // test for printf
//        NSString *tmp3 = [NSString new];
//        for(int i = 0; i < size; i++) {
//            NSString *str = [NSString stringWithFormat:@" %.2X",data[i]];
//            tmp3 = [tmp3 stringByAppendingString:str];
//        }
//        NSLog(@"size ---->>%i",size);
//        NSLog(@"%@",tmp3);

        if (_spsData == nil) {
            int startCodeSPSIndex = 0;
            int endCodeIndex = 0;
            int spsLength = 0;

            for (int i = 4; i < size; i++) {
                if (data[i-1]==0x01 && data[i-2]==0x00 && data[i-3]==0x00 && data[i-4]==0x00) {
                    if (startCodeSPSIndex == 0) {
                        if ((data[i]&0x1F) == 7) {
                            startCodeSPSIndex = i;
                        }
                    }
                    else if (endCodeIndex == 0) {
                        endCodeIndex = i;
                    }
                    else {
                        break;
                    }
                }
            }

            if (startCodeSPSIndex != 0) {
                if (endCodeIndex == 0) {
                    spsLength = size-startCodeSPSIndex;
                }
                else {
                    spsLength = endCodeIndex-startCodeSPSIndex-4;
                }

                NSLog(@"NALU with Type \"%@\" received.", naluTypesStrings[7]);
                _spsData = [NSData dataWithBytes:&(data[startCodeSPSIndex]) length: spsLength];
            }
        }

        if (_ppsData == nil) {
            int startCodePPSIndex = 0;
            int endCodeIndex = 0;
            int ppsLength = 0;

            for (int i = 4; i < size; i++) {
                if (data[i-1]==0x01 && data[i-2]==0x00 && data[i-3]==0x00 && data[i-4]==0x00) {
                    if (startCodePPSIndex == 0) {
                        if ((data[i]&0x1F) == 8) {
                            startCodePPSIndex = i;
                        }
                    }
                    else if (endCodeIndex == 0) {
                        endCodeIndex = i;
                    }
                    else {
                        break;
                    }
                }
            }

            if (startCodePPSIndex != 0) {
                if (endCodeIndex == 0) {
                    ppsLength = size-startCodePPSIndex;
                }
                else {
                    ppsLength = endCodeIndex-startCodePPSIndex-4;
                }

                NSLog(@"NALU with Type \"%@\" received.", naluTypesStrings[8]);
                _ppsData = [NSData dataWithBytes:&(data[startCodePPSIndex]) length: ppsLength];
            }
        }

        if (_spsData!=nil && _ppsData!=nil) {
            // 2. create  CMFormatDescription
            const uint8_t* const parameterSetPointers[2] = { (const uint8_t*)[_spsData bytes], (const uint8_t*)[_ppsData bytes] };
            const size_t parameterSetSizes[2] = { [_spsData length], [_ppsData length] };
            status = CMVideoFormatDescriptionCreateFromH264ParameterSets(kCFAllocatorDefault, 2, parameterSetPointers, parameterSetSizes, 4, &_videoFormatDescr);

            // 3. create VTDecompressionSession
            VTDecompressionOutputCallbackRecord callback;
            callback.decompressionOutputCallback = didDecompress;
            callback.decompressionOutputRefCon = (__bridge void *)self;

//            NSDictionary *destinationImageBufferAttributes =[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO],(id)kCVPixelBufferOpenGLESCompatibilityKey,[NSNumber numberWithInt:kCVPixelFormatType_32BGRA],(id)kCVPixelBufferPixelFormatTypeKey,nil];
//
//            CFDictionaryRef attrs = (__bridge CFDictionaryRef)destinationImageBufferAttributes;

            CFDictionaryRef attrs = NULL;
            const void *keys[] = { kCVPixelBufferPixelFormatTypeKey };
            //      kCVPixelFormatType_420YpCbCr8Planar is YUV420
            //      kCVPixelFormatType_420YpCbCr8BiPlanarFullRange is NV12
            uint32_t v = kCVPixelFormatType_420YpCbCr8BiPlanarFullRange;
            const void *values[] = { CFNumberCreate(NULL, kCFNumberSInt32Type, &v) };
            attrs = CFDictionaryCreate(NULL, keys, values, 1, NULL, NULL);

            status = VTDecompressionSessionCreate(kCFAllocatorDefault, _videoFormatDescr, NULL, attrs, &callback, &_encodingSession);
            
//            int32_t timeSpan = 90000;
//            CMSampleTimingInfo timingInfo;
//            timingInfo.presentationTimeStamp = CMTimeMake(0, timeSpan);
//            timingInfo.duration =  CMTimeMake(3000, timeSpan);
//            timingInfo.decodeTimeStamp = kCMTimeInvalid;
        }
        else {
            return;
        }
    }

    int nalu_type = 0;
    for (int i = 0; i < size; i++) {
        if (i >= 4) {
            if (data[i-1] == 0x01 && data[i-2] == 0x00 && data[i-3] == 0x00 && data[i-4] == 0x00) {
                nalu_type = (data[i] & 0x1F);
                if (nalu_type == 7
                    || nalu_type == 8) {
                    continue;
                }
                break;
            }
        }
    }

    if (nalu_type == 1 || nalu_type == 5) {
        uint8_t *videoBlockBuf = new uint8_t[len];
        if (videoBlockBuf) {
            memcpy(videoBlockBuf, in, len);
        }
        else {
            return;
        }

        // 4. get NALUnit payload into a CMBlockBuffer,
        CMBlockBufferRef videoBlock = NULL;
        status = CMBlockBufferCreateWithMemoryBlock(NULL, videoBlockBuf, len, kCFAllocatorNull, NULL, 0, len, 0, &videoBlock);

        // 5.  making sure to replace the separator code with a 4 byte length code (the length of the NalUnit including the unit code)
        CMItemCount numSamples = 0;
//        static const size_t maxNumSamples = 16;
//        size_t sampleSizeArray[maxNumSamples] = {0};
        uint8_t* leftdata = videoBlockBuf;
        int leftlen = size;
        while (leftlen) {
            int non_spspps_nalu_len = leftlen;
            for (int i=4+4; i<leftlen; i++) {
                if (leftdata[i-1] == 0x01 && leftdata[i-2] == 0x00 && leftdata[i-3] == 0x00 && leftdata[i-4] == 0x00) {
                    non_spspps_nalu_len = i-4;
                    break;
                }
            }

            int reomveHeaderSize = non_spspps_nalu_len-4;
            const uint8_t sourceBytes[] = {(uint8_t)(reomveHeaderSize >> 24), (uint8_t)(reomveHeaderSize >> 16), (uint8_t)(reomveHeaderSize >> 8), (uint8_t)reomveHeaderSize};
            status = CMBlockBufferReplaceDataBytes(sourceBytes, videoBlock, leftdata-videoBlockBuf, 4);
//            sampleSizeArray[numSamples] = non_spspps_nalu_len;
            numSamples++;
//            if (numSamples+1 == maxNumSamples) {
//                break;
//            }

            leftdata += non_spspps_nalu_len;
            leftlen -= non_spspps_nalu_len;
        }
        
//        NSString *tmp3 = [NSString new];
//        for(int i = 0; i < sizeof(sourceBytes); i++) {
//            NSString *str = [NSString stringWithFormat:@" %.2X",sourceBytes[i]];
//            tmp3 = [tmp3 stringByAppendingString:str];
//        }

        // 6. create a CMSampleBuffer.
        CMSampleBufferRef sbRef = NULL;
        size_t sampleSizeArray[] = {(size_t)size};
        status = CMSampleBufferCreate(kCFAllocatorDefault, videoBlock, true, NULL, NULL, _videoFormatDescr, 1, 0, NULL, 1, sampleSizeArray, &sbRef);

        // 7. use VTDecompressionSessionDecodeFrame
        VTDecodeFrameFlags flags = kVTDecodeFrame_EnableAsynchronousDecompression;
        VTDecodeInfoFlags flagOut;
        status = VTDecompressionSessionDecodeFrame(_encodingSession, sbRef, flags, &sbRef, &flagOut);
        CFRelease(sbRef);
        CFRelease(videoBlock);
        delete [] videoBlockBuf;
    }
}


-(void) stop
{
    if (_encodingSession) {
        VTDecompressionSessionInvalidate(_encodingSession);
        CFRelease(_encodingSession);
        _encodingSession = nil;
    }
}

#pragma mark - VideoToolBox Decompress Frame CallBack
/*
 This callback gets called everytime the decompresssion session decodes a frame
 */
void didDecompress( void *decompressionOutputRefCon, void *sourceFrameRefCon, OSStatus status, VTDecodeInfoFlags infoFlags, CVImageBufferRef imageBuffer, CMTime presentationTimeStamp, CMTime presentationDuration )
{
    if (status != noErr || !imageBuffer) {//kVTVideoDecoderBadDataErr
        NSLog(@"Error decompresssing frame at time: %.3f error: %d infoFlags: %u", (float)presentationTimeStamp.value/presentationTimeStamp.timescale, (int)status, (unsigned int)infoFlags);
        return;
    }
    
    // Get video decoder object pointer
    H264HwDecoderImpl* decoder = (__bridge H264HwDecoderImpl*)decompressionOutputRefCon;
    
    // Video decompressd data callback
    if (decoder && decoder._delegate) {
        [decoder._delegate gotDecodedData:imageBuffer];
    }
}

NSString * const naluTypesStrings[] = {
    @"Unspecified (non-VCL)",
    @"Coded slice of a non-IDR picture (VCL)",
    @"Coded slice data partition A (VCL)",
    @"Coded slice data partition B (VCL)",
    @"Coded slice data partition C (VCL)",
    @"Coded slice of an IDR picture (VCL)",
    @"Supplemental enhancement information (SEI) (non-VCL)",
    @"Sequence parameter set (non-VCL)",
    @"Picture parameter set (non-VCL)",
    @"Access unit delimiter (non-VCL)",
    @"End of sequence (non-VCL)",
    @"End of stream (non-VCL)",
    @"Filler data (non-VCL)",
    @"Sequence parameter set extension (non-VCL)",
    @"Prefix NAL unit (non-VCL)",
    @"Subset sequence parameter set (non-VCL)",
    @"Reserved (non-VCL)",
    @"Reserved (non-VCL)",
    @"Reserved (non-VCL)",
    @"Coded slice of an auxiliary coded picture without partitioning (non-VCL)",
    @"Coded slice extension (non-VCL)",
    @"Coded slice extension for depth view components (non-VCL)",
    @"Reserved (non-VCL)",
    @"Reserved (non-VCL)",
    @"Unspecified (non-VCL)",
    @"Unspecified (non-VCL)",
    @"Unspecified (non-VCL)",
    @"Unspecified (non-VCL)",
    @"Unspecified (non-VCL)",
    @"Unspecified (non-VCL)",
    @"Unspecified (non-VCL)",
    @"Unspecified (non-VCL)",
};


@end