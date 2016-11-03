//
//  H264HwEncoderImpl.m
//  h264v1
//
//  Created by hxf on 12/18/15.
//  Copyright (c) 2015 hxf. All rights reserved.
//

#import "H264HwEncoderImpl.h"

@import VideoToolbox;
@import Foundation;

@interface  H264HwEncoderImpl ()
{
    VTCompressionSessionRef _encodingSession;
    dispatch_queue_t _aQueue;
    BOOL _forceKeyframe;
    int  _forceKeyframeAfterFrames;
    CMTime  _frameDuration;
    int  _frameCount;
    int  _bitRate; //unit bit/s
    NSData *_sps;
    NSData *_pps;
}
@end

@implementation H264HwEncoderImpl
@synthesize _error;
@synthesize _delegate;

- (void) initWithMembers
{
    _encodingSession = nil;
    _aQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    _forceKeyframe = false;
    _forceKeyframeAfterFrames = 0;
    _frameDuration = CMTimeMake(1, 30);
    _frameCount = 0;
    _bitRate = 0;
    _sps = NULL;
    _pps = NULL; 
}

- (void) start:(int)width  height:(int)height frameduration:(CMTime)frameduration bitrate:(int)bitrate
{
    _frameDuration = frameduration;

    dispatch_sync(_aQueue, ^{
        
        // For testing out the logic, lets read from a file and then send it to encoder to create h264 stream
        
        // Create the compression session
        OSStatus status = VTCompressionSessionCreate(NULL, width, height, kCMVideoCodecType_H264, NULL, NULL, NULL, didCompressH264, (__bridge void *)(self),  &_encodingSession);
        NSLog(@"H264: VTCompressionSessionCreate %d", (int)status);
        
        if (status != 0)
        {
            NSLog(@"H264: Unable to create a H264 session");
            _error = @"H264: Unable to create a H264 session";
            return ;
        }

        // Set the properties
        status = VTSessionSetProperty(_encodingSession, kVTCompressionPropertyKey_RealTime, kCFBooleanTrue);
        status = VTSessionSetProperty(_encodingSession, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_High_AutoLevel);
        //status = VTSessionSetProperty(_EncodingSession, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_Main_AutoLevel);
        if (status != 0)
        {
            NSLog(@"1");
            return;
        }
        
        // Set key frame interval
        int keyInterval = 20;
        CFNumberRef refKeyInterval = CFNumberCreate(NULL, kCFNumberSInt32Type, &keyInterval);
        status = VTSessionSetProperty(_encodingSession, kVTCompressionPropertyKey_MaxKeyFrameInterval, refKeyInterval);
        CFRelease(refKeyInterval);
        
        // Set h264 level
        status = VTSessionSetProperty(_encodingSession, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_High_AutoLevel);
        if (status != 0)
        {
            NSLog(@"2");
            return;
        }
        
        // Set bitrate
        [self setBitrate_nq :bitrate];

        // Tell the encoder to start encoding
        VTCompressionSessionPrepareToEncodeFrames(_encodingSession);
    });
}

- (void) setBitrate_nq:(int)bitrate
{
    if(bitrate*1024 == _bitRate) {
        return;
    }
  
    _bitRate = bitrate*1024;
    NSLog(@"------------------- set bitrate to %d ---------------", bitrate);
    
    if(_encodingSession) {
        // m_encodeMutex.lock();
        
        int v = _bitRate ;
        CFNumberRef ref = CFNumberCreate(NULL, kCFNumberSInt32Type, &v);
        OSStatus ret = VTSessionSetProperty((VTCompressionSessionRef)_encodingSession, kVTCompressionPropertyKey_AverageBitRate, ref);
        if(ret != noErr) {
            NSLog(@"H264Encode::setBitrate Error setting bitrate! %d", (int) ret);
        }
        CFRelease(ref);
        
        ret = VTSessionCopyProperty((VTCompressionSessionRef)_encodingSession, kVTCompressionPropertyKey_AverageBitRate, kCFAllocatorDefault, &ref);
        if(ret == noErr && ref) {
            SInt32 br = 0;
            
            CFNumberGetValue(ref, kCFNumberSInt32Type, &br);
            
            _bitRate = br;
            CFRelease(ref);
        } else {
            _bitRate = v;
        }
        
        v = _bitRate / 8;
        CFNumberRef bytes = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &v);
        v = 1;
        CFNumberRef duration = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &v);
        
        CFMutableArrayRef limit = CFArrayCreateMutable(kCFAllocatorDefault, 2, &kCFTypeArrayCallBacks);
        
        CFArrayAppendValue(limit, bytes);
        CFArrayAppendValue(limit, duration);
        
        VTSessionSetProperty((VTCompressionSessionRef)_encodingSession, kVTCompressionPropertyKey_DataRateLimits, limit);
        CFRelease(bytes);
        CFRelease(duration);
        CFRelease(limit);
        
        //m_encodeMutex.unlock();
    }
}

- (void) setBitrate:(int)bitrate
{
    dispatch_sync(_aQueue, ^{
        // Set bitrate
        [self setBitrate_nq:bitrate];
    });
}

