//
//  CAudioEncoder.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/21.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#include "CAudioEncoder.h"
#include "itzLog.h"

namespace MediaLib{

#pragma mark - public functions constructor/destructor destructor
CAudioEncoder::CAudioEncoder()
: IDataDelegate(DDT_AUDIOENCODER)
, m_pCAudioConverter(NULL)
{
}

CAudioEncoder::~CAudioEncoder()
{
    if (m_pCAudioConverter) {
        delete m_pCAudioConverter;
        m_pCAudioConverter = NULL;
    }
}

#pragma mark - public functions inherits from IDataDelegate
int CAudioEncoder::PutData(const char* data, int len, long mediaid, int format)
{
    int ir = -1;
    if (!m_pCAudioConverter) {
        m_pCAudioConverter = new CAudioConverter(CAC_AF_8k16bitPCM, CAC_AF_ULAW, CAudioConverter_DataCallback, (long)this);
        if (!m_pCAudioConverter) {
            return ir;
        }
    }

    ir = m_pCAudioConverter->PutData(data, len, 0);

    return ir;
}

#pragma mark - public functions own
void CAudioEncoder::CAudioConverter_DataCallback(int code, long param, long user)
{
    CAudioEncoder* pAudioEncoder = (CAudioEncoder*)user;
    if (pAudioEncoder) {
        pAudioEncoder->OnCAudioConverter_DataCallback(code, param);
    }
}

void CAudioEncoder::OnCAudioConverter_DataCallback(int code, long param)
{
    switch (code) {
        case CAC_DCC_FRAME:
        {
            CAC_DataCallbackFrame* pframe = (CAC_DataCallbackFrame*)param;
            if (pframe) {
#ifdef DEBUG222
                static FILE* file_ulaw = NULL;

                if (!file_ulaw) {
                    char szBuf[512] = {0};
                    int ir = GetDocumentDirectory(szBuf, sizeof(szBuf)-1);
                    if (ir > 0) {
                        snprintf(szBuf+ir, sizeof(szBuf)-ir-1, "/encode_audio_ulaw.pcm");
                        file_ulaw = fopen(szBuf, "w");
                    }
                }
                if (file_ulaw) {
                    fwrite((const void*)pframe->data, pframe->len, 1, file_ulaw);
                }
#endif //#ifdef DEBUG
                DispatchMediaStreamData((const char*)pframe->data, pframe->len, GetMediaStreamId(), GetMediaStreamFormat());
            }
            break;
        }
        default:
            break;
    }
}

}//end MediaLib