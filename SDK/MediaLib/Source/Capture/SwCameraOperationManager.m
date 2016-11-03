//
//  SwCameraOperationManager.m
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/30.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#import "SwCameraOperationManager.h"
#import <AssetsLibrary/AssetsLibrary.h>

//#import "H264HwEncoderImpl.h"
//@interface IH264HwEncoderImplDelegate2 : NSObject <H264HwEncoderImplDelegate>
//{
//    //@public CH264HwEncoderImpl* _cpp_delegate;
//}
//
//@end
//
//@implementation IH264HwEncoderImplDelegate2
//
//- (void)gotSpsPps:(NSData*)sps pps:(NSData*)pps
//{
//    NSLog(@"gotSpsPps %d %d", (int)[sps length], (int)[pps length]);
//    //    if (_cpp_delegate) {
//    //        _cpp_delegate->OnSpsPpsGot();
//    //    }
//}
//
//- (void)gotEncodedData:(NSData*)data isKeyFrame:(BOOL)isKeyFrame
//{
//    if (isKeyFrame) {
//        NSLog(@"gotEncodedData %d [keyFrame]", (int)[data length]);
//    }
//    else {
//        NSLog(@"gotEncodedData %d", (int)[data length]);
//    }
////    if (_cpp_delegate) {
//    //        _cpp_delegate->OnEncodedDataGot();
//    //    }
//}
//
//@end

@implementation SwCameraOperationManager
#pragma mark--初始化等操作
-(id)init
{
    self=[super init];
    if (self) {
        //检查授权
        [self checkDeviceAuthorizationStatus];
    }

    return self;
}
-(void)dealloc
{
}

-(void)setCameraChose:(enum KCameraChose)CameraChose
{
    if (CameraChose!=_CameraChose) {
        _CameraChose=CameraChose;
        //检查闪光灯
        [self CheckflashLightWithCamera:[self ChoseCameraWith:CameraChose]];
    }
}

-(AVCaptureDevice *)ChoseCameraWith:(enum KCameraChose)CameraChose
{
    switch (CameraChose) {
        case KCameraBack:
            return _backCamera;
            break;
        case KCameraFront:
            return _frontCamera;
            break;
        default:
            break;
    }
}

//初始化camera
-(void)initializeCameraWithPreview:(UIImageView *)preview captureWidth:(int)width captureHeight:(int)height frameDuration:(CMTime)frameDuration backCamera:(bool)back
{
    if (_deviceAuthorized) {
        if (!_captureSession){
            _captureSession = [[AVCaptureSession alloc] init];
        }
        if (preview) {
            if (!_captureVideoPreviewLayer){
                _captureVideoPreviewLayer = [[AVCaptureVideoPreviewLayer alloc] initWithSession:_captureSession];
            }
            [_captureVideoPreviewLayer setVideoGravity:AVLayerVideoGravityResizeAspectFill];
            _captureVideoPreviewLayer.frame = preview.bounds;
            [preview.layer addSublayer:_captureVideoPreviewLayer];
        }
        //获取硬件采集设备
        NSArray *devices = [AVCaptureDevice devices];

        if (devices.count==0) {
            [self ShowAlertToUserWithTitle:@"抱歉" Message:@"相机设备不可用"];
            self.CameraSate=KCameraSateDisable;
            return;
        }

        for (AVCaptureDevice *device in devices) {

            if ([device hasMediaType:AVMediaTypeVideo]) {

                if ([device position] == AVCaptureDevicePositionBack) {
                    _backCamera = device;
                }
                else {

                    _frontCamera = device;
                }
            }
        }
        mWidth  = width;
        mHeight = height;
        mframeDuration = frameDuration;

        if (back) { //主摄像头
            self.CameraChose=KCameraBack;
            [self CheckflashLightWithCamera:_backCamera];
        }
        else { //前置摄像头
            self.CameraChose=KCameraFront;
            [self CheckflashLightWithCamera:_frontCamera];
        }
        [self SetCaptureSessionWithCamera:self.CameraChose];
    }

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(statusBarOrientationChange:) name:UIApplicationDidChangeStatusBarOrientationNotification object:nil];
}

- (void)statusBarOrientationChange:(NSNotification *)notification
{
    if (_AVCaptureConnection) {
        UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
        AVCaptureVideoOrientation _avCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;

        switch (orientation) {
            case UIInterfaceOrientationPortrait:
                _avCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
                break;
            case UIInterfaceOrientationPortraitUpsideDown:
                _avCaptureVideoOrientation = AVCaptureVideoOrientationPortraitUpsideDown;
                break;
            case UIInterfaceOrientationLandscapeLeft:
                _avCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
                break;
            case UIInterfaceOrientationLandscapeRight:
                _avCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeRight;
                break;

            default:
                break;
        }

        [_AVCaptureConnection setVideoOrientation:_avCaptureVideoOrientation];
    }
}

