
#include "__PCH.h"
#include "Thread.h"



const int _nMaxSource=64;	
const DWORD _dwBaseMsgId=0x6000;

#ifdef __ENV_WIN
#include "ThunkProc.h"
#include "DataSet.h"
#include <MMSystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//static DWORD _dwThreadMsgQue=TlsAlloc();


struct LockObject:ILockObject
{
	CRITICAL_SECTION _cs;
	LockObject()
	{
		InitializeCriticalSection(&_cs);
	}
	~LockObject()
	{
		DeleteCriticalSection(&_cs);
	}
	virtual int Lock()
	{
		EnterCriticalSection(&_cs);
		return 0;
	}
	virtual void Unlock()
	{
		LeaveCriticalSection(&_cs);
	}
};

ILockObject* ILockObject::Create()
{
	return new LockObject(); 
}

struct EventObject:public IEventObject
{
	HANDLE hEvent;
	EventObject(bool bManual,bool bSiged,const char* name=0)
	{
		hEvent=CreateEvent(0,bManual,bSiged,name);
	}
	virtual ~EventObject()
	{
		CloseHandle(hEvent);
	};
	virtual int Wait(DWORD ms=-1)
	{
		return WaitForSingleObject(hEvent,ms);
	}
	virtual bool Siginal(bool bSig=1)
	{
		bool ret=0;
		if(bSig)
		{
			ret=SetEvent(hEvent);

		}
		else
		{
			ret=ResetEvent(hEvent);
		}
		return ret;
	}
};
IEventObject* Create(bool bManual,bool bSiged,const char* name=0)
{
	return new EventObject(bManual,bSiged,name);
}


static __declspec(thread) IMsgQueue* _pCurrentMsgQue;


struct MsgQueue:public _THUNKPROC,public IMsgQueue
{
	HWND _hwnd;
	long _tid;
	ThreadBase* _pThread;
	typedef IMsgSourceObserver* MSGSRC;
	MSGSRC _srcList[_nMaxSource];
protected:
	MsgQueue(long tid=0)
	{
//		TRACE("+++++++++++++MsgQueue:Contruct--%p\n",this);
		SetThunkData(this,(DWORD)&_ThreadQueueProc);
		ZeroMemory(_srcList,_nMaxSource*sizeof(MSGSRC));
		_hwnd=0;
		_tid=tid;
		_pThread=0;
	}
public:
	void operator delete(void* p)
	{
//		TRACE("xxxxxxxxxxxxxxMsgQueue:delete--%p\n",p);
		VirtualFree(p,0,MEM_RELEASE);
	}

	~MsgQueue()
	{
//		TRACE("----------MsgQueue:Destruct--%p\n",this);
		if(_hwnd) ::DestroyWindow(_hwnd);
		_hwnd=0;
		if (_pThread)
		{
			_pThread->Close();
			delete _pThread;
			_pThread=0;
		}
		
		_tid=0;
	}
	int CreateMsgWindow()
	{
		LPCTSTR szClassName="ThreadQueueWindowClass";
		char szWindowName[64]={0};
		sprintf(szWindowName,"__ThreadQueue__Window::%d",GetCurrentThreadId());
		HINSTANCE hInstance=(HINSTANCE)::GetCurrentProcess();
		WNDCLASSEX   wndclassex = {0};
		wndclassex.cbSize        = sizeof(WNDCLASSEX);
		wndclassex.style         = 0;
		wndclassex.lpfnWndProc   = DefWindowProc;
		wndclassex.cbClsExtra    = 0;
		wndclassex.cbWndExtra    = 0;
		wndclassex.hInstance     = hInstance;
		wndclassex.hIcon         = 0;
		wndclassex.hCursor       = 0;
		wndclassex.hbrBackground = 0;
		wndclassex.lpszMenuName  = NULL;
		wndclassex.lpszClassName = szClassName;
		wndclassex.hIconSm       = 0;

		if (!RegisterClassEx (&wndclassex))
		{
			int rt=::GetLastError();
			if(ERROR_CLASS_ALREADY_EXISTS!=rt)
				return 0;
		}
		_hwnd = CreateWindowEx (0,szClassName,szWindowName,0,0,0,0,0,HWND_MESSAGE,NULL, hInstance,NULL);
		::SetWindowLong(_hwnd,GWL_WNDPROC,(LONG)((_THUNKPROC*)this));
		return _hwnd==0;
	}

	bool CreateThread()
	{
		 _pThread=new ThreadBase();
		 _tid=_pThread->_dwThreadId;
		 _pThread->SetMsgQueue(this);
		 _pThread->Start();

		return 1;
	}

