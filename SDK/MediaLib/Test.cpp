//
//  Test.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/29.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#include <stdio.h>
#include "Test.h"

//#include "type.h"
#include "CaptionDef.h"
#include "UnpackVideo.h"
#include "VideoSender.h"
#include "UnpackAudio.h"
#include "AudioSendManager.h"
#include "SwCameraController.h"
#include "SwH264Encoder.h"

#include "IMediaLib.h"

using namespace::MediaLib;

#pragma mark - 测试代码
//#ifdef DEBUG

class TestFunClassPreprocessor
{
public:
    TestFunClassPreprocessor() {
        m_pUnpackVideo = NULL;
        m_pVideoSender = NULL;
        m_pUnpackAudio = NULL;
        m_pAudioSend = NULL;
    }
    ~TestFunClassPreprocessor() {
        if (NULL != m_pVideoSender)
        {
            delete m_pVideoSender;
            m_pVideoSender = NULL;
        }
        if (NULL != m_pUnpackVideo)
        {
            delete m_pUnpackVideo;
            m_pUnpackVideo = NULL;
        }
        if (NULL != m_pUnpackAudio)
        {
            delete m_pUnpackAudio;
            m_pUnpackAudio = NULL;
        }
        if (NULL != m_pAudioSend)
        {
            delete m_pAudioSend;
            m_pAudioSend = NULL;
        }
    }

public:
    static __stdcall long VideoSendToNet(const char *pbufsend, int sendlen, LONG dwUser) {
        TestFunClassPreprocessor &This = *(TestFunClassPreprocessor *)dwUser;
        if (This.m_pUnpackVideo == NULL)
        {
            This.m_pUnpackVideo = new CUnpackVideo(0);
            This.m_pUnpackVideo->SetFrameDataCallBack(VideoFrameCallBack, dwUser);
        }

        This.m_pUnpackVideo->InputData((char *)pbufsend, sendlen);

        return 0;
    }
    static __stdcall int VideoFrameCallBack(char* pFrame, int nFrameLen, VIDEOFRAMEHEADER *pFrameHeader, FRAMECAPTIONHEADER *pCaptionHeader,
                                  char *szCaption, DWORD addr_remote, LONG dwUser) {
        if (NULL == pFrameHeader)
        {
            return -1;
        }
        TRACE("nFrameLen  = %d\n", nFrameLen);
        return 0;
    }

    static __stdcall long AudioSendToNet(const char *pData, int nLen, LONG dwUser) {
        TestFunClassPreprocessor &This = *(TestFunClassPreprocessor *)dwUser;
        if (NULL == This.m_pUnpackAudio)
        {
            This.m_pUnpackAudio = new CUnpackAudio();
            This.m_pUnpackAudio->SetFrameDataCallBack(AudioFrameCallBack, dwUser);
        }
        return This.m_pUnpackAudio->InputData((char *)pData, nLen);
    }
    static __stdcall int AudioFrameCallBack(char* pData, int nLength, int nAudioCodeCType, DWORD dwFrameNo, DWORD addr_remote, int nChannel, LONG dwUser) {
        TRACE("AudioFrameCallBack nLength = %d, nAudioCodeCType = %d, dwFrameNo = %d\n", nLength, nAudioCodeCType, dwFrameNo);
        return 0;
    }
    void TestVideo() {
        if (NULL == m_pVideoSender)
        {
            m_pVideoSender = new CVideoSender();
            m_pVideoSender->Init(0xC702A8C0, SWVideoSendMask_FEC| SWVideoSendMask_Jitter, FECMatrix_RS_Base + 5, "zds(zh)9_0123456789_01234567890", CODEC_H264, VideoSendToNet, (LONG)this);
        }

        int nFrameNo = 0;
        for (nFrameNo = 0; nFrameNo < 10; nFrameNo++)
        {
            int nFrameDataLen = nFrameNo*500 + 100;
            BYTE *pFrameData = new BYTE[nFrameDataLen];
            pFrameData[0] = 0x00;
            pFrameData[1] = 0x00;
            pFrameData[2] = 0x00;
            pFrameData[3] = 0x01;
            for (int i = 4; i < nFrameDataLen; i++)
            {
                pFrameData[i] = (BYTE)(i + 10*nFrameNo);
            }
            m_pVideoSender->InputData((char *)pFrameData, nFrameDataLen, 704, 576, nFrameNo, nFrameNo%100, 25, 1024, nFrameNo*40);
            delete []pFrameData;
        }
    }
    void TestAudio() {
        if (NULL == m_pAudioSend)
        {
            m_pAudioSend = new CAudioSendManager();
            m_pAudioSend->Init(0xC702A8C0, CODEC_G711U, TRUE, FECMatrix_RS_Base + 5, AudioSendToNet, (LONG)this);
        }

        int nFrameNo = 0;
        for (nFrameNo = 0; nFrameNo < 6; nFrameNo++)
        {
            int nFrameDataLen = nFrameNo*100 + 100;
            BYTE *pFrameData = new BYTE[nFrameDataLen];
            for (int i = 0; i < nFrameDataLen; i++)
            {
                pFrameData[i] = (BYTE)(i + 10*nFrameNo);
            }
            m_pAudioSend->InputData((char *)pFrameData, nFrameDataLen, nFrameNo);
            delete []pFrameData;
        }
    }

protected:
    CUnpackVideo *m_pUnpackVideo;
    CVideoSender *m_pVideoSender;
    CUnpackAudio *m_pUnpackAudio;
    CAudioSendManager *m_pAudioSend;
};