//设置帧率
-(void)SetMinCaptureDuration:(CMTime)frameDuration
{
    mframeDuration = frameDuration;
    if (_AVCaptureConnection) {
        // 设置采集帧率
        _AVCaptureConnection.videoMinFrameDuration = mframeDuration;
    }
}

//切换摄像头
-(void)SwitchCamera
{
    if (!_captureSession) {
        NSLog(@"SwitchCamera failed, _captureSession not inited.");
        return;
    }
    [self StopCaptureImage];
    if (self.CameraChose == KCameraBack) {
        self.CameraChose = KCameraFront;
    }
    else {
        self.CameraChose = KCameraBack;
    }
    [self SetCaptureSessionWithCamera:self.CameraChose];
}

//检查闪光灯
-(void)CheckflashLightWithCamera:(AVCaptureDevice *)camera
{
    if ([camera hasFlash]){
        [camera lockForConfiguration:nil];//加锁
//        self.FlashLightState=KFlashLightOpen;
        [self setFlashLightState:KFlashLightOpen];
        [camera unlockForConfiguration];
    }
    else{
        self.FlashLightState=KNoFlashLight;
    }
}

//检查用户相机授权
- (void)checkDeviceAuthorizationStatus
{
    if ([AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo]==AVAuthorizationStatusDenied) {

        [self ShowAlertToUserWithTitle:@"无法启动相机" Message:@"\n请前往“设置-隐私-相机”允许应用使用相机"];
        _deviceAuthorized=NO;
    }
    else{
        _deviceAuthorized=YES;
    }
}

//设置闪光灯
-(void)setFlashLightState:(enum KFlashLightState)FlashLightState
{
    AVCaptureDevice *current_camera=[self ChoseCameraWith:self.CameraChose];
    _FlashLightState=FlashLightState;
    //设置闪光灯
    if (FlashLightState!=KNoFlashLight) {
        switch (FlashLightState) {
            case KFlashLightOpen:
                [current_camera lockForConfiguration:nil];
                [current_camera setFlashMode:AVCaptureFlashModeOn];
                [current_camera unlockForConfiguration];
                break;
            case KFlashLightOff:
                [current_camera lockForConfiguration:nil];
                [current_camera setFlashMode:AVCaptureFlashModeOff];
                [current_camera unlockForConfiguration];
                break;
            case KFlashLightAuto:
                [current_camera lockForConfiguration:nil];
                [current_camera setFlashMode:AVCaptureFlashModeAuto];
                [current_camera unlockForConfiguration];
                break;
            default:
                break;
        }
    }
}

//设置AVCaptureSession以及输入输出端
-(void)SetCaptureSessionWithCamera:(enum KCameraChose)cameraChose
{
    AVCaptureDevice *current_camera=[self ChoseCameraWith:cameraChose];

    NSError *error = nil;

//    CMTime frameDuration = CMTimeMake(1, 5);
//    NSArray *supportedFrameRateRanges = [current_camera.activeFormat videoSupportedFrameRateRanges];
//    BOOL frameRateSupported = NO;
//    for (AVFrameRateRange *range in supportedFrameRateRanges) {
//        if (CMTIME_COMPARE_INLINE(frameDuration, >=, range.minFrameDuration) &&
//            CMTIME_COMPARE_INLINE(frameDuration, <=, range.maxFrameDuration)) {
//            frameRateSupported = YES;
//        }
//    }
//
//    if (frameRateSupported && [current_camera lockForConfiguration:&error]) {
//        [current_camera setActiveVideoMaxFrameDuration:frameDuration];
//        [current_camera setActiveVideoMinFrameDuration:frameDuration];
//        [current_camera unlockForConfiguration];
//    }

    //设置捕捉照片
    _captureSession.sessionPreset = AVCaptureSessionPresetPhoto;
    //添加输入设备到当前session
    error = nil;
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:current_camera error:&error];
    if (!input) {
        [self ShowAlertToUserWithTitle:@"提示" Message:@"无法连接到相机"];
    }
    //首次添加输入端口
    if (_captureSession.inputs.count>0)
    {
        //清除输入端口
        [_captureSession removeInput:_oldinput];
    }
    [_captureSession addInput:input];
    _oldinput=input;

    //设置实时视频输出
    ////////////////
    // Currently, the only supported key is kCVPixelBufferPixelFormatTypeKey. Recommended pixel format choices are
    // kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange or kCVPixelFormatType_32BGRA.
    // On iPhone 3G, the recommended pixel format choices are kCVPixelFormatType_422YpCbCr8 or kCVPixelFormatType_32BGRA.
    // When using libyuv kCVPixelFormatType_32BGRA is faster
    AVCaptureVideoDataOutput *avCaptureVideoDataOutput = [[AVCaptureVideoDataOutput alloc] init];
    NSDictionary *settings = [[NSDictionary alloc] initWithObjectsAndKeys:
                              [NSNumber numberWithUnsignedInt:kCVPixelFormatType_420YpCbCr8BiPlanarFullRange], kCVPixelBufferPixelFormatTypeKey,
                              [NSNumber numberWithInt: mWidth], (id)kCVPixelBufferWidthKey,
                              [NSNumber numberWithInt: mHeight], (id)kCVPixelBufferHeightKey,
                              nil];
    avCaptureVideoDataOutput.videoSettings = settings;
    //[settings release];

    dispatch_queue_t queue = dispatch_queue_create("com.sinowave.sdk.producer.video.captureoutput", NULL);
    [avCaptureVideoDataOutput setSampleBufferDelegate:self queue:queue];
    if (_captureSession.outputs.count>0)
    {
        //清除输入端口
        [_captureSession removeOutput:_oldvideodataoutput];
    }
    [_captureSession addOutput:avCaptureVideoDataOutput];
    _oldvideodataoutput = avCaptureVideoDataOutput;

    // preset should be set after "addInput" and "addOutput"
    int videoBytes = mHeight * mWidth;
    NSString *sessionPreset = AVCaptureSessionPresetLow;
    if (videoBytes >= (352 * 288)) {
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_5_0
        sessionPreset = AVCaptureSessionPreset352x288;
#else
        sessionPreset = AVCaptureSessionPresetMedium;
#endif
    }
    if (videoBytes >= (640 * 480)) {
        sessionPreset = AVCaptureSessionPreset640x480;
    }
    if (videoBytes >= (1280 * 720) || videoBytes >= (1024 * 768)) { // XGA: GE preferred size
        sessionPreset = AVCaptureSessionPreset1280x720;
    }
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_5_0
    if (videoBytes >= (1920 * 1080)) {
        sessionPreset = AVCaptureSessionPreset1920x1080;
    }
