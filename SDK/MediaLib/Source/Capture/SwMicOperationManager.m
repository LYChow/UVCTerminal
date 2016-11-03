//
//  SwMicOperationManager.m
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/30.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#import "SwMicOperationManager.h"
#import <AssetsLibrary/AssetsLibrary.h>
#import <AudioToolbox/AudioToolbox.h>


@implementation SwMicOperationManager {
//    AVAssetWriter * assetWriter;
//    AVAssetWriterInput * assetWriterInput;
}

#pragma mark--初始化等操作
-(id)init
{
    self=[super init];
    if (self) {
        //检查授权
        [self checkDeviceAuthorizationStatus];
    }

//    NSArray *searchPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//    NSURL *_outputPath = [NSURL fileURLWithPath:[[searchPaths objectAtIndex:0] stringByAppendingPathComponent:@"micOutput.output"]];
//
//    NSError * assetError;
//
//    AudioChannelLayout acl;
//    bzero(&acl, sizeof(acl));
//    acl.mChannelLayoutTag = kAudioChannelLayoutTag_Mono; //kAudioChannelLayoutTag_Stereo;
//    NSDictionary *audioOutputSettings = [NSDictionary dictionaryWithObjectsAndKeys:
//                                         [NSNumber numberWithInt: kAudioFormatULaw],AVFormatIDKey,
//                                         [NSNumber numberWithFloat:8000.0],AVSampleRateKey,//was 44100.0
//                                         [NSData dataWithBytes: &acl length: sizeof( AudioChannelLayout ) ], AVChannelLayoutKey,
//                                         [NSNumber numberWithInt:1],AVNumberOfChannelsKey,
//                                         [NSNumber numberWithInt:64000],AVEncoderBitRateKey,
//                                         nil];
//
//    assetWriterInput = CFRetain((__bridge CFTypeRef)([AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeAudio outputSettings:audioOutputSettings]));
//    [assetWriterInput setExpectsMediaDataInRealTime:YES];
//
//    assetWriter = CFRetain((__bridge CFTypeRef)([AVAssetWriter assetWriterWithURL:_outputPath fileType:AVFileTypeWAVE error:&assetError])); //AVFileTypeAppleM4A
//
//    if (assetError) {
//        NSLog (@"error initing mic: %@", assetError);
//        return nil;
//    }
//
//    if ([assetWriter canAddInput:assetWriterInput]) {
//        [assetWriter addInput:assetWriterInput];
//    } else {
//        NSLog (@"can't add asset writer input...!");
//        return nil;
//    }

    return self;
}

//初始化Mic
-(void)initializeMic
{
    if (_deviceAuthorized) {
        if (!_captureSession){
            _captureSession = [[AVCaptureSession alloc] init];
        }
        //获取硬件采集设备
        NSArray *devices = [AVCaptureDevice devices];

        if (devices.count==0) {
            [self ShowAlertToUserWithTitle:@"抱歉" Message:@"Mic设备不可用"];
            self.MicSate=KMicSateDisable;
            return;
        }

        for (AVCaptureDevice *device in devices) {
            if ([device hasMediaType:AVMediaTypeAudio]) {
                _Mic = device;
                break;
            }
        }

        [self SetCaptureSession];
    }
}

-(void)finitializeMic
{
	if (_captureSession)
	{
		[_captureSession removeInput:_oldInput];
		[_captureSession removeOutput:_oldOutput];
		[_captureSession stopRunning];
	}
}

//检查用户Mic授权
- (void)checkDeviceAuthorizationStatus
{
    if ([AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo]==AVAuthorizationStatusDenied) {

        [self ShowAlertToUserWithTitle:@"无法启动Mic" Message:@"\n请前往“设置-隐私-Mic”允许应用使用Mic"];
        _deviceAuthorized=NO;
    }
    else{
        _deviceAuthorized=YES;
    }
}

//设置AVCaptureSession以及输入输出端
-(void)SetCaptureSession
{
    NSError *error = nil;

    CMTime frameDuration = CMTimeMake(1, 5);
    NSArray *supportedFrameRateRanges = [_Mic.activeFormat videoSupportedFrameRateRanges];
    BOOL frameRateSupported = NO;
    for (AVFrameRateRange *range in supportedFrameRateRanges) {
        if (CMTIME_COMPARE_INLINE(frameDuration, >=, range.minFrameDuration) &&
            CMTIME_COMPARE_INLINE(frameDuration, <=, range.maxFrameDuration)) {
            frameRateSupported = YES;
        }
    }

    if (frameRateSupported && [_Mic lockForConfiguration:&error]) {
        [_Mic setActiveVideoMaxFrameDuration:frameDuration];
        [_Mic setActiveVideoMinFrameDuration:frameDuration];
        [_Mic unlockForConfiguration];
    }

    //添加输入设备到当前session
    error = nil;
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:_Mic error:&error];
    if (!input) {
        [self ShowAlertToUserWithTitle:@"提示" Message:@"无法连接到麦克风"];
    }
    //首次添加输入端口
    if (_captureSession.inputs.count>0)
    {
        //清除输入端口
        [_captureSession removeInput:_oldInput];
    }
    [_captureSession addInput:input];
    _oldInput = input;

    // 设置音频实时输出
    AVCaptureAudioDataOutput *avCaptureAudioDataOutput = [[AVCaptureAudioDataOutput alloc] init];

