//
//  CMediaConfig.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/13.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef CMediaConfig_h
#define CMediaConfig_h
#include <string.h>
#include "IMediaLib.h"

namespace MediaLib{

class CMediaConfigDelegate {
public:
    virtual void SetConfigCallback(IMediaConfigCallback* pCallback) {
        m_pMediaConfigCallback = pCallback;
    }
public:
    IMediaConfigCallback* m_pMediaConfigCallback;
};
class CVideoCaptureConfigDelegate : public CMediaConfigDelegate {
public:
    virtual int GetSupportedFormatCount()=0;
    virtual int GetCurrentFormat()=0;
    virtual int SetCurrentFormat(int iFmt)=0;
    virtual int GetFormatDesp(MediaFormatDesp* pDesp,int iFmt=-1)=0;
    virtual MFRATIO GetCurrentFrameRate()=0;
    virtual int SetCurrentFrameRate(MFRATIO rate)=0;
//    virtual int SnapshotPicture(const char* szImageFilePath,int iImageFileType=0)=0; //async call
};
class CAudioCaptureConfigDelegate : public CMediaConfigDelegate {
};
class CVideoRenderConfigDelegate : public CMediaConfigDelegate {
public:
    virtual int SetRenderMode(int mode)=0;//RenderMode
    virtual int GetRenderMode()=0;
    virtual int CaptureImage(LPCTSTR szFileName,int type=0)=0;
};
class CVideoCaptionConfigDelegate : public CMediaConfigDelegate {
};
class CAudioRenderConfigDelegate : public CMediaConfigDelegate {
};
class CNetSendConfigDelegate : public CMediaConfigDelegate {
public:
    virtual int SetFECType(int type,void* pParam)=0;
    virtual int GetFECType(void* pParam)=0;
    virtual int SetNATHost(const char* szHostAddr)=0;
    virtual int SetSmoothType(int type,void* pParam)=0;
    virtual int GetSmoothType(void* pParam)=0;
    virtual IMediaStreamConfig* GetInputStreamConfig(int iPort=0)=0;
};
class CNetRecvConfigDelegate : public CMediaConfigDelegate {
public:
};
class CVideoStreamConfigDelegate : public CMediaConfigDelegate {
public:
    virtual int GetSupportedFormatCount()=0;
    virtual int GetCurrentFormat()=0;
    virtual int SetCurrentFormat(int iFmt)=0;
    virtual int GetFormatDesp(MediaFormatDesp* pDesp,int iFmt=-1)=0;
    virtual int SetStreamPrivateData(int iName,void* pValue=0)=0;
    virtual int GetStreamPrivateData(int iName,void* pValue,int size)=0;
    virtual int SetBitRate(int BitRate)=0;
};
class CAudioStreamConfigDelegate : public CMediaConfigDelegate {
public:
};

class CVideoCaptureConfig : public IVideoCaptureConfig
{
#pragma mark -
#pragma mark - public functions constructor/destructor destructor
public:
    CVideoCaptureConfig(CVideoCaptureConfigDelegate* pDelegate)
    : m_pConfigDelegate(pDelegate) {
    }
    virtual ~CVideoCaptureConfig() {}

#pragma mark - public functions inherits from IMediaConfig
public:
    virtual void SetConfigCallback(IMediaConfigCallback* pCallback=0) {
        if (m_pConfigDelegate) {
            m_pConfigDelegate->SetConfigCallback(pCallback);
        }
    }

#pragma mark - public functions inherits from IMediaStreamConfig
public:
    virtual int GetSupportedFormatCount(){
        return m_pConfigDelegate->GetSupportedFormatCount();
    }
    virtual int GetCurrentFormat(){
        return m_pConfigDelegate->GetCurrentFormat();
    }
    virtual int SetCurrentFormat(int iFmt){
        return m_pConfigDelegate->SetCurrentFormat(iFmt);
    }
    virtual int GetFormatDesp(MediaFormatDesp* pDesp,int iFmt=-1){
        return m_pConfigDelegate->GetFormatDesp(pDesp, iFmt);
    }
    virtual int SetStreamPrivateData(int iName,void* pValue=0){return -1;}
    virtual int GetStreamPrivateData(int iName,void* pValue,int size){return -1;}
    virtual int SetBitRate(int BitRate){return -1;}
    virtual int StartRecord(const char* szRecordFilePath,int iRecordFileFormat=0){return -1;}
    virtual void StopRecord(){}

#pragma mark - public functions inherits from IVideoStreamConfig
public:
    virtual MFRATIO GetCurrentFrameRate(){
        return m_pConfigDelegate->GetCurrentFrameRate();
    }
    virtual int SetCurrentFrameRate(MFRATIO rate){
        return m_pConfigDelegate->SetCurrentFrameRate(rate);
    }

#pragma mark - public functions inherits from IVideoCaptureConfig
public:
    virtual int SnapshotPicture(const char* szImageFilePath,int iImageFileType=0){return -1;} //async call
    virtual int StartPreview(PWNDOBJ pViewWnd){return -1;}
    virtual void StopPreview(){}

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CVideoCaptureConfigDelegate* m_pConfigDelegate;
};

class CAudioCaptureConfig : public IAudioCaptureConfig
{
#pragma mark -
#pragma mark - public functions constructor/destructor destructor
public:
    CAudioCaptureConfig(CAudioCaptureConfigDelegate* pDelegate)
    : m_pConfigDelegate(pDelegate) {
    }
    virtual ~CAudioCaptureConfig() {}

#pragma mark - public functions inherits from IMediaConfig
public:
    virtual void SetConfigCallback(IMediaConfigCallback* pCallback=0) {
        if (m_pConfigDelegate) {
            m_pConfigDelegate->SetConfigCallback(pCallback);
        }
    }

#pragma mark - public functions inherits from IMediaStreamConfig
public:
    virtual int GetSupportedFormatCount(){return -1;}
    virtual int GetCurrentFormat(){return -1;}
    virtual int SetCurrentFormat(int iFmt){return -1;}
    virtual int GetFormatDesp(MediaFormatDesp* pDesp,int iFmt=-1){return -1;}
    virtual int SetStreamPrivateData(int iName,void* pValue=0){return -1;}
    virtual int GetStreamPrivateData(int iName,void* pValue,int size){return -1;}
    virtual int SetBitRate(int BitRate){return -1;}
    virtual int StartRecord(const char* szRecordFilePath,int iRecordFileFormat=0){return -1;}
    virtual void StopRecord(){}

#pragma mark - public functions inherits from IAudioStreamConfig
public:
    virtual MFRATIO GetCurrentSampleRate(){return -1;}
    virtual int SetCurrentSampleRate(MFRATIO rate){return -1;}
    virtual int SetGain(int val){return -1;}
    virtual int GetGain(){return -1;}

#pragma mark - public functions inherits from IAudioCaptureConfig
public:
    virtual int EnableAEC(bool bEnable=1){return -1;}
    virtual int EnableNoiseReduction(bool bEnable=1){return -1;}
    virtual int EnableAGC(bool bEnable=1){return -1;}

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CAudioCaptureConfigDelegate* m_pConfigDelegate;
};

class CVideoRenderConfig : public IVideoRenderConfig
{
#pragma mark -
#pragma mark - public functions constructor/destructor destructor
public:
    CVideoRenderConfig(CVideoRenderConfigDelegate* pDelegate)
    : m_pConfigDelegate(pDelegate) {
    }
    virtual ~CVideoRenderConfig() {}

#pragma mark - public functions inherits from IMediaConfig
public:
    virtual void SetConfigCallback(IMediaConfigCallback* pCallback=0) {
        if (m_pConfigDelegate) {
            m_pConfigDelegate->SetConfigCallback(pCallback);
        }
    }

#pragma mark - public functions inherits from IVideoRenderConfig
public:
    virtual int GetSourceSize(int& widht,int& height){return -1;}
    virtual int SetRenderSize(int widht,int height){return -1;}
    virtual int GetRenderSize(int& widht,int& height){return m_pConfigDelegate->GetRenderMode();}
    virtual int SetRenderMode(int mode){return m_pConfigDelegate->SetRenderMode(mode);}//RenderMode
    virtual int GetRenderMode(){return -1;}
    virtual int SetIdleBkColor(DWORD dwRGB){return -1;}
    virtual int SetIdleBitmap(LPVOID pBitmap){return -1;}
    virtual int SetBrightness(int percent){return -1;}
    virtual int GetBrightness(){return -1;}
    virtual int CaptureImage(LPCTSTR szFileName,int type=0){return -1;}

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CVideoRenderConfigDelegate* m_pConfigDelegate;
};

class CVideoCaptionConfig : public IVideoCaptionConfig
{
#pragma mark -
#pragma mark - public functions constructor/destructor destructor
public:
    CVideoCaptionConfig(CVideoCaptionConfigDelegate* pDelegate)
    : m_pConfigDelegate(pDelegate) {
    }
    virtual ~CVideoCaptionConfig() {}

#pragma mark - public functions inherits from IMediaConfig
public:
    virtual void SetConfigCallback(IMediaConfigCallback* pCallback=0) {
        if (m_pConfigDelegate) {
            m_pConfigDelegate->SetConfigCallback(pCallback);
        }
    }

#pragma mark - public functions inherits from IVideoCaptionConfig
public:
    virtual int GetCaptionCount(){return -1;}
    virtual int AddCaption(int x,int y,int width,int height,VideoCaptionStyle* pStyle,LPCTSTR szCaption){return -1;}
    virtual int RemoveCaption(int iCaption){return -1;}
    virtual int SetCaptionText(int iCaption,LPCTSTR szCaption){return -1;}
    virtual int SetCaptionStyle(int iCaption,VideoCaptionStyle* pStyle){return -1;}
    virtual int SetCaptionRect(int iCaption,int x,int y,int widht,int height){return -1;}
    virtual int GetCaptionRect(int iCaption,int& x,int& y,int& widht,int& height){return -1;}
    virtual VideoCaptionStyle* GetCaptionStyle(int iCaption){return NULL;}
    virtual LPCTSTR GetCaptionText(int iCaption){return NULL;}
    virtual int CalcCaptionSize(VideoCaptionStyle* pStyle,LPCTSTR szText,int& width,int& height){return -1;}

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CVideoCaptionConfigDelegate* m_pConfigDelegate;
};

class CAudioRenderConfig : public IAudioRenderConfig
{
#pragma mark -
#pragma mark - public functions constructor/destructor destructor
public:
    CAudioRenderConfig(CAudioRenderConfigDelegate* pDelegate)
    : m_pConfigDelegate(pDelegate) {
    }
    virtual ~CAudioRenderConfig() {}

#pragma mark - public functions inherits from IMediaConfig
public:
    virtual void SetConfigCallback(IMediaConfigCallback* pCallback=0) {
        if (m_pConfigDelegate) {
            m_pConfigDelegate->SetConfigCallback(pCallback);
        }
    }

#pragma mark - public functions inherits from IAudioRenderConfig
public:
    virtual int SetVoluem(int volume){return -1;}
    virtual int GetVoluem(){return -1;}

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CAudioRenderConfigDelegate* m_pConfigDelegate;
};

class CNetSendConfig : public INetSendConfig
{
#pragma mark -
#pragma mark - public functions constructor/destructor destructor
public:
    CNetSendConfig(CNetSendConfigDelegate* pDelegate)
    : m_pConfigDelegate(pDelegate) {
    }
    virtual ~CNetSendConfig() {}

#pragma mark - public functions inherits from IMediaConfig
public:
    virtual void SetConfigCallback(IMediaConfigCallback* pCallback=0) {
        if (m_pConfigDelegate) {
            m_pConfigDelegate->SetConfigCallback(pCallback);
        }
    }

#pragma mark - public functions inherits from INetStreamConfig
public:
    virtual int SetMuxFormat(int iMuxFormat){return -1;}
    virtual int GetMuxFormat(){return -1;}
    virtual int SetFECType(int type,void* pParam){return -1;}
    virtual int GetFECType(void* pParam){return -1;}
    virtual int SetNATHost(const char* szHostAddr){return -1;}

#pragma mark - public functions inherits from INetSendConfig
public:
    virtual int SetSmoothType(int type,void* pParam){return -1;}
    virtual int GetSmoothType(void* pParam){return -1;}
    virtual IMediaStreamConfig* GetInputStreamConfig(int iPort=0){
        return m_pConfigDelegate->GetInputStreamConfig();
    }

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CNetSendConfigDelegate* m_pConfigDelegate;
};

class CNetRecvConfig : public INetRecvConfig
{
#pragma mark -
#pragma mark - public functions constructor/destructor destructor
public:
    CNetRecvConfig(CNetRecvConfigDelegate* pDelegate)
    : m_pConfigDelegate(pDelegate) {
    }
    virtual ~CNetRecvConfig() {}

#pragma mark - public functions inherits from IMediaConfig
public:
    virtual void SetConfigCallback(IMediaConfigCallback* pCallback=0) {
        if (m_pConfigDelegate) {
            m_pConfigDelegate->SetConfigCallback(pCallback);
        }
    }

#pragma mark - public functions inherits from INetStreamConfig
public:
    virtual int SetMuxFormat(int iMuxFormat){return -1;}
    virtual int GetMuxFormat(){return -1;}
    virtual int SetFECType(int type,void* pParam){return -1;}
    virtual int GetFECType(void* pParam){return -1;}
    virtual int SetNATHost(const char* szHostAddr){return -1;}
    
#pragma mark - public functions inherits from INetRecvConfig
public:

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CNetRecvConfigDelegate* m_pConfigDelegate;
};

class CVideoStreamConfig : public IVideoStreamConfig
{
#pragma mark -
#pragma mark - public functions constructor/destructor destructor
public:
    CVideoStreamConfig(CVideoStreamConfigDelegate* pDelegate)
    : m_pConfigDelegate(pDelegate) {
    }
    virtual ~CVideoStreamConfig() {}

#pragma mark - public functions inherits from IMediaConfig
public:
    virtual void SetConfigCallback(IMediaConfigCallback* pCallback=0) {
        if (m_pConfigDelegate) {
            m_pConfigDelegate->SetConfigCallback(pCallback);
        }
    }

#pragma mark - public functions inherits from IMediaStreamConfig
public:
    virtual int GetSupportedFormatCount(){
        return m_pConfigDelegate->GetSupportedFormatCount();
    }
    virtual int GetCurrentFormat(){
        return m_pConfigDelegate->GetCurrentFormat();
    }
    virtual int SetCurrentFormat(int iFmt){
        return m_pConfigDelegate->SetCurrentFormat(iFmt);
    }
    virtual int GetFormatDesp(MediaFormatDesp* pDesp,int iFmt=-1){
        return m_pConfigDelegate->GetFormatDesp(pDesp, iFmt);
    }
    virtual int SetStreamPrivateData(int iName,void* pValue=0){
        return m_pConfigDelegate->SetStreamPrivateData(iName, pValue);
    }
    virtual int GetStreamPrivateData(int iName,void* pValue,int size){
        return m_pConfigDelegate->GetStreamPrivateData(iName, pValue, size);
    }
    virtual int SetBitRate(int BitRate){
        return m_pConfigDelegate->SetBitRate(BitRate);
    }

