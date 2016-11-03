//
//  CVideoEncoder.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/19.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#include "CVideoEncoder.h"
#include "CMediaDevice.h"

namespace MediaLib{

#pragma mark - public functions constructor/destructor destructor
CVideoEncoder::CVideoEncoder()
: IDataDelegate(DDT_VIDEOENCODER)
, m_nBitrate(384)
{
    m_pCH264HwEncoderImpl = NULL;
    m_pMediaConfig = new CVideoStreamConfig(this);
}

CVideoEncoder::~CVideoEncoder()
{
    if (m_pCH264HwEncoderImpl) {
        CH264HwEncoderImpl* pCH264HwEncoderImpl = m_pCH264HwEncoderImpl;
        m_pCH264HwEncoderImpl = NULL;
        delete pCH264HwEncoderImpl;
    }
}

#pragma mark - public functions inherits from IDataDelegate
int CVideoEncoder::PutData(const char* data, int len, long mediaid, int format)
{
//    TRACE("CVideoEncoder::PutData(%p, %d bytes, mid: %ld, fmt: %d).\n", data, len, mediaid, format);

    int ir = -1;

    long mymid = GetMediaStreamId();
    if (mymid != mediaid) {
        SetMediaStreamId(mediaid);
    }

    if (!data || len!=sizeof(CVPixelBufferRefWapper)) {
        return ir;
    }
    CVPixelBufferRefWapper* pWpr = (CVPixelBufferRefWapper*)data;

    if (!m_pCH264HwEncoderImpl) {
        m_pCH264HwEncoderImpl = new CH264HwEncoderImpl(CH264HwEncoderImpl_DataCallback, (long)this);
        if (!m_pCH264HwEncoderImpl) {
            return ir;
        }
        m_pCH264HwEncoderImpl->Init(352, 288, pWpr->mfFrameRate.Denominator, pWpr->mfFrameRate.Numerator, m_nBitrate);
    }

    ir = m_pCH264HwEncoderImpl->PutData((char*)pWpr->pCVPixelBufferRef,
                                        pWpr->mfFrameRate.Denominator, pWpr->mfFrameRate.Numerator, 0);

    return ir;
}

#pragma mark - public functions inherits from CMediaStreamConfigDelegate
int CVideoEncoder::GetSupportedFormatCount(){return -1;}
int CVideoEncoder::GetCurrentFormat(){return -1;}
int CVideoEncoder::SetCurrentFormat(int iFmt){return -1;}
int CVideoEncoder::GetFormatDesp(MediaFormatDesp* pDesp,int iFmt){return -1;}

int CVideoEncoder::SetStreamPrivateData(int iName,void* pValue)
{
    int ir = -1;
    switch (iName) {
        case MSPD_VideoCoderKeyFrameIntval://nMinKeyFrameIntval=(int)pParam,-1=tempoarily insert,0=auto,
        {
        //    int* pv=
            int nMinKeyFrameIntval=*((int*)&pValue);
            if (nMinKeyFrameIntval = -1) {
                if (!m_pCH264HwEncoderImpl) {
                    return ir;
                }
                ir = m_pCH264HwEncoderImpl->ForceKeyFrame();
            }
        }
            break;
        default:
            break;
    }
    return ir;
}

int CVideoEncoder::GetStreamPrivateData(int iName,void* pValue,int size)
{
    int ir = -1;
    return ir;
}

int CVideoEncoder::SetBitRate(int BitRate)
{
    int ir = -1;
    m_nBitrate = BitRate;

    if (!m_pCH264HwEncoderImpl) {
        return ir;
    }

    ir = m_pCH264HwEncoderImpl->SetBitrate(BitRate);

    return ir;
}

#pragma mark - public functions own
void CVideoEncoder::CH264HwEncoderImpl_DataCallback(int code, long param, long user)
{
    CVideoEncoder* pVideoEncoder = (CVideoEncoder*)user;
    if (pVideoEncoder) {
        pVideoEncoder->OnCH264HwEncoderImpl_DataCallback(code, param);
    }
}

void CVideoEncoder::OnCH264HwEncoderImpl_DataCallback(int code, long param)
{
    switch (code) {
        case H264HE_DCC_FRAME:
        {
            H264HE_DataCallbackFrame* pframe = (H264HE_DataCallbackFrame*)param;
            if (param) {
                DispatchMediaStreamData((const char*)pframe->buf, pframe->size, GetMediaStreamId(), GetMediaStreamFormat());
            }
            break;
        }
        default:
            break;
    }
}

}//end MediaLib