#ifndef __OSAPI__H__
#define __OSAPI__H__
#include "BasePub.h"
#ifdef __ENV_IOS
#define Sleep(t) usleep(t*1000)
#define __stdcall 
#endif


typedef struct __OSTIMER* POSTIMER;
typedef void (*LPOSTimerProc)(void* pUserData);
POSTIMER OSCreateTimer(LPOSTimerProc pTimeoutProc,void* pUserData);
bool OSStartTimer(POSTIMER pTimer,int timeout);
void OSStopTimer(POSTIMER pTimer);
void OSReleaseTimer(POSTIMER pTimer);

long OSGetSysTick();

typedef struct __OSTHREAD* POSTHREAD;
typedef long (__stdcall *LPOSThreadProc)(void* pUserData);
POSTHREAD OSCreateThread(LPOSThreadProc pThreadProc,void* pUserData,long* pThreadId);
int OSResumeThread(POSTHREAD pThread);
int OSSuspendThread(POSTHREAD pThread);
int OSTerminateThread(POSTHREAD pThread);
void OSExitThread(DWORD dwExcode);
void OSReleaseThread(POSTHREAD pThread);
long OSGetCurrentThreadId();
//extern int Sleep(int ms);
int OSGetLastError();
#ifndef __ENV_WIN
void OutputDebugString(const char* str);
#endif
int StartBackgourndNetwork(int fd);
void StopBackgroundNetwork();
#endif //#ifndef