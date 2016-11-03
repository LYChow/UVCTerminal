//
//  CMediaProcessor.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/29.
//  Copyright © 2015年 华纬讯. All rights reserved.
//
//关于 Processor 说明（2016年01月15日 星期五）：
//    .暂不支持多入多出的结构；
//    .创建时，如果输入输入均大于一路，则强制使用1路输出；
//    .输入输出及各中间处理模块均从 IDataDelegate 派生；
//    .输入输出之间的关联在增删时动态调整:
//        .AddInput
//          创建输入端口对象，并调用 SetInputFormat；
//        .RemoveInput
//          删除输入端口对象，并拆除相关数据链（删除因本输入端口撤销而变成空闲状态的中间处理模块）；
//        .AddOutput
//          创建输出端口对象，并调用 SetOutputFormat；
//        .RemoveOutput
//          删除输出端口对象，并拆除相关数据链（删除因本输出端口撤销而变成空闲状态的中间处理模块）；
//        .SetInputFormat/SetOutputFormat
//          根据已有链路和目标格式，拆除旧格式对应的数据链，建立新的数据链；
////////////////////////////////////////////////////////////////////////////////////////////////

#include "CMediaProcessor.h"
#include "CVideoEncoder.h"
#include "CAudioEncoder.h"
#include "CVideoDecoder.h"
#include "CAudioDecoder.h"
#include "CPreprocessor.h"

