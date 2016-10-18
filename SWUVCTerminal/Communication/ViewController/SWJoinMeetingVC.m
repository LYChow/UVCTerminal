//
//  SWJoinMeetingVC.m
//  SWUVCTerminal
//
//  Created by hxf on 8/12/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWJoinMeetingVC.h"
#import "SWUser.h"
@interface SWJoinMeetingVC ()
#pragma mark -Property
@property(nonatomic,strong) UIButton *backButton;  //返回按钮
@property(nonatomic,strong) UIImageView *avatarImageView; //头像
@property(nonatomic,strong) UILabel *userInfoLabel;  //用户信息
@property(nonatomic,strong) UILabel *meetingInfoLabel;  //会议信息
@property(nonatomic,strong) UIButton *joinMeetingButton; //参加会议
@end

@implementation SWJoinMeetingVC

#pragma mark -life cycle

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title=@"参加会议";
    self.view.backgroundColor=[UIColor colorWithPatternImage:[UIImage imageNamed:SWScreenHeight>SWScreenWidth?@"login_bg":@"login_landSpace_bg"]];
    [self setUpNavTheme];
    

    [self.view addSubview:self.avatarImageView];
    [self.view addSubview:self.userInfoLabel];
    [self.view addSubview:self.meetingInfoLabel];
    [self.view addSubview:self.joinMeetingButton];
    [[self.joinMeetingButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
        SWLog(@"参加会议");
    }];
}

-(void)viewDidLayoutSubviews
{
    CGFloat horizontalPadding =(SWScreenWidth-self.joinMeetingButton.width)/2;
    CGFloat bottomPadding = 50+64;
    
    self.avatarImageView.position=CGPointMake((SWScreenWidth-self.avatarImageView.width)/2, 100);
    self.userInfoLabel.position = CGPointMake((SWScreenWidth-self.userInfoLabel.width)/2, CGRectGetMaxY(self.avatarImageView.frame)+10);
    
    self.joinMeetingButton.position = CGPointMake(horizontalPadding, SWScreenHeight-self.joinMeetingButton.height-bottomPadding);
    
    self.meetingInfoLabel.position =CGPointMake((SWScreenWidth-self.meetingInfoLabel.width)/2, CGRectGetMinY(self.joinMeetingButton.frame)-self.meetingInfoLabel.height-30);
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
    }];
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:backButton];
}


#pragma mark -system object Delegate


#pragma mark -custom Delegate

#pragma mark -event respond



#pragma mark -private methods



#pragma mark -getter methods

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

-(UILabel *)meetingInfoLabel
{
    if (!_meetingInfoLabel) {
        _meetingInfoLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 200, 150)];
        _meetingInfoLabel.font=[UIFont systemFontOfSize:15];
        _meetingInfoLabel.text=@"会议主持 zly1 /n 会议成员 zly /n 会议时间 2016/08/12-15:22:34 /n 会议简介  [默认会议]";
        _meetingInfoLabel.textColor = [UIColor whiteColor];
        _meetingInfoLabel.numberOfLines=0;
        _meetingInfoLabel.textAlignment=NSTextAlignmentLeft;
    }
  return   _meetingInfoLabel;
}

-(UILabel *)userInfoLabel
{
    if (!_userInfoLabel) {
        _userInfoLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 200, 60)];
        _userInfoLabel.font=[UIFont systemFontOfSize:20];
        _userInfoLabel.text=@"zly \n 优云id:23232";
        _userInfoLabel.textColor = [UIColor whiteColor];
        _userInfoLabel.numberOfLines=2;
        _userInfoLabel.textAlignment=NSTextAlignmentCenter;
        
    }
    return _userInfoLabel;
}


-(UIButton *)joinMeetingButton
{
    if (!_joinMeetingButton) {
        _joinMeetingButton =  [[UIButton alloc]initWithFrame:CGRectMake(0, 0, 280, 44)];
        [_joinMeetingButton setImage:[UIImage imageNamed:@"joinMeeting"] forState:UIControlStateNormal];
    }
    return _joinMeetingButton;
}


@end
