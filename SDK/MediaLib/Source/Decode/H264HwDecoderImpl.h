//
//  H264HwDecoderImpl.h
//  cameratest
//
//  Created by hxf on 12/18/15.
//  Copyright (c) 2015 hxf. All rights reserved.
//

#ifndef H264HwDecoderImpl_h
#define H264HwDecoderImpl_h

#import <CoreMedia/CoreMedia.h>

@protocol H264HwDecoderImplDelegate <NSObject>
@optional

-(void) gotDecodedData:(CVImageBufferRef) imageBuffer;

@end

@interface H264HwDecoderImpl : NSObject

-(void) initWithMembers;
-(void) start;
-(void) decode:(int)width height:(int)height inData:(const char*)in inDataLen:(int)len;
-(void) stop;

@property (weak, nonatomic) NSString *_error;
@property (strong, nonatomic) id<H264HwDecoderImplDelegate> _delegate;

@end

#endif /* H264HwDecoderImpl_h */
