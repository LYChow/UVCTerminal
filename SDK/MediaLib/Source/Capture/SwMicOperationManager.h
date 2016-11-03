//
//  SwMicOperationManager.h
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/30.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#ifndef SwMicOperationManager_h
#define SwMicOperationManager_h

#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

//Mic状态
enum KMicSate
{
    KMicSateNormal=0,//Mic正常
    KMicSateDisable//Mic不可用
};

@protocol SwMicControllerDelegate <NSObject>
- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection;//捕获一帧图像
@end

@interface SwMicOperationManager : NSObject <AVCaptureAudioDataOutputSampleBufferDelegate/*, H264HwEncoderImplDelegate*/>
{
    AVCaptureDevice *_Mic;
    AVCaptureSession *_captureSession;
    BOOL _deviceAuthorized;//Mic是否授权
    AVCaptureDeviceInput* _oldInput;
    AVCaptureOutput* _oldOutput;
}
@property(nonatomic,assign)enum KMicSate MicSate;
@property(nonatomic,strong)id<SwMicControllerDelegate> MicDelegate;//代理

//初始化Mic
-(void)initializeMic;
-(void)finitializeMic;
//初始化AVCaptureSession
-(void)SetCaptureSession;

@end


#endif /* SwMicOperationManager_h */
