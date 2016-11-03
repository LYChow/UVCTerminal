//
//  CMediaPort.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/6.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef CMediaPort_h
#define CMediaPort_h

#include <stdio.h>
#include <string.h>
#include "IMediaLib.h"
#include "INetLib.h"
#include "MyHandleList.h"
#include "IDataDelegate.h"
#include "CMediaDevice.h"

namespace MediaLib{

class CMediaDevice;
class CMediaProcessor;

//媒体处理端口,处理能力
struct IMediaPort:public IObject
{
    /*
     pObject=<MediaStorageType>?
     MeidaPort:Same as IMediaProcessor.ConnectInput(...)
     <Device>?
     VideoCapture,AudioCapture,AudioOut :(int)IndexOfDevice
     VideoDisplay :(UIView*)pWindow
     File :(LPCSTR*)szFilename
     Net  :(ISocket*)pSocket

     */
    virtual int SetStorageObject(void* pObject,int lParam)=0;
    virtual void* GetStorageObject(int* lParam=0)=0;
    virtual int GetPortDirection();//<in,out>
    virtual int GetSupportedFormatCount()=0;
    virtual int GetSupportedFormat(int i)=0;
    virtual int SelectFormat(int fmt)=0;
    virtual int GetSelectedFormat()=0;
    bool MatchFormat(int fmt,IBOOL bSelect=1)
    {
        for (int i=0;i<GetSupportedFormatCount();i++)
        {
            if(GetSupportedFormat(i)==fmt)
            {
                if(bSelect) SelectFormat(fmt);
                return 1;
            }
        }
        return 0;
    }

