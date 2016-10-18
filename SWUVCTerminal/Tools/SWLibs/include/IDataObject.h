#ifndef __IDATAOBJECT__H__
#define __IDATAOBJECT__H__
#include "typedef.h"
struct IObject
{
//	virtual void HandleObjectMessage(void* pObject,int ev,void* lpParam){};
	virtual long SetUserData(long lUserData){return 0;}
	virtual long GetUserData(){return 0;}
	virtual int GetProperty(int iProp,void* pProp,int size=0){return 0;};
	virtual int SetProperty(int iProp,void* pProp,int size=0){return 0;};
	virtual void Release(){delete this;}
	virtual ~IObject(){};
};

#define RELEASEOBJ(pObj) {if(pObj) pObj->Release();pObj=0;}

enum DataChannelErrordeCode
{
	DataChannelClose =-1,
	DataChannelReady = 0,
	DataChannelOpen,
};

//single data source and sink
struct IDataSink
{
	virtual int WriteData(const char* buf,int size){return -1;};
};

struct IDataSource
{
//	virtual int SetDataSink(IDataSink* pSink);
	/*
		1.该函数应该在数据通知线程中使用
		返回:
			>0 实际读取的字节数
			=0 数据读完,还有后续数据
			=-1 数据读完,没有后续数据
			<-1 发生错误
	*/
	virtual int ReadData(char* buf,int size){return -1;};
	virtual const char* GetDataPtr(int* size){return 0;};

	//由派生类实现主持接口,以便实派生类保存IDataObserver派生类指针,注册的对象必须由IDataObserver派生
//	virtual void RegisterObserver(IDataObserver* pObserver,long userParam);
};



struct IDataSinkObserver
{
	/*
		nErrCode:0=Ready,1=Opened,-1=Closed
	*/
	virtual void OnSinkCanWrite(int nErrCode,struct IDataSink* pSink,long userParam){};
};

struct IDataSourceObserver
{
	/*
		1.该回调由源的线程驱动,
		2.实现中要调用源的ReadData方法,一次性把所有产生的数据读完
		3.该回调要尽快返回,以避免源端数据堆积,造成数据丢失
		4.返回实际读取的字节数
		nErrCode:0=Ready,1=Opened,-1=Closed
	*/
	virtual void OnSourceCanRead(int nErrCode,IDataSource* pSource,long userParam){};
};

struct IDataSock;
struct IDataSockObserver:public IDataSourceObserver,public IDataSinkObserver
{
	//virtual void OnSinkCanWrite(int nErrCode,IDataSock* pSock,long userParam){};
	//virtual void OnSourceCanRead(int nErrCode,IDataSock* pSock,long userParam){};
};

struct IDataSock:public IDataSource,public IDataSink
{
//	virtual int ReadData(char* buf,int size){return -1;};
//	virtual int WriteData(const char* buf,int size){return -1;};
};

struct IDataPipe:public IDataSink,public IDataSource
{
	//	virtual int ReadData(char* buf,int size){return -1;};
	//	virtual int WriteData(const char* buf,int size){return -1;};
};



//multi data source and sink
struct IMultiDataSource
{
	virtual int ReadData(char* buf,int size,int iSource,long userParam)=0;
};

struct IMultiDataSink
{
	virtual int WriteData(const char* buf,int size,int iSink,long userParam)=0;
};

struct IMultiDataSinkObserver
{
	virtual void OnSinkCanWrite(int nErrCode,struct IMultiDataSink* pSink,int iSink,long userParam){};
};
struct IMultiDataSourceObserver
{
	virtual void OnSourceCanRead(int nErrCode,IMultiDataSource* pSource,int iSource,long userParam){};
};


struct IMultiDataSock:virtual public IMultiDataSource,virtual public IMultiDataSink
{
    
};

struct IMultiDataSockObserver
{
	virtual void OnSourceCanRead(int nErrCode,IMultiDataSock* pSock,int iSource,long userParam){};
	virtual void OnSinkCanWrite(int nErrCode,IMultiDataSock* pSock,int iSink,long userParam){};
};

template<typename TImpl,typename IT>
TImpl* ConvertToDerivedPtr(IT* p)
{
	TImpl* pImpl=(TImpl*)&p;
	int off=LPBYTE(((IT*)pImpl))-LPBYTE(pImpl);
	pImpl=(TImpl*)(LPBYTE(p)-off);
	return pImpl;
}

#endif  //#ifndef
