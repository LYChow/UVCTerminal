//
//  SWCommunicationVC.m
//  SWUVCTerminal
//
//  Created by hxf on 8/23/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWCommunicationVC.h"
#include "SWAppDemo.h"

#import "SWSettingWheel.h"
#import "SWSettingPopView.h"
#import "SWPTZControlView.h"
#import "SWFileManager.h"

#define SWDelayInterval 10
@interface SWCommunicationVC ()<SWSettingWheelDelegate,SWPTZControlViewDelegate>
{
    UIInterfaceOrientation _interfaceOrientation; //记录方向的改变
    NSTimer *delayTimer;
}
#pragma mark -mainView及其子视图

@property(nonatomic,strong)UIView *mainView;        //视频(监控+视频聊天) / 通话的界面

//视频(监控+视频聊天)子视图
@property(nonatomic,strong) UIView *peerVideoView;     //对方视频View
@property(nonatomic,strong) UIView *mineVideoView;  //采集自己视频的View
//通话的界面 子视图
@property(nonatomic,strong) UIImageView *avatarImageView; //头像
@property(nonatomic,strong) UILabel *userInfoLabel;  //用户信息

#pragma mark -监控(云台控制)及其子视图

@property(nonatomic,strong)UIView *monitorView;     //查看监控时的View
@property(nonatomic,strong)UIButton *ptzControlButton; //云台控制button
@property(nonatomic,strong)UIButton *stopWatchButton;  //停止浏览

@property(nonatomic,strong)SWPTZControlView *ptzControllView; //查看监控--->点击云台控制弹出的view


#pragma mark -视频聊天及其子视图

@property(nonatomic,strong)UIView *videoChatView;          //视频聊天时的View
@property(nonatomic,strong)UIButton *videoMuteButton;      //视频聊天静音按钮
@property(nonatomic,strong)UIButton *videoHandUpButton;    //视频聊天挂断按钮

#pragma mark -语音对讲及其子视图
@property(nonatomic,strong)UIView *voiceCallView;          //通话对讲时的View
@property(nonatomic,strong)UIButton *voiceMuteButton;      //语音对讲静音按钮
@property(nonatomic,strong)UIButton *voiceSpeakerButton; //语音对讲免提按钮
@property(nonatomic,strong)UIButton *voiceHandUpButton;    //语音对讲挂断按钮

#pragma mark -转盘设置以及弹出视图
@property(nonatomic,strong) SWSettingWheel *settingWheel; //设置转盘
@property(nonatomic,strong) SWSettingPopView *settingPopView; //设置转盘弹出的View



@end

@implementation SWCommunicationVC

#pragma mark -life cycle


- (void)viewDidLoad {
    [super viewDidLoad];
    
    GetApp().Init((__bridge void*)self.peerVideoView);
    
    //根视图上面的第一层
    [self.view addSubview:self.mainView];
    
//     [[NSNotificationCenter defaultCenter] postNotificationName:@"InterfaceOrientation" object:@"YES"];
    
    //对方挂断电话
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(stopCommunication:) name:SWPeerStopCallNotification object:nil];
}

