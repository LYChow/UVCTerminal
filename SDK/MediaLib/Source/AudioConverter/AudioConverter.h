//
//  AudioConverter.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/3/8.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef AudioConverter_h
#define AudioConverter_h

enum CACAudioFormat {
    CAC_AF_44k16bitPCM = 0,
    CAC_AF_8k16bitPCM,
    CAC_AF_ULAW,
};

struct CAC_DataCallbackFrame {
    unsigned char* data;
    int len;
};
enum CAC_DataCallbackCode {
    CAC_DCC_FRAME = 0,   // param: CAC_DataCallbackFrame*
};
typedef void (*PCAudioConverter_DataCallback)(int code, long param, long user);

class CAudioConverter
{
#pragma mark - public functions constructor/destructor destructor
public:
    CAudioConverter(CACAudioFormat inAF, CACAudioFormat outAF, PCAudioConverter_DataCallback cb, long cbuser);
    virtual ~CAudioConverter();

#pragma mark - public functions own
public:
    int PutData(const char* data, int len, int flags);

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    void* m_AudioConverterRef;
    void* m_AudioStreamBasicDescription_In;
    void* m_AudioStreamBasicDescription_Out;
    PCAudioConverter_DataCallback m_cb;
    long m_lCbuser;
    unsigned char* m_szInputFrameBuf;
    int m_nInputFrameBufDataLen;
    bool m_bFirstConvertedFrameDeliverd;
};

#endif /* AudioConverter_h */
