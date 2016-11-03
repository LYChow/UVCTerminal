//
//  CPreprocessor.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/21.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef CPreprocessor_h
#define CPreprocessor_h

#include "type.h"
#include "VideoSender.h"
#include "UnpackVideo.h"
#include "AudioSendManager.h"
#include "UnpackAudio.h"
#include "IDataDelegate.h"

namespace MediaLib{

class CPreprocessor : public IDataDelegate {
#pragma mark - public functions constructor/destructor destructor
public:
    CPreprocessor(int nDataDelegateType=DDT_NULL);
    virtual ~CPreprocessor();

#pragma mark - public functions inherits from
public:

#pragma mark - public functions inherits from
public:

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
};

class CPreprocessorPackVideo : public CPreprocessor {
#pragma mark - public functions constructor/destructor destructor
public:
    CPreprocessorPackVideo();
    virtual ~CPreprocessorPackVideo();

#pragma mark - public functions inherits from
public:

#pragma mark - public functions inherits from IDataDelegate
public:
    virtual int PutData(const char* data, int len, long mediaid, int format);

#pragma mark - public functions own
public:
    /*发送数据回调函数*/
    static __stdcall  long _VS_NETDATACALLBACK(const char *pbufsend, int sendlen, LONG dwUser);
    long ONVS_NETDATACALLBACK(const char *pbufsend, int sendlen);
    
#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CVideoSender* m_pVideoSender;
    unsigned int m_uWidth;
    unsigned int m_uHeight;
    DWORD m_dwFrameNo;
};

class CPreprocessorPackAudio : public CPreprocessor {
#pragma mark - public functions constructor/destructor destructor
public:
    CPreprocessorPackAudio();
    virtual ~CPreprocessorPackAudio();

#pragma mark - public functions inherits from
public:

#pragma mark - public functions inherits from IDataDelegate
public:
    virtual int PutData(const char* data, int len, long mediaid, int format);

#pragma mark - public functions own
public:
    /*发送数据回调函数*/
    static __stdcall long AudioSendToNet(const char *pData, int nLen, LONG dwUser);
    long OnAudioSendToNet(const char *pData, int nLen);

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CAudioSendManager* m_pAudioSend;
    DWORD m_dwFrameNo;
};

class CPreprocessorUnpackVideo : public CPreprocessor {
#pragma mark - public functions constructor/destructor destructor
public:
    CPreprocessorUnpackVideo();
    virtual ~CPreprocessorUnpackVideo();

#pragma mark - public functions inherits from
public:

#pragma mark - public functions inherits from IDataDelegate
public:
    virtual int PutData(const char* data, int len, long mediaid, int format);

#pragma mark - public functions own
public:
    static __stdcall int VideoFrameCallBack(char* pFrame, int nFrameLen,
                                  VIDEOFRAMEHEADER *pFrameHeader, FRAMECAPTIONHEADER *pCaptionHeader,
                                  char *szCaption, DWORD addr_remote, LONG dwUser);
    int OnVideoFrameCallBack(char* pFrame, int nFrameLen,
                             VIDEOFRAMEHEADER *pFrameHeader, FRAMECAPTIONHEADER *pCaptionHeader,
                                  char *szCaption, DWORD addr_remote);

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CUnpackVideo* m_pUnpackVideo;
};

class CPreprocessorUnpackAudio : public CPreprocessor {
#pragma mark - public functions constructor/destructor destructor
public:
    CPreprocessorUnpackAudio();
    virtual ~CPreprocessorUnpackAudio();

#pragma mark - public functions inherits from
public:

#pragma mark - public functions inherits from IDataDelegate
public:
    virtual int PutData(const char* data, int len, long mediaid, int format);

#pragma mark - public functions own
public:
static __stdcall int AudioFrameCallBack(char* pData, int nLength, int nAudioCodeCType, DWORD dwFrameNo, DWORD addr_remote, int nChannel, LONG dwUser);
int OnAudioFrameCallBack(char* pData, int nLength, int nAudioCodeCType, DWORD dwFrameNo, DWORD addr_remote, int nChannel);

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CUnpackAudio *m_pUnpackAudio;
};

}//end MediaLib




#endif /* CPreprocessor_h */
