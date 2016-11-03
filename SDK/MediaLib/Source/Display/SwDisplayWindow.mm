//
//  SwDisplayWindow.mm
//  SwIMediaLib
//
//  Created by 李招华 on 16/2/14.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <sys/time.h>
#import <UIKit/UIKit.h>
#import <CoreVideo/CoreVideo.h>
//#import <VideoToolbox/VideoToolbox.h>
#import "SwDisplayWindow.h"
#import "AAPLEAGLLayer.h"


//@interface DisplayImage : NSObject
//
//- (void)displayImage;
//- (void)setView:(UIImageView*)view;
//- (void)putImage:(UIImage*)image;
//
//@property (weak, nonatomic) UIImageView* _view;
//@property (weak, nonatomic) UIImage* _image;
//
//@end
//
//@implementation DisplayImage
//
//- (void)displayImage {
//    [self._view setImage:self._image];
//}
//
//- (void)setView:(UIImageView*)view {
//    self._view = view;
//}
//
//- (void)putImage:(UIImage*)image {
//    self._image = image;
//    [self performSelectorOnMainThread:@selector(displayImage) withObject:nil waitUntilDone:YES];
//}
//
//@end

#pragma mark - public functions
CSwDisplayWindow::CSwDisplayWindow(void* _UIImageView)
: m_UIImageView(_UIImageView)
, m_bForceMirror(false)
{
    if (m_UIImageView) {
        UIImageView* pView = ((__bridge UIImageView*)m_UIImageView);
        AAPLEAGLLayer* _glLayer = [[AAPLEAGLLayer alloc] initWithFrame:pView];
        [pView.layer addSublayer:_glLayer];
        m_glLayer = ((__bridge_retained void*)_glLayer);
    }

//    DisplayImage* pdi = [[DisplayImage alloc] init];
//    [pdi setView:((__bridge UIImageView*)_UIImageView)];
//    m_DisplayImage = ((__bridge_retained void*) pdi);
}

CSwDisplayWindow::~CSwDisplayWindow()
{
    AAPLEAGLLayer* _glLayer = (__bridge_transfer AAPLEAGLLayer*)m_glLayer;
    if (_glLayer) {
        [_glLayer removeFromSuperlayer];
        _glLayer = nil;
    }

//    DisplayImage* pdi = (__bridge_transfer DisplayImage*)m_DisplayImage;
//    pdi = nil;
}

