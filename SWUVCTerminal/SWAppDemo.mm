//
//  SWAppDemo.cpp
//  SinoVCDemo
//
//  Created by zhangyujun on 16/1/3.
//  Copyright © 2016年 sinowave. All rights reserved.
//

#include "SWAppDemo.h"
#include "ISWApi.h"
#include "ISWCall.h"
#import "SWCommunicationVC.h"
#import "SWTalkBackVC.h"
#include "SWLoginViewController.h"
#import <AudioToolbox/AudioToolbox.h>

#import "AppDelegate.h"

#import "SWNavigationController.h"
#import "SWTerminal.h"
#import "UIApplication+topmostViewController.h"
#import "SWUser.h"
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
    reconnectTipsView=nil;
	LoginView=nil;
	MsgBox=nil;
    terminalList =[NSMutableArray array];
    chairmanList =[NSMutableArray array];
    _pApi=CreateSWApi(this);
    _pUA=_pApi->CreateUA(this);
	_msgTimeout=3;
    
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
    if (_UAState==UA_ReConnect)
    {
        if (!reconnectTipsView) {
            reconnectTipsView = [[UILabel alloc] initWithFrame:CGRectMake((SWScreenWidth-240)/2, (SWScreenHeight-30)/2, 240, 30)];
            reconnectTipsView.text=@"请检查网络连接是否正确!";
            reconnectTipsView.backgroundColor = [UIColor blackColor];
            reconnectTipsView.alpha=0.3;
            reconnectTipsView.textColor=[UIColor whiteColor];
            reconnectTipsView.textAlignment=NSTextAlignmentCenter;
        }
        UIWindow *window = [[UIApplication sharedApplication].windows lastObject];
        [window addSubview:reconnectTipsView];
    }
    else
    {
        if (reconnectTipsView) {
            [reconnectTipsView removeFromSuperview];
        }
    }
};
void SWAppDemo::OnUACall(ISWUA* pua,ISWCall* pCall,int msg,long info)
{
    UIViewController *viewController = [UIApplication topmostViewController];
    //如果正在通讯界面,获取对方的call,直接拒绝
    if ([viewController isMemberOfClass:[SWCommunicationVC class]])
    {
    pCall->Reject(0);
    }
    else
    {
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
                //通讯界面时不作处理
                if (![viewController isMemberOfClass:[SWCommunicationVC class]])
                {
			//play alert tone
				ShowConfirmBox("Do you accept this call?");
                }
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
    
    [terminalList removeAllObjects];
    [chairmanList removeAllObjects];
    for (int i=0; i<terminalCount; i++) {
        //获取终端信息
        
        String strName;
        String strNameAliase;
        String strNameFriendly;
        String strPhoneNumber;
        String strDomain;
        String strDepartment;
        String strChairman;
        String strMessage;
        String strConfInfo;//会议信息
        String strBandInfo;
        String strBandOwner;
        String strUserData1;
        String strUserData2;
        
        
        VC3Terminal *v;
        VC3Terminal* terminal =(VC3Terminal*)GetApp().GetUA()->GetTerminal(i);
        SWTerminal *model =[[SWTerminal alloc] init];
        model.name = [NSString stringWithCString:terminal->Name() encoding:NSUTF8StringEncoding];
        model.aliase = [NSString stringWithCString:terminal->Aliase() encoding:NSUTF8StringEncoding];
        model.friendlyName = [NSString stringWithCString:terminal->FriendlyName() encoding:NSUTF8StringEncoding];
        model.department = [NSString stringWithCString:terminal->Department() encoding:NSUTF8StringEncoding];
        model.domain= [NSString stringWithCString:terminal->Domain() encoding:NSUTF8StringEncoding];
        model.ipAddress= [NSString stringWithCString:terminal->IPAddress() encoding:NSUTF8StringEncoding];
        model.phoneNumber= [NSString stringWithCString:terminal->PhoneNumber() encoding:NSUTF8StringEncoding];
        model.confInfo= [NSString stringWithCString:terminal->ConfInfo() encoding:NSUTF8StringEncoding];
        model.bandInfo= [NSString stringWithCString:terminal->BandInfo() encoding:NSUTF8StringEncoding];
//        model.userPtr= [NSString stringWithCString:terminal->Name() encoding:NSUTF8StringEncoding];
        model.type = [NSNumber numberWithUnsignedInt:terminal->Type()];
        model.level = [NSNumber numberWithUnsignedInt:terminal->Level()];
        model.grant= [NSNumber numberWithUnsignedInt:terminal->Grant()];
        model.netId= [NSNumber numberWithUnsignedInt:terminal->GetNetId()];
        //判断chairman是否为空
        //1.不为空认为是会议主席,添加到会议主席列表中
        //2.为空时把type=2(指挥/会议终端)和type=3(监控前端)的,加入到对讲列表中,过滤当前正在登录的用户
        
        if (terminal->Chairman()) {
            if (model.name.length) [chairmanList addObject:model]; //监听数组的变化去刷新列表
        }
        else
        {
            //获取当前登录的用户
            SWUser *user =[SWUser user];
            
            if ([model.type integerValue]==2 || [model.type integerValue]==3) {
                if (![model.name isEqualToString:user.userName] && model.name.length) {
                    [terminalList addObject:model]; 
                }
                
            }
        }
    }
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
	if (pCall->GetState()==CallState_Connect)
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



