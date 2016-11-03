//
//  AudioConverter.mm
//  SwIMediaLib
//
//  Created by 李招华 on 16/3/8.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#include "AudioConverter.h"
#import <AVFoundation/AVFoundation.h>

bool FillAudioStreamBasicDescriptionByAF(CACAudioFormat af, AudioStreamBasicDescription* ades) {
    if (!ades) {
        return false;
    }
    switch (af) {
        case CAC_AF_8k16bitPCM:
            ades->mSampleRate = 8000.0; // 采样率
            ades->mFormatID = kAudioFormatLinearPCM; // 编码
            ades->mFormatFlags = kLinearPCMFormatFlagIsSignedInteger|kLinearPCMFormatFlagIsPacked;
            ades->mBytesPerPacket = 2;
            ades->mFramesPerPacket = 1;
            ades->mBytesPerFrame = 2;
            ades->mChannelsPerFrame = 1;
            ades->mBitsPerChannel = 16;
            ades->mReserved = 0;
            return true;
            break;
        case CAC_AF_ULAW:
            ades->mSampleRate = 8000.0; // 采样率
            ades->mFormatID = kAudioFormatULaw; // 编码
            ades->mFormatFlags = kLinearPCMFormatFlagIsSignedInteger|kLinearPCMFormatFlagIsPacked;
            ades->mBytesPerPacket = 1;
            ades->mFramesPerPacket = 1;
            ades->mBytesPerFrame = 1;
            ades->mChannelsPerFrame = 1;
            ades->mBitsPerChannel = 8;
            ades->mReserved = 0;
            return true;
            break;
        default:
            break;
    }
    return false;
}

#pragma mark - public functions constructor/destructor destructor
CAudioConverter::CAudioConverter(CACAudioFormat inAF, CACAudioFormat outAF, PCAudioConverter_DataCallback cb, long cbuser)
: m_AudioConverterRef(NULL)
, m_AudioStreamBasicDescription_In(NULL)
, m_AudioStreamBasicDescription_Out(NULL)
, m_cb(cb)
, m_lCbuser(cbuser)
, m_szInputFrameBuf(NULL)
, m_nInputFrameBufDataLen(0)
, m_bFirstConvertedFrameDeliverd(false)
{
    AudioStreamBasicDescription* inades, * outades;

    inades = new AudioStreamBasicDescription;
    if (inades) {
        memset(inades, 0, sizeof(AudioStreamBasicDescription));
        FillAudioStreamBasicDescriptionByAF(inAF, inades);
        m_AudioStreamBasicDescription_In = (AudioStreamBasicDescription*)inades;
    }

    outades = new AudioStreamBasicDescription;
    if (outades) {
        memset(outades, 0, sizeof(AudioStreamBasicDescription));
        FillAudioStreamBasicDescriptionByAF(outAF, outades);
        m_AudioStreamBasicDescription_Out = (AudioStreamBasicDescription*)outades;
    }
}

CAudioConverter::~CAudioConverter()
{
    if (m_AudioConverterRef) {
       // CFRelease((AudioConverterRef)m_AudioConverterRef);
	   AudioConverterDispose((AudioConverterRef)m_AudioConverterRef);
    }
    if (m_AudioStreamBasicDescription_In) {
        delete (AudioStreamBasicDescription*)m_AudioStreamBasicDescription_In;
    }
    if (m_AudioStreamBasicDescription_Out) {
        delete (AudioStreamBasicDescription*)m_AudioStreamBasicDescription_Out;
    }
    if (m_szInputFrameBuf) {
        delete [] m_szInputFrameBuf;
    }
}

