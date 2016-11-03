//
//  CMediaProcessor.h
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/29.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#ifndef CMediaProcessor_h
#define CMediaProcessor_h

#include "MyHandleList.h"
#include "IMediaLib.h"
#include "CMediaDevice.h"
#include "CMediaPort.h"

namespace MediaLib{

class CMediaProcessor : public IMediaProcessor
{
#pragma mark - public functions constructor/destructor destructor
public:
    CMediaProcessor(int nInCount,int nOutCount,int fmtIn=0,int fmtOut=0);
    virtual ~CMediaProcessor();

#pragma mark - public functions inherits from IMediaProcessor
public:
#pragma mark --input
    virtual int SetInputCount(int count);
    virtual int GetInputCount();
    virtual int AddInput(int fmt=0);
    virtual int RemoveInput(int iPort);
    virtual int SetInputFormat(int fmt,int iPort=-1);
    virtual int GetInputFormat(int iPort=0);
    virtual int SetInputStorageObject(void* pStrogeObject,long lParam,int iPort);
    virtual void* GetInputStorageObject(long* lParam=0,int iPort=0);
//    virtual int InputConnect(IMediaProcessor* pProcessor,int iPeerOutPort=0,int iPort=0);

#pragma mark --output
    virtual int SetOutputCount(int count);
    virtual int GetOutputCount();
    virtual int AddOutput(int fmt=0);
    virtual int RemoveOutput(int iPort);
    virtual int SetOutputFormat(int fmt,int iPort=-1);
    virtual int GetOutputFormat(int iPort=0);
    virtual int SetOutputStorageObject(void* pStrogeObject,long lParam,int iPort); // 仅用于连接末端节点
    virtual void* GetOutputStorageObject(long* lParam=0,int iPort=0);

#pragma mark --foundation
    virtual void SetMediaStreamId(long mid,bool bOutput,int iPort=0);
    virtual long GetMediaStreamId(bool bOutput,int iPort=0);
    virtual IDataSource* GetOutputSource(int iPort=0);
    virtual IDataSink* GetInputSink(int iPort=0);
//    virtual int GetInputConnectedPort(IMediaProcessor*& pProcessor,int iPort=0);
//    virtual int GetOutputConnectedPort(IMediaProcessor*& pProcessor,int iPort=0);
    virtual int Start();
//    virtual int Pause();
    virtual int Stop();
    virtual int GetStatus(); // 0 idle, 1 working
    virtual int EnumConfig(int* pConfigTypes=0,int count=-1);
    virtual IMediaConfig* GetConfig(int configType);
    virtual long SetUserData(long lUserData);
    virtual long GetUserData();
    long m_lUserData;

#pragma mark - public functions inherits from IDataSockObserver
    virtual void OnSinkCanWrite(int nErrCode,IDataSink* pSink,long userParam);
    virtual void OnSourceCanRead(int nErrCode,IDataSource* pSource,long userParam);

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:
    virtual int SetInputFormat(int fmt, CMediaInputPort* pInputPort);
    virtual int SetOutputFormat(int fmt, CMediaOutputPort* pOutputPort);
    virtual int CreateDataDelegateChainFor(IDataDelegate* pDataDelegate);
    virtual int CreateDataDelegateChainBetween(CMediaInputPort* pInputPort, CMediaOutputPort* pOutputPort);
    virtual int RemoveDataDelegateChainFrom(IDataDelegate* pDataDelegate, bool bIncludeCurrentNode=true);
    virtual bool ExistDataDelegateChainBetween(IDataDelegate* pSourceDataDelegate, IDataDelegate* pReceiverDataDelegate);
    virtual void DeleteOutdatedDataDelegates();

#pragma mark - protected members
protected:
    MyHandleList<CMediaInputPort*> m_listInputPort;
    int m_nDefaultInputFormat;

    MyHandleList<CMediaOutputPort*> m_listOutputPort;
    int m_nDefaultOutputFormat;

    MyHandleList<IDataDelegate*> m_listForsakedDataDelegate;

    int m_nStatus; // 0 idle, 1 working, 2 paused
};

}//end MediaLib

#endif /* CMediaProcessor_h */