TestFunClassPreprocessor* g_pT = NULL;
void TestFunPreprocessor(int n, long p1, long p2)
{
    printf("TestFun %d.\n", n);
    if (!g_pT) {
        g_pT = new TestFunClassPreprocessor();
        if (!g_pT) {
            printf("TestFun new TestFunClass() return NULL.\n");
            return;
        }
    }

    switch (n) {
        case TFI_RESET:
            printf("Reset!\n");
            if (g_pT) {
                delete g_pT;
                g_pT = NULL;
            }
            break;
        case TFI_PREPROCESS_VIDEO:
            printf("Test Video!\n");
            g_pT->TestVideo();
            break;
        case TFI_PREPROCESS_AUDIO:
            printf("Test Audio!\n");
            g_pT->TestAudio();
            break;

        default:
            break;
    }
}

CSwCameraOperationManager com;
BOOL bCapturing = FALSE;
void TestFunCapture(int n, long p1, long p2)
{
    if (!bCapturing) {
        bCapturing = TRUE;
        com.Init(1920, 1080, 1, 30, NULL, 0, (void*)p1, false);
    }
    else
    {
        com.SwitchCamera();
    }
}

void CH264HwEncoderImpl_DataCallback(int code, long param, long user)
{
}

void TestFunEncode(int n, long p1, long p2)
{
    CH264HwEncoderImpl chhei(CH264HwEncoderImpl_DataCallback, 0);
}

void TestFunMediaLibObj(int n, long p1, long p2)
{
    MediaLib::IMediaLib* pLib = MediaLib::CreateMediaLib();

#if 0

    MediaLib::IMediaProcessor* pProcessor = pLib->CreateMediaProcessor(1,1,fmtDeviceVideo,fmtVC3NetVideo);
    pProcessor->SetInputStorageObject(pLib->GetDevice(MediaDeviceVideoCap, 0), 0, 0);
    pProcessor->Start();
    Sleep(1000*20);
#else

    // 创建视频采集/编码/发送 Processor
    MediaLib::IMediaProcessor* pProcessor = pLib->CreateMediaProcessor(1,0,fmtDeviceVideo,MediaFormatMask(0,MediaStorageProcessor,1,VC3PackedVideo,MediaTypeVideo));
    pProcessor->SetInputStorageObject(pLib->GetDevice(MediaDeviceVideoCap, 0), 0, 0);

///

    // 添加预览设备
    int nPreviewDevIndex = pLib->AddVideoPlayWindow((PWNDOBJ)p2);
    MediaLib::IMediaDevice* pPreview = pLib->GetDevice(MediaDeviceVideoWindow, nPreviewDevIndex);
    int nPreviewOutPortIndex = pProcessor->AddOutput(fmtDeviceVideo);
    pProcessor->SetOutputStorageObject(pPreview, 0, nPreviewOutPortIndex);

///

    // 添加显示设备
    int nMonitorDevIndex = pLib->AddVideoPlayWindow((PWNDOBJ)p1);
    MediaLib::IMediaDevice* pMonitor = pLib->GetDevice(MediaDeviceVideoWindow, nMonitorDevIndex);
    int nVC3PackedOutPortIndex = pProcessor->AddOutput(MediaFormatMask(0,MediaStorageProcessor,1,VC3PackedVideo,MediaTypeVideo));

    // 创建视频接收/解码/显示 Processor
    MediaLib::IMediaProcessor* pProcessor2 = pLib->CreateMediaProcessor(1,1,MediaFormatMask(0,MediaStorageProcessor,1,VC3PackedVideo,MediaTypeVideo),fmtDeviceVideo);
    pProcessor2->SetInputStorageObject(pProcessor, nVC3PackedOutPortIndex, 0);
    pProcessor2->SetOutputStorageObject(pMonitor, 0, 0);

    // 获取视频采集配置对象
    IVideoCaptureConfig* vccfg = dynamic_cast<IVideoCaptureConfig*>(pProcessor->GetConfig(mcfgVideoCap));
    if (vccfg) {
        vccfg->SetCurrentFrameRate(30); // 设置视频采集编码帧率

        int nCaptureWidth = 1280;
        int nCaptureHeight = 720;
        // 设置采集尺寸
        int nCapturePixelCount = nCaptureWidth*nCaptureHeight;
        int nSupportedFormatCount = vccfg->GetSupportedFormatCount();
        if (nSupportedFormatCount > 0) {
            int nFormatIndexFitFrameSize = -1;
            int nPixelCountFitFrameSize = 0;
            for (int i=0; i<nSupportedFormatCount; i++) { // 枚举支持格式
                VideoFormatDesp vfd;
                if (vccfg->GetFormatDesp(&vfd, i) == 0)
                {
                    printf("GetFormatDesp got (%dx%d).\n", vfd.FrameSize.width, vfd.FrameSize.height);
                    int nPixelCount = vfd.FrameSize.width*vfd.FrameSize.height;
                    if (nPixelCount<=nCapturePixelCount && nPixelCount>nPixelCountFitFrameSize) {
                        nPixelCountFitFrameSize = nPixelCount;
                        nFormatIndexFitFrameSize = i;
                    }
                }
            }
            if (nFormatIndexFitFrameSize > -1
                && nFormatIndexFitFrameSize != vccfg->GetCurrentFormat()) {
                vccfg->SetCurrentFormat(nFormatIndexFitFrameSize);
            }
        }
    }

    // 获取视频发送配置对象
    INetSendConfig* nscfg = dynamic_cast<INetSendConfig*>(pProcessor->GetConfig(mcfgNetSend));
    if (nscfg) {
        // 获取视频编码配置对象
        IMediaStreamConfig* mscfg = nscfg->GetInputStreamConfig();
        if (mscfg) {
            mscfg->SetBitRate(256); // 设置视频编码带宽
            mscfg->SetStreamPrivateData(MSPD_VideoCoderKeyFrameIntval, (void*)(-1)); // 强插关键帧
        }
    }

    // 获取视频显示配置对象
    IVideoRenderConfig* vrcfg = dynamic_cast<IVideoRenderConfig*>(pProcessor2->GetConfig(mcfgVideoRender));
    if (vrcfg) {
        int md;
        RenderMode* rm = (RenderMode*)&md;
        rm->Mirror = false; // 填充显示模式（镜像显示）
        vrcfg->SetRenderMode(md); // 设置显示模式
    }

    // 启动 Processor
    pProcessor2->Start();

///

    // 启动 Processor
    pProcessor->Start();


//    Sleep(1000*20);
//    pProcessor2->Release();
#endif
    
//    pProcessor->Release();
//    pLib->Release();
}

