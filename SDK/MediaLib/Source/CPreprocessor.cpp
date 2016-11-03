//
//  CPreprocessor.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/21.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#include "CPreprocessor.h"
#include "DimDetector.h"
#include "itzLog.h"

namespace MediaLib{

#pragma mark - public functions constructor/destructor destructor
CPreprocessor::CPreprocessor(int nDataDelegateType/*=DDT_NULL*/)
: IDataDelegate(nDataDelegateType)
{
}

CPreprocessor::~CPreprocessor()
{
}

#pragma mark - public functions inherits from
#pragma mark - public functions own


#pragma mark - public functions constructor/destructor destructor
CPreprocessorPackVideo::CPreprocessorPackVideo()
: CPreprocessor(DDT_PREPROCESSORPACKVIDEO)
, m_pVideoSender(NULL)
, m_uWidth(0)
, m_uHeight(0)
, m_dwFrameNo(0)
{
}

CPreprocessorPackVideo::~CPreprocessorPackVideo()
{
    if (m_pVideoSender)
    {
        delete m_pVideoSender;
        m_pVideoSender = NULL;
    }
}

#ifdef DEBUG333
FILE* fileH264=NULL;
#endif //#ifdef DEBUG
#pragma mark - public functions inherits from IDataDelegate
int CPreprocessorPackVideo::PutData(const char* data, int len, long mediaid, int format)
{
    int ir = -1;

    if (!data || len<1) {
        return ir;
    }

    if (GetMediaStreamId() != mediaid) {
        SetMediaStreamId(mediaid);
        if (m_pVideoSender) {
            delete m_pVideoSender;
            m_pVideoSender = NULL;
        }
    }

    bool keyframe = false;
    unsigned long w, h;
    ir = do_read_sps((unsigned char*)data, len, &w, &h);
    if (ir >= 0) {
        keyframe = true;
        if (m_uWidth!=w || m_uHeight!=h) {
            m_uWidth = w;
            m_uHeight = h;
        }
    }

    if (!m_pVideoSender)
    {
        m_pVideoSender = new CVideoSender();
        m_pVideoSender->Init(mediaid,
                             SWVideoSendMask_FEC|SWVideoSendMask_Jitter,
                             FECMatrix_RS_Base+5,
                             "",
                             CODEC_H264,
                             _VS_NETDATACALLBACK,
                             (LONG)this);
    }

#ifdef DEBUG333
    if (!fileH264) {
        char szBuf[512] = {0};
        int ir = GetDocumentDirectory(szBuf, sizeof(szBuf)-1);
        if (ir > 0) {
            snprintf(szBuf+ir, sizeof(szBuf)-ir-1, "/encode.h264");
            fileH264 = fopen(szBuf, "w");
        }
    }
    if (fileH264) {
        fwrite((const void*)data, len, 1, fileH264);
    }
#endif //#ifdef DEBUG


    ir = m_pVideoSender->InputData((char *)data, len, m_uWidth, m_uHeight, m_dwFrameNo++, keyframe, 30, 1024, GetTickCount());
    TRACE("m_pVideoSender->InputData(len %d, w/h %u/%u, fn %u)\n", len, m_uWidth, m_uHeight, m_dwFrameNo);

    return ir;
}
    
#pragma mark - public functions own
long CPreprocessorPackVideo::_VS_NETDATACALLBACK(const char *pbufsend, int sendlen, LONG dwUser) {
    CPreprocessorPackVideo* pPreprocessorPackVideo = (CPreprocessorPackVideo *)dwUser;
    if (pPreprocessorPackVideo) {
        return pPreprocessorPackVideo->ONVS_NETDATACALLBACK(pbufsend, sendlen);
    }

    return -1;
}

long CPreprocessorPackVideo::ONVS_NETDATACALLBACK(const char *pbufsend, int sendlen)
{
    DispatchMediaStreamData(pbufsend, sendlen, GetMediaStreamId(), GetMediaStreamFormat());
    return 0;
}


#pragma mark - public functions constructor/destructor destructor
CPreprocessorPackAudio::CPreprocessorPackAudio()
: CPreprocessor(DDT_PREPROCESSORPACKAUDIO)
, m_pAudioSend(NULL)
, m_dwFrameNo(0)
{
}

CPreprocessorPackAudio::~CPreprocessorPackAudio()
{
    if (m_pAudioSend) {
        delete m_pAudioSend;
        m_pAudioSend = NULL;
    }
}

#pragma mark - public functions inherits from IDataDelegate
int CPreprocessorPackAudio::PutData(const char* data, int len, long mediaid, int format) {
    int ir = -1;

    if (!data || len<1) {
        return ir;
    }

    if (GetMediaStreamId() != mediaid) {
        SetMediaStreamId(mediaid);
        if (m_pAudioSend) {
            delete m_pAudioSend;
            m_pAudioSend = NULL;
        }
    }

    if (!m_pAudioSend)
    {
        m_pAudioSend = new CAudioSendManager();
        m_pAudioSend->Init(GetMediaStreamId(), CODEC_G711U, TRUE, FECMatrix_RS_Base+5, AudioSendToNet, (LONG)this);
    }

    ir = m_pAudioSend->InputData((char *)data, len, m_dwFrameNo++);
    TRACE("m_pAudioSend->InputData(len %d, fn %u)\n", len, m_dwFrameNo);

    return ir;
}

long CPreprocessorPackAudio::AudioSendToNet(const char *pData, int nLen, LONG dwUser)
{
    CPreprocessorPackAudio* pPreprocessorPackAudio = (CPreprocessorPackAudio*)dwUser;
    if (pPreprocessorPackAudio) {
        return pPreprocessorPackAudio->OnAudioSendToNet(pData, nLen);
    }
    return -1;
}

long CPreprocessorPackAudio::OnAudioSendToNet(const char *pData, int nLen)
{
    DispatchMediaStreamData(pData, nLen, GetMediaStreamId(), GetMediaStreamFormat());
    return 0;
}

#pragma mark - public functions own


#pragma mark - public functions constructor/destructor destructor
CPreprocessorUnpackVideo::CPreprocessorUnpackVideo()
: CPreprocessor(DDT_PREPROCESSORUNPACKVIDEO)
, m_pUnpackVideo(NULL)
{
}

CPreprocessorUnpackVideo::~CPreprocessorUnpackVideo()
{
    if (m_pUnpackVideo) {
        delete m_pUnpackVideo;
        m_pUnpackVideo = NULL;
    }
}

#pragma mark - public functions inherits from IDataDelegate
int CPreprocessorUnpackVideo::PutData(const char* data, int len, long mediaid, int format)
{
    int ir = -1;

    if (data && len>8) {
        mediaid = ((uint32_t*)data)[1];
    }

    if (mediaid != GetMediaStreamId()) {
        return ir;
    }

    if (!m_pUnpackVideo) {
        m_pUnpackVideo = new CUnpackVideo(0);
        if (m_pUnpackVideo) {
            m_pUnpackVideo->SetFrameDataCallBack(VideoFrameCallBack, (long)this);
        }
    }

    if (m_pUnpackVideo) {
        ir = m_pUnpackVideo->InputData((char*)data, len);
    }

    return ir;
};

#pragma mark - public functions own
int CPreprocessorUnpackVideo::VideoFrameCallBack(char* pFrame, int nFrameLen,
                                                 VIDEOFRAMEHEADER *pFrameHeader, FRAMECAPTIONHEADER *pCaptionHeader,
                                                 char *szCaption, DWORD addr_remote, LONG dwUser)
{
    CPreprocessorUnpackVideo* pPreprocessorUnpackVideo = (CPreprocessorUnpackVideo*)dwUser;
    if (pPreprocessorUnpackVideo) {
        return pPreprocessorUnpackVideo->OnVideoFrameCallBack(pFrame, nFrameLen, pFrameHeader, pCaptionHeader, szCaption, addr_remote);
    }

    return -1;
}

int CPreprocessorUnpackVideo::OnVideoFrameCallBack(char* pFrame, int nFrameLen,
                         VIDEOFRAMEHEADER *pFrameHeader, FRAMECAPTIONHEADER *pCaptionHeader,
                         char *szCaption, DWORD addr_remote)
{
    if (!pFrame || nFrameLen<1 || !pFrameHeader) {
        return -1;
    }

#ifdef DEBUG222
    static FILE* fileH264=NULL;

    if (!fileH264) {
        char szBuf[512] = {0};
        int ir = GetDocumentDirectory(szBuf, sizeof(szBuf)-1);
        if (ir > 0) {
            snprintf(szBuf+ir, sizeof(szBuf)-ir-1, "/unpack_term_vc3.h264");
            TRACE("will write data to %s\n", szBuf);
            fileH264 = fopen(szBuf, "w");
        }
    }
    if (fileH264) {
        fwrite((const void*)pFrame, nFrameLen, 1, fileH264);
    }
#endif //#ifdef DEBUG

    static const DWORD flagFOURCCH264 = MAKEFOURCC('H','2','6','4');//, MAKEFOURCC('D','I','V','X')等
    switch (pFrameHeader->flagFOURCC) {
        case flagFOURCCH264:
            DispatchMediaStreamData(pFrame, nFrameLen, addr_remote, GetMediaStreamFormat());
            TRACE("nFrameLen  = %d\n", nFrameLen);
            break;
        default:
            break;
    }
    return 0;
}


#pragma mark - public functions constructor/destructor destructor
CPreprocessorUnpackAudio::CPreprocessorUnpackAudio()
: CPreprocessor(DDT_PREPROCESSORUNPACKAUDIO)
, m_pUnpackAudio(NULL)
{
}

CPreprocessorUnpackAudio::~CPreprocessorUnpackAudio()
{
    if (m_pUnpackAudio) {
        delete m_pUnpackAudio;
        m_pUnpackAudio = NULL;
    }
}

#pragma mark - public functions inherits from IDataDelegate
int CPreprocessorUnpackAudio::PutData(const char* data, int len, long mediaid, int format)
{
    int ir = -1;

    if (data && len>8) {
        mediaid = ((uint32_t*)data)[1];
    }

    if (mediaid != GetMediaStreamId()) {
        return ir;
    }

    if (NULL == m_pUnpackAudio)
    {
        m_pUnpackAudio = new CUnpackAudio();
        m_pUnpackAudio->SetFrameDataCallBack(AudioFrameCallBack, (LONG)this);
    }

    ir = m_pUnpackAudio->InputData((char *)data, len);

    return ir;
}

#pragma mark - public functions own
int CPreprocessorUnpackAudio::AudioFrameCallBack(char* pData, int nLength, int nAudioCodeCType, DWORD dwFrameNo, DWORD addr_remote, int nChannel, LONG dwUser)
{
    CPreprocessorUnpackAudio* pPreprocessorUnpackAudio = (CPreprocessorUnpackAudio*)dwUser;
    if (pPreprocessorUnpackAudio) {
        return pPreprocessorUnpackAudio->OnAudioFrameCallBack(pData, nLength, nAudioCodeCType, dwFrameNo, addr_remote, nChannel);
    }
    return -1;
}

int CPreprocessorUnpackAudio::OnAudioFrameCallBack(char* pData, int nLength, int nAudioCodeCType, DWORD dwFrameNo, DWORD addr_remote, int nChannel)
{
    TRACE("AudioFrameCallBack nLength = %d, nAudioCodeCType = %d, dwFrameNo = %d\n", nLength, nAudioCodeCType, dwFrameNo);
    switch (nAudioCodeCType) {
        case CODEC_G711U:
            DispatchMediaStreamData(pData, nLength, GetMediaStreamId(), GetMediaStreamFormat());
            break;
        default:
            break;
    }
    return 0;
}

}//end MediaLib