-(void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

-(void)viewDidLayoutSubviews
{
    [super viewDidLayoutSubviews];
    
    
    self.peerVideoView.frame=[UIScreen mainScreen].bounds;
    self.mainView.frame=[UIScreen mainScreen].bounds;
    self.mineVideoView.position=CGPointMake(SWScreenWidth-self.mineVideoView.width, SWScreenHeight-self.mineVideoView.height);
    

        //语音对讲
        if (_mainViewType==SWAudioMainViewType)
        {
            self.voiceCallView.frame=[UIScreen mainScreen].bounds;
            self.avatarImageView.position=CGPointMake((SWScreenWidth-self.avatarImageView.width)/2, 50);
            self.userInfoLabel.position = CGPointMake((SWScreenWidth-self.userInfoLabel.width)/2, CGRectGetMaxY(self.avatarImageView.frame)+10);
            
            self.voiceHandUpButton.position = CGPointMake((SWScreenWidth-self.voiceHandUpButton.width)/2, SWScreenHeight-40-self.voiceHandUpButton.height);
            
            self.voiceMuteButton.position = CGPointMake(SWScreenWidth/2-20-self.voiceMuteButton.width, self.voiceHandUpButton.y-40-self.voiceMuteButton.height);
            self.voiceSpeakerButton.position= CGPointMake(SWScreenWidth/2+20, self.voiceMuteButton.y);
            
            self.settingWheel.center=CGPointMake(SWScreenWidth, SWScreenHeight/2);
        }
        else if (_mainViewType==SWVideoMainViewType)
        {
            //查看监控
            if (_videoType==SWMonitorVideoType)
            {
                self.monitorView.frame=[UIScreen mainScreen].bounds;
                self.ptzControlButton.position=CGPointMake(SWScreenWidth/2-20-self.ptzControlButton.width, SWScreenHeight-self.stopWatchButton.height-60);
                self.stopWatchButton.position= CGPointMake(SWScreenWidth/2+20, self.ptzControlButton.y);
                self.settingWheel.center=CGPointMake(SWScreenWidth, SWScreenHeight/2);
                
            }//视频聊天
            else if (_videoType==SWChatVideoType)
            {
                self.videoChatView.frame=[UIScreen mainScreen].bounds;
                self.videoMuteButton.position=CGPointMake((SWScreenWidth-2*self.videoMuteButton.width)/3, SWScreenHeight-60);
                self.videoHandUpButton.position= CGPointMake(SWScreenWidth-((SWScreenWidth-2*self.videoMuteButton.width)/3)-self.videoHandUpButton.width, self.videoMuteButton.y);
                self.settingWheel.center=CGPointMake(SWScreenWidth, SWScreenHeight/2);
            }
        }
}

-(void)dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self name:SWPeerStopCallNotification object:nil];
}

#pragma mark -system object Delegate



#pragma mark -横竖屏旋转
//支持设备自动旋转
-(BOOL)shouldAutorotate
{
    return YES;
}

-(UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskAll;
}


//强制旋转屏幕
- (void)orientationToPortrait:(UIInterfaceOrientation)orientation {
    /**
     从A进入B的时候,把B强制转换成横屏,返回的时候,需要在A出现的时候再转换为原来的方向,不然会有问题
     个人建议可以在B的viewWillAppear调用这个方法,转换屏幕(例如转换为横屏),然后在A的viewWillAppear中转换回来;
     */
    
    SEL selector = NSSelectorFromString(@"setOrientation:");
    NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:[UIDevice instanceMethodSignatureForSelector:selector]];
    [invocation setSelector:selector];
    [invocation setTarget:[UIDevice currentDevice]];
    int val = orientation;
    [invocation setArgument:&val atIndex:2];//前两个参数已被target和selector占用
    [invocation invoke];
    
}


#pragma mark -custom Delegate
#pragma mark -SWPTZControlViewDelegate
-(void)callBackPtzControllEventType:(SWPTZControlType)eventType
{
    //0=stop,1=left,2=up,3=right,4=down
    switch (eventType) {
        case 1:
            GetApp().GetPtzControl()->Move(2);
            break;
        case 2:
               GetApp().GetPtzControl()->Move(1);
            break;
        case 3:
            GetApp().GetPtzControl()->Move(4);
            break;
        case 4:
            GetApp().GetPtzControl()->Move(3);
            break;
        case 5:
            GetApp().GetPtzControl()->Move3D(0, 0, 1);//ZoomIn
            break;
        case 6:
            GetApp().GetPtzControl()->Move3D(0, 0, 0);//ZoomOut
            break;
        default:
            break;
    }
 
}


