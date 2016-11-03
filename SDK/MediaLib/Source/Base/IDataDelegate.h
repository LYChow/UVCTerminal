//
//  IDataDelegate.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/19.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef IDataDelegate_h
#define IDataDelegate_h

#include "MyHandleList.h"
#include "SwBaseWin.h"
#include "IMediaLib.h"

namespace MediaLib{

enum __dataDelegateType {
    DDT_NULL = 0,
    DDT_INPUTPORT,
    DDT_OUTPUTPORT,
    DDT_VIDEOENCODER,
    DDT_AUDIOENCODER,
    DDT_PREPROCESSORPACKVIDEO,
    DDT_PREPROCESSORPACKAUDIO,
    DDT_PREPROCESSORUNPACKVIDEO,
    DDT_PREPROCESSORUNPACKAUDIO,
    DDT_VIDEODECODER,
    DDT_AUDIODECODER,

    DDT_MAX
};

class IDataDelegate {
public:
#pragma mark --constructor/destructor destructor
    IDataDelegate(int nDataDelegateType=DDT_NULL)
    : m_nDataDelegateType(nDataDelegateType)
    , m_listSource(false, 5000)
    , m_listReceiver(false, 5000)
    , m_lMediaId(0)
    , m_format(0)
    , m_pMediaConfig(NULL)
    , m_bForsaked(false)
    , m_dwTicForsaked(0)
    {
    }
    virtual ~IDataDelegate() {
        while (IDataDelegate* source=m_listSource.PickHeadHandle()) {
            source->UnregisterDataReceiver(this);
        }
    }

#pragma mark --DataReceiver
    virtual bool RegisterDataReceiver(IDataDelegate* receiver, bool internalCall=false) {
        if (!m_listReceiver.AddTailHandle(receiver)) {
            return false;
        }
//        if (!internalCall && receiver->GetDataDelegateType()!=DDT_OUTPUTPORT && receiver->GetDataSourceCount()==1) {
//            receiver->SetMediaStreamId(m_lMediaId);
//        }
        if (!internalCall) {
            PassMediaStreamID(false);
        }
        return true;
    }
    virtual bool UnregisterDataReceiver(IDataDelegate* receiver) {
        return m_listReceiver.RemoveHandle(receiver);
    }
    virtual int GetDataReceiverCount() {
        int nCount = 0;
        for (IDataDelegate* pDataDelegate=m_listReceiver.GetHeadHandle();
             pDataDelegate;
             pDataDelegate=m_listReceiver.GetNextHandle(pDataDelegate)) {
            if (!pDataDelegate->IsForsaked()) {
                nCount++;
            }
        }
        return nCount;
    }
    virtual IDataDelegate* GetNextDataReceiver(IDataDelegate* pDelegate=NULL) {
        if (!pDelegate) {
            return m_listReceiver.GetHeadHandle();
        }
        else {
            return m_listReceiver.GetNextHandle(pDelegate);
        }
    }

#pragma mark --DataSource
    virtual bool AddDataSource(IDataDelegate* source) {
        if (!source || !source->RegisterDataReceiver(this, true)) {
            return false;
        }
        if (!m_listSource.AddTailHandle(source)) {
            source->UnregisterDataReceiver(this);
            return false;
        }
//        if (source->GetDataDelegateType()!=DDT_INPUTPORT && source->GetDataReceiverCount()==1) {
//            source->SetMediaStreamId(m_lMediaId, true);
//        }
        PassMediaStreamID(true);
        return true;
    }
    virtual bool RemoveDataSource(IDataDelegate* source) {
        if (!source) {
            return false;
        }
        source->UnregisterDataReceiver(this);
        return m_listSource.RemoveHandle(source);
    }
    virtual int GetDataSourceCount() {
        int nCount = 0;
        for (IDataDelegate* pDataDelegate=m_listSource.GetHeadHandle();
             pDataDelegate;
             pDataDelegate=m_listSource.GetNextHandle(pDataDelegate)) {
            if (!pDataDelegate->IsForsaked()) {
                nCount++;
            }
        }
        return nCount;
    }
    virtual IDataDelegate* GetNextDataSource(IDataDelegate* pDelegate=NULL) {
        if (!pDelegate) {
            return m_listSource.GetHeadHandle();
        }
        else {
            return m_listSource.GetNextHandle(pDelegate);
        }
    }

#pragma mark --foundation
    virtual bool IsDataDelegateIdle() {
        return (m_bForsaked || GetDataReceiverCount()*GetDataSourceCount()<1);
    }
    virtual int GetDataDelegateType() {
        return m_nDataDelegateType;
    }
    enum __SetMediaStreamIdFlags {
        SMSIDF_NULL = 0,
        SMSIDF_INFECTSOURCE = 0x1,
        SMSIDF_INFECTRECEIVER = 0x2,
    };
    virtual void SetMediaStreamId(long mid, int flags=SMSIDF_NULL) {
        m_lMediaId = mid;
        if (flags&SMSIDF_INFECTSOURCE) {
            PassMediaStreamID(true);
        }
        if (flags&SMSIDF_INFECTRECEIVER) {
            PassMediaStreamID(false);
        }
    }
    virtual void PassMediaStreamID(bool toSource=false) {
//        DDCT_RawVideoToVC3PackedVideo, // >RawVideo> CVideoEncoder >H.264/H.265> CPreprocessorPackVideo >VC3PackedVideo>
//        DDCT_RawAudioToVC3PackedAudio, // >RawAudio> CAudioEnconder >G.711-u/ADPCM> PreprocessorPackAudio >VC3PackedAudio>
//        DDCT_VC3PackedVideoToRawVideo, // >VC3PackedVideo> PreprocessorUnpackVideo >H.264/H.265> CVideoDecoder >RawVideo>
//        DDCT_VC3PackedAudioToRawAudio, // >VC3PackedAudio> PreprocessorUnpackAudio >G.711-u/ADPCM> CAudioDecoder >RawVideo>
        if (!toSource) {
            for (IDataDelegate* pDataReceiver=m_listReceiver.GetHeadHandle();
                 pDataReceiver;
                 pDataReceiver=m_listReceiver.GetNextHandle(pDataReceiver)) {
                int nReceiverDataDelegateType = pDataReceiver->GetDataDelegateType();
                if (pDataReceiver->GetDataSourceCount() == 1
                    && (nReceiverDataDelegateType == DDT_VIDEODECODER
                        || nReceiverDataDelegateType == DDT_PREPROCESSORPACKVIDEO
                        || nReceiverDataDelegateType == DDT_AUDIOENCODER
                        || nReceiverDataDelegateType == DDT_PREPROCESSORPACKAUDIO
                        )
                    ) {
                    pDataReceiver->SetMediaStreamId(m_lMediaId, SMSIDF_INFECTRECEIVER);
                }
            }
        }
        else {
            for (IDataDelegate* pDataSource=m_listSource.GetHeadHandle();
                 pDataSource;
                 pDataSource=m_listSource.GetNextHandle(pDataSource)) {
                int nSourceDataDelegateType = pDataSource->GetDataDelegateType();
                if (pDataSource->GetDataReceiverCount() == 1
                    && (nSourceDataDelegateType == DDT_VIDEODECODER
                        || nSourceDataDelegateType == DDT_PREPROCESSORUNPACKVIDEO
                        || nSourceDataDelegateType == DDT_AUDIODECODER
                        || nSourceDataDelegateType == DDT_PREPROCESSORUNPACKAUDIO
                        )
                    ) {
                    pDataSource->SetMediaStreamId(m_lMediaId, SMSIDF_INFECTSOURCE);
                }
            }
        }
    }
    virtual long GetMediaStreamId() {
        return m_lMediaId;
    }
    virtual void SetMediaStreamFormat(int format) {
        if (int(m_format) != format) {
            m_format = format;
            if (m_pMediaConfig) {
                delete m_pMediaConfig;
                m_pMediaConfig = NULL;
            }
        }
    }
    virtual int GetMediaStreamFormat() {
        return (int)m_format;
    }
    virtual IMediaConfig* GetConfig() {
        return m_pMediaConfig;
    }
    virtual void Forsake() {
        m_bForsaked = true;
        m_dwTicForsaked = GetTickCount();
    }
    virtual bool IsForsaked(DWORD* pDwTicForsaked=NULL) {
        if (pDwTicForsaked) {
            *pDwTicForsaked = m_dwTicForsaked;
            return m_bForsaked;
        }
        return m_bForsaked;
    }

public:
    virtual void DispatchMediaStreamData(const char* data, int len, long mediaid, int format) {
        for (IDataDelegate* pDataReceiver=m_listReceiver.GetHeadHandle();
             pDataReceiver;
             pDataReceiver=m_listReceiver.GetNextHandle(pDataReceiver)) {
            pDataReceiver->PutData(data, len, mediaid, format);
        }
    }

public:
    virtual int PutData(const char* data, int len, long mediaid, int format)=0;


protected:
    int m_nDataDelegateType;
    long m_lMediaId;
//    int m_nFormat;
    MediaFormatMask m_format;
    IMediaConfig* m_pMediaConfig;

    MyHandleList<IDataDelegate*> m_listSource;
    MyHandleList<IDataDelegate*> m_listReceiver;

    bool m_bForsaked;
    DWORD m_dwTicForsaked;
};

}//end MediaLib

#endif /* IDataDelegate_h */
