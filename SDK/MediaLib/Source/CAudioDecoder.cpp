//
//  CAudioDecoder.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/21.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#include "CAudioDecoder.h"
#include "itzLog.h"

namespace MediaLib{

#pragma mark - public functions constructor/destructor destructor
CAudioDecoder::CAudioDecoder()
: IDataDelegate(DDT_AUDIODECODER)
, m_pCAudioConverter(NULL)
{
}

CAudioDecoder::~CAudioDecoder()
{
    if (m_pCAudioConverter) {
        delete m_pCAudioConverter;
        m_pCAudioConverter = NULL;
    }
}

#pragma mark - public functions inherits from IDataDelegate
int CAudioDecoder::PutData(const char* data, int len, long mediaid, int format)
{
    int ir = -1;
    if (!m_pCAudioConverter) {
        m_pCAudioConverter = new CAudioConverter(CAC_AF_ULAW, CAC_AF_8k16bitPCM, CAudioConverter_DataCallback, (long)this);
        if (!m_pCAudioConverter) {
            return ir;
        }
    }

    ir = m_pCAudioConverter->PutData(data, len, 0);
    
    return ir;
}

#pragma mark - public functions own
void CAudioDecoder::CAudioConverter_DataCallback(int code, long param, long user)
{
    CAudioDecoder* pAudioDecoder = (CAudioDecoder*)user;
    if (pAudioDecoder) {
        pAudioDecoder->OnCAudioConverter_DataCallback(code, param);
    }
}

void CAudioDecoder::OnCAudioConverter_DataCallback(int code, long param)
{
    switch (code) {
        case CAC_DCC_FRAME:
        {
            CAC_DataCallbackFrame* pframe = (CAC_DataCallbackFrame*)param;
            if (pframe) {
#ifdef DEBUG222
                static FILE* fileDecode8k16bit = NULL;

                if (!fileDecode8k16bit) {
                    char szBuf[512] = {0};
                    int ir = GetDocumentDirectory(szBuf, sizeof(szBuf)-1);
                    if (ir > 0) {
                        snprintf(szBuf+ir, sizeof(szBuf)-ir-1, "/decode_audio8k16b.pcm");
                        fileDecode8k16bit = fopen(szBuf, "w");
                    }
                }
                if (fileDecode8k16bit) {
                    fwrite((const void*)pframe->data, pframe->len, 1, fileDecode8k16bit);
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
