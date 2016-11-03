//
//  CVideoDecoder.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/21.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef CVideoDecoder_h
#define CVideoDecoder_h

#include "SwH264Decoder.h"
#include "IDataDelegate.h"

namespace MediaLib{

class CVideoDecoder : public IDataDelegate {
#pragma mark - public functions constructor/destructor destructor
public:
    CVideoDecoder();
    virtual ~CVideoDecoder();

#pragma mark - public functions inherits from
public:

#pragma mark - public functions inherits from IDataDelegate
public:
    virtual int PutData(const char* data, int len, long mediaid, int format);

#pragma mark - public functions own
public:
    static void CH264HwDecoderImpl_DataCallback(int code, long param, long user);
    void OnCH264HwDecoderImpl_DataCallback(int code, long param);

#pragma mark - protected functions
protected:
    void Release();

#pragma mark - protected members
protected:
    CH264HwDecoderImpl* m_pCH264HwDecoderImpl;
    unsigned int m_uWidth;
    unsigned int m_uHeight;
};

}//end MediaLib


#endif /* CVideoDecoder_h */
