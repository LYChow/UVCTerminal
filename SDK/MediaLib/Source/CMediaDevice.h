//
//  CMediaDevice.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/6.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef CMediaDevice_h
#define CMediaDevice_h
#include <string.h>
#include "IMediaDef.h"
#include "MyHandleList.h"
#include "SwCameraController.h"
#include "SwDisplayWindow.h"
#include "SwMicController.h"
#include "SwAudioPlayer.h"
#include "CMediaConfig.h"

namespace MediaLib{

struct CVPixelBufferRefWapper {
    void* pCVPixelBufferRef;
    MFRATIO mfFrameRate;
    bool bNeedMirrorWhenDisplay;
    bool bNeedCopyWhenDisplay;
    CVPixelBufferRefWapper()
    : pCVPixelBufferRef(NULL)
    , mfFrameRate(30)
    , bNeedMirrorWhenDisplay(false)
    , bNeedCopyWhenDisplay(true)
    {
    }
};

class CMediaDevice : public IMediaDevice {
#pragma mark - public functions constructor/destructor destructor
public:
    CMediaDevice(int type=MediaDeviceUnknown, LPCTSTR name=NULL, long id=0);
    virtual ~CMediaDevice();

#pragma mark - public functions inherits from IMediaDevice
public:
    virtual int GetType();
    virtual LPCTSTR GetDeviceName();
    virtual long GetDeviceId();
    virtual void* GetObject() {
        return this;
    }

#pragma mark - public functions inherits from IObject
public:
    virtual long SetUserData(long lUserData) {
        m_lUserData = lUserData;
        return 0;
    }
    virtual long GetUserData() {
        return m_lUserData;
    }

#pragma mark - public functions own
public:
    virtual IMediaConfig* GetConfig(){return m_pConfig;}
    virtual int RegisterDataSink(IDataSink* pSink);
    virtual int UnregisterDataSink(IDataSink* pSink);
    virtual int WriteData(const char* buf,int size){return -1;}
    virtual int Start()=0;
    // virtual int Pause()=0;
    virtual int Stop()=0;

#pragma mark - protected functions
protected:
    virtual int DispatchCapData(const char* buf, int size);

#pragma mark - protected members
protected:
    int DeviceType;
    char DeviceName[260];
    long DeviceId;
    IMediaConfig* m_pConfig;
    MyHandleList<IDataSink*> m_listSink;
    long m_lUserData;
};

enum __device_id_VideoCap {
__device_id_VideoCap_front = 0,
__device_id_VideoCap_back,
};
class CMediaDeviceVideoCap : public CMediaDevice, public CVideoCaptureConfigDelegate {
#pragma mark - public functions constructor/destructor destructor
public:
    CMediaDeviceVideoCap(LPCTSTR name=NULL, long id=__device_id_VideoCap_front);
    virtual ~CMediaDeviceVideoCap();

#pragma mark - public functions inherits from CMediaDevice
public:
//    virtual IMediaConfig* GetConfig(){return this;}
    virtual int Start();
//    virtual int Pause();
    virtual int Stop();

#pragma mark - public functions inherits from CVideoCaptureConfigDelegate
public:
    virtual int GetSupportedFormatCount();
    virtual int GetCurrentFormat();
    virtual int SetCurrentFormat(int iFmt);
    virtual int GetFormatDesp(MediaFormatDesp* pDesp,int iFmt=-1);
    virtual MFRATIO GetCurrentFrameRate();
    virtual int SetCurrentFrameRate(MFRATIO rate);

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:
    static void CameraOperationManager_DataCallback(int code, long param, long user);
    void OnCameraOperationManager_DataCallback(int code, long param);

#pragma mark - protected members
protected:
    CSwCameraOperationManager* m_pCSwCameraOperationManager;
    MFRATIO m_mfFrameRate;
#define VFD_COUNT   128
    VideoFormatDesp m_szVideoFormatDesp[VFD_COUNT];
    int m_nVideoFormatDespCount;
    int m_nCurrentVideoFormatDesp;
};

class CMediaDeviceAudioCap : public CMediaDevice, public CAudioCaptureConfigDelegate {
#pragma mark - public functions constructor/destructor destructor
public:
    CMediaDeviceAudioCap(LPCTSTR name=NULL, long id=0);
    virtual ~CMediaDeviceAudioCap();

#pragma mark - public functions inherits from CMediaDevice
public:
    virtual int Start();
//    virtual int Pause();
    virtual int Stop();

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:
    static void MicOperationManager_DataCallback(int code, long param, long user);
    void OnMicOperationManager_DataCallback(int code, long param);

#pragma mark - protected members
protected:
    CSwMicOperationManager* m_pCSwMicOperationManager;
};

class CMediaDeviceVideoWindow : public CMediaDevice, public CVideoRenderConfigDelegate {
#pragma mark - public functions constructor/destructor destructor
public:
    CMediaDeviceVideoWindow(LPCTSTR name=NULL, long id=0);
    virtual ~CMediaDeviceVideoWindow();

#pragma mark - public functions inherits from CMediaDevice
public:
    virtual int WriteData(const char* buf,int size);
    virtual int Start();
//    virtual int Pause();
    virtual int Stop();

#pragma mark - public functions inherits from CVideoRenderConfigDelegate
public:
    virtual int SetRenderMode(int mode);//RenderMode
    virtual int GetRenderMode();
    virtual int CaptureImage(LPCTSTR szFileName,int type);

#pragma mark - public functions own
public:
    
#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CSwDisplayWindow m_CSwDisplayWindow;
    char m_szName[256];
    int m_nStatus; // 0 idle, 1 working, 2 paused
    int m_nRenderMode;
};

class CMediaDeviceAudioOut : public CMediaDevice, public CAudioRenderConfigDelegate {
#pragma mark - public functions constructor/destructor destructor
public:
    CMediaDeviceAudioOut(LPCTSTR name=NULL, long id=0);
    virtual ~CMediaDeviceAudioOut();

#pragma mark - public functions inherits from CMediaDevice
public:
    virtual int WriteData(const char* buf,int size);
    virtual int Start();
//    virtual int Pause();
    virtual int Stop();

#pragma mark - public functions own
public:
    
#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
CSwAudioPlayer* m_pCSwAudioPlayer;
};

}//end MediaLib


#endif /* CMediaDevice_h */