namespace MediaLib{

#pragma mark - public functions constructor/destructor destructor
CMediaProcessor::CMediaProcessor(int nInCount,int nOutCount,int fmtIn/*=0*/,int fmtOut/*=0*/)
: m_listInputPort(false, 5000)
, m_listOutputPort(false, 5000)
, m_listForsakedDataDelegate(true)
{
    m_nDefaultInputFormat = fmtIn;
    m_nDefaultOutputFormat = fmtOut;

    SetInputCount(nInCount);
    SetOutputCount(nOutCount); // 如果 nInCount>1，nOutCount 将被强制限制到1

    m_nStatus = 0;
    m_lUserData = 0;
}

CMediaProcessor::~CMediaProcessor()
{
    Stop();
    while (CMediaInputPort* pInputPort = m_listInputPort.GetHeadHandle()) {
        RemoveDataDelegateChainFrom(pInputPort);
    }
    while (CMediaOutputPort* pOutputPort = m_listOutputPort.GetHeadHandle()) {
        RemoveDataDelegateChainFrom(pOutputPort);
    }
}

#pragma mark - public functions inherits from IMediaProcessor
#pragma mark --input
int CMediaProcessor::SetInputCount(int count)
{
    int ir = -1;

    if (count < 0) {
        count = 0;
    }

    if (m_listOutputPort.GetCount()>1 && count>1) {
        return ir; // 暂不支持多入多出的Processor 2016年01月15日 星期五
    }

    while (int nCount=m_listInputPort.GetCount() > count) {
        RemoveInput(nCount-1);
    }

    while (m_listInputPort.GetCount() < count) {
        ir = AddInput(m_nDefaultInputFormat);
        if (ir < 0) {
            break;
        }
    }

    if (m_listInputPort.GetCount() == count) {
        ir = 0;
    }

    return ir;
}

int CMediaProcessor::GetInputCount()
{
    return m_listInputPort.GetCount();
}

int CMediaProcessor::AddInput(int fmt/*=0*/)
{
    if (m_listOutputPort.GetCount() > 1
        && m_listInputPort.GetCount() > 0) {
        return -1; // 暂不支持多入多出的Processor 2016年01月15日 星期五
    }

    int iIndex = -1;
    CMediaInputPort* pPort = new CMediaInputPort(this, fmtUnknown);
    if (pPort) {
//        pPort->SetInuse();
        if (!m_listInputPort.AddTailHandle(pPort)) {
            delete pPort;
        }
        else {
            SetInputFormat(fmt, pPort);
            iIndex = m_listInputPort.GetIndex(pPort);
            if (m_nStatus == 1) {
                pPort->MPSStart();
            }
        }
    }
    return iIndex;
}

int CMediaProcessor::RemoveInput(int iPort)
{
    return RemoveDataDelegateChainFrom(m_listInputPort.GetHandleAt(iPort));
}

int CMediaProcessor::SetInputFormat(int fmt,int iPort/*=-1*/)
{
    int ir = -1;
    CMediaInputPort* pPort = m_listInputPort.GetHandleAt(iPort);

    if (pPort) {
        ir = SetInputFormat(fmt, pPort);
    }
    else {
        pPort = m_listInputPort.GetHeadHandle();
        while (pPort) {
            ir = SetInputFormat(fmt, pPort);
            //ir = pPort->SelectFormat(fmt);
            if (ir < 0) {
                break;
            }
            pPort = m_listInputPort.GetNextHandle(pPort);
        }
    }

    return ir;
}

int CMediaProcessor::GetInputFormat(int iPort/*=0*/)
{
    int fmt = 0;
    CMediaInputPort* pPort = m_listInputPort.GetHandleAt(iPort);

    if (pPort) {
        fmt = pPort->GetSelectedFormat();
    }

    return fmt;
}

//IMediaInputPort* CMediaProcessor::GetInput(int i/*=0*/)
//{
//    // 暂不实现 2016年01月06日 星期三
//    return m_listInputPort.GetHandleAt(i);
//}

int CMediaProcessor::SetInputStorageObject(void* pStrogeObject,long lParam,int iPort)
{
    int ir = -1;
    CMediaInputPort* pPort = m_listInputPort.GetHandleAt(iPort);
    if (!pPort) {
        return ir;
    }

    ir = pPort->SetStorageObject(pStrogeObject, lParam);
    int fmt = pPort->GetSelectedFormat();
    MediaFormatMask mf(fmt);
    switch (mf.Storage) {
        case MediaStorageProcessor:
        {
            IMediaProcessor* pProcessor = (IMediaProcessor*)pStrogeObject;
            if (pProcessor) {
                pProcessor->SetOutputStorageObject(this, iPort, lParam);
            }
            break;
        }
        case MediaStorageNet:
        {
            ISocket* pSock = (ISocket*)pStrogeObject;
            if (pSock) {
                pSock->RegisterObserver(this, iPort, SocketObserverRead|SocketObserverThread);
            }
            break;
        }
        default:
            break;
    }

    return ir;
}

void* CMediaProcessor::GetInputStorageObject(long* lParam/*=0*/,int iPort/*=0*/)
{
    void* pStrogeObject = NULL;
    int iParam = -1;
    CMediaInputPort* pPort = m_listInputPort.GetHandleAt(iPort);

    if (pPort) {
        pStrogeObject = pPort->GetStorageObject(&iParam);
        if (lParam) {
            *lParam = iParam;
        }
    }

    return pStrogeObject;
}

//int CMediaProcessor::InputConnect(IMediaProcessor* pProcessor,int iPeerOutPort/*=0*/,int iPort/*=0*/)
//{
//    int ir = -1;
//    if (!pProcessor || iPeerOutPort<0 || iPort<0) {
//        return ir;
//    }
//
//    ir = SetInputStroate(pProcessor,iPeerOutPort,iPort);
//    if (ir < 0) {
//        return ir;
//    }
//
//    IMediaOutputPort* pPort = pProcessor->GetOutput(iPeerOutPort);
//    if(pPort)
//    {
//        ir = pPort->SetStorageObject(this,iPort);
//    }
//
//    return ir;
//}

#pragma mark --output
int CMediaProcessor::SetOutputCount(int count)
{
    int ir = -1;

    if (count < 0) {
        count = 0;
    }

    if (m_listInputPort.GetCount()>1 && count>1) {
        return ir; // 暂不支持多入多出的Processor 2016年01月15日 星期五
    }

    while (int nCount=m_listOutputPort.GetCount() > count) {
        RemoveOutput(nCount-1);
    }

    while (m_listOutputPort.GetCount() < count) {
        ir = AddOutput(m_nDefaultOutputFormat);
        if (ir < 0) {
            break;
        }
    }

    if (m_listOutputPort.GetCount() == count) {
        ir = 0;
    }

    return ir;
}

int CMediaProcessor::GetOutputCount()
{
    return m_listOutputPort.GetCount();
}

int CMediaProcessor::AddOutput(int fmt/*=0*/)
{
    if (m_listInputPort.GetCount() > 1
        && m_listOutputPort.GetCount() > 0) {
        return -1; // 暂不支持多入多出的Processor 2016年01月15日 星期五
    }

    int iIndex = -1;
    CMediaOutputPort* pPort = new CMediaOutputPort(this, fmtUnknown);
    if (pPort) {
//        pPort->SetInuse();
        if (!m_listOutputPort.AddTailHandle(pPort)) {
            delete pPort;
        }
        else {
            SetOutputFormat(fmt, pPort);
            iIndex = m_listOutputPort.GetIndex(pPort);
            if (m_nStatus == 1) {
                pPort->MPSStart();
            }
        }
    }
    return iIndex;
}

int CMediaProcessor::RemoveOutput(int iPort)
{
    return RemoveDataDelegateChainFrom(m_listOutputPort.GetHandleAt(iPort));
}

int CMediaProcessor::SetOutputFormat(int fmt,int iPort/*=-1*/)
{
    int ir = -1;
    CMediaOutputPort* pPort = m_listOutputPort.GetHandleAt(iPort);

    if (pPort) {
        ir = SetOutputFormat(fmt, pPort);
    }
    else {
        pPort = m_listOutputPort.GetHeadHandle();
        while (pPort) {
            ir = SetOutputFormat(fmt, pPort);
            //ir = pPort->SelectFormat(fmt);
            if (ir < 0) {
                break;
            }
            pPort = m_listOutputPort.GetNextHandle(pPort);
        }
    }

    return ir;
}

int CMediaProcessor::GetOutputFormat(int iPort/*=0*/)
{
    int fmt = 0;
    CMediaOutputPort* pPort = m_listOutputPort.GetHandleAt(iPort);

    if (pPort) {
        fmt = pPort->GetSelectedFormat();
    }

    return fmt;
}

int CMediaProcessor::SetOutputStorageObject(void* pStrogeObject,long lParam,int iPort)
{
    int ir = -1;
    CMediaOutputPort* pPort = m_listOutputPort.GetHandleAt(iPort);
    if (!pPort) {
        return ir;
    }

    ir = pPort->SetStorageObject(pStrogeObject, lParam);
    int fmt = pPort->GetSelectedFormat();
    MediaFormatMask mf(fmt);
    switch (mf.Storage) {
        case MediaStorageNet:
        {
            ISocket* pSock = (ISocket*)pStrogeObject;
            if (pSock) {
                pSock->RegisterObserver(this, iPort, SocketObserverWrite|SocketObserverThread);
            }
            break;
        }
        default:
            break;
    }
    
    return ir;
}

void* CMediaProcessor::GetOutputStorageObject(long* lParam/*=0*/,int iPort/*=0*/)
{
    void* pStrogeObject = NULL;
    int iParam = -1;

    CMediaOutputPort* pPort = m_listOutputPort.GetHandleAt(iPort);

    if (pPort) {
        pStrogeObject = pPort->GetStorageObject(&iParam);
        if (lParam) {
            *lParam = iParam;
        }
    }
    
    return pStrogeObject;
}

#pragma mark --foundation
void CMediaProcessor::SetMediaStreamId(long mid,bool bOutput,int iPort/*=0*/)
{
    MediaPortShell* pPort = NULL;
    if (!bOutput) {
        pPort = m_listInputPort.GetHandleAt(iPort);
    }
    else {
        pPort = m_listOutputPort.GetHandleAt(iPort);
    }

    if (pPort) {
        long oldmid = pPort->GetMediaStreamId();
        if (oldmid != mid) {
            RemoveDataDelegateChainFrom(pPort, false);

            pPort->SetMediaStreamId(mid,
                                    bOutput?
                                    CMediaInputPort::SMSIDF_INFECTSOURCE:
                                    CMediaInputPort::SMSIDF_INFECTRECEIVER);

            int ir = CreateDataDelegateChainFor(pPort);
            if (ir != 0) {
                RemoveDataDelegateChainFrom(pPort, false);
            }
        }
    }
}

long CMediaProcessor::GetMediaStreamId(bool bOutput,int iPort/*=0*/)
{
    long lId = 0;
    MediaPortShell* pPort = NULL;
    if (!bOutput) {
        pPort = m_listInputPort.GetHandleAt(iPort);
    }
    else {
        pPort = m_listOutputPort.GetHandleAt(iPort);
    }

    if (pPort) {
        lId = pPort->GetMediaStreamId();
    }

    return lId;
}

IDataSource* CMediaProcessor::GetOutputSource(int iPort/*=0*/)
{
    IDataSource* pSource = NULL;

    CMediaOutputPort* pPort = m_listOutputPort.GetHandleAt(iPort);

    if (pPort) {
        pSource = pPort->GetDataSource();
    }

    return pSource;
}

IDataSink* CMediaProcessor::GetInputSink(int iPort/*=0*/)
{
    IDataSink* pSink = NULL;

    CMediaInputPort* pPort = m_listInputPort.GetHandleAt(iPort);
    if (pPort) {
        pSink = pPort->GetDataSink();
    }

    return pSink;
}

//IMediaOutputPort* CMediaProcessor::GetOutput(int i/*=0*/)
//{
//    // 暂不实现 2016年01月06日 星期三
//    return m_listOutputPort.GetHandleAt(i);
//}
//
//int CMediaProcessor::GetInputConnectedPort(IMediaProcessor*& pProcessor,int iPort/*=0*/)
//{
//    int ir = -1;
//    CMediaInputPort* pPort = m_listInputPort.GetHandleAt(iPort);
//
//    if (pPort) {
//        MediaFormatMask fmt(0,0,0,0);
//        fmt = pPort->GetSelectedFormat();
//        if (fmt.Storage == MediaStorageMediaPort) {
//            int lParam = 0;
//            void* pStorageObject = pPort->GetStorageObject(&lParam);
//            if (pStorageObject) {
//                pProcessor = (IMediaProcessor*)pStorageObject;
//                ir = lParam;
//            }
//        }
//    }
//
//    return ir;
//}
//
//int CMediaProcessor::GetOutputConnectedPort(IMediaProcessor*& pProcessor,int iPort/*=0*/)
//{
//    int ir = -1;
//    CMediaOutputPort* pPort = m_listOutputPort.GetHandleAt(iPort);
//
//    if (pPort) {
//        MediaFormatMask fmt(0,0,0,0);
//        fmt = pPort->GetSelectedFormat();
//        if (fmt.Storage == MediaStorageMediaPort) {
//            int lParam = 0;
//            void* pStorageObject = pPort->GetStorageObject(&lParam);
//            if (pStorageObject) {
//                pProcessor = (IMediaProcessor*)pStorageObject;
//                ir = lParam;
//            }
//        }
//    }
//
//    return ir;
//}

int CMediaProcessor::Start()
{
    if (m_nStatus == 1) {
        return 0;
    }

    m_nStatus = 1;
    CMediaOutputPort* pOutputPort = m_listOutputPort.GetHeadHandle();
    while (pOutputPort) {
        pOutputPort->MPSStart();
        pOutputPort = m_listOutputPort.GetNextHandle(pOutputPort);
    }
    CMediaInputPort* pInputPort = m_listInputPort.GetHeadHandle();
    while (pInputPort) {
        pInputPort->MPSStart();
        pInputPort = m_listInputPort.GetNextHandle(pInputPort);
    }
    return 0;
}

//int CMediaProcessor::Pause()
//{
//    m_nStatus = 2;
//    CMediaOutputPort* pOutputPort = m_listOutputPort.GetHeadHandle();
//    while (pOutputPort) {
//        pOutputPort->MPSPause();
//        pOutputPort = m_listOutputPort.GetNextHandle(pOutputPort);
//    }
//    CMediaInputPort* pInputPort = m_listInputPort.GetHeadHandle();
//    while (pInputPort) {
//        pInputPort->MPSPause();
//        pInputPort = m_listInputPort.GetNextHandle(pInputPort);
//    }
//    return 0;
//}

int CMediaProcessor::Stop()
{
    m_nStatus = 0;
    CMediaOutputPort* pOutputPort = m_listOutputPort.GetHeadHandle();
    while (pOutputPort) {
        pOutputPort->MPSStop();
        pOutputPort = m_listOutputPort.GetNextHandle(pOutputPort);
    }
    CMediaInputPort* pInputPort = m_listInputPort.GetHeadHandle();
    while (pInputPort) {
        pInputPort->MPSStop();
        pInputPort = m_listInputPort.GetNextHandle(pInputPort);
    }
    return 0;
}

int CMediaProcessor::GetStatus()
{
    return m_nStatus;
}

//struct CVideoCaptureConfig : public IVideoCaptureConfig
//struct CAudioCaptureConfig : public IAudioCaptureConfig
//struct CVideoRenderConfig : public IVideoRenderConfig
//struct CVideoCaptionConfig : public IVideoCaptionConfig
//struct CAudioRenderConfig : public IAudioRenderConfig
//struct CNetSendConfig : public INetSendConfig
//struct CNetRecvConfig : public INetRecvConfig
int CMediaProcessor::EnumConfig(int* pConfigTypes/*=0*/,int count/*=-1*/)
{
    int ir = 0;
    if (!pConfigTypes || count<1) {
        return ir;
    }
    IMediaConfig* pMediaConfig = NULL;
    CMediaOutputPort* pOutputPort = m_listOutputPort.GetHeadHandle();
    while (pOutputPort) {
        if (ir >= count) {
            break;
        }
        pMediaConfig = pOutputPort->GetConfig();
        if (pMediaConfig) {
            *(pConfigTypes+ir) = pMediaConfig->GetType();
            ir++;
        }
        pOutputPort = m_listOutputPort.GetNextHandle(pOutputPort);
    }
    CMediaInputPort* pInputPort = m_listInputPort.GetHeadHandle();
    while (pInputPort) {
        if (ir >= count) {
            break;
        }
        pMediaConfig = pInputPort->GetConfig();
        if (pMediaConfig) {
            *(pConfigTypes+ir) = pMediaConfig->GetType();
            ir++;
        }
        pInputPort = m_listInputPort.GetNextHandle(pInputPort);
    }

    return ir;
}

IMediaConfig* CMediaProcessor::GetConfig(int configType)
{
    IMediaConfig* pMediaConfig = NULL;

    CMediaOutputPort* pOutputPort = m_listOutputPort.GetHeadHandle();
    while (pOutputPort) {
        pMediaConfig = pOutputPort->GetConfig();
        if (pMediaConfig && pMediaConfig->GetType()==configType) {
            return pMediaConfig;
        }
        pOutputPort = m_listOutputPort.GetNextHandle(pOutputPort);
    }
    CMediaInputPort* pInputPort = m_listInputPort.GetHeadHandle();
    while (pInputPort) {
        pMediaConfig = pInputPort->GetConfig();
        if (pMediaConfig && pMediaConfig->GetType()==configType) {
            return pMediaConfig;
        }
        pInputPort = m_listInputPort.GetNextHandle(pInputPort);
    }
    
    return NULL;
}

long CMediaProcessor::SetUserData(long lUserData)
{
    m_lUserData = lUserData;
    return 0;
}

long CMediaProcessor::GetUserData()
{
    return m_lUserData;
}

#pragma mark - public functions inherits from IDataSockObserver
void CMediaProcessor::OnSinkCanWrite(int nErrCode, IDataSink* pSink, long userParam)
{
    CMediaOutputPort* pPort = m_listOutputPort.GetHandleAt((int)userParam);
    if (!pPort) {
        return;
    }

    pPort->OnSinkCanWrite(nErrCode, pSink);
}

void CMediaProcessor::OnSourceCanRead(int nErrCode, IDataSource* pSource, long userParam)
{
    CMediaInputPort* pPort = m_listInputPort.GetHandleAt((int)userParam);
    if (!pPort) {
        return;
    }

    pPort->OnSourceCanRead(nErrCode, pSource);
}

#pragma mark - public functions own

#pragma mark - class CMediaProcessor - protected functions
int CMediaProcessor::SetInputFormat(int fmt, CMediaInputPort* pInputPort)
{
    int ir = -1;
    if (!pInputPort) {
        return ir;
    }

    if (fmt != pInputPort->GetMediaStreamFormat()) {
        RemoveDataDelegateChainFrom(pInputPort, false);
    }

    ir = pInputPort->SelectFormat(fmt);
    if (ir != 0) {
        return ir;
    }

    ir = CreateDataDelegateChainFor(pInputPort);
    if (ir != 0) {
        RemoveDataDelegateChainFrom(pInputPort, false);
    }

    return ir;
}

int CMediaProcessor::SetOutputFormat(int fmt, CMediaOutputPort* pOutputPort)
{
    int ir = -1;
    if (!pOutputPort) {
        return ir;
    }

    if (fmt != pOutputPort->GetMediaStreamFormat()) {
        RemoveDataDelegateChainFrom(pOutputPort, false);
    }

    ir = pOutputPort->SelectFormat(fmt);
    if (ir != 0) {
        return ir;
    }

    ir = CreateDataDelegateChainFor(pOutputPort);
    if (ir != 0) {
        RemoveDataDelegateChainFrom(pOutputPort, false);
    }

    return ir;
}

int CMediaProcessor::CreateDataDelegateChainFor(IDataDelegate* pDataDelegate)
{
    int ir = -1;
    if (!pDataDelegate) {
        return ir;
    }

    int nDataDelegateType = pDataDelegate->GetDataDelegateType();
    if (nDataDelegateType == DDT_INPUTPORT) {
        ir = 0;
        CMediaInputPort* pInputPort = dynamic_cast<CMediaInputPort*>(pDataDelegate);
        for (CMediaOutputPort* pOutputPort=m_listOutputPort.GetHeadHandle(); pOutputPort; pOutputPort=m_listOutputPort.GetNextHandle(pOutputPort)) {
            ir = CreateDataDelegateChainBetween(pInputPort, pOutputPort);
            if (ir != 0) {
                break;
            }
        }
        pInputPort->PassMediaStreamID(false);
    }
    else if (nDataDelegateType == DDT_OUTPUTPORT) {
        ir = 0;
        CMediaOutputPort* pOutputPort = dynamic_cast<CMediaOutputPort*>(pDataDelegate);
        for (CMediaInputPort* pInputPort=m_listInputPort.GetHeadHandle(); pInputPort; pInputPort=m_listInputPort.GetNextHandle(pInputPort)) {
            ir = CreateDataDelegateChainBetween(pInputPort, pOutputPort);
            if (ir != 0) {
                break;
            }
        }
        pOutputPort->PassMediaStreamID(true);
    }

    return ir;
}

//int GetTypeCountFromMediaFormat(MediaFormatMask& mf)
//{
//    int nMediaTypeCount = 0;
//    if (mf.HasType(MediaTypeVideo)) {
//        nMediaTypeCount++;
//    }
//    if (mf.HasType(MediaTypeAudio)) {
//        nMediaTypeCount++;
//    }
//    if (mf.HasType(MediaTypeData)) {
//        nMediaTypeCount++;
//    }
//    return nMediaTypeCount;
//}
//int GetTypeCountFromMediaFormat(int fmt)
//{
//    MediaFormatMask mf(fmt);
//    return GetTypeCountFromMediaFormat(mf);
//}
int CMediaProcessor::CreateDataDelegateChainBetween(CMediaInputPort* pInputPort, CMediaOutputPort* pOutputPort)
{
    DeleteOutdatedDataDelegates();
    if (ExistDataDelegateChainBetween(pInputPort, pOutputPort)) {
        return 0;
    }

    int ir = -1;
    if (!pInputPort || !pOutputPort) {
        return ir;
    }

    MediaFormatMask inmf(pInputPort->GetSelectedFormat());
    inmf.Storage = 0;
    int infmt = inmf;

    MediaFormatMask outmf(pOutputPort->GetSelectedFormat());
    outmf.Storage = 0;
    int outfmt = outmf;

    enum __dataDelegateChainType {
        DDCT_NULL = 0,

        DDCT_COPY,
        DDCT_RawVideoToVC3PackedVideo, // >RawVideo> CVideoEncoder >H.264/H.265> CPreprocessorPackVideo >VC3PackedVideo>
        DDCT_RawAudioToVC3PackedAudio, // >RawAudio> CAudioEnconder >G.711-u/ADPCM> PreprocessorPackAudio >VC3PackedAudio>
        DDCT_VC3PackedVideoToRawVideo, // >VC3PackedVideo> PreprocessorUnpackVideo >H.264/H.265> CVideoDecoder >RawVideo>
        DDCT_VC3PackedAudioToRawAudio, // >VC3PackedAudio> PreprocessorUnpackAudio >G.711-u/ADPCM> CAudioDecoder >RawVideo>
//        DDCT_VC3PackedDataToRawData,
//        DDCT_RawDataToVC3PackedData,
    } ddcType = DDCT_NULL;

    if (infmt == outfmt) { // copy
        ddcType = DDCT_COPY;
    }/*if (infmt == outfmt) {...}*/
    else {
        if (ddcType==DDCT_NULL && inmf.HasType(MediaTypeVideo) && outmf.HasType(MediaTypeVideo)) {
            if (inmf.Format == VC3PackedVideo) {
                if (outmf.Format==VC3PackedVideo) {
                    ddcType = DDCT_COPY;
                }
                else if (outmf.Format == RawVideo) {
                    ddcType = DDCT_VC3PackedVideoToRawVideo;
                }
            }
            else if (inmf.Format == RawVideo) {
                if (outmf.Format == VC3PackedVideo) {
                    ddcType = DDCT_RawVideoToVC3PackedVideo;
                }
                else if (outmf.Format == RawVideo) {
                    ddcType = DDCT_COPY;
                }
            }
        }
        if (ddcType==DDCT_NULL && inmf.HasType(MediaTypeAudio) && outmf.HasType(MediaTypeAudio)) {
            if (inmf.Format == VC3PackedAudio) {
                if (outmf.Format == VC3PackedAudio) {
                    ddcType = DDCT_COPY;
                }
                else if (outmf.Format == RawAudio) {
                    ddcType = DDCT_VC3PackedAudioToRawAudio;
                }
            }
            else if (inmf.Format == RawAudio) {
                if (outmf.Format == VC3PackedAudio) {
                    ddcType = DDCT_RawAudioToVC3PackedAudio;
                }
                else if (outmf.Format == RawAudio) {
                    ddcType = DDCT_COPY;
                }
            }
        }
        if (ddcType==DDCT_NULL && inmf.HasType(MediaTypeData) && outmf.HasType(MediaTypeData)) {
            if (inmf.Format == VC3PackedData) {
                if (outmf.Format == VC3PackedData) {
                    ddcType = DDCT_COPY;
                }
                else if (outmf.Format == RawData) {
//                    ddcType = DDCT_VC3PackedDataToRawData;
                }
            }
            else if (inmf.Format == RawData) {
                if (outmf.Format == VC3PackedData) {
//                    ddcType = DDCT_RawDataToVC3PackedData;
                }
                else if (outmf.Format == RawData) {
                    ddcType = DDCT_COPY;
                }
            }
        }
    }

    IDataDelegate* pDataDelegateIterator = NULL;
    if (pOutputPort->IsDataDelegateIdle()) { // 输出端口空闲，从上游向下游检查
        switch (ddcType) {
            case DDCT_COPY:
                if (pOutputPort->AddDataSource(pInputPort)) {
                    ir = 0;
                }
                break;
            case DDCT_RawVideoToVC3PackedVideo:
            {
                // CVideoEncoder
                CVideoEncoder* pVideoEncoder = NULL;
                for (pDataDelegateIterator=pInputPort->GetNextDataReceiver();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pInputPort->GetNextDataReceiver(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_VIDEOENCODER
                        && pDataDelegateIterator->GetMediaStreamId() == pInputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pVideoEncoder = dynamic_cast<CVideoEncoder*>(pDataDelegateIterator);
                }
                else {
                    pVideoEncoder = new CVideoEncoder;
                    if (!pVideoEncoder) {
                        break;
                    }
                    pVideoEncoder->SetMediaStreamId(pInputPort->GetMediaStreamId());
                    if (!pVideoEncoder->AddDataSource(pInputPort)) {
                        delete pVideoEncoder;
                        break;
                    }
                }
                // CPreprocessorPackVideo
                CPreprocessorPackVideo* pPreprocessorPackVideo = NULL;
                for (pDataDelegateIterator=pVideoEncoder->GetNextDataReceiver();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pVideoEncoder->GetNextDataReceiver(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_PREPROCESSORPACKVIDEO
                        && pDataDelegateIterator->GetMediaStreamId() == pInputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pPreprocessorPackVideo = dynamic_cast<CPreprocessorPackVideo*>(pDataDelegateIterator);
                }
                else {
                    pPreprocessorPackVideo = new CPreprocessorPackVideo;
                    if (!pPreprocessorPackVideo) {
                        if (pVideoEncoder->GetDataReceiverCount() < 1) {
                            RemoveDataDelegateChainFrom(pVideoEncoder);
                        }
                        break;
                    }
                    pPreprocessorPackVideo->SetMediaStreamId(pInputPort->GetMediaStreamId());
                    if (!pPreprocessorPackVideo->AddDataSource(pVideoEncoder)) {
                        if (pVideoEncoder->GetDataReceiverCount() < 1) {
                            RemoveDataDelegateChainFrom(pVideoEncoder);
                        }
                        delete pPreprocessorPackVideo;
                        break;
                    }
                }

                if (!pOutputPort->AddDataSource(pPreprocessorPackVideo)) {
                    if (pPreprocessorPackVideo->GetDataReceiverCount() < 1) {
                        RemoveDataDelegateChainFrom(pPreprocessorPackVideo);
                    }
                    break;
                }
                ir = 0;
                break;
            }
            case DDCT_RawAudioToVC3PackedAudio:
            {
                // CAudioEncoder
                CAudioEncoder* pAudioEncoder = NULL;
                for (pDataDelegateIterator=pInputPort->GetNextDataReceiver();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pInputPort->GetNextDataReceiver(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_AUDIOENCODER
                        && pDataDelegateIterator->GetMediaStreamId() == pInputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pAudioEncoder = dynamic_cast<CAudioEncoder*>(pDataDelegateIterator);
                }
                else {
                    pAudioEncoder = new CAudioEncoder;
                    if (!pAudioEncoder) {
                        break;
                    }
                    pAudioEncoder->SetMediaStreamId(pInputPort->GetMediaStreamId());
                    if (!pAudioEncoder->AddDataSource(pInputPort)) {
                        delete pAudioEncoder;
                        break;
                    }
                }
                // CPreprocessorPackAudio
                CPreprocessorPackAudio* pPreprocessorPackAudio = NULL;
                for (pDataDelegateIterator=pAudioEncoder->GetNextDataReceiver();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pAudioEncoder->GetNextDataReceiver(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_PREPROCESSORPACKAUDIO
                        && pDataDelegateIterator->GetMediaStreamId() == pInputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pPreprocessorPackAudio = dynamic_cast<CPreprocessorPackAudio*>(pDataDelegateIterator);
                }
                else {
                    pPreprocessorPackAudio = new CPreprocessorPackAudio;
                    if (!pPreprocessorPackAudio) {
                        if (pAudioEncoder->GetDataReceiverCount() < 1) {
                            RemoveDataDelegateChainFrom(pAudioEncoder);
                        }
                        break;
                    }
                    pPreprocessorPackAudio->SetMediaStreamId(pInputPort->GetMediaStreamId());
                    if (!pPreprocessorPackAudio->AddDataSource(pAudioEncoder)) {
                        if (pAudioEncoder->GetDataReceiverCount() < 1) {
                            RemoveDataDelegateChainFrom(pAudioEncoder);
                        }
                        delete pPreprocessorPackAudio;
                        break;
                    }
                }

                if (!pOutputPort->AddDataSource(pPreprocessorPackAudio)) {
                    if (pPreprocessorPackAudio->GetDataReceiverCount() < 1) {
                        RemoveDataDelegateChainFrom(pPreprocessorPackAudio);
                    }
                    break;
                }
                ir = 0;
                break;
            }
            case DDCT_VC3PackedVideoToRawVideo:
            {
                // CPreprocessorUnpackVideo
                CPreprocessorUnpackVideo* pPreprocessorUnpackVideo = NULL;
                for (pDataDelegateIterator=pInputPort->GetNextDataReceiver();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pInputPort->GetNextDataReceiver(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_PREPROCESSORUNPACKVIDEO
                        && pDataDelegateIterator->GetMediaStreamId() == pOutputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pPreprocessorUnpackVideo = dynamic_cast<CPreprocessorUnpackVideo*>(pDataDelegateIterator);
                }
                else {
                    pPreprocessorUnpackVideo = new CPreprocessorUnpackVideo;
                    if (!pPreprocessorUnpackVideo) {
                        break;
                    }
                    pPreprocessorUnpackVideo->SetMediaStreamId(pOutputPort->GetMediaStreamId());
                    if (!pPreprocessorUnpackVideo->AddDataSource(pInputPort)) {
                        delete pPreprocessorUnpackVideo;
                        break;
                    }
                }
                // CVideoDecoder
                CVideoDecoder* pVideoDecoder = NULL;
                for (pDataDelegateIterator=pPreprocessorUnpackVideo->GetNextDataReceiver();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pPreprocessorUnpackVideo->GetNextDataReceiver(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_VIDEODECODER
                        && pDataDelegateIterator->GetMediaStreamId() == pOutputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pVideoDecoder = dynamic_cast<CVideoDecoder*>(pDataDelegateIterator);
                }
                else {
                    pVideoDecoder = new CVideoDecoder;
                    if (!pVideoDecoder) {
                        if (pPreprocessorUnpackVideo->GetDataReceiverCount() < 1) {
                            RemoveDataDelegateChainFrom(pPreprocessorUnpackVideo);
                        }
                        break;
                    }
                    pVideoDecoder->SetMediaStreamId(pOutputPort->GetMediaStreamId());
                    if (!pVideoDecoder->AddDataSource(pPreprocessorUnpackVideo)) {
                        if (pPreprocessorUnpackVideo->GetDataReceiverCount() < 1) {
                            RemoveDataDelegateChainFrom(pPreprocessorUnpackVideo);
                        }
                        delete pVideoDecoder;
                        break;
                    }
                }

                if (!pOutputPort->AddDataSource(pVideoDecoder)) {
                    if (pVideoDecoder->GetDataReceiverCount() < 1) {
                        RemoveDataDelegateChainFrom(pVideoDecoder);
                    }
                    break;
                }
                ir = 0;
                break;
            }
            case DDCT_VC3PackedAudioToRawAudio:
            {
                // CPreprocessorUnpackAudio
                CPreprocessorUnpackAudio* pPreprocessorUnpackAudio = NULL;
                for (pDataDelegateIterator=pInputPort->GetNextDataReceiver();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pInputPort->GetNextDataReceiver(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_PREPROCESSORUNPACKAUDIO
                        && pDataDelegateIterator->GetMediaStreamId() == pOutputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pPreprocessorUnpackAudio = dynamic_cast<CPreprocessorUnpackAudio*>(pDataDelegateIterator);
                }
                else {
                    pPreprocessorUnpackAudio = new CPreprocessorUnpackAudio;
                    if (!pPreprocessorUnpackAudio) {
                        break;
                    }
                    pPreprocessorUnpackAudio->SetMediaStreamId(pOutputPort->GetMediaStreamId());
                    if (!pPreprocessorUnpackAudio->AddDataSource(pInputPort)) {
                        delete pPreprocessorUnpackAudio;
                        break;
                    }
                }
                // CAudioDecoder
                CAudioDecoder* pAudioDecoder = NULL;
                for (pDataDelegateIterator=pPreprocessorUnpackAudio->GetNextDataReceiver();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pPreprocessorUnpackAudio->GetNextDataReceiver(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_AUDIODECODER
                        && pDataDelegateIterator->GetMediaStreamId() == pOutputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pAudioDecoder = dynamic_cast<CAudioDecoder*>(pDataDelegateIterator);
                }
                else {
                    pAudioDecoder = new CAudioDecoder;
                    if (!pAudioDecoder) {
                        if (pPreprocessorUnpackAudio->GetDataReceiverCount() < 1) {
                            RemoveDataDelegateChainFrom(pPreprocessorUnpackAudio);
                        }
                        break;
                    }
                    pAudioDecoder->SetMediaStreamId(pOutputPort->GetMediaStreamId());
                    if (!pAudioDecoder->AddDataSource(pPreprocessorUnpackAudio)) {
                        if (pPreprocessorUnpackAudio->GetDataReceiverCount() < 1) {
                            RemoveDataDelegateChainFrom(pPreprocessorUnpackAudio);
                        }
                        delete pAudioDecoder;
                        break;
                    }
                }

                if (!pOutputPort->AddDataSource(pAudioDecoder)) {
                    if (pAudioDecoder->GetDataReceiverCount() < 1) {
                        RemoveDataDelegateChainFrom(pAudioDecoder);
                    }
                    break;
                }
                ir = 0;
                break;
            }
            default:
                break;
        }
    }
    else if (pInputPort->IsDataDelegateIdle()) { // 输出端口已存在数据链，输入端口空闲，从下游向上游检查
        switch (ddcType) {
            case DDCT_COPY:
                if (pOutputPort->AddDataSource(pInputPort)) {
                    ir = 0;
                }
                break;
            case DDCT_RawVideoToVC3PackedVideo:
            {
                // CPreprocessorPackVideo
                CPreprocessorPackVideo* pPreprocessorPackVideo = NULL;
                for (pDataDelegateIterator=pOutputPort->GetNextDataSource();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pOutputPort->GetNextDataSource(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_PREPROCESSORPACKVIDEO
                        && pDataDelegateIterator->GetMediaStreamId() == pInputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pPreprocessorPackVideo = dynamic_cast<CPreprocessorPackVideo*>(pDataDelegateIterator);
                }
                else {
                    pPreprocessorPackVideo = new CPreprocessorPackVideo;
                    if (!pPreprocessorPackVideo) {
                        break;
                    }
                    pPreprocessorPackVideo->SetMediaStreamId(pInputPort->GetMediaStreamId());
                    if (!pOutputPort->AddDataSource(pPreprocessorPackVideo)) {
                        delete pPreprocessorPackVideo;
                        break;
                    }
                }
                // CVideoEncoder
                CVideoEncoder* pVideoEncoder = NULL;
                for (pDataDelegateIterator=pPreprocessorPackVideo->GetNextDataSource();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pPreprocessorPackVideo->GetNextDataSource(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_VIDEOENCODER
                        && pDataDelegateIterator->GetMediaStreamId() == pInputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pVideoEncoder = dynamic_cast<CVideoEncoder*>(pDataDelegateIterator);
                }
                else {
                    pVideoEncoder = new CVideoEncoder;
                    if (!pVideoEncoder) {
                        if (pPreprocessorPackVideo->GetDataSourceCount() < 1) {
                            RemoveDataDelegateChainFrom(pPreprocessorPackVideo);
                        }
                        break;
                    }
                    pVideoEncoder->SetMediaStreamId(pInputPort->GetMediaStreamId());
                    if (!pPreprocessorPackVideo->AddDataSource(pVideoEncoder)) {
                        if (pPreprocessorPackVideo->GetDataSourceCount() < 1) {
                            RemoveDataDelegateChainFrom(pPreprocessorPackVideo);
                        }
                        delete pVideoEncoder;
                        break;
                    }
                }

                if (!pVideoEncoder->AddDataSource(pInputPort)) {
                    if (pVideoEncoder->GetDataSourceCount() < 1) {
                        RemoveDataDelegateChainFrom(pVideoEncoder);
                    }
                    break;
                }
                ir = 0;
                break;
            }
            case DDCT_RawAudioToVC3PackedAudio:
            {
                // CPreprocessorPackAudio
                CPreprocessorPackAudio* pPreprocessorPackAudio = NULL;
                for (pDataDelegateIterator=pOutputPort->GetNextDataSource();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pOutputPort->GetNextDataSource(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_PREPROCESSORPACKAUDIO
                        && pDataDelegateIterator->GetMediaStreamId() == pInputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pPreprocessorPackAudio = dynamic_cast<CPreprocessorPackAudio*>(pDataDelegateIterator);
                }
                else {
                    pPreprocessorPackAudio = new CPreprocessorPackAudio;
                    if (!pPreprocessorPackAudio) {
                        break;
                    }
                    pPreprocessorPackAudio->SetMediaStreamId(pInputPort->GetMediaStreamId());
                    if (!pOutputPort->AddDataSource(pPreprocessorPackAudio)) {
                        delete pPreprocessorPackAudio;
                        break;
                    }
                }
                // CAudioEncoder
                CAudioEncoder* pAudioEncoder = NULL;
                for (pDataDelegateIterator=pPreprocessorPackAudio->GetNextDataSource();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pPreprocessorPackAudio->GetNextDataSource(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_AUDIOENCODER
                        && pDataDelegateIterator->GetMediaStreamId() == pInputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pAudioEncoder = dynamic_cast<CAudioEncoder*>(pDataDelegateIterator);
                }
                else {
                    pAudioEncoder = new CAudioEncoder;
                    if (!pAudioEncoder) {
                        if (pPreprocessorPackAudio->GetDataSourceCount() < 1) {
                            RemoveDataDelegateChainFrom(pPreprocessorPackAudio);
                        }
                        break;
                    }
                    pAudioEncoder->SetMediaStreamId(pInputPort->GetMediaStreamId());
                    if (!pPreprocessorPackAudio->AddDataSource(pAudioEncoder)) {
                        if (pPreprocessorPackAudio->GetDataSourceCount() < 1) {
                            RemoveDataDelegateChainFrom(pPreprocessorPackAudio);
                        }
                        delete pAudioEncoder;
                        break;
                    }
                }

                if (!pAudioEncoder->AddDataSource(pInputPort)) {
                    if (pAudioEncoder->GetDataSourceCount() < 1) {
                        RemoveDataDelegateChainFrom(pAudioEncoder);
                    }
                    break;
                }
                ir = 0;
                break;
            }
            case DDCT_VC3PackedVideoToRawVideo:
            {
                // CVideoDecoder
                CVideoDecoder* pVideoDecoder = NULL;
                for (pDataDelegateIterator=pOutputPort->GetNextDataSource();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pOutputPort->GetNextDataSource(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_VIDEODECODER
                        && pDataDelegateIterator->GetMediaStreamId() == pOutputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pVideoDecoder = dynamic_cast<CVideoDecoder*>(pDataDelegateIterator);
                }
                else {
                    pVideoDecoder = new CVideoDecoder;
                    if (!pVideoDecoder) {
                        break;
                    }
                    pVideoDecoder->SetMediaStreamId(pOutputPort->GetMediaStreamId());
                    if (!pOutputPort->AddDataSource(pVideoDecoder)) {
                        delete pVideoDecoder;
                        break;
                    }
                }
                // CPreprocessorUnpackVideo
                CPreprocessorUnpackVideo* pPreprocessorUnpackVideo = NULL;
                for (pDataDelegateIterator=pVideoDecoder->GetNextDataSource();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pVideoDecoder->GetNextDataSource(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_PREPROCESSORUNPACKVIDEO
                        && pDataDelegateIterator->GetMediaStreamId() == pOutputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pPreprocessorUnpackVideo = dynamic_cast<CPreprocessorUnpackVideo*>(pDataDelegateIterator);
                }
                else {
                    pPreprocessorUnpackVideo = new CPreprocessorUnpackVideo;
                    if (!pPreprocessorUnpackVideo) {
                        if (pVideoDecoder->GetDataSourceCount() < 1) {
                            RemoveDataDelegateChainFrom(pVideoDecoder);
                        }
                        break;
                    }
                    pPreprocessorUnpackVideo->SetMediaStreamId(pOutputPort->GetMediaStreamId());
                    if (!pVideoDecoder->AddDataSource(pPreprocessorUnpackVideo)) {
                        if (pVideoDecoder->GetDataSourceCount() < 1) {
                            RemoveDataDelegateChainFrom(pVideoDecoder);
                        }
                        delete pPreprocessorUnpackVideo;
                        break;
                    }
                }

                if (!pPreprocessorUnpackVideo->AddDataSource(pInputPort)) {
                    if (pPreprocessorUnpackVideo->GetDataSourceCount() < 1) {
                        RemoveDataDelegateChainFrom(pPreprocessorUnpackVideo);
                    }
                    break;
                }
                ir = 0;
                break;
            }
            case DDCT_VC3PackedAudioToRawAudio:
            {
                // CAudioDecoder
                CAudioDecoder* pAudioDecoder = NULL;
                for (pDataDelegateIterator=pOutputPort->GetNextDataSource();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pOutputPort->GetNextDataSource(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_AUDIODECODER
                        && pDataDelegateIterator->GetMediaStreamId() == pOutputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pAudioDecoder = dynamic_cast<CAudioDecoder*>(pDataDelegateIterator);
                }
                else {
                    pAudioDecoder = new CAudioDecoder;
                    if (!pAudioDecoder) {
                        break;
                    }
                    pAudioDecoder->SetMediaStreamId(pOutputPort->GetMediaStreamId());
                    if (!pOutputPort->AddDataSource(pAudioDecoder)) {
                        delete pAudioDecoder;
                        break;
                    }
                }
                // CPreprocessorUnpackAudio
                CPreprocessorUnpackAudio* pPreprocessorUnpackAudio = NULL;
                for (pDataDelegateIterator=pAudioDecoder->GetNextDataSource();
                     pDataDelegateIterator;
                     pDataDelegateIterator=pAudioDecoder->GetNextDataSource(pDataDelegateIterator)) {
                    if (pDataDelegateIterator->GetDataDelegateType() == DDT_PREPROCESSORUNPACKAUDIO
                        && pDataDelegateIterator->GetMediaStreamId() == pOutputPort->GetMediaStreamId()) {
                        break;
                    }
                }
                if (pDataDelegateIterator) {
                    pPreprocessorUnpackAudio = dynamic_cast<CPreprocessorUnpackAudio*>(pDataDelegateIterator);
                }
                else {
                    pPreprocessorUnpackAudio = new CPreprocessorUnpackAudio;
                    if (!pPreprocessorUnpackAudio) {
                        if (pAudioDecoder->GetDataSourceCount() < 1) {
                            RemoveDataDelegateChainFrom(pAudioDecoder);
                        }
                        break;
                    }
                    pPreprocessorUnpackAudio->SetMediaStreamId(pOutputPort->GetMediaStreamId());
                    if (!pAudioDecoder->AddDataSource(pPreprocessorUnpackAudio)) {
                        if (pAudioDecoder->GetDataSourceCount() < 1) {
                            RemoveDataDelegateChainFrom(pAudioDecoder);
                        }
                        delete pPreprocessorUnpackAudio;
                        break;
                    }
                }

                if (!pPreprocessorUnpackAudio->AddDataSource(pInputPort)) {
                    if (pPreprocessorUnpackAudio->GetDataSourceCount() < 1) {
                        RemoveDataDelegateChainFrom(pPreprocessorUnpackAudio);
                    }
                    break;
                }
                ir = 0;
                break;
            }
            default:
                break;
        }
    }
//    else { // 已建立数据链（输入输出端口都存在数据链，不能重复建立
//        return ir;
//    }

    return ir;
}

int CMediaProcessor::RemoveDataDelegateChainFrom(IDataDelegate* pDataDelegate, bool bIncludeCurrentNode/*=true*/)
{
    if (!pDataDelegate) {
        return -1;
    }

    if (bIncludeCurrentNode) {
        int nDataDelegateType = pDataDelegate->GetDataDelegateType();
        if (nDataDelegateType==DDT_INPUTPORT || nDataDelegateType==DDT_OUTPUTPORT) {
            MediaPortShell* pmps = dynamic_cast<MediaPortShell*>(pDataDelegate);
            pmps->MPSStop();
        }
        pDataDelegate->Forsake();
    }

    for (IDataDelegate* pSource=pDataDelegate->GetNextDataSource();
         pSource;
         pSource=pDataDelegate->GetNextDataSource(pSource)) {
        pDataDelegate->RemoveDataSource(pSource); // pSource->UnregisterDataReceiver(pDataDelegate);
        if (pSource->IsDataDelegateIdle() && pSource->GetDataDelegateType()!=DDT_INPUTPORT) {
            RemoveDataDelegateChainFrom(pSource);
        }
    }

    for (IDataDelegate* pDataReceiver=pDataDelegate->GetNextDataReceiver();
         pDataReceiver;
         pDataReceiver=pDataDelegate->GetNextDataReceiver(pDataReceiver)) {
        pDataReceiver->RemoveDataSource(pDataDelegate);
        if (pDataReceiver->IsDataDelegateIdle() && pDataReceiver->GetDataDelegateType()!=DDT_OUTPUTPORT) {
            RemoveDataDelegateChainFrom(pDataReceiver);
        }
    }

    if (bIncludeCurrentNode) {
        int nDataDelegateType = pDataDelegate->GetDataDelegateType();
        if (nDataDelegateType == DDT_INPUTPORT) {
            m_listInputPort.RemoveHandle(dynamic_cast<CMediaInputPort*>(pDataDelegate));
        }
        else if (nDataDelegateType == DDT_OUTPUTPORT) {
            m_listOutputPort.RemoveHandle(dynamic_cast<CMediaOutputPort*>(pDataDelegate));
        }
        m_listForsakedDataDelegate.AddTailHandle(pDataDelegate);
    }
    
    return 0;
}

bool CMediaProcessor::ExistDataDelegateChainBetween(IDataDelegate* pSourceDataDelegate, IDataDelegate* pReceiverDataDelegate)
{
    if (!pSourceDataDelegate || !pReceiverDataDelegate) {
        return false;
    }

    for (IDataDelegate* pDataDelegateIterator=pSourceDataDelegate->GetNextDataReceiver();
         pDataDelegateIterator;
         pDataDelegateIterator=pSourceDataDelegate->GetNextDataReceiver(pDataDelegateIterator)) {
        if (pDataDelegateIterator==pReceiverDataDelegate || ExistDataDelegateChainBetween(pDataDelegateIterator, pReceiverDataDelegate)) {
            return true;
        }
    }

    return false;
}

void CMediaProcessor::DeleteOutdatedDataDelegates()
{
    while (IDataDelegate* pDataDelegate = m_listForsakedDataDelegate.GetHeadHandle()) {
        DWORD tic=GetTickCount(), ticForsaked=0;
        if (pDataDelegate->IsForsaked(&ticForsaked)) {
            if (tic-ticForsaked > 5000) {
                m_listForsakedDataDelegate.DeleteHandle(pDataDelegate);
            }
            else {
                break;
            }
        }
        else {
            pDataDelegate->Forsake();
            break;
        }
    }
}

}//end MediaLib
