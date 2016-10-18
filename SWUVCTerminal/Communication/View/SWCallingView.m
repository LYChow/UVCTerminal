//
//  SWCallingView.m
//  SWUVCTerminal
//
//  Created by hxf on 8/23/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWCallingView.h"

@interface SWCallingView()
{

}
@property(nonatomic,strong)UIWindow *keyWindow;

@property(nonatomic,strong) UIImageView *avatarImageView; //头像
@property(nonatomic,strong) UILabel *userInfoLabel;  //用户信息
@property(nonatomic,strong) UIImageView *callingImageView; //呼叫...
@property(nonatomic,strong) UIButton *cancelCallingButton; //取消呼叫
@property(nonatomic,strong) UIButton *acceptButton; //接听
@property(nonatomic,strong) UIButton *handUpButton; //挂断
@property(nonatomic,strong) NSTimer* callingTimer;  //控制动画
@end

@implementation SWCallingView

static SWCallingView *callingView=nil;

#pragma mark -life cycle
+(instancetype)shareManager
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        callingView = [[SWCallingView alloc] init];
    });
    return callingView;
}

-(void)layoutSubviews
{
    [super layoutSubviews];
   
     _callingTimer=nil;
    self.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"login_bg"]];
    
    [self.keyWindow addSubview:self];
    self.frame=self.keyWindow.bounds;
    
    [self addSubview:self.avatarImageView];
    [self addSubview:self.userInfoLabel];
    [self addSubview:self.callingImageView];
    
    self.avatarImageView.position=CGPointMake((SWScreenWidth-self.avatarImageView.width)/2, 100);
    self.userInfoLabel.position = CGPointMake((SWScreenWidth-self.userInfoLabel.width)/2, CGRectGetMaxY(self.avatarImageView.frame)+10);
    self.callingImageView.position=CGPointMake((SWScreenWidth-self.callingImageView.width)/2, SWScreenHeight-self.callingImageView.height-130);
    
    if (_callDirectType==SWCallOutcomingDirectType)
    {

        [self addSubview:self.cancelCallingButton];
        [self.acceptButton removeFromSuperview];
        [self.handUpButton removeFromSuperview];
        
        
         self.cancelCallingButton.position=CGPointMake((SWScreenWidth-self.cancelCallingButton.width)/2, SWScreenHeight-self.cancelCallingButton.height-30);
    }
    else if (_callDirectType==SWCallIncomingDirectType)
    {
        [self addSubview:self.acceptButton];
        [self addSubview:self.handUpButton];
        [self.cancelCallingButton removeFromSuperview];
        
        self.acceptButton.position = CGPointMake(SWScreenWidth/2-20-self.acceptButton.width, SWScreenHeight-self.acceptButton.height-40);
        self.handUpButton.position = CGPointMake(SWScreenWidth/2+20, self.acceptButton.y);
    }
}

#pragma mark -event Methods

-(void)show
{
    [self layoutSubviews];
    [self startAnimation];
    
    self.y=0;
    self.hidden=NO;
}

-(void)hidden
{
    //取消动画
    [self stopAnimation];
    
    //1秒后calling界面消失
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        self.y=SWScreenHeight;
        self.hidden=YES;
    });
}



#pragma mark -private methods

-(void)startAnimation
{
    [self stopAnimation];
    [NSTimer scheduledTimerWithTimeInterval:0.8 target:self selector:@selector(switchCallingImage:) userInfo:nil repeats:YES];
}

-(void)stopAnimation
{
        if (self.callingTimer)
        {
        [self.callingTimer invalidate];
        self.callingTimer=nil;
        }
    self.callingImageView.image=[UIImage imageNamed:@"calling0"];
}

-(void)switchCallingImage:(NSTimer *)timer
{
    self.callingTimer=timer;
    static int timesIndex =0;
    UIImage *image = [UIImage imageNamed:[NSString stringWithFormat:@"calling%i",(timesIndex++)%3]];
    self.callingImageView.image=image;
}

#pragma mark -getter methods
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
        _userInfoLabel.text=self.peerTerminalName.length?self.peerTerminalName:@"unknown";
        _userInfoLabel.textColor = [UIColor whiteColor];
        _userInfoLabel.numberOfLines=0;
        _userInfoLabel.textAlignment=NSTextAlignmentCenter;
        
    }
    return _userInfoLabel;
}

-(UIButton *)cancelCallingButton
{
    if (!_cancelCallingButton) {
        _cancelCallingButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 280 , 36)];
        [_cancelCallingButton setImage:[UIImage imageNamed:@"cancelCalling"] forState:UIControlStateNormal];
        
        [[_cancelCallingButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            [self hidden];
            if (self.callBackSignal) {
                [self.callBackSignal sendNext:SWCancled];
            }
            
        }];
    }
    return _cancelCallingButton;
}

-(UIButton *)acceptButton
{
    if (!_acceptButton) {
        _acceptButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 132, 36)];
        [_acceptButton setImage:[UIImage imageNamed:@"onCallAccept"] forState:UIControlStateNormal];
        
        [[_acceptButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            [self hidden];
            if (self.callBackSignal) {
                [self.callBackSignal sendNext:SWAccept];
            }
            
        }];
    }
    return _acceptButton;
}

-(UIButton *)handUpButton
{
    if (!_handUpButton) {
        _handUpButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0,132, 36)];
        [_handUpButton setImage:[UIImage imageNamed:@"onCallHandUp"] forState:UIControlStateNormal];
        
        [[_handUpButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            [self hidden];
            if (self.callBackSignal) {
                [self.callBackSignal sendNext:SWHandUp];
            }
            
        }];
    }
    return _handUpButton;
}



-(UIImageView *)callingImageView
{
    if (!_callingImageView) {
        _callingImageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"calling0"]];
        _callingImageView.frame  =CGRectMake(0, 0, 120, 120);
    }
    return _callingImageView;
}

-(UIWindow *)keyWindow
{
    if (!_keyWindow) {
        _keyWindow = [UIApplication sharedApplication].keyWindow;
        if (!_keyWindow)
        {
            _keyWindow = [[UIApplication sharedApplication].windows objectAtIndex:0];
        }
    }
    return _keyWindow;
}

@end
