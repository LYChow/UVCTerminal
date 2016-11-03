//
//  SwCameraController.mm
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/4.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "SwCameraController.h"
#import "SwCameraOperationManager.h"

#pragma mark -  IH264HwEncoderImplDelegate
@interface ISwCameraControllerDelegate : NSObject <SwCameraControllerDelegate>
{
    @public CSwCameraOperationManager* _cpp_delegate;
}

@end

@implementation ISwCameraControllerDelegate

- (void)didFinishPick_Image:(UIImage *)image//取景完成
{
    NSLog(@"ISwCameraControllerDelegate didFinishPick_Image");
    if (_cpp_delegate) {
        _cpp_delegate->OnFinishPickImage((__bridge void*)(image));
    }
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    NSLog(@"ISwCameraControllerDelegate captureOutput");

    try {
    if (_cpp_delegate) {
        CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
//        if(CVPixelBufferLockBaseAddress(pixelBuffer, 0) == kCVReturnSuccess) {
            _cpp_delegate->OnCaptureFrame((void*)pixelBuffer);
//            CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
//        }

//        CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
//        if(CVPixelBufferLockBaseAddress(pixelBuffer, 0) == kCVReturnSuccess) {
//            UInt8 *bufferPtr = (UInt8 *)CVPixelBufferGetBaseAddress(pixelBuffer);
//            size_t buffeSize = CVPixelBufferGetDataSize(pixelBuffer);
//            size_t bufWidth  = CVPixelBufferGetWidth(pixelBuffer);
//            size_t bufHeight = CVPixelBufferGetHeight(pixelBuffer);
//
//            //NSLog(@"recv yuv data, len=%zu, w=%zu, h=%zu", buffeSize, bufWidth, bufHeight);
//            _cpp_delegate->OnCaptureFrame(bufferPtr, buffeSize, bufWidth, bufHeight);
//
//            CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
//        }
    }
    } catch (...) {
        printf("captureOutput exception!\n");
    }
//    usleep(1000*1000);
//    NSLog(@"ISwCameraControllerDelegate captureOutput return!!!!!!");
}

@end

#pragma mark - class CSwCameraOperationManager
const SwCameraOperationManager* TransferSwCameraOperationManager(void* _pSwCameraOperationManager)
{
    SwCameraOperationManager* pSwCameraOperationManager = (__bridge SwCameraOperationManager*)(_pSwCameraOperationManager);
    return pSwCameraOperationManager;
}

#pragma mark - public functions
CSwCameraOperationManager::CSwCameraOperationManager()
{
    ISwCameraControllerDelegate* pSwCameraControllerDelegate = [[ISwCameraControllerDelegate alloc] init];
    if (!pSwCameraControllerDelegate) {
        printf("[[ISwCameraControllerDelegate alloc] init] Failed.\n");
    }
    else
    {
        pSwCameraControllerDelegate->_cpp_delegate = this;
    }
    SwCameraOperationManager* pSwCameraOperationManager = [[SwCameraOperationManager alloc] init];
    if (!pSwCameraOperationManager) {
        printf("[[SwCameraOperationManager alloc] init] Failed.\n");
    }
    else
    {
        pSwCameraOperationManager.CameraDelegate = pSwCameraControllerDelegate;
        m_pSwCameraOperationManager = (__bridge_retained void*)pSwCameraOperationManager;
    }
    m_pDataCallback = NULL;
    m_lDataCallbackUser = 0;
}

CSwCameraOperationManager::~CSwCameraOperationManager()
{
    SwCameraOperationManager* pSwCameraOperationManager = (__bridge_transfer SwCameraOperationManager*)(m_pSwCameraOperationManager);
	[pSwCameraOperationManager StopCaptureImage];
    pSwCameraOperationManager.CameraDelegate = nil;
    pSwCameraOperationManager = nil;
}

bool CSwCameraOperationManager::Init(int w, int h, int frameduration_value, int frameduration_timescale, PCSwCameraOperationManager_DataCallback cb, long cbuser, void* UIImageView_preview, bool back/*=FALSE*/)
{
    m_pDataCallback = cb;
    m_lDataCallbackUser = cbuser;

    [TransferSwCameraOperationManager(m_pSwCameraOperationManager) initializeCameraWithPreview:((__bridge UIImageView*)UIImageView_preview)
                                                                                  captureWidth:w
                                                                                 captureHeight:h
                                                                                 frameDuration:CMTimeMake(frameduration_value, frameduration_timescale)
                                                                                    backCamera:back];
    return TRUE;
}

void CSwCameraOperationManager::SetMinFrameDuration(int value, int timescale)
{
    [TransferSwCameraOperationManager(m_pSwCameraOperationManager) SetMinCaptureDuration:CMTimeMake(value, timescale)];
}

bool CSwCameraOperationManager::SwitchCamera()
{
    [TransferSwCameraOperationManager(m_pSwCameraOperationManager) SwitchCamera];
    return TRUE;
}

void CSwCameraOperationManager::OnFinishPickImage(void* UIImage)
{
    printf("CSwCameraOperationManager::OnFinishPickImage\n");
    if (m_pDataCallback) {
        m_pDataCallback(COM_DCC_FINISHPICKIMAGE, (long)UIImage, m_lDataCallbackUser);
    }
}

//CH264HwEncoderImpl* penc = NULL;
void CSwCameraOperationManager::OnCaptureFrame(void* _CVImageBufferRef)
{
#ifdef DEBUG
    CVImageBufferRef pixelBuffer = (CVImageBufferRef)_CVImageBufferRef;
    if(CVPixelBufferLockBaseAddress(pixelBuffer, 0) == kCVReturnSuccess) {
        UInt8 *bufferPtr = (UInt8 *)CVPixelBufferGetBaseAddress(pixelBuffer);
        size_t buffeSize = CVPixelBufferGetDataSize(pixelBuffer);
        size_t bufWidth  = CVPixelBufferGetWidth(pixelBuffer);
        size_t bufHeight = CVPixelBufferGetHeight(pixelBuffer);

//        printf("CSwCameraOperationManager::OnCaptureFrame(size: %zu, w: %zu, h %zu(w*h=%lu)).\n",
//               buffeSize, bufWidth, bufHeight, bufWidth*bufHeight);

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
        CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    }
//    return;

//    if (!penc) {
//        penc = new CH264HwEncoderImpl(NULL, 0);
//    }
//    if (penc) {
//        penc->PutData((const char*)_CVImageBufferRef, 1, 0);
//    }
//    return;
#endif //#ifdef DEBUG

    if (m_pDataCallback) {
        m_pDataCallback(COM_DCC_CAPTUREFRAME, (long)_CVImageBufferRef, m_lDataCallbackUser);
    }
}

#pragma mark - protected functions