#pragma mark -SWSettingWheelDelegate
-(void)callBackClilkItemTitle:(NSString *)title
{
    
    if ([title isEqualToString:SWRecordVideo]) {
        
    }
    else if ([title isEqualToString:SWHiddenMineVideo])
    {
        self.mineVideoView.hidden=!self.mineVideoView.hidden;
    }
    else if ([title isEqualToString:SWCaptureImage])
    {
        
        [SWFileManager saveSnapShots:self.view];
        
    }
    else if ([title isEqualToString:SWPopSetting])
    {
        SWSettingPopView *settingPopView = [[SWSettingPopView alloc] initWithFrame:self.view.bounds andSettingPopViewHeight:self.settingWheel.height withPopViewOriginY:self.settingWheel.y];
        [self.view addSubview:settingPopView];
    }
    else if ([title isEqualToString:SWSwitchFrontCapture])
    {
        
    }
    else if ([title isEqualToString:SWRecordAudio])
    {
        
    }
}

-(void)touchingSettingWheelView
{
    SWLogFunc(@"touching");
    videoChatIndex = 0;
    monitorIndex = 0;
    voiceCallIndex = 0;
}


#pragma mark -event respond
//触摸语音对讲界面
static int voiceCallIndex =  0 ;

-(void)touchForVoiceCallView:(UITapGestureRecognizer *)recognizer
{
    if (self.settingWheel.hidden) {
        self.settingWheel.hidden=NO;
        self.voiceMuteButton.hidden=NO;
        self.voiceHandUpButton.hidden=NO;
        self.voiceSpeakerButton.hidden=NO;
        
        
        [delayTimer invalidate];
        voiceCallIndex = 0;
        [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(autoHiddenVoiceCallView:) userInfo:nil repeats:YES];
        
    }
    else
    {
        self.settingWheel.hidden=YES;
        self.voiceMuteButton.hidden=YES;
        self.voiceHandUpButton.hidden=YES;
        self.voiceSpeakerButton.hidden=YES;
    }
}

-(void)autoHiddenVoiceCallView:(NSTimer *)timer
{
    delayTimer = timer;
    voiceCallIndex ++;
    if (voiceCallIndex>SWDelayInterval) {
        self.settingWheel.hidden=YES;
        self.voiceMuteButton.hidden=YES;
        self.voiceHandUpButton.hidden=YES;
        self.voiceSpeakerButton.hidden=YES;
        
        voiceCallIndex = 0;
        
        [timer invalidate];
    }
}

static int videoChatIndex =  0 ;
-(void)touchForVideoChatView:(UITapGestureRecognizer *)recognizer
{
    
    if (self.settingWheel.hidden) {
        
        self.settingWheel.hidden=NO;
        self.videoMuteButton.hidden=NO;
        self.videoHandUpButton.hidden=NO;

        [delayTimer invalidate];
        videoChatIndex = 0;
        [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(autoHiddenVideoChatView:) userInfo:nil repeats:YES];
    }
    else
    {
        self.settingWheel.hidden=YES;
        self.videoMuteButton.hidden=YES;
        self.videoHandUpButton.hidden=YES;
    }
}

//10s后自动隐藏
-(void)autoHiddenVideoChatView:(NSTimer *)timer
{
    
    delayTimer = timer;
    videoChatIndex ++;

    if (videoChatIndex>SWDelayInterval) {
        self.settingWheel.hidden=YES;
        self.videoMuteButton.hidden=YES;
        self.videoHandUpButton.hidden=YES;
        videoChatIndex = 0;
        [timer invalidate];
    }
}


static int monitorIndex =  0 ;
-(void)touchForMonitorView:(UITapGestureRecognizer *)recognizer
{
    if (self.settingWheel.hidden) {
        
        self.settingWheel.hidden=NO;
        self.ptzControlButton.hidden=NO;
        self.stopWatchButton.hidden=NO;
    
        [delayTimer invalidate];
        monitorIndex = 0;
        [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(autoHiddenMonitorView:) userInfo:nil repeats:YES];
    }
    else
    {
        self.settingWheel.hidden=YES;
        self.ptzControlButton.hidden=YES;
        self.stopWatchButton.hidden=YES;
    }
}
//10s后自动隐藏
-(void)autoHiddenMonitorView:(NSTimer *)timer
{

    delayTimer = timer;
     monitorIndex ++;
     if (monitorIndex>SWDelayInterval) {
        self.settingWheel.hidden=YES;
        self.ptzControlButton.hidden=YES;
        self.stopWatchButton.hidden=YES;
        monitorIndex = 0;
        
        [timer invalidate];
    }
}

