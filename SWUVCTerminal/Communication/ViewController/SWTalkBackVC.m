//
//  SWTalkBackVC.m
//  SWUVCTerminal
//
//  Created by hxf on 8/12/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWTalkBackVC.h"
#import "SWCallingView.h"
#import "SWCommunicationVC.h"
#import "SWUser.h"
#import "SWSetting.h"
@interface SWTalkBackVC ()<UINavigationBarDelegate>
{
    ISWApi* SWApi;
    ISWUA* SWUA;
    NSTimer* MsgBoxTimer;
    int InfoBoxType;  //0.call 1.onCall
}
#pragma mark Property
@property(nonatomic,strong) UIButton *backButton;  //返回按钮
@property(nonatomic,strong) UIImageView *avatarImageView; //头像
@property(nonatomic,strong) UILabel *userInfoLabel;  //用户信息
@property(nonatomic,strong) UIButton *startVideoButton; //视频聊天
@property(nonatomic,strong) UIButton *startAudioButton; //语音对讲

@end

@implementation SWTalkBackVC

#pragma mark -life cycle


- (void)viewDidLoad {
    [super viewDidLoad];
    
    GetApp().InitAudio();
    SWApi=GetApp().GetApi();
    SWUA=GetApp().GetUA();
    //保存实例到GetApp,用来调用call && oncall的消息
    GetApp().SetTalkBackView(self);
    
    
    self.view.backgroundColor = [UIColor whiteColor];
    self.title=@"对讲";
    [self setUpNavTheme];
    
    
    [self.view addSubview:self.avatarImageView];
    [self.view addSubview:self.userInfoLabel];
    [self.view addSubview:self.startVideoButton];
    [self.view addSubview:self.startAudioButton];
    
    if ([self respondsToSelector:@selector(edgesForExtendedLayout)]) {
        self.edgesForExtendedLayout = UIRectEdgeNone;
    }
}


-(void)viewDidLayoutSubviews
{
    CGFloat horizontalPadding =(SWScreenWidth-self.startVideoButton.width)/2;
    CGFloat bottomPadding = 50;
    
    self.avatarImageView.position=CGPointMake((SWScreenWidth-self.avatarImageView.width)/2, 50);
    self.userInfoLabel.position = CGPointMake((SWScreenWidth-self.userInfoLabel.width)/2, CGRectGetMaxY(self.avatarImageView.frame)+10);

    self.startAudioButton.position=CGPointMake(horizontalPadding, SWScreenHeight-64-self.startAudioButton.height-bottomPadding);
    
    self.startVideoButton.position = CGPointMake(horizontalPadding, CGRectGetMinY(self.startAudioButton.frame)-30-self.startVideoButton.height);
}

-(void)setUpNavTheme
{
    
    UIButton *backButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [backButton setImage:[UIImage imageNamed:@"back"] forState:UIControlStateNormal];
    [backButton sizeToFit];
    backButton.contentEdgeInsets = UIEdgeInsetsMake(0, -20, 0, 0);
    
    [[backButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
        
        if ([self.navigationController respondsToSelector:@selector(popViewControllerAnimated:)]) {
            [self.navigationController popViewControllerAnimated:YES];
        }
       
            [self dismissViewControllerAnimated:YES completion:nil];
        
            GetApp().destoryTalkBackView();
    }];
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:backButton];
}


#pragma mark -system object Delegate

