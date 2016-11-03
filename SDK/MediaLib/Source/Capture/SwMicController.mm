//
//  SwMicController.mm
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/4.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "SwMicController.h"
#import "SwMicOperationManager.h"
#include "AudioConverter.h"
#include "itzLog.h"

#pragma mark -  IH264HwEncoderImplDelegate
@interface ISwMicControllerDelegate : NSObject <SwMicControllerDelegate>
{
    @public CSwMicOperationManager* _cpp_delegate;
}

@end

@implementation ISwMicControllerDelegate {
    unsigned char* captureFrameBuf;
    int captureFrameBufDataLen;
    bool isFirstConvertedFrameDeliverd;
    AudioConverterRef audioConverter;
    AudioStreamBasicDescription inputFormat; // 输入音频格式
    AudioStreamBasicDescription outputFormat; // 输出音频格式
}

-(id)init
{
    self=[super init];
    if (self) {
        captureFrameBuf = NULL;
        captureFrameBufDataLen = 0;
        isFirstConvertedFrameDeliverd = false;
    }
	audioConverter=nil;

    return self;
}

- (void)dealloc
{
     if (audioConverter) {
        //CFRelease(audioConverter);
		 AudioConverterDispose(audioConverter);
		audioConverter=nil;
    }
   if (captureFrameBuf) {
        free(captureFrameBuf);
		captureFrameBuf=0;
    }

    //[super dealloc];
}

#define AUDIO_FRAMEDURATION  40/*ms*/
- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    //NSLog(@"ISwMicControllerDelegate captureOutput");

    if (audioConverter == nil)
    { // 初始化转换器
        inputFormat = *(CMAudioFormatDescriptionGetStreamBasicDescription(CMSampleBufferGetFormatDescription(sampleBuffer))); // 输入音频格式
        memset(&outputFormat, 0, sizeof(outputFormat));
        outputFormat.mSampleRate = 8000.0; // 采样率改为8000
        outputFormat.mFormatID = kAudioFormatLinearPCM; // PCM编码
        outputFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger|kLinearPCMFormatFlagIsPacked; // inputFormat.mFormatFlags;
        outputFormat.mBytesPerPacket = 2;
        outputFormat.mFramesPerPacket = 1;
        outputFormat.mBytesPerFrame = 2;
        outputFormat.mChannelsPerFrame = 1;
        outputFormat.mBitsPerChannel = 16;

        AudioClassDescription *desc = [self getAudioClassDescriptionWithType:outputFormat.mFormatID fromManufacturer:kAppleHardwareAudioCodecManufacturer];
        if (AudioConverterNewSpecific(&inputFormat, &outputFormat, 1, desc, &audioConverter)!=noErr || audioConverter==nil) {
            NSLog(@"ISwMicControllerDelegate captureOutput: AudioConverterNewSpecific failed");
            return;
        }
    }

    // 获取采集到的音频数据内存
    CMBlockBufferRef blockBuffer = nil;
    AudioBufferList captureBufferList;
    if (CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(sampleBuffer, NULL, &captureBufferList, sizeof(captureBufferList), NULL, NULL, 0, &blockBuffer) != noErr)
    {
        NSLog(@"ISwMicControllerDelegate captureOutput: CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer failed");
        return;
    }

    int lenofaudioframe = inputFormat.mSampleRate*inputFormat.mBytesPerFrame*AUDIO_FRAMEDURATION/1000;
    int lenof3audioframe = 3*lenofaudioframe;
    if (!captureFrameBuf) {
        int buflen = lenof3audioframe;
        if (buflen < captureBufferList.mBuffers[0].mDataByteSize) {
            buflen = captureBufferList.mBuffers[0].mDataByteSize;
        }
        buflen *= 2;
        captureFrameBuf = (unsigned char*)malloc(buflen);
        captureFrameBufDataLen = 0;
    }

#ifdef DEBUG222
    static FILE* filePCM44k16bit = NULL;

    if (!filePCM44k16bit) {
        char szBuf[512] = {0};
        int ir = GetDocumentDirectory(szBuf, sizeof(szBuf)-1);
        if (ir > 0) {
            snprintf(szBuf+ir, sizeof(szBuf)-ir-1, "/capture_audio44k.pcm");
            filePCM44k16bit = fopen(szBuf, "w");
        }
    }
    if (filePCM44k16bit) {
        fwrite((const void*)captureBufferList.mBuffers[0].mData, captureBufferList.mBuffers[0].mDataByteSize, 1, filePCM44k16bit);
    }
