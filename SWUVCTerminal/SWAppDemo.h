//
//  SWAppDemo.hpp
//  SinoVCDemo
//
//  Created by zhangyujun on 16/1/3.
//  Copyright © 2016年 sinowave. All rights reserved.
//

#ifndef SWAppDemo_hpp
#define SWAppDemo_hpp

#define TRACEINFO
#include <CoreFoundation/CoreFoundation.h>
#include <pthread/pthread.h>
#include <stdio.h>
#include <string.h>

#include "ISWApi.h"
@class SWLoginViewController;
@class SWTalkBackVC;
//no compile code
//#define __NCC

//typedef const char* LPCTSTR;
//typedef unsigned int DWORD;
using namespace SWApi;

#define TONE_RING	21
#define TONE_RINGBACK	27

@protocol SWAppDelegate
-(void)OnUALogin:(int)ret;
-(void)OnUAStateChange:(int)state;
-(void)OnUACall:(ISWCall*)pCall Msg:(int)msg Info:(long)lInfo;
-(void)OnUAReceiveShortMessage:(ITerminal*)pTerm Message:(LPCTSTR)szMessage Info:(LPCTSTR)szInfo;
-(void)OnUAReceiveTerminalCommand:(ITerminal*)pTerm cmdCode:(int)cmd intParam:(int)lParam strParam:(LPCTSTR)szParam;
-(void)OnUATerminalInfoUpdated:(ITerminal*)pTerm Flags:(DWORD)dwUpdateFlag;
-(void)OnUAMessage:(int)msg intParam:(int)lParam strParam:(LPCTSTR)szParam;
-(void)OnSWMediaDeviceStateChange:(int)nDevType Index:(int)iDev State:(int)state;
@end

@class UIViewController;
@class ViewController;
@class SWTalkBackVC;
@class UIAlertController;
struct SWAppDemo:public ISWUAEventHandler,public ISWApiEventHandler
{
	ISWApi* _pApi;
	ISWUA* _pUA;
    IPTZControl *_ptzControl;
	ISWVideoPlayer* VPlayer;
	ISWVideoCapturer* VCap;
	ISWNetVideoReceiver* VReceiver;
	ISWNetVideoSender* VSender;

	ISWAudioPlayer* APlayer;
	ISWAudioCapturer* ACap;
	ISWNetAudioReceiver* AReceiver;
	ISWNetAudioSender* ASender;


    SWLoginViewController* LoginView;
    SWTalkBackVC* callView;
    UILabel *reconnectTipsView;
	UIAlertController* MsgBox;
    NSMutableArray *terminalList;
    NSMutableArray *chairmanList;
	int _msgTimeout;
	int _UAState;

public:
	SWAppDemo();
	~SWAppDemo();

    void SetLoginView(SWLoginViewController* pView);
    void SetTalkBackView(SWTalkBackVC* pView);
    void destoryTalkBackView();
 	ISWApi* GetApi(){return _pApi;}
	ISWUA* GetUA(){return _pUA;};
    IPTZControl *GetPtzControl(){return _ptzControl;};
	int Init(void* pViedoView);
    
    int InitAudio();

public:
	virtual void OnUALogin(ISWUA* pua,int ret);
	virtual void OnUAStateChange(ISWUA* pua,int state);
	virtual void OnUACall(ISWUA* pua,ISWCall* pCall,int msg,long info);
	virtual void OnUARecvShortMessage(ISWUA* pua,ITerminal* pTerm,LPCTSTR szMessage,LPCTSTR szInfo);
	virtual void OnUARecvTerminalCommand(ISWUA* pua,ITerminal* pTerm,int cmd,int lParam,LPCTSTR szParam);
	virtual void OnUATerminalInfoUpdate(ISWUA* pua,ITerminal* pTerm,DWORD dwUpdateFlag);
	virtual void OnUAMessage(ISWUA* pua,int msg,int iParam,long lParam);
    virtual void OnSWMediaDeviceStateChange(int nDevType,int iDev,int state);
public:
	int StartTalk(ITerminal* pTerm);
	int StopTalk();
	int OpenTerminalMedia(int type,ITerminal* pTerm);
	int CloseTerminalMedia(int type,ITerminal* pTerm);

	int ShowConfirmBox(const char* szMessage,const char* szBtn1Text="Cancel",const char* szBtn2Text="Ok",const char* szTitle="Info");
	int ShowMsgBox(const char* szMessage,const char* szBtnText="Cancel",const char* szTitle="Info");
	int CancelInfoBox();
	int PlaySound(int iSound);
	int StopPlaySound();
	int GetMsgTimeout(){return _msgTimeout;}

   
};

SWAppDemo& GetApp();

const char* ToCStr(NSString* str);


#endif /* SWAppDemo_hpp */