#pragma mark -public Methods (InfoBoxType=1为收到呼叫,InfoBoxType=0为呼叫,取消呼叫)
-(int)ShowConfirmBox:(const char*)szMessage withBtn1Text:(const char*)szBtn1Text andBtn2Text:(const char*)szBtn2Text andTitle:(const char*)szTitle
{
    InfoBoxType=1; //收到呼叫

//    if ([[SWSetting setting].isAutoAccept boolValue])
//    {
//        SWCommunicationVC *communicationVC =[[SWCommunicationVC alloc] init];
//        communicationVC.mainViewType=SWVideoMainViewType;
//        communicationVC.videoType=SWChatVideoType;
//        
//        communicationVC.handUpSignal=[RACSubject subject];
//        [communicationVC.handUpSignal subscribeNext:^(id x) {
//            if ([x isEqualToString:SWVideoHandUp])
//            {
//                //挂断视频通话
//                [self OnBtnStopTalk];
//            }
//            else if ([x isEqualToString:SWVoiceHandUp])
//            {
//                //挂断语音对讲
//                [self OnBtnStopAudio];
//            }
//        }];
//        [self presentViewController:communicationVC animated:NO completion:nil];
//        [self OnEndInfoBox:0]; //接受对方呼叫
//    }
//    else
//    {
        NSString *onMessageName =[NSString stringWithCString:szMessage encoding:NSUTF8StringEncoding];
        
        
        //oncall setup
        if ([onMessageName isEqualToString:@"Do you accept this call?"])
        {
            //弹出自定义的OnCalling
            SWCallingView *callingView =[SWCallingView shareManager];
            callingView.peerTerminalName=self.peerTerminalName;
            callingView.callDirectType=SWCallIncomingDirectType;
            [callingView show];
            callingView.callBackSignal = [RACSubject subject];
            [callingView.callBackSignal subscribeNext:^(id x) {
                
                if ([x isEqualToString:SWAccept])
                {
                    [callingView hidden];
                    
                    //根据呼进来的类型,初始化显示界面
                    SWCommunicationVC *communicationVC =[[SWCommunicationVC alloc] init];
                    communicationVC.mainViewType=SWVideoMainViewType;
                    communicationVC.videoType=SWChatVideoType;
                    
                    communicationVC.handUpSignal=[RACSubject subject];
                    [communicationVC.handUpSignal subscribeNext:^(id x) {
                        
                        if ([x isEqualToString:SWVideoHandUp]) {
                            //挂断视频通话
                            [self OnBtnStopTalk];
                        }
                        else if ([x isEqualToString:SWVoiceHandUp])
                        {
                            //挂断语音对讲
                            [self OnBtnStopAudio];
                        }
                    }];
                    
                    [self presentViewController:communicationVC animated:NO completion:nil];
                    
                    [self OnEndInfoBox:0]; //接受对方呼叫
                }
                else if ([x isEqualToString:SWHandUp])
                {
                    [callingView hidden];
                    
                    [self OnEndInfoBox:1];   //拒绝对方呼叫
                }
            }];
        }
        else if([onMessageName isEqualToString:@"Peer Cancel Call!"])
        {
            //对方取消呼叫
            [[SWCallingView shareManager] hidden];
        }
        else if([onMessageName isEqualToString:@"Peer Stop Call!"])
        {
            
#warning 由于目前SDK存在问题,对方取消呼叫时调用的是挂断的逻辑,以下代码在SDK修正后删除
            [[SWCallingView shareManager] hidden];
            
            //对方挂断时,音视频界面通话界面消失,收到通知时dismiss
            [[NSNotificationCenter defaultCenter] postNotificationName:SWPeerStopCallNotification object:nil];
        }
//    }
    return 0;
}

//calling,peerAcceptCall(outComing) callBack
-(int)ShowMsgBox:(const char*)szMessage withButtonText:(const char*)szBtnText andTitle:(const char*)szTitle
{
    InfoBoxType=0;   //呼叫对方
    //收到的消息有1.Peer Cancel Call! 2.Peer Stop Call! 3.Peer Accept Call! 4.Peer Reject Call! 5.Calling...
    SWCallingView *callingView =[SWCallingView shareManager];
    
    NSString *onMessageName =[NSString stringWithCString:szMessage encoding:NSUTF8StringEncoding];
    if ([onMessageName isEqualToString:@"Calling..."])
    {
        //显示呼叫界面
        callingView.callDirectType = SWCallOutcomingDirectType;
        callingView.callBackSignal = [RACSubject subject];
        callingView.peerTerminalName=self.peerTerminalName;
        //点击了取消呼叫
        [callingView.callBackSignal subscribeNext:^(id x) {
            if ([x isEqualToString:SWCancled])
            {
                [self OnEndInfoBox:0];    //取消呼叫
                [callingView hidden];
            }
        }];
        
        [callingView show];
        
        MsgBoxTimer=[NSTimer scheduledTimerWithTimeInterval:50 target:self selector:@selector(handleTimeout:) userInfo:nil repeats:NO];
    }
    else if ([onMessageName isEqualToString:@"Peer Accept Call!"]) //对方接收呼叫
    {
        [callingView hidden];
        
        //根据呼进来的类型,初始化显示界面
        SWCommunicationVC *communicationVC =[[SWCommunicationVC alloc] init];
        communicationVC.mainViewType=SWVideoMainViewType;
        communicationVC.videoType=SWChatVideoType;
        
        communicationVC.handUpSignal=[RACSubject subject];
        [communicationVC.handUpSignal subscribeNext:^(id x) {
            
            if ([x isEqualToString:SWVideoHandUp]) {
                //挂断视频通话
                [self OnBtnStopTalk];
            }
            else if ([x isEqualToString:SWVoiceHandUp])
            {
            //挂断语音对讲
                [self OnBtnStopAudio];
            }
        }];

        
        [self presentViewController:communicationVC animated:NO completion:nil];
    }
    else if ([onMessageName isEqualToString:@"Peer Stop Call!"])
    {
        //对方挂断时,音视频界面通话界面消失,收到通知时dismiss
         [[NSNotificationCenter defaultCenter] postNotificationName:SWPeerStopCallNotification object:nil];
    }
    else
    {
     [callingView hidden];
    }
    return 0;
}