int CSwDisplayWindow::PutData(const char* data, int flags)
{
    int ir = -1;
    if (!data) {
        return ir;
    }

    bool bNeedCopy = (flags&SDWPDF_COPY);
    bool bNeedMirror = (m_bForceMirror||(flags&SDWPDF_MIRROR));

#ifdef DEBUG
    static int ncount = 0;
    ncount++;
//    if (ncount == 12) {
//        ncount = 0;
//    }
//    printf("CSwDisplayWindow::PutData! %d\n", ncount);

//    if (ncount%30) {
//        return 0;
//    }
//
//    // 获取解码后的数据，并保存到相册
//    { // 该代码可用于转存 kCVPixelFormatType_32BGRA，而 AAPLEAGLLayer 用于渲染 kCVPixelFormatType_420YpCbCr8BiPlanarFullRange
//        CVPixelBufferRef imageBuffer = (CVImageBufferRef)data;
//        CVPixelBufferLockBaseAddress(imageBuffer, 0);
//        void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
//        size_t width = CVPixelBufferGetWidth(imageBuffer);
//        size_t height = CVPixelBufferGetHeight(imageBuffer);
//        size_t bufferSize = CVPixelBufferGetDataSize(imageBuffer);
//        size_t bytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, 0);
//
//        CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
//        CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, baseAddress, bufferSize, NULL);
//
//        CGImageRef cgImage = CGImageCreate(width, height, 8, 32, bytesPerRow, rgbColorSpace, kCGImageAlphaNoneSkipFirst|kCGBitmapByteOrder32Little, provider, NULL, true, kCGRenderingIntentDefault);
//
//        UIImage *image = [UIImage imageWithCGImage:cgImage];
//
//        CGImageRelease(cgImage);
//        CGDataProviderRelease(provider);
//        CGColorSpaceRelease(rgbColorSpace);
//
//        NSData* imageData = UIImageJPEGRepresentation(image, 1.0);
//        image = [UIImage imageWithData:imageData];
//        CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
//
//        UIImageWriteToSavedPhotosAlbum(image,nil,nil,nil);
//    }

    static int ndisplayedcount = 0;
    ndisplayedcount++;
    NSLog(@"CSwDisplayWindow::display! %d\n", ndisplayedcount);
#endif //#ifdef DEBUG
    struct timeval tv;
    gettimeofday(&tv, 0);
    try {
        AAPLEAGLLayer* _glLayer = (__bridge AAPLEAGLLayer*)m_glLayer;
        if (_glLayer) {
            if (bNeedCopy) {
                CVPixelBufferRef imageBuffer = (CVImageBufferRef)data;
                CVPixelBufferLockBaseAddress(imageBuffer, 0);

                size_t width = CVPixelBufferGetWidth(imageBuffer);
                size_t height = CVPixelBufferGetHeight(imageBuffer);
                OSType pixelFormatType = CVPixelBufferGetPixelFormatType(imageBuffer);

    //            CFDictionaryRef attrs = NULL;
    //            const void *keys[] = { kCVPixelBufferPixelFormatTypeKey, kCVPixelBufferIOSurfacePropertiesKey };
                //      kCVPixelFormatType_420YpCbCr8Planar is YUV420
                //      kCVPixelFormatType_420YpCbCr8BiPlanarFullRange is NV12
                uint32_t v = kCVPixelFormatType_420YpCbCr8BiPlanarFullRange;
                const void *values[] = { CFNumberCreate(NULL, kCFNumberSInt32Type, &v) };
    //            attrs = CFDictionaryCreate(NULL, keys, values, 1, NULL, NULL);

                NSDictionary* pixelBufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:
                                                       [NSDictionary dictionary],
                                                       (id)kCVPixelBufferIOSurfacePropertiesKey, nil/*,
                                                       (id)kCVPixelBufferPixelFormatTypeKey, CFNumberCreate(NULL, kCFNumberSInt32Type, &v)*/];

                CVPixelBufferRef pixelBuffer = nil;
                int ir = CVPixelBufferCreate(NULL, width, height, pixelFormatType, (__bridge CFDictionaryRef)pixelBufferAttributes, &pixelBuffer);

                CVPixelBufferLockBaseAddress(pixelBuffer, 0);

    //            void *baseAddress_new = CVPixelBufferGetBaseAddress(pixelBuffer);
    //            size_t bufferSize_new = CVPixelBufferGetDataSize(pixelBuffer);
    //            if (baseAddress_new && baseAddress && bufferSize_new==bufferSize) {
    //                memcpy(baseAddress_new, baseAddress, bufferSize);
    //            }

                size_t planeCount = CVPixelBufferGetPlaneCount(pixelBuffer);
                for (int i=0; i<planeCount; i++) {
                    char* baseAddress = (char*)CVPixelBufferGetBaseAddressOfPlane(imageBuffer, i);
                    size_t bytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, i);
                    size_t heightOfPlane = CVPixelBufferGetHeightOfPlane(imageBuffer, i);
                    size_t bufferSize = bytesPerRow*heightOfPlane;

                    char* baseAddress_new = (char*)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, i);
                    size_t bytesPerRow_new = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, i);
                    size_t heightOfPlane_new = CVPixelBufferGetHeightOfPlane(pixelBuffer, i);
                    size_t bufferSize_new = bytesPerRow_new*heightOfPlane_new;

                    if (baseAddress_new && baseAddress) {
                        if (bufferSize_new == bufferSize) {
                            memcpy(baseAddress_new, baseAddress, bufferSize);
                        }
                        else if (heightOfPlane == heightOfPlane_new) {
                            size_t bytesPerCopy = bytesPerRow<bytesPerRow_new?bytesPerRow:bytesPerRow_new;
                            for (int i=0; i<heightOfPlane; i++) {
                                memcpy(baseAddress_new+bytesPerRow_new*i, baseAddress+bytesPerRow*i, bytesPerCopy);
                            }
                        }
                    }
                }

                CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);

                CVPixelBufferUnlockBaseAddress(imageBuffer, 0);

                dispatch_sync(dispatch_get_main_queue(), ^{
                    //NSLog(@"main_queue block called.");
                    [_glLayer setPixelBuffer:pixelBuffer mirror:bNeedMirror];
                    CVPixelBufferRelease(pixelBuffer);
    //                usleep(300*1000);
                });
                pixelBuffer = nil;

            }
            else {
                CVPixelBufferRef pixelBuffer = /*CVPixelBufferRetain*/((CVImageBufferRef)data);
    //            CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
                dispatch_sync(dispatch_get_main_queue(), ^{
                    //NSLog(@"main_queue block called.");
                    [_glLayer setPixelBuffer:pixelBuffer mirror:bNeedMirror];
    //                CVPixelBufferRelease(pixelBuffer);
    //                usleep(300*1000);
                });
    //            CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
            }

            ir = 0;
        }
    } catch (...) {
        printf("exception!\n");
    }
    struct timeval tv2;
    gettimeofday(&tv2, 0);
    Log(@"CSwDisplayWindow::displaypixelBuffer done! cost %ld/%d\n", tv2.tv_sec-tv.tv_sec, tv2.tv_usec-tv.tv_usec);

//    DisplayImage* pdi = (__bridge DisplayImage*)m_DisplayImage;
//    if (pdi) {
//        CIImage *ciImage = [CIImage imageWithCVPixelBuffer:(CVImageBufferRef)data];
//        UIImage *uiImage = [UIImage imageWithCIImage:ciImage];
//        [pdi putImage:uiImage];
//        ir = 0;
//    }

    return ir;
}

void CSwDisplayWindow::Mirror(bool mirror)
{
    m_bForceMirror = mirror;
}

