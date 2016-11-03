//
//  CVideoEncoder.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/19.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef CVideoEncoder_h
#define CVideoEncoder_h

#include "SwH264Encoder.h"
#include "IDataDelegate.h"
#include "CMediaConfig.h"

namespace MediaLib{

class CVideoEncoder : public IDataDelegate, public CVideoStreamConfigDelegate {
#pragma mark - public functions constructor/destructor destructor
public:
    CVideoEncoder();
    virtual ~CVideoEncoder();

#pragma mark - public functions inherits from IDataDelegate
public:
    virtual int PutData(const char* data, int len, long mediaid, int format);

#pragma mark - public functions inherits from CMediaStreamConfigDelegate
public:
    virtual int GetSupportedFormatCount();
    virtual int GetCurrentFormat();
    virtual int SetCurrentFormat(int iFmt);
    virtual int GetFormatDesp(MediaFormatDesp* pDesp,int iFmt=-1);
    virtual int SetStreamPrivateData(int iName,void* pValue=0);
    virtual int GetStreamPrivateData(int iName,void* pValue,int size);
    virtual int SetBitRate(int BitRate);

#pragma mark - public functions own
public:
    static void CH264HwEncoderImpl_DataCallback(int code, long param, long user);
    void OnCH264HwEncoderImpl_DataCallback(int code, long param);

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CH264HwEncoderImpl* m_pCH264HwEncoderImpl;
    int m_nBitrate;
};

}//end MediaLib

#endif /* CVideoEncoder_h */