//移动自己画面的位置
-(void)handlePan:(UIPanGestureRecognizer *)recognizer
{
    CGPoint translation = [recognizer translationInView:recognizer.view];
    recognizer.view.transform = CGAffineTransformTranslate(recognizer.view.transform, translation.x, translation.y);
    [recognizer setTranslation:CGPointZero inView:recognizer.view];
    
    CGPoint point = [recognizer.view convertRect:recognizer.view.bounds toView:nil].origin;
    
    self.mineVideoView.position=point;
    if (self.mineVideoView.x<0) {
        self.mineVideoView.x=0;
    }
    
    if (self.mineVideoView.y<0) {
        self.mineVideoView.y=0;
    }
    
    if (self.mineVideoView.x>SWScreenWidth-self.mineVideoView.width) {
        self.mineVideoView.x=SWScreenWidth-self.mineVideoView.width;
    }
    if (self.mineVideoView.y>SWScreenHeight-self.mineVideoView.height) {
        self.mineVideoView.y=SWScreenHeight-self.mineVideoView.height;
    }
   
}

-(void)stopCommunication:(NSNotification *)noti
{
    [self dismissViewControllerAnimated:YES completion:nil];
}


#pragma mark -private methods



#pragma mark -getter methods

#pragma mark -mainView及其子视图

-(UIView *)mainView
{
    if (!_mainView)
    {
        _mainView = [[UIView alloc]initWithFrame:[UIScreen mainScreen].bounds];
        
        if (_mainViewType==SWAudioMainViewType)
        {
            //语音对讲
            _mainView.backgroundColor=[UIColor colorWithPatternImage:[UIImage imageNamed:@"login_bg"]];
            [_mainView addSubview:self.avatarImageView];
            [_mainView addSubview:self.userInfoLabel];
            
            //加载上层二级视图
            [_mainView addSubview:self.voiceCallView];
            
            //10s后隐藏
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(SWDelayInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                [self touchForVoiceCallView:nil];
            });
            
        }
        else if (_mainViewType==SWVideoMainViewType)
        {
            //视频聊天/监控
            [_mainView addSubview:self.peerVideoView];
            
            //加载上层二级视图
            if (_videoType==SWMonitorVideoType) {
             [_mainView addSubview:self.monitorView];
                
                //10s后隐藏
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(SWDelayInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                    [self touchForMonitorView:nil];
                });
            }
            else if (_videoType == SWChatVideoType)
            {
             [_mainView addSubview:self.videoChatView];
                
                //10s后隐藏
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(SWDelayInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                    [self touchForVideoChatView:nil];
                });
            }
            
            
            
            
        }
    }
    return _mainView;
}


//视频(监控+视频聊天)子视图
-(UIView *)peerVideoView
{
    if (!_peerVideoView) {
        _peerVideoView = [[UIView alloc] initWithFrame:[UIScreen mainScreen].bounds];
        _peerVideoView.backgroundColor=[UIColor blackColor];
    }
    return _peerVideoView;
}

-(UIView *)mineVideoView
{
    if (!_mineVideoView) {
        _mineVideoView =[[UIView alloc] initWithFrame:CGRectMake(0, 0, 160, 90)];
        _mineVideoView.backgroundColor = [UIColor grayColor];
        _mineVideoView.userInteractionEnabled=YES;
        //拖动手势
        
        UIPanGestureRecognizer *panGestureRecognizer = [[UIPanGestureRecognizer alloc]
                                                        initWithTarget:self
                                                        action:@selector(handlePan:)];
        [_mineVideoView addGestureRecognizer:panGestureRecognizer];
    }
    return _mineVideoView;
}