#endif
    if ([current_camera supportsAVCaptureSessionPreset:sessionPreset]) {
        NSLog(@"For output video(%dx%d) we selected %s preset", mWidth, mHeight, [sessionPreset UTF8String]);
        _captureSession.sessionPreset = sessionPreset;
    }
    else {
        NSLog(@"%s not supported as preset, fallback to %s", [sessionPreset UTF8String], [AVCaptureSessionPresetMedium UTF8String]);
        _captureSession.sessionPreset = AVCaptureSessionPresetMedium;
    }

    //遍历连接 查找video采集端口
    for (AVCaptureConnection *connection in avCaptureVideoDataOutput.connections) {
        //遍历输入端口
        for (AVCaptureInputPort *port in [connection inputPorts]) {

            if ([[port mediaType] isEqual:AVMediaTypeVideo] ) {
                _AVCaptureConnection = connection;
                break;
            }
        }
        //找到video端口后停止遍历
        if (_AVCaptureConnection) {
            break;
        }
    }
    if (_AVCaptureConnection) {
        // 设置采集帧率
        _AVCaptureConnection.videoMinFrameDuration = mframeDuration;

        // 设置图像旋转
        UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
        AVCaptureVideoOrientation _avCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;

        //if (cameraChose == KCameraFront) {
        switch (orientation) {
            case UIInterfaceOrientationPortrait:
                _avCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
                break;
            case UIInterfaceOrientationPortraitUpsideDown:
                _avCaptureVideoOrientation = AVCaptureVideoOrientationPortraitUpsideDown;
                break;
            case UIInterfaceOrientationLandscapeLeft:
                _avCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
                break;
            case UIInterfaceOrientationLandscapeRight:
                _avCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeRight;
                break;

            default:
                break;
        }

        [_AVCaptureConnection setVideoOrientation:_avCaptureVideoOrientation];
    }

    //[avCaptureVideoDataOutput release];

//    [h264Encoder start:mWidth height:mHeight framerate:30 bitrate:1024];
//    h264Encoder._delegate = self;
    ////////////////

    //设置静态图片输出
    if (!_stillImageOutput)
    {
        _stillImageOutput = [[AVCaptureStillImageOutput alloc] init];
    }
    NSDictionary *outputSettings = [[NSDictionary alloc] initWithObjectsAndKeys: AVVideoCodecJPEG, AVVideoCodecKey, nil];
    [_stillImageOutput setOutputSettings:outputSettings];
    if (_captureSession.outputs.count>0)
    {
        //清除输入端口
        [_captureSession removeOutput:_oldstillimageoutput];
    }
    [_captureSession addOutput:_stillImageOutput];
    _oldstillimageoutput=_stillImageOutput;
    [_captureSession startRunning];
}

