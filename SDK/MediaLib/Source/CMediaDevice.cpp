//
//  CMediaDevice.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/6.
//  Copyright © 2016年 华纬讯. All rights reserved.
//
#include "CMediaDevice.h"
#include "itzLog.h"

namespace MediaLib{

#pragma mark - public functions constructor/destructor destructor
CMediaDevice::CMediaDevice(int type, LPCTSTR name, long id)
: m_listSink(false)
{
    DeviceType = type;
    memset(DeviceName, 0, sizeof(DeviceName));
    strncpy(DeviceName, name, sizeof(DeviceName)-1);
    DeviceId = id;
    m_pConfig = NULL;
    m_lUserData = 0;
}

CMediaDevice::~CMediaDevice()
{
}

#pragma mark - public functions inherits from IMediaDevice
int CMediaDevice::GetType()
{
    return DeviceType;
}

LPCTSTR CMediaDevice::GetDeviceName()
{
    return DeviceName;
}

long CMediaDevice::GetDeviceId()
{
    return DeviceId;
}

#pragma mark - public functions own
int CMediaDevice::RegisterDataSink(IDataSink* pSink)
{
    int ir = -1;
    if (pSink) {
        if (m_listSink.GetCount() < 1) {
            Start();
        }
        if (m_listSink.AddTailHandle(pSink)) {
            ir = 0;
        }
    }
    return ir;
}

int CMediaDevice::UnregisterDataSink(IDataSink* pSink)
{
    int ir = -1;
    if (pSink && m_listSink.RemoveHandle(pSink)) {
        ir = 0;
        if (m_listSink.GetCount() < 1) {
            Stop();
        }
    }
    return ir;
}

#pragma mark - CMediaDevice::protected functions
int CMediaDevice::DispatchCapData(const char* buf, int size)
{
    int ir = -1;
    for (IDataSink* pSink=m_listSink.GetHeadHandle(); pSink; pSink=m_listSink.GetNextHandle(pSink)) {
        pSink->WriteData(buf, size);
        ir = 0;
    }
    return ir;
}


#pragma mark - public functions constructor/destructor destructor
MFSIZE g_szMFSIZE[] = {
    {196, 144},
    {320, 240},
    {352, 288},
    {640, 480},
    {960, 540},
    {1280, 720},
    {1920, 1080}
};
CMediaDeviceVideoCap::CMediaDeviceVideoCap(LPCTSTR name/*=NULL*/, long id/*=__device_id_VideoCap_front*/)
: CMediaDevice(MediaDeviceVideoCap, name, id)
, m_mfFrameRate(30)
, m_nVideoFormatDespCount(0)
, m_nCurrentVideoFormatDesp(-1)
{
    m_pCSwCameraOperationManager = NULL;
    m_pConfig = new CVideoCaptureConfig(this);
    SetCurrentFrameRate(m_mfFrameRate);
	ZeroVar(m_szVideoFormatDesp);
	for (int i=0; i<VFD_COUNT&&i<sizeof(g_szMFSIZE)/sizeof(MFSIZE); i++) {
		m_szVideoFormatDesp[i].dwSize=sizeof(VideoFormatDesp);
        m_szVideoFormatDesp[i].FormatType = vfRaw;
        m_szVideoFormatDesp[i].FrameSize = g_szMFSIZE[i];
        m_nVideoFormatDespCount++;
    }
}

CMediaDeviceVideoCap::~CMediaDeviceVideoCap()
{
    Stop();
}

#pragma mark - public functions inherits from CMediaDevice
int CMediaDeviceVideoCap::Start()
{
    int ir = -1;

    if (m_nCurrentVideoFormatDesp >= m_nVideoFormatDespCount) {
        if (m_nVideoFormatDespCount < 1) {
            return ir;
        }
        m_nCurrentVideoFormatDesp = 0;
    }

    if (!m_pCSwCameraOperationManager) {
        m_pCSwCameraOperationManager = new CSwCameraOperationManager();
        if (!m_pCSwCameraOperationManager) {
            return ir;
        }
        if (!(double(m_mfFrameRate)>.0)) {
            m_mfFrameRate.Numerator = 1;
            m_mfFrameRate.Denominator = 1;
        }

        bool br = m_pCSwCameraOperationManager->Init(m_szVideoFormatDesp[m_nCurrentVideoFormatDesp].FrameSize.width,
                                                     m_szVideoFormatDesp[m_nCurrentVideoFormatDesp].FrameSize.height,
                                                     m_mfFrameRate.Denominator, m_mfFrameRate.Numerator,
                                                     CameraOperationManager_DataCallback, (long)this,
                                                     NULL, (GetDeviceId()==__device_id_VideoCap_back));
        if (br) {
            ir = 0;
        }
    }

    return ir;
}

int CMediaDeviceVideoCap::Stop()
{
    int ir = -1;

    if (m_pCSwCameraOperationManager) {
        CSwCameraOperationManager* pCSwCameraOperationManager = m_pCSwCameraOperationManager;
        m_pCSwCameraOperationManager = NULL;
        delete pCSwCameraOperationManager;
		pCSwCameraOperationManager=0;
        ir = 0;
    }

    return ir;
}

#pragma mark - public functions inherits from CVideoCaptureConfigDelegate
int CMediaDeviceVideoCap::GetSupportedFormatCount()
{
    return m_nVideoFormatDespCount;
}

int CMediaDeviceVideoCap::GetCurrentFormat()
{
    return m_nCurrentVideoFormatDesp;
}

int CMediaDeviceVideoCap::SetCurrentFormat(int iFmt)
{
    int ir = -1;
    if (iFmt < 0
        || iFmt >= m_nVideoFormatDespCount) {
        return ir;
    }

    if (m_nCurrentVideoFormatDesp != iFmt) {
        m_nCurrentVideoFormatDesp = iFmt;
        if (m_pCSwCameraOperationManager) {
            Stop();
            Start();
        }
    }

    ir = 0;
    return ir;
}

int CMediaDeviceVideoCap::GetFormatDesp(MediaFormatDesp* pDesp,int iFmt)
{
    int ir = -1;
    if (!pDesp || pDesp->dwSize!=sizeof(VideoFormatDesp)
        || iFmt<0 || iFmt>=m_nVideoFormatDespCount) {
        return ir;
    }

    *((VideoFormatDesp*)pDesp) = m_szVideoFormatDesp[iFmt];

    ir = 0;
    return ir;
}

MFRATIO CMediaDeviceVideoCap::GetCurrentFrameRate()
{
    return m_mfFrameRate;
}

int CMediaDeviceVideoCap::SetCurrentFrameRate(MFRATIO rate)
{
    m_mfFrameRate = rate;
    if (!(double(m_mfFrameRate)>.0)) {
        m_mfFrameRate.Numerator = 1;
        m_mfFrameRate.Denominator = 1;
    }
    if (m_pCSwCameraOperationManager) {
        m_pCSwCameraOperationManager->SetMinFrameDuration(m_mfFrameRate.Denominator, m_mfFrameRate.Numerator);
    }
    return 0;
}

#pragma mark - protected functions
void CMediaDeviceVideoCap::CameraOperationManager_DataCallback(int code, long param, long user)
{
    CMediaDeviceVideoCap* pMediaDeviceVideoCap = (CMediaDeviceVideoCap*)user;
    if (pMediaDeviceVideoCap) {
        pMediaDeviceVideoCap->OnCameraOperationManager_DataCallback(code, param);
    }
}

void CMediaDeviceVideoCap::OnCameraOperationManager_DataCallback(int code, long param)
{
    switch (code) {
        case COM_DCC_CAPTUREFRAME:
        {
            void* _CVImageBufferRef = (void*)param;
            if (_CVImageBufferRef) {
                CVPixelBufferRefWapper wpr;
                wpr.pCVPixelBufferRef = _CVImageBufferRef;
                wpr.mfFrameRate = m_mfFrameRate;
                wpr.bNeedMirrorWhenDisplay = (DeviceId==__device_id_VideoCap_front);
                wpr.bNeedCopyWhenDisplay = true;
                for (IDataSink* pSink=m_listSink.GetHeadHandle(); pSink; pSink=m_listSink.GetNextHandle(pSink)) {
                    pSink->WriteData((const char*)&wpr, sizeof(wpr));
                }
            }
            break;
        }
        case COM_DCC_FINISHPICKIMAGE:
        {
            break;
        }

        default:
            break;
    }
}

#pragma mark - public functions constructor/destructor destructor
CMediaDeviceAudioCap::CMediaDeviceAudioCap(LPCTSTR name/*=NULL*/, long id/*=0*/)
: CMediaDevice(MediaDeviceAudioCap, name, id)
{
    m_pCSwMicOperationManager = NULL;
    m_pConfig = new CAudioCaptureConfig(this);
}

CMediaDeviceAudioCap::~CMediaDeviceAudioCap()
{
    Stop();
}

#pragma mark - public functions inherits from CMediaDevice
int CMediaDeviceAudioCap::Start()
{
    int ir = -1;

    if (!m_pCSwMicOperationManager) {
        m_pCSwMicOperationManager = new CSwMicOperationManager();
        if (!m_pCSwMicOperationManager) {
            return ir;
        }
        bool br = m_pCSwMicOperationManager->Init(MicOperationManager_DataCallback, (long)this);
        if (br) {
            ir = 0;
        }
    }

    return ir;
}

int CMediaDeviceAudioCap::Stop()
{
    int ir = -1;

    if (m_pCSwMicOperationManager) {
        //CSwMicOperationManager* pCSwMicOperationManager = m_pCSwMicOperationManager;
        //m_pCSwMicOperationManager = NULL;
        //delete pCSwMicOperationManager;
		m_pCSwMicOperationManager->Fini();
		delete m_pCSwMicOperationManager;
		m_pCSwMicOperationManager = 0;
        ir = 0;
    }

    return ir;
}

#pragma mark - protected functions
void CMediaDeviceAudioCap::MicOperationManager_DataCallback(int code, long param, long user)
{
    CMediaDeviceAudioCap* pMediaDeviceAudioCap = (CMediaDeviceAudioCap*)user;
    if (pMediaDeviceAudioCap) {
        pMediaDeviceAudioCap->OnMicOperationManager_DataCallback(code, param);
    }
}

void CMediaDeviceAudioCap::OnMicOperationManager_DataCallback(int code, long param)
{
    switch (code) {
        case MOM_DCC_CAPTUREFRAME:
        {
            MOM_DataCallbackFrame* pframe = (MOM_DataCallbackFrame*)param;
            if (pframe) {
#ifdef DEBUG222
                static FILE* filePCM8k16bit = NULL;

                if (!filePCM8k16bit) {
                    char szBuf[512] = {0};
                    int ir = GetDocumentDirectory(szBuf, sizeof(szBuf)-1);
                    if (ir > 0) {
                        snprintf(szBuf+ir, sizeof(szBuf)-ir-1, "/capture_audio8k16b.pcm");
                        filePCM8k16bit = fopen(szBuf, "w");
                    }
                }
                if (filePCM8k16bit) {
                    fwrite((const void*)pframe->pcm8k16bit, pframe->len, 1, filePCM8k16bit);
                }
#endif //#ifdef DEBUG
                for (IDataSink* pSink=m_listSink.GetHeadHandle(); pSink; pSink=m_listSink.GetNextHandle(pSink)) {
                    pSink->WriteData((const char*)pframe->pcm8k16bit, pframe->len);
                }
            }
        }
            break;

        default:
            break;
    }
}

#pragma mark - public functions constructor/destructor destructor
CMediaDeviceVideoWindow::CMediaDeviceVideoWindow(LPCTSTR name/*=NULL*/, long id/*=0*/)
: CMediaDevice(MediaDeviceVideoWindow, name, id)
, m_CSwDisplayWindow((void*)id)
, m_nStatus(0)
//, m_nRenderMode(0)
{
    strncpy(m_szName, name, sizeof(m_szName)-1);
    m_pConfig = new CVideoRenderConfig(this);
    RenderMode* rm = (RenderMode*)&m_nRenderMode;
    RenderMode Rm;
    *rm = Rm;
    SetRenderMode(m_nRenderMode);
}

CMediaDeviceVideoWindow::~CMediaDeviceVideoWindow()
{
    Stop();
}

#pragma mark - public functions inherits from CMediaDevice
int CMediaDeviceVideoWindow::WriteData(const char* buf,int size)
{
    int ir = -1;
    if (m_nStatus != 1) {
        return ir;
    }

    if (!buf || size!=sizeof(CVPixelBufferRefWapper)) {
        return ir;
    }

    CVPixelBufferRefWapper* pWpr = (CVPixelBufferRefWapper*)buf;
    ir = m_CSwDisplayWindow.PutData((char*)pWpr->pCVPixelBufferRef,
                                    0|(pWpr->bNeedMirrorWhenDisplay?SDWPDF_MIRROR:0)|(pWpr->bNeedCopyWhenDisplay?SDWPDF_COPY:0));

    return ir;
}

int CMediaDeviceVideoWindow::Start()
{
    if (m_nStatus != 1) {
        m_nStatus = 1;
    }

    return 0;
}

int CMediaDeviceVideoWindow::Stop()
{
    if (m_nStatus != 0) {
        m_nStatus = 0;
        return 0;
    }

    return -1;
}

#pragma mark - public functions inherits from CVideoRenderConfigDelegate
int CMediaDeviceVideoWindow::SetRenderMode(int mode)
{
    m_nRenderMode = mode;
    RenderMode* rm = (RenderMode*)&m_nRenderMode;
    m_CSwDisplayWindow.Mirror(rm->Mirror);
    return 0;
}

int CMediaDeviceVideoWindow::GetRenderMode()
{
    return m_nRenderMode;
}

int CMediaDeviceVideoWindow::CaptureImage(LPCTSTR szFileName,int type=0){return -1;}

#pragma mark - public functions constructor/destructor destructor
CMediaDeviceAudioOut::CMediaDeviceAudioOut(LPCTSTR name/*=NULL*/, long id/*=0*/)
: CMediaDevice(MediaDeviceAudioOut, name, id)
, m_pCSwAudioPlayer(NULL)
{
    m_pConfig = new CAudioRenderConfig(this);
}

CMediaDeviceAudioOut::~CMediaDeviceAudioOut()
{
    if (m_pCSwAudioPlayer) {
        delete m_pCSwAudioPlayer;
		m_pCSwAudioPlayer=0;
    }
}

#pragma mark - public functions inherits from CMediaDevice
int CMediaDeviceAudioOut::WriteData(const char* buf,int size)
{
    int ir = -1;

    if (!m_pCSwAudioPlayer) {
        return ir;
    }

    ir = m_pCSwAudioPlayer->PutData(buf, size, 0);

    return ir;
}

int CMediaDeviceAudioOut::Start()
{
    int ir = -1;
    if (!m_pCSwAudioPlayer) {
        m_pCSwAudioPlayer = new CSwAudioPlayer;
        if (!m_pCSwAudioPlayer) {
            return ir;
        }
    }

    ir = 0;
    
    return ir;
}

int CMediaDeviceAudioOut::Stop()
{
    if (m_pCSwAudioPlayer) {
        delete m_pCSwAudioPlayer;
		m_pCSwAudioPlayer=0;
    }
    return 0;
}


#pragma mark - public functions own

#pragma mark - protected functions

}//end MediaLib