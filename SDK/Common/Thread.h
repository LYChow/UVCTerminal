#ifndef __THREAD__H__
#define __THREAD__H__
#include "OSApi.h"
#include "IDataObject.h"
#include "DataSet.h"

struct ILockObject
{
	virtual ~ILockObject(){};
	virtual void Release(){delete this;}
	virtual int Lock()=0;
	virtual void Unlock()=0;
	static ILockObject* Create();
};

struct __Lock
{
	ILockObject* pLock;
	__Lock(ILockObject* p)
	{
		pLock=p;
		if(pLock) pLock->Lock();
	}
	~__Lock()
	{
		if(pLock) pLock->Unlock();
	}
};

struct IEventObject
{
	virtual ~IEventObject(){};
	virtual void Release(){delete this;}
	virtual int Wait(DWORD ms=-1)=0;
	virtual bool Siginal(bool bSig=1)=0;
	static IEventObject* Create(bool bManual=0,bool bSiged=0,const char* name=0);
};


struct IMsgSourceObserver
{
	virtual int OnMsgSourceMessage(DWORD dwSourceId,UINT msg,void* pMsg)
	{
		TRACE("OnMsgSourceMessage:tid=%d,this=%p,srcid=%d,msg=%d,pMsg=%p\n",OSGetCurrentThreadId(),
			this,dwSourceId,msg,pMsg);
		return 0;
	};
};

struct IMsgQueue:public IObject
{
	virtual int PumpSourceMessage()=0;
	virtual int DispatchSourceMessage(DWORD dwSourceId,UINT msg,void* pMsg)=0;
	virtual int PostSourceMessage(DWORD dwSourceId,UINT msg,void* pMsg)=0;
	virtual int SendSourceMessage(DWORD dwSourceId,UINT msg,void* pMsg)=0;
	virtual DWORD AddMsgSource(IMsgSourceObserver* pObserver)=0;
	virtual void RemoveMsgSource(DWORD dwSourceId)=0;

	static IMsgQueue* GetMain();
	static IMsgQueue* Create(LPCTSTR name=0);
};

struct MsgDispatcher:public IMsgSourceObserver
{
	IMsgQueue* pQue;
	DWORD dwSourceId;
	MsgDispatcher(IMsgQueue* q=0,DWORD srcId=0)
	{
		pQue=q;dwSourceId=srcId;
	}

	bool AttachToQue(IMsgQueue* q){pQue=q;dwSourceId=q->AddMsgSource(this);return dwSourceId!=0;}
	IMsgQueue* DetachQue()
	{
		IMsgQueue* p=pQue;
		if(pQue){pQue->RemoveMsgSource(dwSourceId);}
		pQue=0;dwSourceId=0;
		return p;
	}
	int PostMsg(int msg,void* pMsg){return pQue->PostSourceMessage(dwSourceId,msg,pMsg);}
	int SendMsg(int msg,void* pMsg){return pQue->SendSourceMessage(dwSourceId,msg,pMsg);}
};


struct IThreadBase
{
	virtual IMsgQueue* GetMsgQueue(){return 0;};
	virtual int Start()=0;
	virtual int Stop()=0;
	virtual int Close()=0;
	virtual int Worker()=0;
};


struct ThreadBase:public IThreadBase
{
	ThreadBase();
	~ThreadBase();
	void SetMsgQueue(IMsgQueue* pQue);
	IMsgQueue* GetMsgQueue(){return _pMsgQue;}
	virtual int Start();
	virtual int Stop();
	virtual int Close();
	virtual int Worker();
public:
	POSTHREAD _pThread;
	long _dwThreadId;
	IMsgQueue* _pMsgQue;
};

//////////////////////////////////////////////////////////////////////////

template <typename T,typename ARG_T=T&>
class LockQueue
{
public:
	LockQueue()
	{
		m_lock=ILockObject::Create();
	}
	~LockQueue()
	{
		m_lock->Release();
	}
	ILockObject* GetLockObject(){return m_lock;}
	POSPTR Queue(ARG_T p)
	{
		__Lock lock(m_lock);
		return m_list.AddTail(p);
	}

	IRESULT Dequeue(ARG_T pp)
	{
		__Lock lock(m_lock);

		if (m_list.IsEmpty())
		{
			//*pp = NULL;
			return -1;
		}

		pp= m_list.RemoveHead();
		return 0;
	}

	POSPTR PutBack(ARG_T p)
	{
		__Lock lock(m_lock);
		return m_list.AddHead(p);
	}

	void Clear() 
	{
		__Lock lock(m_lock);
		m_list.RemoveAll();
	}


private:
	ILockObject*    m_lock; 
	BaseList<T,ARG_T>   m_list;
};

struct _TimeUse
{
	DWORD _tmStart;
	_TimeUse();
	void Reset();
	DWORD GetUse();
	void TraceUse(const char* fmt=0);

};

#endif
