//
//  H264HwEncoderImpl.h
//  h264v1
//
//  Created by hxf on 12/18/15.
//  Copyright (c) 2015 hxf. All rights reserved.
//
#ifndef H264HwEncoderImpl_h
#define H264HwEncoderImpl_h

#import <CoreMedia/CoreMedia.h>

@protocol H264HwEncoderImplDelegate <NSObject>

- (void)gotSpsPps:(NSData*)sps pps:(NSData*)pps;
- (void)gotEncodedData:(NSData*)data isKeyFrame:(BOOL)isKeyFrame;

@end

@interface H264HwEncoderImpl : NSObject 

- (void) initWithMembers;
- (void) start:(int)width  height:(int)height frameduration:(CMTime)frameduration bitrate:(int)bitrate;
- (void) changeResolution:(int)width  height:(int)height;
- (void) forceKeyFrame;
- (void) setBitrate_nq:(int)bitrate;//unit kbits/s
- (void) setBitrate:(int)bitrate;//unit kbits/s
- (void) encode:(CVImageBufferRef )pixelBuffer;
- (void) stop;


@property (weak, nonatomic) NSString *_error;
@property (strong, nonatomic) id<H264HwEncoderImplDelegate> _delegate;

@end

#endif /*H264HwEncoderImpl_h*/