    virtual int StartRecord(const char* szRecordFilePath,int iRecordFileFormat=0){return -1;}
    virtual void StopRecord(){}

#pragma mark - public functions inherits from IVideoStreamConfig
public:
    virtual MFRATIO GetCurrentFrameRate(){return -1;}
    virtual int SetCurrentFrameRate(MFRATIO rate){return -1;}

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CVideoStreamConfigDelegate* m_pConfigDelegate;
};

class CAudioStreamConfig : public IAudioStreamConfig
{
#pragma mark -
#pragma mark - public functions constructor/destructor destructor
public:
    CAudioStreamConfig(CAudioStreamConfigDelegate* pDelegate)
    : m_pConfigDelegate(pDelegate) {
    }
    virtual ~CAudioStreamConfig() {}

#pragma mark - public functions inherits from IMediaConfig
public:
    virtual void SetConfigCallback(IMediaConfigCallback* pCallback=0) {
        if (m_pConfigDelegate) {
            m_pConfigDelegate->SetConfigCallback(pCallback);
        }
    }

#pragma mark - public functions inherits from IMediaStreamConfig
public:
    virtual int GetSupportedFormatCount(){return -1;}
    virtual int GetCurrentFormat(){return -1;}
    virtual int SetCurrentFormat(int iFmt){return -1;}
    virtual int GetFormatDesp(MediaFormatDesp* pDesp,int iFmt=-1){return -1;}
    virtual int SetStreamPrivateData(int iName,void* pValue=0){return -1;}
    virtual int GetStreamPrivateData(int iName,void* pValue,int size){return -1;}
    virtual int SetBitRate(int BitRate){return -1;}
//    virtual int GetSupportedFormatCount(){
//        return m_pConfigDelegate->GetSupportedFormatCount();
//    }
//    virtual int GetCurrentFormat(){
//        return m_pConfigDelegate->GetCurrentFormat();
//    }
//    virtual int SetCurrentFormat(int iFmt){
//        return m_pConfigDelegate->SetCurrentFormat(iFmt);
//    }
//    virtual int GetFormatDesp(MediaFormatDesp* pDesp,int iFmt=-1){
//        return m_pConfigDelegate->GetFormatDesp(pDesp, iFmt);
//    }
//    virtual int SetStreamPrivateData(int iName,void* pValue=0){
//        return m_pConfigDelegate->SetStreamPrivateData(iName, pValue);
//    }
//    virtual int GetStreamPrivateData(int iName,void* pValue,int size){
//        return m_pConfigDelegate->GetStreamPrivateData(iName, pValue, size);
//    }
//    virtual int SetBitRate(int BitRate){
//        return m_pConfigDelegate->SetBitRate(BitRate);
//    }

    virtual int StartRecord(const char* szRecordFilePath,int iRecordFileFormat=0){return -1;}
    virtual void StopRecord(){}

#pragma mark - public functions inherits from IAudioStreamConfig
public:
    virtual MFRATIO GetCurrentSampleRate(){return -1;}
    virtual int SetCurrentSampleRate(MFRATIO rate){return -1;}
    virtual int SetGain(int val){return -1;}
    virtual int GetGain(){return -1;}

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CAudioStreamConfigDelegate* m_pConfigDelegate;
};

}

#endif /* CMediaConfig_h */