	virtual int PumpSourceMessage()
	{
		MSG wndMsg;
		int ret=::GetMessage(&wndMsg,_hwnd,0,0);
		TRACETHREAD("MsgQue_Pumpmsg:ret=%d,hwnd=%p,srcid=%p,msg=%d,pMsg=%p\n",ret,wndMsg.hwnd,wndMsg.message,wndMsg.wParam,(void*)wndMsg.lParam);
		if (ret)
		{
			if(_hwnd)
			{
				::DispatchMessageA(&wndMsg);
			}
			else
			{
				DWORD dwSourceId=wndMsg.message;
				int iSrc=dwSourceId-_dwBaseMsgId;
				if(iSrc>-1 && iSrc<_nMaxSource&& _srcList[iSrc])
				{
					_srcList[iSrc]->OnMsgSourceMessage(dwSourceId,wndMsg.wParam,(void*)wndMsg.lParam);
				}
				
			}
		}
		return ret;
	}


	virtual int DispatchSourceMessage(DWORD dwSourceId,UINT msg,void* pMsg)
	{
		int iSrc=dwSourceId-_dwBaseMsgId;
		if(iSrc>-1 && iSrc<_nMaxSource&& _srcList[iSrc])
		{
			return _srcList[iSrc]->OnMsgSourceMessage(dwSourceId,msg,pMsg);
		}
		return 0;

	}
	virtual int PostSourceMessage(DWORD dwSourceId,UINT msg,void* pMsg)
	{
		TRACETHREAD("MsgQueuePostSourceMsg: tid=%d, msg=%p,wp=%d,lp=%d\n",_tid,dwSourceId,msg,int(pMsg));
		if (dwSourceId==0)
		{
			PostThreadMessage(_tid,WM_QUIT,0,0);
			return 0;
		}
		if(_hwnd)	
			return ::PostMessage(_hwnd,dwSourceId,msg,long(pMsg));
		return ::PostThreadMessageA(_tid,dwSourceId,msg,long(pMsg));
	};
	virtual int SendSourceMessage(DWORD dwSourceId,UINT msg,void* pMsg)
	{
		if(_hwnd)
			return ::SendMessage(_hwnd,dwSourceId,msg,long(pMsg));
		return 0;
	};
	int FindSource(IMsgSourceObserver* pSrc)
	{
		for (int i=0;i<_nMaxSource;i++)
		{
			if (_srcList[i]==pSrc)
			{
				return i;
			}
		}
		return -1;

	}
	virtual DWORD AddMsgSource(IMsgSourceObserver* pSource)
	{
		int iSrc=FindSource(pSource);
		if(iSrc>-1) return _dwBaseMsgId+iSrc;
		iSrc=FindSource(0);
		if (iSrc<0)
		{
			return 0;
		}
		_srcList[iSrc]=pSource;
		return _dwBaseMsgId+iSrc;

	}
	virtual void RemoveMsgSource(DWORD dwSourceId)
	{
		int iSrc=dwSourceId-_dwBaseMsgId;
		if(iSrc>-1 && iSrc<_nMaxSource)
		{
			_srcList[iSrc]=0;
		}
	}
	static long CALLBACK _ThreadQueueProc(MsgQueue* pMsgQue,HWND hwnd,UINT msg,UINT wParam,long lParam)
	{
		if (msg<WM_USER)
		{
			return ::DefWindowProc(hwnd,msg,wParam,lParam);
		}
		return pMsgQue->DispatchSourceMessage(msg,wParam,(void*)lParam);
	}

