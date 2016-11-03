//
//  CMediaPort.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/6.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#include "CMediaPort.h"
#include "CMediaProcessor.h"
#include "CMediaDevice.h"
#include "CPreprocessor.h"

namespace MediaLib{

#pragma mark - public functions own
//int MediaPortShell::MPSStart()
//{
//    switch (m_format.Storage) {
//        case MediaStorageMediaPort:
//            break;
//        case MediaStorageDevice:
////            m_pStorageObjectDevice->Start();
//            break;
//        case MediaStorageFile:
//            break;
//        case MediaStorageNet:
////            m_pStorageObjectSocket->OpenSock(SocketIpAddr serverIp, UINT serverPort);
//            break;
//        default:
//            break;
//    }
//    m_nStatus = 1;
//    return 0;
//};

//int MediaPortShell::MPSPause()
//{
//    m_nStatus = 2;
//    return 0;
//};

//int MediaPortShell::MPSStop()
//{
//    switch (m_format.Storage) {
//        case MediaStorageMediaPort:
//            break;
//        case MediaStorageDevice:
////            m_pStorageObjectDevice->Stop();
//            break;
//        case MediaStorageFile:
//            break;
//        case MediaStorageNet:
////            m_pStorageObjectSocket->CloseSock();
//            break;
//        default:
//            break;
//    }
//    m_nStatus = 0;
//    return 0;
//}

#pragma mark - public functions constructor/destructor destructor
CMediaInputPort::CMediaInputPort(CMediaProcessor* pProcessor, int format)
: MediaPortShell(pProcessor, format, DDT_INPUTPORT)
{
}

CMediaInputPort::~CMediaInputPort()
{
    MPSStop();
}

#pragma mark - public functions inherits from IMediaPort
int CMediaInputPort::SetStorageObject(void* pObject,int lParam)
{
    m_pStorageObject = pObject;
    m_lStorageObjectParam = lParam;

    return 0;
}

void* CMediaInputPort::GetStorageObject(int* lParam/*=0*/)
{
    if (lParam) {
        *lParam = m_lStorageObjectParam;
    }
    return m_pStorageObject;
}

int CMediaInputPort::GetPortDirection()//<in,out>
{
    // 暂不实现 2016年01月06日 星期三
    return -1;
}

int CMediaInputPort::GetSupportedFormatCount()
{
    // 暂不实现 2016年01月06日 星期三
    return -1;
}

int CMediaInputPort::GetSupportedFormat(int i)
{
    // 暂不实现 2016年01月06日 星期三
    return -1;
}

int CMediaInputPort::SelectFormat(int fmt)
{
    SetMediaStreamFormat(fmt);
//    if (m_format.Storage == MediaStorageNet) {
//        m_pMediaConfig = new CNetRecvConfig(this); // 暂不实现 lzh 2016年04月16日 星期六
//    }
    return 0;
}

int CMediaInputPort::GetSelectedFormat()
{
    return GetMediaStreamFormat();
//    return MPSGetFormat();
}

#pragma mark - public functions inherits from IDataSink
int CMediaInputPort::WriteData(const char* buf,int size)
{
    int ir = -1;
    if (!buf || size<1) {
        return ir;
    }

    ir = PutData(buf, size, GetMediaStreamId(), 0);

    return ir;
}

#pragma mark - public functions inherits from MediaPortShell

int CMediaInputPort::MPSStart()
{
    m_nStatus = 1;
    if (m_pStorageObject) {
        switch (m_format.Storage) {
            case MediaStorageProcessor:
                break;
            case MediaStorageDevice:
                if (m_pStorageObjectDevice) {
                    m_pStorageObjectDevice->RegisterDataSink(this);
                }
                break;
            case MediaStorageNet:
//                m_pStorageObjectSocket->OpenSock(SocketIpAddr serverIp, UINT serverPort);
                break;
            case MediaStorageFile:
                break;
            default:
                break;
        }
    }
    return 0;
};

//int CMediaInputPort::MPSPause()
//{
//    m_nStatus = 2;
//    return 0;
//};

int CMediaInputPort::MPSStop()
{
    if (m_nStatus == 0) {
        return 0;
    }
    
    MediaPortShell::MPSStop();
    if (m_pStorageObject) {
        switch (m_format.Storage) {
            case MediaStorageProcessor:
                break;
            case MediaStorageDevice:
                m_pStorageObjectDevice->UnregisterDataSink(this);
                break;
            case MediaStorageNet:
    //            m_pStorageObjectSocket->CloseSock();
                break;
            case MediaStorageFile:
                break;
            default:
                break;
        }
    }
    m_nStatus = 0;
    return 0;
}

#pragma mark - public functions inherits from IDataDelegate
int CMediaInputPort::PutData(const char* data, int len, long mediaid, int format)
{
    int ir = -1;
    if (!data || len<1) {
        return ir;
    }
    if (IsForsaked() || m_nStatus!=1) {
        return ir;
    }

    DispatchMediaStreamData(data, len, mediaid, format);

    return ir;
}

#pragma mark - public functions own
IDataSink* CMediaInputPort::GetDataSink()
{
    return this;
}

void CMediaInputPort::OnSourceCanRead(int nErrCode, IDataSource* pSource)
{
    const int buf_sz = 2048;
    char buf[buf_sz];
    int len = 0;
    while (1)
    {
        len = pSource->ReadData(buf, buf_sz);
        if (len < 1) {
            break;
        }
        PutData(buf, len, GetMediaStreamId(), 0);
    }
}

#pragma mark - CMediaInputPort::protected functions


#pragma mark - public functions constructor/destructor destructor
CMediaOutputPort::CMediaOutputPort(CMediaProcessor* pProcessor, int format)
: MediaPortShell(pProcessor, format, DDT_OUTPUTPORT)
{
    m_nMaxBufCount = 64; // 最大缓冲约合128KB（假设每包2KB）
}

CMediaOutputPort::~CMediaOutputPort()
{
}

#pragma mark - public functions inherits from IMediaPort
int CMediaOutputPort::SetStorageObject(void* pObject,int lParam)
{
    m_pStorageObject = pObject;
    m_lStorageObjectParam = lParam;
    return 0;
}

void* CMediaOutputPort::GetStorageObject(int* lParam/*=0*/)
{
    if (lParam) {
        *lParam = m_lStorageObjectParam;
    }
    return m_pStorageObject;
}

int CMediaOutputPort::GetPortDirection()//<in,out>
{
    // 暂不实现 2016年01月06日 星期三
    return -1;
}

int CMediaOutputPort::GetSupportedFormatCount()
{
    // 暂不实现 2016年01月06日 星期三
    return -1;
}

int CMediaOutputPort::GetSupportedFormat(int i)
{
    // 暂不实现 2016年01月06日 星期三
    return -1;
}

int CMediaOutputPort::SelectFormat(int fmt)
{
    SetMediaStreamFormat(fmt);
    if (m_format.Storage == MediaStorageNet
        || m_format.Format == VC3PackedVideo) {
        m_pMediaConfig = new CNetSendConfig(this);
    }

    return 0;
}

int CMediaOutputPort::GetSelectedFormat()
{
    return GetMediaStreamFormat();
//    return MPSGetFormat();
}

#pragma mark - public functions inherits from IDataSource
int CMediaOutputPort::ReadData(char* buf,int size)
{ // 暂不实现拉模式 2016年01月12日 星期二
    return -1;
}

#pragma mark - public functions inherits from MediaPortShell

int CMediaOutputPort::MPSStart()
{
    m_nStatus = 1;
    if (m_pStorageObject) {
        switch (m_format.Storage) {
            case MediaStorageProcessor:
                break;
            case MediaStorageDevice:
                if (m_pStorageObjectDevice) {
                    m_pStorageObjectDevice->Start();
                }
                break;
            case MediaStorageNet:
    //            m_pStorageObjectSocket->OpenSock(SocketIpAddr serverIp, UINT serverPort);
                break;
            case MediaStorageFile:
                break;
            default:
                break;
        }
    }
    return 0;
};

//int CMediaOutputPort::MPSPause()
//{
//    m_nStatus = 2;
//    return 0;
//};

int CMediaOutputPort::MPSStop()
{
    if (m_nStatus == 0) {
        return 0;
    }

    MediaPortShell::MPSStop();
    if (m_pStorageObject) {
        switch (m_format.Storage) {
            case MediaStorageProcessor:
                break;
            case MediaStorageDevice:
                m_pStorageObjectDevice->Stop();
                break;
            case MediaStorageNet:
    //            m_pStorageObjectSocket->CloseSock();
                break;
            case MediaStorageFile:
                break;
            default:
                break;
        }
    }
    m_nStatus = 0;
    return 0;
}

#pragma mark - public functions inherits from IDataDelegate
int CMediaOutputPort::PutData(const char* data, int len, long mediaid, int format)
{
    MediaFormatMask mfm(GetMediaStreamFormat());
    switch (mfm.Storage) {
        case MediaStorageProcessor:
        {
            if (!m_pStorageObjectPeerProcessor) {
                break;
            }
            IDataSink* pSink = m_pStorageObjectPeerProcessor->GetInputSink(m_lStorageObjectParam);
            if (!pSink) {
                break;
            }
            /*int ir = */pSink->WriteData(data, len);
            break;
        }
        case MediaStorageDevice:
        {
            if (!m_pStorageObjectDevice) {
                break;
            }
            /*int ir = */m_pStorageObjectDevice->WriteData(data, len);
            break;
        }
        case MediaStorageFile: // 暂不实现 2016年01月12日 星期二
            break;
        case MediaStorageNet:
        {
            TryWriteDataToStorageNet(data, len);
            break;
        }
            
        default:
            break;
    }
    return 0;
}

#pragma mark - public functions inherits from CNetSendConfigDelegate
int CMediaOutputPort::SetFECType(int type,void* pParam){return -1;} // ????? 待实现
int CMediaOutputPort::GetFECType(void* pParam){return -1;}
int CMediaOutputPort::SetNATHost(const char* szHostAddr){return -1;}
int CMediaOutputPort::SetSmoothType(int type,void* pParam){return -1;}
int CMediaOutputPort::GetSmoothType(void* pParam){return -1;}
IMediaStreamConfig* CMediaOutputPort::GetInputStreamConfig(int iPort)
{
    MediaFormatMask mfm(GetMediaStreamFormat());
    if (mfm.Storage != MediaStorageFile
        && mfm.Format != VC3PackedVideo)
    {
        return NULL;
    }
    IDataDelegate* pPreprocessor = GetNextDataSource();
    if (!pPreprocessor) {
        return NULL;
    }
    IDataDelegate* pEncoder = pPreprocessor->GetNextDataSource();
    if (!pEncoder) {
        return NULL;
    }
    return dynamic_cast<IMediaStreamConfig*>(pEncoder->GetConfig());
}

#pragma mark - public functions own
IDataSource* CMediaOutputPort::GetDataSource()
{
    return this;
}

void CMediaOutputPort::SetMaxBufCount(int nCount)
{
    m_nMaxBufCount = nCount;
}

void CMediaOutputPort::OnSinkCanWrite(int nErrCode, IDataSink* pSink)
{
    if (!pSink || (IDataSink*)m_pStorageObjectSocket!=pSink) {
        return;
    }

    while (MOPBuffer* pmopb = m_buf.PickHeadHandle()) {
        int ir = TryWriteDataToStorageNet(pmopb->_buf, pmopb->_size);
        delete pmopb;
        if (ir == 1/*delayed*/) {
            break;
        }
    }
}

int CMediaOutputPort::TryWriteDataToStorageNet(const char* buf, int size)
{
    MediaFormatMask mfm(GetMediaStreamFormat());
    if (mfm.Storage != MediaStorageNet) {
        return -1;
    }
    if (!m_pStorageObjectSocket) {
        return -2;
    }

    int ir = m_pStorageObjectSocket->WriteData(buf, size);
    if (ir == size) {
        return 0;
    }
    else if (ir < 1) {
        ir = 0;
    }
    if (m_buf.GetCount() > m_nMaxBufCount) {
        return -3;
    }
    MOPBuffer* pmopb = new MOPBuffer(buf+ir, size-ir);
    if (!pmopb) {
        return -4;
    }
    if (!m_buf.AddTailHandle(pmopb)) {
        delete pmopb;
        return -5;
    }
    return 1;
}

#pragma mark - CMediaInputPort::protected functions

}//end MediaLib