AudioClassDescription* GetAudioClassDescriptionWithType(UInt32 type, UInt32 manufacturer) { // 获得相应的编码器
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

struct inUserData__ {
    AudioBufferList* abfl;
    AudioStreamBasicDescription* ades;
};

OSStatus inputDataProc(AudioConverterRef inAudioConverter,
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

    inUserData__* iud = (inUserData__*)inUserData;
    AudioBufferList bufferList = *(AudioBufferList*)iud->abfl;

    ioData->mNumberBuffers = 1;
    ioData->mBuffers[0].mNumberChannels = 1;
    ioData->mBuffers[0].mData           = bufferList.mBuffers[0].mData;
    ioData->mBuffers[0].mDataByteSize   = bufferList.mBuffers[0].mDataByteSize;

    *ioNumberDataPackets = ioData->mBuffers[0].mDataByteSize/iud->ades->mBytesPerPacket;

    return noErr;
}

#pragma mark - public functions own
#define AUDIO_FRAMEDURATION  40/*ms*/
int CAudioConverter::PutData(const char* data, int len, int flags)
{
    int ir = -1;

    if (!data || len<1) {
        return ir;
    }

    if (!m_AudioStreamBasicDescription_In || !m_AudioStreamBasicDescription_Out) {
        return ir;
    }
    AudioStreamBasicDescription* _adesIn = (AudioStreamBasicDescription*)m_AudioStreamBasicDescription_In;
    AudioStreamBasicDescription* _adesOut = (AudioStreamBasicDescription*)m_AudioStreamBasicDescription_Out;

    if (m_AudioConverterRef == NULL)
    { // 初始化转换器
        AudioClassDescription *desc = GetAudioClassDescriptionWithType(_adesOut->mFormatID, kAppleHardwareAudioCodecManufacturer);
        AudioConverterRef _AudioConverterRef = NULL;
        if (AudioConverterNewSpecific(_adesIn, _adesOut, 1, desc, &_AudioConverterRef)!=noErr || _AudioConverterRef==NULL) {
            NSLog(@"CAudioConverter::PutData: AudioConverterNewSpecific failed");
            return ir;
        }
        m_AudioConverterRef = _AudioConverterRef;
    }

    int lenofaudioframe = _adesIn->mSampleRate*_adesIn->mBytesPerFrame*AUDIO_FRAMEDURATION/1000;
    int lenof3audioframe = 3*lenofaudioframe;
    if (!m_szInputFrameBuf) {
        int buflen = lenof3audioframe;
        if (buflen < len) {
            buflen = len;
        }
        buflen *= 2;
        m_szInputFrameBuf = new unsigned char[buflen];
        m_nInputFrameBufDataLen = 0;
    }

    // 拼帧
    memcpy(m_szInputFrameBuf+m_nInputFrameBufDataLen, data, len);
    m_nInputFrameBufDataLen += len;

    unsigned char* itertor = m_szInputFrameBuf;
    int leftlen = m_nInputFrameBufDataLen;
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
        int buflen = _adesOut->mSampleRate*_adesOut->mBytesPerFrame*3*AUDIO_FRAMEDURATION/1000;
        unsigned char* buf = new unsigned char[buflen];
        memset(buf, 0, buflen);

        // 构造一个输出缓冲列表
        AudioBufferList outBufferList;
        outBufferList.mNumberBuffers = 1;
        outBufferList.mBuffers[0].mNumberChannels = 1;
        outBufferList.mBuffers[0].mDataByteSize = buflen; // 设置输出缓冲区大小
        outBufferList.mBuffers[0].mData = buf; // 设置输出缓冲区

        UInt32 outputDataPacketSize = buflen/_adesOut->mBytesPerPacket;
        inUserData__ iud;
        memset(&iud, 0, sizeof(iud));
        iud.abfl = &inBufferList;
        iud.ades = _adesIn;
        OSStatus ossr = AudioConverterFillComplexBuffer((AudioConverterRef)m_AudioConverterRef, inputDataProc, &iud, &outputDataPacketSize, &outBufferList, NULL);
        if (ossr != noErr)
        {
            NSLog(@"CAudioConverter::PutData: AudioConverterFillComplexBuffer with %u bytes failed, r=%u.",
                  (unsigned int)inBufferList.mBuffers[0].mDataByteSize, (int)ossr);
            delete [] buf;
			buf=0;
            break;//kAudioConverterErr_InvalidInputSize
        }
        unsigned int outlen = outBufferList.mBuffers[0].mDataByteSize; //编码后的数据大小

        NSLog(@"CAudioConverter::PutData: AudioConverterFillComplexBuffer with %u bytes ok, outlen: %u.",
              (unsigned int)inBufferList.mBuffers[0].mDataByteSize, outlen);

        if (m_cb) {
            CAC_DataCallbackFrame frame;
            if (!m_bFirstConvertedFrameDeliverd) {
                m_bFirstConvertedFrameDeliverd = true;

                memset(&frame, 0, sizeof(frame));
                frame.data = buf;
                frame.len = outlen/3;
                m_cb(CAC_DCC_FRAME, (long)&frame, m_lCbuser);
            }
            memset(&frame, 0, sizeof(frame));
            frame.data = buf+outlen/3;
            frame.len = outlen/3;
            m_cb(CAC_DCC_FRAME, (long)&frame, m_lCbuser);
        }
        delete [] buf;
		buf=0;
    }

    if (leftlen>0 && itertor!=m_szInputFrameBuf) {
        memcpy(m_szInputFrameBuf, itertor, leftlen);
    }
    m_nInputFrameBufDataLen = leftlen;

    ir = 0;

    return ir;
}

#pragma mark - protected functions