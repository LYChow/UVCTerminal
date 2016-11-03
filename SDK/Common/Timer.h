#ifndef __ITIMER__H__
#define __ITIMER__H__
#include "OSApi.h"
#include "IDataObject.h"
struct ITimerObserver
{
	virtual void OnTimerTimeout(struct  ITimer* pTiemr,long name)=0;
};

struct ITimer:public IObject
{
public:
	virtual bool Start(int timeout,long name=-1){return 0;};
	virtual void Stop(){};
};

struct SecondTimer:public ITimer
{
	POSTIMER _pTimer;
	ITimerObserver* _pObserver;
	long _name;
public:
	SecondTimer(ITimerObserver* pObserver,long name);
	~SecondTimer();
	virtual bool Start(int second,long name=-1);
	virtual void Stop();
};

ITimer* CreateSecondTimer(ITimerObserver* pObserver,long name);

#endif
