//
//  SWAppDemo.cpp
//  SinoVCDemo
//
//  Created by zhangyujun on 16/1/3.
//  Copyright © 2016年 sinowave. All rights reserved.
//

#include "SWAppDemo.h"
#include "ISWApi.h"
#import "SWCommunicationVC.h"
#import "SWTalkBackVC.h"
#include "SWLoginViewController.h"
#import <AudioToolbox/AudioToolbox.h>

#import "AppDelegate.h"

#import "SWNavigationController.h"

#import "UIApplication+topmostViewController.h"
/*
static POSTHREAD g_tthread=0;
long __stdcall MyThreadProc(void* pUserData)
{
	while (1) {
		NSLog(@"-------------------------- mythread exec,%ld\n",time(0));
		Sleep(1000);
	}
	return 0;
}

*/


static SWAppDemo* theApp=0;
SWAppDemo& GetApp()
{
	if(theApp==0)
	{
		theApp=new SWAppDemo();
//		g_tthread=OSCreateThread(MyThreadProc, 0, 0);
	}
	return *theApp;
}
const char* ToCStr(NSString* str)
{
	return [str UTF8String];
}




SWAppDemo::SWAppDemo()
{
	callView=nil;
	LoginView=nil;
	MsgBox=nil;
    _pApi=CreateSWApi(this);
    _pUA=_pApi->CreateUA(this);
	_msgTimeout=3;

	//TRACE("---输出汉字\n");
}
SWAppDemo::~SWAppDemo()
{
	VPlayer->Release();
	_pUA->Release();
	_pApi->Release();
}

void SWAppDemo::SetLoginView(SWLoginViewController* pView)
{
    LoginView=pView;
}

//terminalList跳转到对讲类型页面时把对象保存为demo的变量,收到MVS消息时,用该对象调用
void SWAppDemo::SetTalkBackView(SWTalkBackVC* pView)
{
    callView=pView;
}

void SWAppDemo::destoryTalkBackView()
{
    callView = nil;
}

int SWAppDemo::Init(void* pViedoView)
{
	VPlayer=_pApi->CreateVideoPlayer(pViedoView);
	VCap=_pApi->GetVideoCapturer(0);
	VSender=_pApi->GetNetVideoSender();
	VReceiver=_pApi->GetNetVideoReceiver();

	APlayer=_pApi->GetAudioPlayer(0);
	ACap=_pApi->GetAudioCapturer(0);
	ASender=_pApi->GetNetAudioSender();
	AReceiver=_pApi->GetNetAudioReceiver();

	return  0;
}

int SWAppDemo::InitAudio()
{
    APlayer=_pApi->GetAudioPlayer(0);
    ACap=_pApi->GetAudioCapturer(0);
    ASender=_pApi->GetNetAudioSender();
    AReceiver=_pApi->GetNetAudioReceiver();
    return 0;
}

