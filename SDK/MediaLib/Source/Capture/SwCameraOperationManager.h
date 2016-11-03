//
//  SwCameraOperationManager.h
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/30.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#ifndef SwCameraOperationManager_h
#define SwCameraOperationManager_h

#import <AVFoundation/AVFoundation.h>
//#import "H264HwEncoderImpl.h"
#import <UIKit/UIKit.h>

//摄像头
enum KCameraChose
{
    KCameraFront=0,//副
    KCameraBack//主
};

//相机状态
enum KCameraSate
{
    KCameraSateNormal=0,//相机正常
    KCameraSateDisable//相机不可用
};

//闪光灯
enum KFlashLightState
{
    KFlashLightOff=0,//关闭
    KFlashLightOpen,//开启
    KFlashLightAuto,//自动
    KNoFlashLight//不可用
};

@protocol SwCameraControllerDelegate <NSObject>
- (void)didFinishPick_Image:(UIImage *)image;//捕获一张静态图像
- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection;//捕获一帧图像
@end

@interface SwCameraOperationManager : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate/*, H264HwEncoderImplDelegate*/>
{
    AVCaptureDevice *_frontCamera;//前置摄像头®
    AVCaptureDevice *_backCamera;//后置摄像头
    AVCaptureSession *_captureSession;
    AVCaptureVideoPreviewLayer *_captureVideoPreviewLayer;//采集显示
    AVCaptureStillImageOutput *_stillImageOutput;//静态图片输出
    BOOL _deviceAuthorized;//相机是否授权
    int mWidth;
    int mHeight;
    CMTime mframeDuration;

    AVCaptureDeviceInput* _oldinput;//记录切换摄像头前输入端
    AVCaptureVideoDataOutput* _oldvideodataoutput;//记录切换摄像头前的视频输出端
    AVCaptureStillImageOutput* _oldstillimageoutput;//记录切换摄像头前静态图片输出端

    AVCaptureConnection* _AVCaptureConnection;
}
@property(nonatomic,assign)enum KFlashLightState FlashLightState;
@property(nonatomic,assign)enum KCameraSate CameraSate;
@property(nonatomic,assign)enum KCameraChose CameraChose;
@property(nonatomic,strong)id<SwCameraControllerDelegate> CameraDelegate;//代理
//@property(nonatomic,strong)UIImage *GetPicture;//相机获取的图片

//初始化camera
-(void)initializeCameraWithPreview:(UIImageView *)preview captureWidth:(int)width captureHeight:(int)height frameDuration:(CMTime)frameDuration backCamera:(bool)back;
//设置帧率
-(void)SetMinCaptureDuration:(CMTime)frameDuration;
//切换摄像头
-(void)SwitchCamera;
//设置闪光灯
-(void)setFlashLightState:(enum KFlashLightState)FlashLightState;
//初始化AVCaptureSession
-(void)SetCaptureSessionWithCamera:(enum KCameraChose)cameraChose;
//捕捉图片
- (void)CaptureImage;
//保存到相册
-(void)SavePictureToLibraryWithImage:(UIImage *)image;
- (void)StopCaptureImage;

@end


#endif /* SwCameraOperationManager_h */
