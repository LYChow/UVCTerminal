//
//  SwH264Decoder.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/2/3.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef SwH264Decoder_h
#define SwH264Decoder_h

enum H264HD_DataCallbackCode {
    H264HD_DCC_DECODEFRAME = 0,   // param: CVImageBufferRef
};
typedef void (*PH264HwDecoderImpl_DataCallback)(int code, long param, long user);

class CH264HwDecoderImpl
{
#pragma mark - public functions constructor/destructor destructor
public:
    CH264HwDecoderImpl(PH264HwDecoderImpl_DataCallback m_pDataCallback, long m_lDataCallbackUser);
    virtual ~CH264HwDecoderImpl();

#pragma mark - public functions own
public:
    virtual void start();
    virtual void decode(int width, int height, const char* data, int len);
    virtual void stop();

    virtual void OnGotDecodeData(void* _CVImageBufferRef);

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    void* m_pH264HwDecoderImpl;
    PH264HwDecoderImpl_DataCallback m_pDataCallback;
    long m_lDataCallbackUser;
};

#endif /* SwH264Decoder_h */