//语音对讲子视图
-(UIImageView *)avatarImageView
{
    if (!_avatarImageView) {
        _avatarImageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"avatar"]];
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
        _userInfoLabel.font=[UIFont systemFontOfSize:20];
        _userInfoLabel.text=@"zly \n 优云id:23232";
        _userInfoLabel.textColor = [UIColor redColor];
        _userInfoLabel.numberOfLines=2;
        _userInfoLabel.textAlignment=NSTextAlignmentCenter;
        
    }
    return _userInfoLabel;
}

#pragma mark -监控(云台控制)及其子视图

-(UIView *)monitorView
{
    if (!_monitorView) {
        //查看监控时的View
        _monitorView = [[UIView alloc] initWithFrame:[UIScreen mainScreen].bounds];
        UITapGestureRecognizer *recognizer =[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(touchForMonitorView:)];
        [_monitorView addGestureRecognizer:recognizer];
        
        [_monitorView addSubview:self.mineVideoView];
        [_monitorView addSubview:self.ptzControlButton];
        [_monitorView addSubview:self.stopWatchButton];
        [_monitorView addSubview:self.settingWheel];
        
    }
    return _monitorView;
}

-(UIButton *)ptzControlButton
{
    if (!_ptzControlButton) {
        _ptzControlButton =[[UIButton alloc] initWithFrame:CGRectMake(0, 0,132, 36)];
        [_ptzControlButton setImage:[UIImage imageNamed:@"ptzControl"] forState:UIControlStateNormal];
        
        
        [[_ptzControlButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            
            //addSubView ptzControl
            [_monitorView addSubview:self.ptzControllView];
        }];
    }
    return _ptzControlButton;
}

-(UIButton *)stopWatchButton
{
    if (!_stopWatchButton) {
        _stopWatchButton =[[UIButton alloc] initWithFrame:CGRectMake(0, 0,132, 36)];
        [_stopWatchButton setImage:[UIImage imageNamed:@"stopWatch"] forState:UIControlStateNormal];
        
        [[_stopWatchButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            
            
            if (self.handUpSignal) {
                [self.handUpSignal sendNext:SWMonitorHandUp];
            }
            
            [self dismissViewControllerAnimated:YES completion:nil];
        }];
        
    }
    return _stopWatchButton;
}

-(SWPTZControlView *)ptzControllView
{
    if (!_ptzControllView) {
        _ptzControllView = [[SWPTZControlView alloc] initWithFrame:[UIScreen mainScreen].bounds];
        _ptzControllView.delegate=self;
    }
    return _ptzControllView;
}



#pragma mark -视频聊天及其子视图

-(UIView *)videoChatView
{
    if (!_videoChatView) {
        //视频聊天时的View
        
        _videoChatView = [[UIView alloc] initWithFrame:[UIScreen mainScreen].bounds];
        
        UITapGestureRecognizer *recognizer =[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(touchForVideoChatView:)];
        [_videoChatView addGestureRecognizer:recognizer];
        
        [_videoChatView addSubview:self.mineVideoView];
        [_videoChatView addSubview:self.videoMuteButton];
        [_videoChatView addSubview:self.videoHandUpButton];
        [_videoChatView addSubview:self.settingWheel];
        
        
    }
    return _videoChatView;
}

-(UIButton *)videoMuteButton
{
    if (!_videoMuteButton) {
        _videoMuteButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 132, 36)];
        [_videoMuteButton setImage:[UIImage imageNamed:@"videoMute"] forState:UIControlStateNormal];
        _voiceMuteButton.backgroundColor=[UIColor greenColor];
        
        [[_videoMuteButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            SWLog(@"视频聊天--->静音");
        }];
    }
    return _videoMuteButton;
}