#endif //#ifdef DEBUG

    // 拼帧
    memcpy(captureFrameBuf+captureFrameBufDataLen, captureBufferList.mBuffers[0].mData, captureBufferList.mBuffers[0].mDataByteSize);
    captureFrameBufDataLen += captureBufferList.mBuffers[0].mDataByteSize;

    CFRelease(blockBuffer);

    unsigned char* itertor = captureFrameBuf;
    int leftlen = captureFrameBufDataLen;
    while (leftlen >= lenof3audioframe) {
        // 构造一个转换输入缓冲列表
        AudioBufferList inBufferList;
        inBufferList.mNumberBuffers = 1;
        inBufferList.mBuffers[0].mNumberChannels = 1;
        inBufferList.mBuffers[0].mDataByteSize = lenof3audioframe;
        inBufferList.mBuffers[0].mData = itertor;

        // 迭代器前进
        itertor += lenofaudioframe;
        leftlen -= lenofaudioframe;

        // 输出缓冲内存
        int buflen = outputFormat.mSampleRate*outputFormat.mBytesPerFrame*3*AUDIO_FRAMEDURATION/1000;
        unsigned char* buf = (unsigned char*)malloc(buflen);
        memset(buf, 0, buflen);

        // 构造一个输出缓冲列表
        AudioBufferList outBufferList;
        outBufferList.mNumberBuffers = 1;
        outBufferList.mBuffers[0].mNumberChannels = 1;
        outBufferList.mBuffers[0].mDataByteSize = buflen; // 设置输出缓冲区大小
        outBufferList.mBuffers[0].mData = buf; // 设置输出缓冲区

        UInt32 outputDataPacketSize = buflen/outputFormat.mBytesPerPacket;
        OSStatus ossr = AudioConverterFillComplexBuffer(audioConverter, inputDataProc_cap, &inBufferList, &outputDataPacketSize, &outBufferList, NULL);
        if (ossr != noErr)
        {
            NSLog(@"ISwMicControllerDelegate captureOutput: AudioConverterFillComplexBuffer with %u bytes failed, r=%u.",
                  (unsigned int)inBufferList.mBuffers[0].mDataByteSize, (int)ossr);
            free(buf);
            break;//kAudioConverterErr_InvalidInputSize
        }
        unsigned int outlen = outBufferList.mBuffers[0].mDataByteSize; //编码后的数据大小
        NSLog(@"ISwMicControllerDelegate captureOutput: AudioConverterFillComplexBuffer with %u bytes ok, outlen: %u.",
              (unsigned int)inBufferList.mBuffers[0].mDataByteSize, outlen);

        if (_cpp_delegate) {
            if (!isFirstConvertedFrameDeliverd) {
                isFirstConvertedFrameDeliverd = true;
                _cpp_delegate->OnCaptureFrame(buf, outlen/3);
            }
            _cpp_delegate->OnCaptureFrame(buf+outlen/3, outlen/3);
        }
        free(buf);
    }

    if (leftlen>0 && itertor!=captureFrameBuf) {
        memcpy(captureFrameBuf, itertor, leftlen);
    }
    captureFrameBufDataLen = leftlen;

    //NSLog(@"ISwMicControllerDelegate captureOutput return!!!!!!");
}

-(AudioClassDescription*)getAudioClassDescriptionWithType:(UInt32)type fromManufacturer:(UInt32)manufacturer { // 获得相应的编码器
    static AudioClassDescription audioDesc;

    UInt32 encoderSpecifier = type, size = 0;
    OSStatus status;

    memset(&audioDesc, 0, sizeof(audioDesc));
    status = AudioFormatGetPropertyInfo(kAudioFormatProperty_Encoders, sizeof(encoderSpecifier), &encoderSpecifier, &size);
    if (status) {
        return nil;
    }

    uint32_t count = size / sizeof(AudioClassDescription);
    AudioClassDescription descs[count];
    status = AudioFormatGetProperty(kAudioFormatProperty_Encoders, sizeof(encoderSpecifier), &encoderSpecifier, &size, descs);
    for (uint32_t i = 0; i < count; i++)
    {
        if ((type == descs[i].mSubType) && (manufacturer == descs[i].mManufacturer))
        {
            memcpy(&audioDesc, &descs[i], sizeof(audioDesc));
            break;
        }
    }
    return &audioDesc;
}