//extern LPCTSTR ESLoginRet(int);
void SWAppDemo::OnUALogin(ISWUA* pua,int ret)
{
	

    
    
    
    if(LoginView)
	{
		[LoginView OnLogin:ret];
	}
#warning 自动登录无提示版,勿删
//    else
//    {
//        //收到MVS的信令时,发一个通知
//        [[NSNotificationCenter defaultCenter]postNotificationName:SWOnLoginNotification object:@{@"ret":[NSNumber numberWithInt:ret]}];
//    }
    

};
void SWAppDemo::OnUAStateChange(ISWUA* pua,int state)
{
	_UAState=state;
};
void SWAppDemo::OnUACall(ISWUA* pua,ISWCall* pCall,int msg,long info)
{
    
    //如果当前界面为语音对讲,视频通话界面,不做处理
    UIViewController *viewController = [UIApplication topmostViewController];
    if ([viewController isMemberOfClass:[SWCommunicationVC class]]) return;
    
    
    //收到onCall消息,不存在就创建一个,退出callView界面时将SWAppDemo中的变量置为nil,如果存在退出时置为nil
    if (callView==nil) {
        callView = [[SWTalkBackVC alloc] init];
        callView.peerTerminalName =[NSString stringWithUTF8String:pCall->GetPeer()->Name()];
        
        
        if (viewController.navigationController) {
            [viewController.navigationController pushViewController:callView animated:YES];
        }
        else
        {
            [viewController presentViewController:[[SWNavigationController alloc] initWithRootViewController:callView] animated:YES completion:nil];
        }
        
	}
	int type=pCall->GetType();
	//this demo only process VideoTalkCall
	if (type!=CallType_VideoTalk) {
		return;
	}
	int dir=pCall->GetDirection();
	ITerminal* pTerm=pCall->GetPeer();

	//incomming call msg
	if (dir==CallDir_InComming)
	{
		switch (msg)
		{
		case CallMsg_Setup:   //收到呼叫
			{
				PlaySound(TONE_RING);
//show confirm UI,wait user accept or reject
#if 1
			//play alert tone
				ShowConfirmBox("Do you accept this call?");

//auto accept incomming call
#else
			//show term video
			OpenTerminalMedia(1, pTerm);
			//open term audio
			OpenTerminalMedia(2, pTerm);

			pCall->Accept();

#endif
			}
			break;
		case CallMsg_Cancel:   //对方取消呼叫
				StopPlaySound();
			ShowConfirmBox("Peer Cancel Call!");
			pCall->ReleaseCall();
			break;
		case CallMsg_Release:
			{
				StopPlaySound();
				ShowConfirmBox("Peer Stop Call!");  //对方挂断
				CloseTerminalMedia(1, pTerm);
				CloseTerminalMedia(2, pTerm);
				pCall->ReleaseCall();
			}
			break;

		default:
			break;
		}

	}
	else
	{
		switch (msg)
		{
			case CallMsg_Alert:
				//play ringback tone
				PlaySound(TONE_RINGBACK);
				break;
			case CallMsg_Accept:
			{
				StopPlaySound();
				//CancelInfoBox();
				ShowMsgBox("Peer Accept Call!");
				//close wait peer msg UI
				OpenTerminalMedia(1, pTerm);
				OpenTerminalMedia(2, pTerm);
			}
				break;
			case CallMsg_Reject:
			{
				StopPlaySound();
				ShowMsgBox("Peer Reject Call!");
				//close wait peer msg UI
				pCall->ReleaseCall();
			}
				break;
			case CallMsg_ReleaseComplete:
			{
				StopPlaySound();
				ShowMsgBox("Peer Stop Call!");
				CloseTerminalMedia(1, pTerm);
				CloseTerminalMedia(2, pTerm);
			}
				break;
		default:
			break;
		}
	}
};
void SWAppDemo::OnUARecvShortMessage(ISWUA* pua,ITerminal* pTerm,LPCTSTR szMessage,LPCTSTR szInfo)
{

};
void SWAppDemo::OnUARecvTerminalCommand(ISWUA* pua,ITerminal* pTerm,int cmd,int lParam,LPCTSTR szParam)
{
    
};
void SWAppDemo::OnUATerminalInfoUpdate(ISWUA* pua,ITerminal* pTerm,DWORD dwUpdateFlag)
{
    int  terminalCount =GetApp().GetUA()->GetTerminalCount();
//    for (int i=0; i<terminalCount; i++) {
//        //获取终端信息
//        ITerminal* terminal = GetApp().GetUA()->GetTerminal(i);
//        printf("terminal:%i %s\t%s\t%s\t%s\t%s\t%%s\t%s\t%s\t%s\t%s\t%s\t%s\t%u\t%u\t%u\t \n",i,terminal->Name(),terminal->Aliase(),terminal->FriendlyName(),terminal->Department(),terminal->Domain(),terminal->IPAddress(),terminal->PhoneNumber(),terminal->Chairman(),terminal->ConfInfo(),terminal->BandInfo(),terminal->UserPtr(),terminal->Type(),terminal->Level(),terminal->Grant(),terminal->GetNetId());
//    }
};
void SWAppDemo::OnUAMessage(ISWUA* pua,int msg,int iParam,long lParam)
{
#ifndef __NCC

	const char* szParam=(const char*)lParam;

	if (msg==UAMsg_NetOpenMedia)
	{
		if(iParam==1)//video
		{
			SWMEDIASOURCEID sid=VCap->AddSource(1);
			VSender->StratSend(sid, pua->GetSelf()->GetNetId(), szParam);
		}
		else if(iParam==2) //audio
		{
			SWMEDIASOURCEID sid=ACap->AddSource(1);
			ASender->StratSend(sid, pua->GetSelf()->GetNetId(), szParam);

		}
	}
	else if(msg==UAMsg_NetCloseMedia)
	{
		if (iParam==1)
		{
			SWMEDIASOURCEID sid=VCap->GetSource(1);
			VSender->StopSend(sid,pua->GetSelf()->GetNetId());
		}
		else if(iParam)
		{
			SWMEDIASOURCEID sid=ACap->GetSource(1);
			ASender->StopSend(sid,pua->GetSelf()->GetNetId());
		}
	}
	else if(msg==UAMsg_NetVideoNeedKeyFrame)
	{
		VSender->InsertKeyFrame(szParam);
	}
#endif
};