-(UIButton *)videoHandUpButton
{
    if (!_videoHandUpButton) {
        _videoHandUpButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0,132, 36)];
        [_videoHandUpButton setImage:[UIImage imageNamed:@"onCallHandUp"] forState:UIControlStateNormal];
        
        [[_videoHandUpButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            
            if (self.handUpSignal) {
                [self.handUpSignal sendNext:SWVideoHandUp];
            }
           
            [self dismissViewControllerAnimated:NO completion:nil];
            
            [self orientationToPortrait:UIInterfaceOrientationPortrait];
//            [self goHome];
            
        }];
    }
    return _videoHandUpButton;
}



#pragma mark -语音对讲及其子视图
-(UIView *)voiceCallView
{
    if (!_voiceCallView) {
        //语音对讲时的View
        _voiceCallView = [[UIView alloc] initWithFrame:[UIScreen mainScreen].bounds];
        _voiceCallView.backgroundColor= [UIColor colorWithWhite:0 alpha:0.1];
    
        
        UITapGestureRecognizer *recognizer =[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(touchForVoiceCallView:)];
        [_voiceCallView addGestureRecognizer:recognizer];
        
        
        [_voiceCallView addSubview:self.voiceMuteButton];
        [_voiceCallView addSubview:self.voiceSpeakerButton];
        [_voiceCallView addSubview:self.voiceHandUpButton];
        [_voiceCallView addSubview:self.settingWheel];
        
    }
    return _voiceCallView;
}

-(UIButton *)voiceMuteButton
{
    if (!_voiceMuteButton) {
        _voiceMuteButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 79, 100)];
        [_voiceMuteButton setImage:[UIImage imageNamed:@"audioMute"] forState:UIControlStateNormal];
        
        [[_voiceMuteButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            SWLog(@"语音对讲--->静音");
        }];
    }
    return _voiceMuteButton;
}

-(UIButton *)voiceSpeakerButton
{
    if (!_voiceSpeakerButton) {
        _voiceSpeakerButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0,79, 100)];
        [_voiceSpeakerButton setImage:[UIImage imageNamed:@"audioSpeaker"] forState:UIControlStateNormal];

        
        [[_voiceSpeakerButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            SWLog(@"语音对讲--->免提");
        }];
    }
    return _voiceSpeakerButton;
}

-(UIButton *)voiceHandUpButton
{
    if (!_voiceHandUpButton) {
        _voiceHandUpButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 280 , 36)];
        [_voiceHandUpButton setImage:[UIImage imageNamed:@"audioHandUp"] forState:UIControlStateNormal];
        
        [[_voiceHandUpButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            
            if (self.handUpSignal) {
                [self.handUpSignal sendNext:SWVoiceHandUp];
            }
            
            [self dismissViewControllerAnimated:YES completion:nil];
        }];
    }
    return _voiceHandUpButton;
}



#pragma mark -转盘设置以及弹出视图
-(SWSettingWheel *)settingWheel
{
    if (!_settingWheel)
    {
    //设置转盘
        _settingWheel = [[SWSettingWheel alloc] initWithFrame:CGRectMake(0, 0, 300, 300)];
        
        //这种做法的原因是防止popView和settingWheel超过边界
        if (_settingWheel.width>((MIN(SWScreenHeight, SWScreenWidth) - SWPopSettingViewPaddingToLeftEdge)/SWPopSettingViewAspectRatio)) {
            CGFloat width=(MIN(SWScreenHeight, SWScreenWidth) - SWPopSettingViewPaddingToLeftEdge)/SWPopSettingViewAspectRatio;
            _settingWheel.frame = CGRectMake(0, 0, width, width);
        }
        _settingWheel.delegate=self;
        _settingWheel.backgroundColor = [UIColor clearColor];
    }
    return _settingWheel;
}

-(SWSettingPopView *)settingPopView
{
    if (!_settingPopView) {
    //设置弹出的View
    }
    return _settingPopView;
}


@end
