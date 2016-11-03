#include "__PCH.h"
#include "OSApi.h"

#ifdef __ENV_WIN
#include "ThunkProc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int StartBackgourndNetwork(int fd){return 0;};
void StopBackgroundNetwork(){};


struct __OSTIMER:public _THUNKPROC
{
	UINT idEvent;
	void* pUserData;
	LPOSTimerProc pUserTimerProc;
	__OSTIMER(LPOSTimerProc pProc,void* pData)
	{
		idEvent=0;
		pUserData=pData;
		pUserTimerProc=pProc;
		SetThunkData(this,(DWORD)&_TimerProc);
	}
	int Start(int timeout)
	{
		if(this==0) return 0;
		Stop();
		idEvent=::SetTimer(NULL,0,timeout,(TIMERPROC)this);
		return 1;
	}
	void Stop()
	{
		if(this)
		{
			if(idEvent) KillTimer(NULL,idEvent);
			idEvent=0;
		}
	}
	static VOID CALLBACK _TimerProc(POSTIMER pTimer,HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime)
	{
		pTimer->pUserTimerProc(pTimer->pUserData);
	};
};
POSTIMER OSCreateTimer(LPOSTimerProc pTimeoutProc,void* pUserData)
{
//	return new __OSTIMER(pTimeoutProc,pUserData);
	//typedef LPVOID (WINAPI *pVirtualAlloc)( LPVOID lpAddress,    SIZE_T dwSize,	    DWORD flAllocationType,    DWORD flProtect);

	//pVirtualAlloc pfun=(pVirtualAlloc)GetProcAddress(GetModuleHandle("kernel32.dll"),"VirtualAlloc");

	__OSTIMER* p=(__OSTIMER*)VirtualAlloc(NULL,sizeof(__OSTIMER),MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
#pragma push_macro("new")
#undef new
	::new( p ) __OSTIMER(pTimeoutProc,pUserData);
#pragma pop_macro("new")
	DWORD dwProt=0;
//	VirtualProtect(p,sizeof(__OSTIMER),PAGE_EXECUTE_READWRITE,&dwProt);
	return p;
}
bool OSStartTimer(POSTIMER pTimer,int timeout){return pTimer->Start(timeout);}
void OSStopTimer(POSTIMER pTimer){pTimer->Stop();}
void OSReleaseTimer(POSTIMER pTimer)
{
	pTimer->Stop();
	TRACE("virtual free=%d\n",VirtualFree(pTimer,0,MEM_RELEASE));
}

POSTHREAD OSCreateThread(LPOSThreadProc pThreadProc,void* pUserData,long* pThreadId)
{
	return (POSTHREAD)::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)pThreadProc,pUserData,CREATE_SUSPENDED,(LPDWORD)pThreadId);
}
int OSResumeThread(POSTHREAD pThread){return ::ResumeThread((HANDLE)pThread);};
int OSSuspendThread(POSTHREAD pThread){return ::SuspendThread((HANDLE)pThread);};
void OSReleaseThread(POSTHREAD pThread)
{
	HANDLE hThread=(HANDLE)pThread;
	if(!hThread) return;
	int ret=0;
	do 
	{
		DWORD dwExitCode=0;
		bool rt=GetExitCodeThread(hThread,&dwExitCode);
		int t=0;
		if (dwExitCode==STILL_ACTIVE)
		{
			Sleep(100);
			t++;
			if (t>50)
			{
				TerminateThread(hThread,1);
				ret =1;
				break;
			}
			continue;
		}
	} while(0);

	CloseHandle(hThread);
	hThread=0;

};

long OSGetCurrentThreadId()
{
	return GetCurrentThreadId();
}

#else
#include "DispatchWrapper.h"
//typedef void (*LPOSTimerProc)(void* pUserData);

//struct __OSTIMER:public dispatch_source_s{};

POSTIMER OSCreateTimer(LPOSTimerProc pTimeoutProc,void* pUserData)
{
	dispatch_source_t st=dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,0,0,dispatch_get_main_queue());
	dispatch_set_context(st,pUserData);
	dispatch_source_set_event_handler_f(st,pTimeoutProc);
	dispatch_source_set_timer(st,dispatch_time(DISPATCH_TIME_FOREVER,0),DISPATCH_TIME_FOREVER,0);
	dispatch_resume(st);
	TRACETIMER("OSCreateTimer[%p]:pProc=%p,pUser=%p\n",st,pTimeoutProc,pUserData);
	return (POSTIMER)st;
	
}
bool OSStartTimer(POSTIMER pTimer,int timeout)
{
	dispatch_source_t st=(dispatch_source_t)pTimer;
	uint64_t tv=NSEC_PER_SEC*timeout/1000;
	dispatch_source_set_timer(st,dispatch_time(DISPATCH_TIME_NOW,tv),tv,0);
	TRACETIMER("OSStartTimer[%p]:timeval=%d,pUser=%p\n",st,timeout,dispatch_get_context(st));
	return 1;
	
}
void OSStopTimer(POSTIMER pTimer)
{
	dispatch_source_t st=(dispatch_source_t)pTimer;
	dispatch_source_set_timer(st,dispatch_time(DISPATCH_TIME_FOREVER,0),DISPATCH_TIME_FOREVER,0);
	TRACETIMER("OSStopTimer[%p]:pUser=%p\n",st,dispatch_get_context(st));
}
void OSReleaseTimer(POSTIMER pTimer)
{
	dispatch_source_t st=(dispatch_source_t)pTimer;
	TRACETIMER("OSStopTimer[%p]:pUser=%p\n",st,dispatch_get_context(st));
	dispatch_release(st);
	st=0;
}


//typedef struct __OSTHREAD* POSTHREAD;
//typedef long (__stdcall *LPOSThreadProc)(void* pUserData);
typedef void* (*PPThreadProc)(void*);
POSTHREAD OSCreateThread(LPOSThreadProc pThreadProc,void* pUserData,long* pThreadId)
{
	pthread_t t=0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_set_qos_class_np(&attr, qos_class_main(), 0);
	
	pthread_create(&t, &attr, (PPThreadProc)pThreadProc, pUserData);
	pthread_attr_destroy(&attr);
	if(pThreadId) *pThreadId=t->__sig;
	
	return (POSTHREAD)t;
	
}
int OSResumeThread(POSTHREAD pThread)
{
	return 0;
}
int OSSuspendThread(POSTHREAD pThread)
{
	return 0;
}
int OSTerminateThread(POSTHREAD pThread)
{
	pthread_kill((pthread_t)pThread,SIGKILL);
	return 0;
}
void OSExitThread(DWORD dwExcode)
{
	pthread_exit((void*)(long)dwExcode);
}
void OSReleaseThread(POSTHREAD pThread)
{
	
}
long OSGetCurrentThreadId()
{
	return (long)pthread_self();
}



#endif

long OSGetSysTick(){return (long)time(0);};
int OSGetLastError()
{
#ifdef __ENV_WIN
	return GetLastError();
#else
	return errno;
#endif
}