    int MatchFormat(IMediaPort* pPort,IBOOL bSelect=1,int fmtPrefer=-1)
    {
        int fmtSel=-1;
        if(fmtPrefer>-1 && MatchFormat(fmtPrefer,bSelect))
        {
            fmtSel= fmtPrefer;
        }
        else
        {
            for (int i=0;i<GetSupportedFormatCount();i++)
            {
                int fmt=GetSupportedFormat(i);
                if(fmt!=fmtPrefer)
                {
                    for (int j=0;j<pPort->GetSupportedFormatCount();j++)
                    {
                        if(pPort->MatchFormat(fmt))
                        {
                            fmtSel=fmt;
                            break;
                        }
                    }
                }
                if(fmtSel>-1) break;
            }
            
        }
        return fmtSel;	
    }
};

struct MediaPortShell : public IDataDelegate {
#pragma mark - public functions constructor/destructor destructor
public:
    MediaPortShell(CMediaProcessor* pProcessor, int format, int nDataDelegateType=DDT_NULL)
    : /*m_format(format)
    ,*/ IDataDelegate(nDataDelegateType)
    {
//        m_bInuse = false;
        m_pProcessor = pProcessor;
        m_nStatus = 0;
        m_pStorageObject = NULL;
        m_lStorageObjectParam = 0;
    }
    virtual ~MediaPortShell() {
        if (m_nStatus != 0) {
            MediaPortShell::MPSStop();
        }
    }

#pragma mark - public functions inherits from IDataDelegate
public:
    virtual int PutData(const char* data, int len, long mediaid, int format) {return -1;}
    virtual IMediaConfig* GetConfig() {
        if (m_format.Storage == MediaStorageDevice) {
            if (m_pStorageObjectDevice) {
                return m_pStorageObjectDevice->GetConfig();
            }
        }
        else if (m_format.Storage == MediaStorageNet) {
            if (GetDataDelegateType() == DDT_INPUTPORT) {
            }
            else if (GetDataDelegateType() == DDT_OUTPUTPORT) {
            }
        }
        return IDataDelegate::GetConfig();
    }

#pragma mark - public functions own
public:
//    virtual void MPSSetInuse(bool bInuse=true) {
//        m_bInuse = bInuse;
//    }
//    virtual bool MPSIsInuse() {
//        return m_bInuse;
//    }
//    virtual void MPSSetFormat(int format) {
//        m_format = format;
//    }
//    virtual int MPSGetFormat() {
//        return int(m_format);
//    }
    virtual int MPSStart() {
        m_nStatus = 1;
        return 0;
    }
//    virtual int MPSPause();
    virtual int MPSStop() {
        m_nStatus = 0;
        return 0;
    }

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
//    bool m_bInuse;
    CMediaProcessor* m_pProcessor;
//    MediaFormatMask m_format;
    int m_nStatus;  // 0 idle, 1 working, 2 paused
    union {
        void* m_pStorageObject;
        IMediaProcessor* m_pStorageObjectPeerProcessor;
        CMediaDevice* m_pStorageObjectDevice;
        LPCTSTR* m_pStorageObjectFilename;
        ISocket* m_pStorageObjectSocket;
    };
    long m_lStorageObjectParam;
};

class CMediaInputPort : public MediaPortShell, public IDataSink/*, public CNetRecvConfigDelegate*/ {
#pragma mark - public functions constructor/destructor destructor
public:
    CMediaInputPort(CMediaProcessor* pProcessor, int format);
    virtual ~CMediaInputPort();

#pragma mark - public functions inherits from IMediaPort
public:
    virtual int SetStorageObject(void* pObject,int lParam);
    virtual void* GetStorageObject(int* lParam=0);
    virtual int GetPortDirection();//<in,out>
    virtual int GetSupportedFormatCount();
    virtual int GetSupportedFormat(int i);
    virtual int SelectFormat(int fmt);
    virtual int GetSelectedFormat();

#pragma mark - public functions inherits from IDataSink
public:
    virtual int WriteData(const char* buf,int size);

#pragma mark - public functions inherits from MediaPortShell
public:
virtual int MPSStart();
//    virtual int MPSPause();
virtual int MPSStop();

#pragma mark - public functions inherits from IDataDelegate
public:
    virtual int PutData(const char* data, int len, long mediaid, int format);

#pragma mark - public functions own
public:
    virtual IDataSink* GetDataSink();
    virtual void OnSourceCanRead(int nErrCode, IDataSource* pSource);
    
#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
};


class CMediaOutputPort : public MediaPortShell, public IDataSource, public CNetSendConfigDelegate {
#pragma mark - public functions constructor/destructor destructor
public:
    CMediaOutputPort(CMediaProcessor* pProcessor, int format);
    virtual ~CMediaOutputPort();

#pragma mark - public functions inherits from IMediaPort
public:
    virtual int SetStorageObject(void* pObject,int lParam);
    virtual void* GetStorageObject(int* lParam=0);
    virtual int GetPortDirection();//<in,out>
    virtual int GetSupportedFormatCount();
    virtual int GetSupportedFormat(int i);
    virtual int SelectFormat(int fmt);
    virtual int GetSelectedFormat();

#pragma mark - public functions inherits from IDataSource
public:
    virtual int ReadData(char* buf,int size);

#pragma mark - public functions inherits from MediaPortShell
public:
    virtual int MPSStart();
    //    virtual int MPSPause();
    virtual int MPSStop();

#pragma mark - public functions inherits from IDataDelegate
public:
    virtual int PutData(const char* data, int len, long mediaid, int format);

#pragma mark - public functions inherits from CNetSendConfigDelegate
public:
    virtual int SetFECType(int type,void* pParam);
    virtual int GetFECType(void* pParam);
    virtual int SetNATHost(const char* szHostAddr);
    virtual int SetSmoothType(int type,void* pParam);
    virtual int GetSmoothType(void* pParam);
    virtual IMediaStreamConfig* GetInputStreamConfig(int iPort=0);

#pragma mark - public functions own
public:
    virtual IDataSource* GetDataSource();
    virtual void SetMaxBufCount(int nCount);
    virtual void OnSinkCanWrite(int nErrCode, IDataSink* pSink);
    virtual int TryWriteDataToStorageNet(const char* buf, int size); // return: 0 OK, 1 delayed

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    struct MOPBuffer {
        char* _buf;
        int _size;
        MOPBuffer(const char* buf=NULL, int size=0) {
            _size = 0;
            _buf = NULL;
            put(buf, size);
        }
        virtual ~MOPBuffer() {
            if (_buf)
                delete[] _buf;
        }
        virtual void put(const char* buf, int size) {
            if (_buf)
                delete[] _buf;
            _size = size;
            _buf = NULL;
            if (size > 0) {
                _buf = new char[size];
                if (buf) {
                    memcpy(_buf, buf, size);
                }
            }
        }
    };
    MyHandleList<MOPBuffer*> m_buf;
    int m_nMaxBufCount;
};

}//end MediaLib


#endif /* CMediaPort_h */