//    NSArray *searchPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//    _outputPath = [NSURL fileURLWithPath:[[searchPaths objectAtIndex:0] stringByAppendingPathComponent:@"micOutput.output"]];

//    AudioChannelLayout acl;
//    bzero( &acl, sizeof(acl));
//    acl.mChannelLayoutTag = kAudioChannelLayoutTag_Mono;
//    NSDictionary *audioOutputSettings = [NSDictionary dictionaryWithObjectsAndKeys:
//                                         [NSNumber numberWithInt: kAudioFormatULaw],AVFormatIDKey,
//                                         [NSNumber numberWithFloat:8000.0],AVSampleRateKey,
//                                         [NSData dataWithBytes: &acl length: sizeof( AudioChannelLayout ) ], AVChannelLayoutKey,
//                                         [NSNumber numberWithInt:1],AVNumberOfChannelsKey,
//                                         [NSNumber numberWithInt:64000],AVEncoderBitRateKey,
//                                         nil];
//    avCaptureAudioDataOutput.audioSettings = audioOutputSettings;
//
    dispatch_queue_t queue = dispatch_queue_create("com.sinowave.sdk.producer.audio.captureoutput", NULL);
    [avCaptureAudioDataOutput setSampleBufferDelegate:self queue:queue];
    if (_captureSession.outputs.count>0)
    { //清除输入端口
        [_captureSession removeOutput:_oldOutput];
    }
    [_captureSession addOutput:avCaptureAudioDataOutput];
    _oldOutput = avCaptureAudioDataOutput;

    // preset should be set after "addInput" and "addOutput"
    //An AVCaptureSession preset suitable for high quality video and audio output.
//    _captureSession.sessionPreset = AVCaptureSessionPresetHigh;
    NSString *sessionPreset = AVCaptureSessionPresetHigh;
    if ([_Mic supportsAVCaptureSessionPreset:sessionPreset]) {
        NSLog(@"For output audio we selected %s preset", [sessionPreset UTF8String]);
        _captureSession.sessionPreset = sessionPreset;
    }
    else {
        NSLog(@"%s not supported as audio preset, fallback to %s", [sessionPreset UTF8String], [AVCaptureSessionPresetLow UTF8String]);
        _captureSession.sessionPreset = AVCaptureSessionPresetLow;
    }

//    [assetWriter startWriting];
    [_captureSession startRunning];
}

#pragma mark--提示用户
-(void)ShowAlertToUserWithTitle:(NSString *)title Message:(NSString*)mesg
{
    UIAlertView *alert=[[UIAlertView alloc]initWithTitle:nil message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles: nil];
    [alert setTitle:title];
    [alert setMessage:mesg];
    [alert show];
}

#pragma mark--麦克风捕获输出
-(void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    [self.MicDelegate captureOutput:captureOutput didOutputSampleBuffer:sampleBuffer fromConnection:connection];

//    AudioBufferList audioBufferList;
//    NSMutableData *data= [[NSMutableData alloc] init];
//    CMBlockBufferRef blockBuffer;
//    CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(sampleBuffer, NULL, &audioBufferList, sizeof(audioBufferList), NULL, NULL, 0, &blockBuffer);
//    for (int y = 0; y < audioBufferList.mNumberBuffers; y++) {
//        AudioBuffer audioBuffer = audioBufferList.mBuffers[y];
//        Float32 *frame = (Float32*)audioBuffer.mData;
//        [data appendBytes:frame length:audioBuffer.mDataByteSize];
//    }
//    // append [data bytes] to your NSOutputStream
//    // These two lines write to disk, you may not need this, just providing an example
//    [assetWriter startSessionAtSourceTime:CMSampleBufferGetPresentationTimeStamp(sampleBuffer)];
//    [assetWriterInput appendSampleBuffer:sampleBuffer];
//
//        NSString *requestStr = [NSString stringWithFormat:@"POST /transmitaudio?id=%@ HTTP/1.0\r\n\r\n",self.restClient.sessionId];
//        NSData *requestData = [requestStr dataUsingEncoding:NSUTF8StringEncoding];
//        [self.socket writeData:requestData withTimeout:5 tag:0];
//        [self.socket writeData:data withTimeout:5 tag:0];
//
//    //stop upload audio data
//    CFRelease(blockBuffer);
//    blockBuffer = nil;
//    data = nil;
}

@end
