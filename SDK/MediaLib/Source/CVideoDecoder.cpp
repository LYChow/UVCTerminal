//
//  CVideoDecoder.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/21.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#include "CVideoDecoder.h"
#include "DimDetector.h"
#include "itzLog.h"
#include "CMediaDevice.h"

namespace MediaLib{

#pragma mark - public functions constructor/destructor destructor
CVideoDecoder::CVideoDecoder()
: IDataDelegate(DDT_VIDEODECODER)
, m_pCH264HwDecoderImpl(NULL)
, m_uWidth(0)
, m_uHeight(0)
{
}

CVideoDecoder::~CVideoDecoder()
{
    Release();
}

#pragma mark - public functions inherits from IDataDelegate
int CVideoDecoder::PutData(const char* data, int len, long mediaid, int format)
{
    int ir = -1;

    bool keyframeWithSps = false;
    unsigned long w, h;
    int irsps = do_read_sps((unsigned char*)data, len, &w, &h);
    if (irsps >= 0) {
        keyframeWithSps = true;
        if (m_uWidth!=w || m_uHeight!=h) {
            Release();
            m_uWidth = w;
            m_uHeight = h;
        }
    }

    if (m_uWidth*m_uHeight == 0) {
        return ir;
    }

    if (!m_pCH264HwDecoderImpl) {
        m_pCH264HwDecoderImpl = new CH264HwDecoderImpl(CH264HwDecoderImpl_DataCallback, (long)this);
        if (m_pCH264HwDecoderImpl) {
            m_pCH264HwDecoderImpl->start();
        }
        if (!m_pCH264HwDecoderImpl) {
            return ir;
        }
    }

    const char* leftdata = data;
    int leftlen = len;
    if (keyframeWithSps) {
        leftdata = NULL;
        int spspps_len = len;
        int nalu_type = 0;
        for (int i=4; i<len; i++) {
            if (data[i-1] == 0x01 && data[i-2] == 0x00 && data[i-3] == 0x00 && data[i-4] == 0x00) {
                nalu_type = (data[i] & 0x1F);
                if (nalu_type == 7
                    || nalu_type == 8) {
                    continue;
                }

                spspps_len = i-4;
                break;
            }
        }

        m_pCH264HwDecoderImpl->decode(m_uWidth, m_uHeight, data, spspps_len);
        leftdata = data+spspps_len;
        leftlen -= spspps_len;
    }

//    while (leftlen) {
//        int non_spspps_nalu_len = leftlen;
//        for (int i=4+4; i<leftlen; i++) {
//            if (leftdata[i-1] == 0x01 && leftdata[i-2] == 0x00 && leftdata[i-3] == 0x00 && leftdata[i-4] == 0x00) {
//                non_spspps_nalu_len = i-4;
//                break;
//            }
//        }
//        m_pCH264HwDecoderImpl->decode(m_uWidth, m_uHeight, leftdata, non_spspps_nalu_len);
//        leftdata += non_spspps_nalu_len;
//        leftlen -= non_spspps_nalu_len;
//    }

    if (leftlen > 0) {
        m_pCH264HwDecoderImpl->decode(m_uWidth, m_uHeight, leftdata, leftlen);
    }

    ir = 0;
    return ir;
}

#pragma mark - public functions own
void CVideoDecoder::CH264HwDecoderImpl_DataCallback(int code, long param, long user)
{
    CVideoDecoder* pVideoDecoder = (CVideoDecoder*)user;
    if (pVideoDecoder) {
        pVideoDecoder->OnCH264HwDecoderImpl_DataCallback(code, param);
    }
}

void CVideoDecoder::OnCH264HwDecoderImpl_DataCallback(int code, long param)
{
    switch (code) {
        case H264HD_DCC_DECODEFRAME:
        {
            CVPixelBufferRefWapper wpr;
            wpr.pCVPixelBufferRef = (void*)param;
            wpr.bNeedMirrorWhenDisplay = false;
            wpr.bNeedCopyWhenDisplay = false;
            DispatchMediaStreamData((const char*)&wpr, sizeof(wpr), GetMediaStreamId(), GetMediaStreamFormat());
            break;
        }
        default:
            break;
    }
}

#pragma mark - protected functions
void CVideoDecoder::Release()
{
    if (m_pCH264HwDecoderImpl) {
        m_pCH264HwDecoderImpl->stop();
        delete m_pCH264HwDecoderImpl;
        m_pCH264HwDecoderImpl = NULL;
    }
}

}//end MediaLib