-(int)CancelInfoBox
{
    return 0;
}

-(void)OnEndInfoBox:(int)result
{
    GetApp().StopPlaySound();
    if (MsgBoxTimer!=nil) {
        [MsgBoxTimer invalidate];
        MsgBoxTimer=nil;
    }
    
    if (InfoBoxType==1)//oncall 收到呼叫(接听/拒绝)
    {
        ISWCall* pCall=SWUA->GetCall(0);
        if (result==0) //accept
        {
            GetApp().OpenTerminalMedia(3, pCall->GetPeer());
            pCall->Accept();
        }
        else if(result ==1) //reject
        {
            pCall->Reject(0);
        }
    }
    else if (InfoBoxType==0)  //call  主动呼叫(取消呼叫)
    {
        if (result==0) //取消呼叫
        {
            ISWCall* pCall=SWUA->GetCall(0);
            pCall->Cancel();
        }
        else
        {
        
        }
    }
}


#pragma mark -event respond

-(void)handleTimeout:(NSTimer*)theTimer
{
    if (MsgBoxTimer) {
        [MsgBoxTimer invalidate];
        MsgBoxTimer=nil;
    }
    SWCallingView *callingView =[SWCallingView shareManager];
    [callingView hidden];
    [self OnEndInfoBox:0];
}

- (void)onBtnStartTalk
{
    const char* szTerm=ToCStr(_peerTerminalName);
    ITerminal* pTerm=SWUA->AddTerminal(szTerm);//or ITerminal* pTerm=SWUA->GetTerminalByName(szTerm);
#if 1
    GetApp().StartTalk(pTerm);
    //test create media stream on call accept
#else
    //open video from term to self
    GetApp().OpenTerminalMedia(2,pTerm);
    //open audio
    //	GetApp().OpenTerminalMedia(2, pTerm);
    
#endif
    
}

- (void)OnBtnStopTalk
{
#if 1
    GetApp().StopTalk();
#else
    GetApp().CloseTerminalMedia(3, 0);
#endif
}

- (void)OnBtnStartAudio {
    const char* szTerm=ToCStr(_peerTerminalName);
    ITerminal* pTerm=SWUA->GetTerminalByName(szTerm);
    GetApp().OpenTerminalMedia(2, pTerm);
}
- (void)OnBtnStopAudio {
    GetApp().CloseTerminalMedia(2, 0);
    
}


#pragma mark -private methods
//dry独立出来,代码复用


#pragma mark -getter and setter methods
//所有的属性都使用getter进行初始化


-(UIImageView *)avatarImageView
{
    if (!_avatarImageView) {
        UIImage *avatar = [UIImage imageWithContentsOfFile:[SWDocumentPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@_avatar.png",[SWUser user].userName]]];
        _avatarImageView = [[UIImageView alloc] initWithImage:avatar?avatar:[UIImage imageNamed:@"avatar"]];
        _avatarImageView.frame  =CGRectMake(0, 0, 100, 100);
        _avatarImageView.layer.cornerRadius=50;
        _avatarImageView.clipsToBounds=YES;
    }
    return _avatarImageView;
}

-(UILabel *)userInfoLabel
{
    if (!_userInfoLabel) {
        _userInfoLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 200, 60)];
        _userInfoLabel.font=[UIFont systemFontOfSize:15];
        _userInfoLabel.text=_peerTerminalName;
        _userInfoLabel.textColor = [UIColor grayColor];
        _userInfoLabel.numberOfLines=2;
        _userInfoLabel.textAlignment=NSTextAlignmentCenter;
        
    }
    return _userInfoLabel;
}

-(UIButton *)startVideoButton
{
    if (!_startVideoButton) {
        _startVideoButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 280, 44)];
        [_startVideoButton setImage:[UIImage imageNamed:@"talk"] forState:UIControlStateNormal];
        
        [[_startVideoButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            [self onBtnStartTalk];
            
            
#warning 测试代码
//            SWCommunicationVC *communicationVC =[[SWCommunicationVC alloc] init];
//            communicationVC.mainViewType=SWVideoMainViewType;
//            communicationVC.videoType=SWChatVideoType;
//            
//            [self presentViewController:communicationVC animated:NO completion:nil];
            
        }];
    }
    return _startVideoButton;
}

-(UIButton *)startAudioButton
{
    if (!_startAudioButton) {
        _startAudioButton = [[UIButton alloc]initWithFrame:CGRectMake(0, 0, 280, 44)];
        [_startAudioButton setImage:[UIImage imageNamed:@"speak"] forState:UIControlStateNormal];
        
        [[_startAudioButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
//           [self OnBtnStartAudio];
            [MBProgressHUD showError:@"语音对讲暂不可用"];
        }];
    }
    return _startAudioButton;
}


@end