- (void) encode:(CVImageBufferRef )pixelBuffer
{
//    // test for force key frame
//    static int encode_count = 0;
//    if (encode_count > 200 && encode_count%10 == 0)
//    {
//        [self forceKeyFrame];
//    }
//    encode_count++;
    
    dispatch_sync(_aQueue, ^{
        
        _frameCount++;
        // Get the CV Image buffer
        CVImageBufferRef imageBuffer = pixelBuffer;// (CVImageBufferRef)CMSampleBufferGetImageBuffer(sampleBuffer);
        
        // Create properties
        CMTime presentationTimeStamp = CMTimeMake(_frameCount, 1000);
        VTEncodeInfoFlags flags;

        NSDictionary *properties = nil;
        if(_forceKeyframeAfterFrames >= 0) {
            
            if (_forceKeyframeAfterFrames <= 0 && _forceKeyframe) {
                _forceKeyframe = false;
                properties = @{(__bridge NSString *)kVTEncodeFrameOptionKey_ForceKeyFrame: @YES};
            } else {
                _forceKeyframeAfterFrames--;
            }
        }
        
        // Pass it to the encoder
        OSStatus statusCode = VTCompressionSessionEncodeFrame(_encodingSession,
                                                              imageBuffer,
                                                              presentationTimeStamp,
                                                              _frameDuration,//kCMTimeInvalid,
                                                              (__bridge CFDictionaryRef)properties,
                                                              NULL,
                                                              &flags);
        // Check for error
        if (statusCode != noErr) {
            NSLog(@"H264: VTCompressionSessionEncodeFrame failed with %d", (int)statusCode);
            _error = @"H264: VTCompressionSessionEncodeFrame failed ";
            
            // End the session
            VTCompressionSessionInvalidate(_encodingSession);
            CFRelease(_encodingSession);
            _encodingSession = NULL;
            _error = NULL;
            return;
        }
       // NSLog(@"H264: VTCompressionSessionEncodeFrame Success");
    });
}

- (void) changeResolution:(int)width  height:(int)height
{
}

- (void) forceKeyFrame
{
    _forceKeyframeAfterFrames = 3;
    _forceKeyframe = true;
}

- (void) stop
{
    if (_encodingSession)
    {
        // Mark the completion
        VTCompressionSessionCompleteFrames(_encodingSession, kCMTimeInvalid);
        
        // End the session
        VTCompressionSessionInvalidate(_encodingSession);
        CFRelease(_encodingSession);
        _encodingSession = nil;
        _error = nil;
    }
}

void didCompressH264(void *outputCallbackRefCon, void *sourceFrameRefCon, OSStatus status, VTEncodeInfoFlags infoFlags,
                     CMSampleBufferRef sampleBuffer )
{
    //NSLog(@"didCompressH264 called with status %d infoFlags %d", (int)status, (int)infoFlags);
    if (status != 0) return;
    
    if (!CMSampleBufferDataIsReady(sampleBuffer))
    {
        NSLog(@"didCompressH264 data is not ready ");
        return;
    }
    H264HwEncoderImpl* encoder = (__bridge H264HwEncoderImpl*)outputCallbackRefCon;
    
    // Check if we have got a key frame first
    bool keyframe = !CFDictionaryContainsKey( (CFArrayGetValueAtIndex(CMSampleBufferGetSampleAttachmentsArray(sampleBuffer, true), 0)), kCMSampleAttachmentKey_NotSync);
    
    if (keyframe)
    {
        CMFormatDescriptionRef format = CMSampleBufferGetFormatDescription(sampleBuffer);
        // CFDictionaryRef extensionDict = CMFormatDescriptionGetExtensions(format);
        // Get the extensions
        // From the extensions get the dictionary with key "SampleDescriptionExtensionAtoms"
        // From the dict, get the value for the key "avcC"
        
        size_t sparameterSetSize, sparameterSetCount;
        const uint8_t *sparameterSet;
        OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 0, &sparameterSet, &sparameterSetSize, &sparameterSetCount, 0 );
        if (statusCode == noErr)
        {
            // Found sps and now check for pps
            size_t pparameterSetSize, pparameterSetCount;
            const uint8_t *pparameterSet;
            OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 1, &pparameterSet, &pparameterSetSize, &pparameterSetCount, 0 );
            if (statusCode == noErr)
            {
                // Found pps
                encoder->_sps = [NSData dataWithBytes:sparameterSet length:sparameterSetSize];
                encoder->_pps = [NSData dataWithBytes:pparameterSet length:pparameterSetSize];
                if (encoder._delegate)
                {
                    [encoder._delegate gotSpsPps:encoder->_sps pps:encoder->_pps];
                }
            }
        }
    }
    
    CMBlockBufferRef dataBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
    size_t length, totalLength;
    char *dataPointer;
    OSStatus statusCodeRet = CMBlockBufferGetDataPointer(dataBuffer, 0, &length, &totalLength, &dataPointer);
    if (statusCodeRet == noErr) {
        
        size_t bufferOffset = 0;
        static const int AVCCHeaderLength = 4;
        while (bufferOffset < totalLength - AVCCHeaderLength) {
            
            // Read the NAL unit length
            uint32_t NALUnitLength = 0;
            memcpy(&NALUnitLength, dataPointer + bufferOffset, AVCCHeaderLength);
            
            // Convert the length value from Big-endian to Little-endian
            NALUnitLength = CFSwapInt32BigToHost(NALUnitLength);
            
            NSData* data = [[NSData alloc] initWithBytes:(dataPointer + bufferOffset + AVCCHeaderLength) length:NALUnitLength];
            if (encoder._delegate) {
                [encoder._delegate gotEncodedData:data isKeyFrame:keyframe];
            }
            
            // Move to the next NAL unit in the block buffer
            bufferOffset += AVCCHeaderLength + NALUnitLength;
        }
        
    }
    
}


@end