void SWAppDemo::OnSWMediaDeviceStateChange(int nDevType,int iDev,int state)
{

}

int SWAppDemo::StartTalk(ITerminal* pTerm)
{
	//create call
	ISWCall* pCall=_pUA->CreateCall(pTerm, CallType_VideoTalk);
	if (pCall) {
		//show wait peer answer UI
		//send call setup msg,wait peer call message( handle inf ISWUAEventHandler->OnUACallMsg(...)
		pCall->Setup();
		ShowMsgBox("Calling...");
////		_msgTimeout=3;
//        _msgTimeout=15;
	}
	return 0;
}

int SWAppDemo::StopTalk()
{
	StopPlaySound();
	ISWCall* pCall=_pUA->GetCall(0);
	//if (pCall->GetState()==CallState_Connect)
	{
		CloseTerminalMedia(3, 0);
	}
	pCall->ReleaseCall();
	return 0;
}

int SWAppDemo::OpenTerminalMedia(int type,ITerminal* pTerm)
{
#ifndef __NCC
	if((type&1)!=0)
	{
		SWMEDIASOURCEID sid=VReceiver->AddSource(pTerm->GetNetId());
		VPlayer->PlaySource(sid);
	}
	if((type&2)!=0)
	{
		SWMEDIASOURCEID sid=AReceiver->AddSource(pTerm->GetNetId());
		APlayer->PlaySource(sid);
	}
#endif
	return 0;
}

int SWAppDemo::CloseTerminalMedia(int type,ITerminal* pTerm)
{
#ifndef __NCC

	if(type&1)
	{
		VPlayer->PlaySource(0);
	}
	if(type&2)
	{
		APlayer->PlaySource(0);
	}
#endif
	return 0;
}

int SWAppDemo::ShowConfirmBox(const char* szMessage,const char* szBtn1Text,const char* szBtn2Text,const char* szTitle)
{
	
    if (callView!=nil) {
		[callView ShowConfirmBox:szMessage withBtn1Text:szBtn1Text andBtn2Text:szBtn2Text andTitle:szTitle];
	}
	return 0;
}
int SWAppDemo::ShowMsgBox(const char* szMessage,const char* szBtnText,const char* szTitle)
{
    if (callView!=nil) {
		[callView ShowMsgBox:szMessage withButtonText:szBtnText andTitle:szTitle];
	}
	return 0;
}
int SWAppDemo::CancelInfoBox()
{
    if (callView!=nil) {
		[callView CancelInfoBox];
	}
	return 0;
}

static SystemSoundID g_soundId=0;

void SoundCompletionProc(  SystemSoundID       ssID, void* __nullable    clientData)
{
	if(g_soundId) AudioServicesPlaySystemSound(g_soundId);

}

int SWAppDemo::PlaySound(int iSound)
{
	if(g_soundId)
	{
		StopPlaySound();
	}
	g_soundId=1000+iSound;
	TRACEINFO("PlaySound id=%d\n",g_soundId);
	AudioServicesAddSystemSoundCompletion(g_soundId, nullptr, NULL, SoundCompletionProc, 0);
	AudioServicesPlaySystemSound(g_soundId);

	return 0;
}
int SWAppDemo::StopPlaySound()
{
	if(g_soundId)
	{
		AudioServicesRemoveSystemSoundCompletion(g_soundId);
		AudioServicesDisposeSystemSoundID(g_soundId);
		g_soundId=0;
	}
	return 0;
}



