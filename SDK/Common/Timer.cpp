#include "__PCH.h"
#include "Timer.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static void SecondTimerTimeout(void* pUserData)
{
	SecondTimer* pt=(SecondTimer*)pUserData;
	if(pt->_pObserver) 
		pt->_pObserver->OnTimerTimeout(pt,pt->_name);
}


SecondTimer::SecondTimer(ITimerObserver* pObserver,long name)
{
	_pObserver=pObserver;_name=name;
	_pTimer=OSCreateTimer(SecondTimerTimeout,this);
}
SecondTimer::~SecondTimer()
{
	OSReleaseTimer(_pTimer);

}
bool SecondTimer::Start(int second,long name)
{
	if(second==0) return 0;
	second*=1000;
	if(name>=0) _name=name;
	return OSStartTimer(_pTimer,second);
}
void SecondTimer::Stop()
{
	OSStopTimer(_pTimer);
}

ITimer* CreateSecondTimer(ITimerObserver* pObserver,long name)
{
	return new SecondTimer(pObserver,name);
}