OSStatus inputDataProc_cap(AudioConverterRef inAudioConverter,
                       UInt32* ioNumberDataPackets,
                       AudioBufferList* ioData,
                       AudioStreamPacketDescription*__nullable *__nullable outDataPacketDescription,
                       void*__nullable inUserData) {
    //编码过程中，会要求这个函数来填充输入数据，也就是原始PCM数据
    NSLog(@"AudioConverterFillComplexBuffer inputDataProc called.");

    if(outDataPacketDescription)
    {
        NSLog(@"_converterComplexInputDataProc cannot provide input data; it doesn't know how to provide packet descriptions");
        *outDataPacketDescription = NULL;
        *ioNumberDataPackets = 0;
        ioData->mNumberBuffers = 0;
        return 501;
    }

    AudioBufferList bufferList = *(AudioBufferList*)inUserData;
    ioData->mNumberBuffers = 1;
    ioData->mBuffers[0].mNumberChannels = 1;
    ioData->mBuffers[0].mData           = bufferList.mBuffers[0].mData;
    ioData->mBuffers[0].mDataByteSize   = bufferList.mBuffers[0].mDataByteSize;

    *ioNumberDataPackets = ioData->mBuffers[0].mDataByteSize/2;
    
    return noErr;
}

@end

#pragma mark - class CSwMicOperationManager
const SwMicOperationManager* TransferSwMicOperationManager(void* _pSwMicOperationManager)
{
    SwMicOperationManager* pSwMicOperationManager = (__bridge SwMicOperationManager*)(_pSwMicOperationManager);
    return pSwMicOperationManager;
}

#pragma mark - public functions
CSwMicOperationManager::CSwMicOperationManager()
{
    ISwMicControllerDelegate* pSwMicControllerDelegate = [[ISwMicControllerDelegate alloc] init];
    if (!pSwMicControllerDelegate) {
        printf("[[ISwMicControllerDelegate alloc] init] Failed.\n");
    }
    else
    {
        pSwMicControllerDelegate->_cpp_delegate = this;
    }
    SwMicOperationManager* pSwMicOperationManager = [[SwMicOperationManager alloc] init];
    if (!pSwMicOperationManager) {
        printf("[[SwMicOperationManager alloc] init] Failed.\n");
    }
    else
    {
        pSwMicOperationManager.MicDelegate = pSwMicControllerDelegate;
        m_pSwMicOperationManager = (__bridge_retained void*)pSwMicOperationManager;
    }
    m_pDataCallback = NULL;
    m_lDataCallbackUser = 0;
}

CSwMicOperationManager::~CSwMicOperationManager()
{
    SwMicOperationManager* pSwMicOperationManager = (__bridge_transfer SwMicOperationManager*)(m_pSwMicOperationManager);
    pSwMicOperationManager.MicDelegate = nil;
    pSwMicOperationManager = nil;
}

bool CSwMicOperationManager::Init(PCSwMicOperationManager_DataCallback cb, long cbuser)
{
    m_pDataCallback = cb;
    m_lDataCallbackUser = cbuser;

    [TransferSwMicOperationManager(m_pSwMicOperationManager) initializeMic];
    return TRUE;
}

bool CSwMicOperationManager::Fini()
{
	[TransferSwMicOperationManager(m_pSwMicOperationManager) finitializeMic];
	return TRUE;
}

void CSwMicOperationManager::OnCaptureFrame(unsigned char* pcm8k16bit, int len)
{
    if (pcm8k16bit && len>0 && m_pDataCallback) {
        MOM_DataCallbackFrame frame;
        memset(&frame, 0, sizeof(frame));
        frame.pcm8k16bit = pcm8k16bit;
        frame.len = len;
        m_pDataCallback(MOM_DCC_CAPTUREFRAME, (long)&frame, m_lDataCallbackUser);
    }
}

#pragma mark - protected functions