//
//  SwEncoder.h
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/31.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#ifndef SwEncoder_h
#define SwEncoder_h

struct H264HE_DataCallbackFrame {
    unsigned char* buf;
    int size;
    bool keyframe;
};
enum H264HE_DataCallbackCode {
    H264HE_DCC_FRAME = 0,   // param: H264HE_DataCallbackFrame*
};
typedef void (*PCH264HwEncoderImpl_DataCallback)(int code, long param, long user);

class CH264HwEncoderImpl
{
#pragma mark - public functions
public:
    CH264HwEncoderImpl(PCH264HwEncoderImpl_DataCallback cb, long cbuser);
    virtual ~CH264HwEncoderImpl();

public:
    int Init(int width, int height, int frameduration_value, int frameduration_timescale, int bitrate);
    int PutData(const char* data, int frameduration_value, int frameduration_timescale, int flags);
    int SetBitrate(int bitrate);
    int ForceKeyFrame();
    void Release();

public:
    void OnEncodedDataGot(const char* data, int len, int type); // type: 0 P, 1 IDR, 2 sps, 3 pps

#pragma mark - protected functions
protected:
    void AsureFrameBufSize(int newdatalen);

#pragma mark - protected members
protected:
    int m_nWidth;
    int m_nHeight;
    int m_nFrameduration_value;
    int m_nFrameduration_timescale;
    int m_nBitrate;

    void* m_pH264HwEncoderImpl;
    PCH264HwEncoderImpl_DataCallback m_pDataCallback;
    long m_lDataCallbackUser;

    char* m_pFrameBuf;
    int m_nFrameBufSize;
    int m_nPreDataLen;
    bool m_bPreDataReady;
};

#endif /* SwEncoder_h */
