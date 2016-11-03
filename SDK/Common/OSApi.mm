//
//  OSApi.m
//  SinoVCDemo
//
//  Created by zhangyujun on 15/12/30.
//  Copyright © 2015年 sinowave. All rights reserved.
//
#include "__PCH.h"
#include "OSApi.h"
#include <Foundation/Foundation.h>
//#include <CF>


#if 0

@interface MyTimer : NSObject
{
	ITimerObserver* _pObserver;
	long _userParam;
	NSTimer* _Timer;
}
-(void)start:(ITimerObserver*)pObserver withParam:(long)userParam timeout:(int)timeout;
-(void)timeout;
-(void)stop;

@end

@implementation MyTimer

-(void)start:(ITimerObserver *)pObserver withParam:(long)userParam timeout:(int)timeout
{
	_pObserver=pObserver;
	_userParam=userParam;
	_Timer = [NSTimer timerWithTimeInterval:timeout
									 target:self
								   selector:@selector(timeout)
								   userInfo:nil
									repeats:YES];
	
	CFRunLoopAddTimer(CFRunLoopGetCurrent(), (__bridge CFRunLoopTimerRef)_Timer, (__bridge CFStringRef)NSDefaultRunLoopMode);
}
-(void)timeout
{
	_pObserver->OnTimerTimeout((__bridge HTIMER)self, _userParam);
}

-(void)stop
{
	_Timer=nil;
}

-(ITimerObserver*)observer
{
	return _pObserver;
}

-(long)userParam
{
	return _userParam;
}

@end



HTIMER CreateTimer(int timeout,ITimerObserver* pObserver,long name)
{
	HTIMER hTimer=0;
	MyTimer* Timer=[[MyTimer alloc] init];
	[Timer start:pObserver withParam:name timeout:timeout];
	hTimer=(__bridge HTIMER)Timer;
	return hTimer;
}

int StartTimer(HTIMER hTimer,int timeout,long name,ITimerObserver* pObserver)
{
	MyTimer* Timer=(__bridge MyTimer*)hTimer;
	if(hTimer)
	{
		if(pObserver==NULL) pObserver=[Timer observer];
		if(name==0) name=[Timer userParam];
		[Timer stop];
		[Timer start:pObserver withParam:name timeout:timeout];
	}
	return  0;
}

void StopTimer(HTIMER hTimer)
{
	MyTimer* Timer=(__bridge MyTimer*)hTimer;
	if(hTimer)
	{
		[Timer stop];
	}
}


void ReleaseTimer(HTIMER hTimer)
{
	MyTimer* Timer=(__bridge MyTimer*)hTimer;
	if(hTimer)
	{
		[Timer stop];
		Timer=nil;
	}
}

#endif //#if 0

void OutputDebugString(const char* str)
{
//	printf(str);
	unsigned long encode=CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingDOSChineseSimplif);
	NSString* pstr=[[NSString alloc] initWithCString:str encoding:encode];
	str=[pstr UTF8String];
//	NSLog(@"%@\n",pstr);
	printf(str);
	pstr=nullptr;

//	printf(str);
}

static void ReadStreamClientCallBack(CFReadStreamRef stream, CFStreamEventType type, void *clientCallBackInfo)
{
	NSLog(@"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!reastream event\n");
}

static CFReadStreamRef _AliveRead=0;
static CFWriteStreamRef _AliveWrite=0;
int StartBackgourndNetwork(int fd)
{
	static CFSocketContext m_ctx={0};
	CFSocketNativeHandle sock=(CFSocketNativeHandle)fd;
	CFStreamCreatePairWithSocket(0, sock, &_AliveRead, &_AliveWrite);
	if(_AliveWrite==0 || _AliveRead==0)
	{
		_AliveRead=0;_AliveWrite=0;
		return 1;
	}

	bool ret=0;

	CFReadStreamSetClient(_AliveRead, (CFOptionFlags)0x1f, ReadStreamClientCallBack,(CFStreamClientContext*)&m_ctx);//(__bridge void*)self);


	ret=CFReadStreamSetProperty(_AliveRead, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanFalse);
	ret=CFWriteStreamSetProperty(_AliveWrite, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanFalse);
	ret=CFReadStreamSetProperty(_AliveRead, kCFStreamNetworkServiceType, kCFStreamNetworkServiceTypeVoIP);
	ret=CFWriteStreamSetProperty(_AliveWrite, kCFStreamNetworkServiceType, kCFStreamNetworkServiceTypeVoIP);
	CFReadStreamScheduleWithRunLoop(_AliveRead, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	CFWriteStreamScheduleWithRunLoop(_AliveWrite, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

	CFReadStreamOpen(_AliveRead);
	CFWriteStreamOpen(_AliveWrite);
//	[(__bridge NSInputStream*)_AliveRead setProperty:NSStreamNetworkServiceTypeVoIP forKey:NSStreamNetworkServiceType];
//	[(__bridge NSOutputStream*)_AliveWrite setProperty:NSStreamNetworkServiceTypeVoIP forKey:NSStreamNetworkServiceType];
//	StopBackgroundNetwork();

	return 0;
}
void StopBackgroundNetwork()
{
	if (_AliveRead==0) {
		return;
	}
	CFReadStreamSetClient(_AliveRead, kCFStreamEventNone, NULL, NULL);
	CFReadStreamClose(_AliveRead);
	CFRelease(_AliveRead);
	_AliveRead = NULL;
	CFWriteStreamSetClient(_AliveWrite, kCFStreamEventNone, NULL, NULL);
	CFWriteStreamClose(_AliveWrite);
	CFRelease(_AliveWrite);
	_AliveWrite = NULL;

}