	static MsgQueue* New(int tid=0)
	{
		MsgQueue* p=(MsgQueue*)VirtualAlloc(NULL,sizeof(MsgQueue),MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
		#pragma push_macro("new")
		#undef new
				::new( p ) MsgQueue(tid);
		#pragma pop_macro("new")
		return p;
	}
	

};

IMsgQueue* IMsgQueue::GetMain()
{
	static AutoDeletePtr<MsgQueue> _MainPtr;
	if(_MainPtr==NULL)
	{
		MsgQueue* p=MsgQueue::New(GetCurrentThreadId());
		p->CreateMsgWindow();
		_MainPtr=p;
	}
	return (MsgQueue*)_MainPtr;;
}

IMsgQueue* IMsgQueue::Create(LPCTSTR name)
{
	MsgQueue* p=MsgQueue::New();
	p->CreateThread();
	return p;
}



#else
#include "DispatchWrapper.h"
struct MsgQueue:public IMsgQueue
{
	DQ _q;
	typedef IMsgSourceObserver* MSGSRC;
	MSGSRC _srcList[_nMaxSource];
	
public: 
	IMsgSourceObserver* GetObserver(DWORD dwSourceId)
	{
		return _srcList[dwSourceId-_dwBaseMsgId];
	}
	virtual int PumpSourceMessage()
	{
		return 0;
	};
	virtual int DispatchSourceMessage(DWORD dwSourceId,UINT msg,void* pMsg)
	{
		int iSrc=dwSourceId-_dwBaseMsgId;
		if(iSrc>-1 && iSrc<_nMaxSource&& _srcList[iSrc])
		{
			return _srcList[iSrc]->OnMsgSourceMessage(dwSourceId,msg,pMsg);
		}
		return 0;
		
	}
	virtual int PostSourceMessage(DWORD dwSourceId,UINT msg,void* pMsg)
	{
		_q.AsyncCall(^{
			GetObserver(dwSourceId)->OnMsgSourceMessage(dwSourceId,msg,pMsg);
		});
		return 1;
	};
	virtual int SendSourceMessage(DWORD dwSourceId,UINT msg,void* pMsg)
	{
		return 0;
	};
	int FindSource(IMsgSourceObserver* pSrc)
	{
		for (int i=0;i<_nMaxSource;i++)
		{
			if (_srcList[i]==pSrc)
			{
				return i;
			}
		}
		return -1;
		
	}
	virtual DWORD AddMsgSource(IMsgSourceObserver* pSource)
	{
		int iSrc=FindSource(pSource);
		if(iSrc>-1) return _dwBaseMsgId+iSrc;
		iSrc=FindSource(0);
		if (iSrc<0)
		{
			return 0;
		}
		_srcList[iSrc]=pSource;
		return _dwBaseMsgId+iSrc;
		
	}
	virtual void RemoveMsgSource(DWORD dwSourceId)
	{
		int iSrc=dwSourceId-_dwBaseMsgId;
		if(iSrc>-1 && iSrc<_nMaxSource)
		{
			_srcList[iSrc]=0;
		}
	}
	
	static MsgQueue* New()
	{
		MsgQueue* p=new MsgQueue();
		return p;
	}
	
	
};



IMsgQueue* IMsgQueue::GetMain()
{
	MsgQueue* p=new MsgQueue();
	p->_q.FromMain();
	return p;
}
IMsgQueue* IMsgQueue::Create(LPCTSTR name)
{
	static unsigned int index=0;
	char szName[64];
	sprintf(szName,"__TempMsgQueue__%08x__",index);index++;
	if(name==0 || *name==0) name=szName;
	MsgQueue* p=new MsgQueue();
	p->_q.Create(name,0);
	return p;
}

#endif
//---------------end msg que

static long __stdcall _BaseThreadProc(void* pThreadBase)
{
	return ((IThreadBase*)pThreadBase)->Worker();
}


ThreadBase::ThreadBase()
{
	_pMsgQue=0;
	_pThread=OSCreateThread(_BaseThreadProc,this,&_dwThreadId);
}
ThreadBase::~ThreadBase()
{
	OSReleaseThread(_pThread);
}

void ThreadBase::SetMsgQueue(IMsgQueue* p)
{
	_pMsgQue=p;
}

int ThreadBase::Start()
{
	if(!_pThread)
	{
		_pThread=OSCreateThread(_BaseThreadProc,this,&_dwThreadId);
	}
	int ret= OSResumeThread(_pThread);
#ifdef __ENV_WIN	
	while(0==::PostThreadMessageA(_dwThreadId,0,0,0))
	{
		Sleep(10);
	}
#else
	
#endif	
	return ret;
}

int ThreadBase::Stop()
{
	if (_pThread)
	{
		OSSuspendThread(_pThread);
	}
	return 0;
}

int ThreadBase::Close()
{
	int ret=0;
	if (_pThread)
	{
		if(_pMsgQue)
		{
			_pMsgQue->PostSourceMessage(0,0,0);
		}
		OSReleaseThread(_pThread);
		_pThread=0;
	}
	_pMsgQue=0;
	_dwThreadId=0;
	return ret;
}

int ThreadBase::Worker()
{
	int ret=0;
	TRACETHREAD("thread start tid=%ld\n",OSGetCurrentThreadId());
#ifdef __ENV_WIN	
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	_pCurrentMsgQue=_pMsgQue;
	while (_pMsgQue && _pMsgQue->PumpSourceMessage());
	_pMsgQue=0;
#else
	
#endif	
	TRACETHREAD("thread exit tid=%ld\n",OSGetCurrentThreadId());
	return ret;

}


_TimeUse::_TimeUse()
{
	Reset();
}
void _TimeUse::Reset()
{
	_tmStart=OSGetSysTick();
}
DWORD _TimeUse::GetUse()
{
	return OSGetSysTick()-_tmStart;
}
void _TimeUse::TraceUse(const char* fmt)
{
	if(fmt==0) fmt="Time use=%d\n";
	TRACE(fmt,GetUse());
}