void TestFunMediaLibObjAudo(int n, long p1, long p2)
{
    MediaLib::IMediaLib* pLib = MediaLib::CreateMediaLib();

    MediaLib::IMediaProcessor* pProcessor = pLib->CreateMediaProcessor(1,0,fmtDeviceAudio,MediaFormatMask(0,MediaStorageProcessor,1,VC3PackedAudio,MediaTypeAudio));
    pProcessor->SetInputStorageObject(pLib->GetDevice(MediaDeviceAudioCap, 0), 0, 0);

///

//    MediaLib::IMediaDevice* pPreview = pLib->GetDevice(MediaDeviceAudioOut, 0);
//    int nPreviewOutPortIndex = pProcessor->AddOutput(fmtDeviceAudio);
//    pProcessor->SetOutputStorageObject(pPreview, 0, nPreviewOutPortIndex);

///

    MediaLib::IMediaDevice* pMonitor = pLib->GetDevice(MediaDeviceAudioOut, 0);
    int nVC3PackedOutPortIndex = pProcessor->AddOutput(MediaFormatMask(0,MediaStorageProcessor,1,VC3PackedAudio,MediaTypeAudio));

    MediaLib::IMediaProcessor* pProcessor2 = pLib->CreateMediaProcessor(1,1,MediaFormatMask(0,MediaStorageProcessor,1,VC3PackedAudio,MediaTypeAudio),fmtDeviceAudio);
    pProcessor2->SetInputStorageObject(pProcessor, nVC3PackedOutPortIndex, 0);
    pProcessor2->SetOutputStorageObject(pMonitor, 0, 0);

    pProcessor2->Start();

///

    pProcessor->Start();


//    Sleep(1000*20);
//    pProcessor2->Release();

//    pProcessor->Release();
//    pLib->Release();
}

void TestFun(int n, long p1/*=0*/, long p2/*=0*/) {
    switch (n) {
        case TFI_RESET:
            TestFunPreprocessor(n, p1, p2);
            break;
        case TFI_PREPROCESS_VIDEO:
        case TFI_PREPROCESS_AUDIO:
            TestFunPreprocessor(n, p1, p2);
            break;
        case TFI_CAPTURE:
            TestFunCapture(n, p1, p2);
            break;
        case TFI_ENCODE:
            TestFunEncode(n, p1, p2);
            break;
        case TFI_MEDIALIBOBJ:
            TestFunMediaLibObj(n, p1, p2);
            break;
        case TFI_MEDIALIBOBJ_audio:
            TestFunMediaLibObjAudo(n, p1, p2);
            break;

        default:
            break;
    }
}


//#endif //#ifdef DEBUG
#pragma mark - 测试代码结束