#pragma mark--捕捉并处理处理图片
//捕捉图片
- (void)CaptureImage {
    //查找video采集端口
    AVCaptureConnection *videoConnection = nil;
    //遍历连接
    for (AVCaptureConnection *connection in _stillImageOutput.connections) {
        //遍历输入端口
        for (AVCaptureInputPort *port in [connection inputPorts]) {

            if ([[port mediaType] isEqual:AVMediaTypeVideo] ) {
                videoConnection = connection;
                break;
            }
        }
        //找到video端口后停止遍历
        if (videoConnection) {
            break;
        }
    }

    [_stillImageOutput captureStillImageAsynchronouslyFromConnection:videoConnection completionHandler: ^(CMSampleBufferRef imageSampleBuffer, NSError *error) {

        if (imageSampleBuffer != NULL) {

            NSData *imageData = [AVCaptureStillImageOutput jpegStillImageNSDataRepresentation:imageSampleBuffer];

            //  [self processImage:[UIImage imageWithData:imageData]];
            [self SavePictureToLibraryWithImage:[UIImage imageWithData:imageData]];
            [self.CameraDelegate didFinishPick_Image:[UIImage imageWithData:imageData]];
        }
    }];
}

- (UIImage*)imageWithImage:(UIImage *)sourceImage scaledToWidth:(float) i_width
{
    float oldWidth = sourceImage.size.width;
    float scaleFactor = i_width / oldWidth;

    float newHeight = sourceImage.size.height * scaleFactor;
    float newWidth = oldWidth * scaleFactor;

    UIGraphicsBeginImageContext(CGSizeMake(newWidth, newHeight));
    [sourceImage drawInRect:CGRectMake(0, 0, newWidth, newHeight)];
    UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return newImage;
}

//停止
- (void)StopCaptureImage{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
	[_captureSession stopRunning];
	[_captureSession removeInput:_oldinput];
	[_captureSession removeOutput:_oldinput];
}

#pragma mark--保存到相册
-(void)SavePictureToLibraryWithImage:(UIImage *)image
{
    ALAssetsLibrary*library=[[ALAssetsLibrary alloc]init];
    [library writeImageToSavedPhotosAlbum:[image CGImage] orientation:(ALAssetOrientation)[image imageOrientation] completionBlock:^(NSURL *assetURL, NSError *error) {
    }];
}

#pragma mark--提示用户
-(void)ShowAlertToUserWithTitle:(NSString *)title Message:(NSString*)mesg
{
    UIAlertView *alert=[[UIAlertView alloc]initWithTitle:nil message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles: nil];
    [alert setTitle:title];
    [alert setMessage:mesg];
    [alert show];
}

//H264HwEncoderImpl* h264Encoder = nil;
//int w=0, h=0;
#pragma mark--摄像头捕获输出
-(void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
//    CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
//    if(CVPixelBufferLockBaseAddress(pixelBuffer, 0) == kCVReturnSuccess) {
//        //        UInt8 *bufferPtr = (UInt8 *)CVPixelBufferGetBaseAddress(pixelBuffer);
//        //        size_t buffeSize = CVPixelBufferGetDataSize(pixelBuffer);
//        size_t bufWidth  = CVPixelBufferGetWidth(pixelBuffer);
//        size_t bufHeight = CVPixelBufferGetHeight(pixelBuffer);
//
//        if (h264Encoder) {
//            if (w!=bufWidth || h!=bufHeight) {
//                h264Encoder._delegate = nil;
//                h264Encoder = nil;
//            }
//        }
//        if (!h264Encoder) {
//            w = bufWidth;
//            h = bufHeight;
//            h264Encoder = [H264HwEncoderImpl alloc];
//            [h264Encoder initWithMembers];
//            h264Encoder._delegate = [[IH264HwEncoderImplDelegate2 alloc] init];
//            [h264Encoder start:w height:h framerate:30 bitrate:1024];
//        }
//        if (h264Encoder) {
//            [h264Encoder encode:pixelBuffer];
//        }
//
//        CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
//    }
//    return;

//    [h264Encoder encode:sampleBuffer];

//    CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
//    if(CVPixelBufferLockBaseAddress(pixelBuffer, 0) == kCVReturnSuccess){
//        UInt8 *bufferPtr = (UInt8 *)CVPixelBufferGetBaseAddress(pixelBuffer);
//        size_t buffeSize = CVPixelBufferGetDataSize(pixelBuffer);
//        size_t bufWidth  = CVPixelBufferGetWidth(pixelBuffer);
//        size_t bufHeight = CVPixelBufferGetHeight(pixelBuffer);
//
//
//        NSLog(@"recv yuv data, len=%zu, w=%zu, h=%zu", buffeSize, bufWidth, bufHeight);
//
//        CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
//    }

    [self.CameraDelegate captureOutput:captureOutput didOutputSampleBuffer:sampleBuffer fromConnection:connection];
}

@end
