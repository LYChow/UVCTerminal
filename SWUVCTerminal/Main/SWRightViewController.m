//
//  SWRightViewController.m
//  SWUVCTerminal
//
//  Created by hxf on 8/9/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWRightViewController.h"
#import "SWTerminalListVC.h"
#import "SWUser.h"
#import "SWNavigationController.h"
#import <Crashlytics/Crashlytics.h>
#define meetingScale  0.8


NSString *InterfaceOrientation = @"InterfaceOrientation";    //旋转屏幕的通知

@interface SWRightViewController ()<UIGestureRecognizerDelegate,UINavigationControllerDelegate,UIImagePickerControllerDelegate>

{

}

@property(nonatomic,strong) UIButton *backButton;  //返回按钮
@property(nonatomic,strong) UIButton *coverButton; //遮挡
@property(nonatomic,strong) UIImageView *avatarImageView; //头像
@property(nonatomic,strong) UILabel *userInfoLabel;  //用户信息
@property(nonatomic,strong) UIButton *talkBackButton; //对讲
@property(nonatomic,strong) UIButton *joinMeetingButton; //参加会议

@end

@implementation SWRightViewController

#pragma mark -life cycle


- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor=[UIColor colorWithPatternImage:[UIImage imageNamed:SWScreenHeight>SWScreenWidth?@"login_bg":@"login_landSpace_bg"]];
    
    [self.view addSubview:self.backButton];
    [self.view addSubview:self.avatarImageView];
    [self.view addSubview:self.userInfoLabel];
    [self.view addSubview:self.talkBackButton];
    [self.view addSubview:self.joinMeetingButton];
    [self.view addSubview:self.coverButton];
    
    
    
    [[self.backButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
         [self.delegateSignal sendNext:nil];
    }];
    
    //rightView在原点位置时coverButton隐藏
    [[self.view rac_valuesAndChangesForKeyPath:@"x" options:NSKeyValueObservingOptionNew observer:nil]subscribeNext:^(RACTuple  *x) {
        self.coverButton.hidden=![[[x allObjects] firstObject] integerValue];
    }];
    

    


    [[self.talkBackButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
        SWTerminalListVC *communicationVC = [[SWTerminalListVC alloc] init];
        communicationVC.communicationType=SWTalkBackCommunicationType;
        [self presentViewController:[[SWNavigationController alloc]initWithRootViewController:communicationVC] animated:YES completion:nil];
    }];

    
    
    
    [[self.joinMeetingButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
        SWTerminalListVC *communicationVC = [[SWTerminalListVC alloc] init];
        communicationVC.communicationType=SWMeetingCommunicationType;
        [self presentViewController:[[SWNavigationController alloc]initWithRootViewController:communicationVC] animated:YES completion:nil];
    }];
    
 
    
    //登录成功时刷新一下用户显示信息
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(updateUserInfo) name:SWUserInfoUpdateNotification object:nil];

}




//子视图布局
-(void)viewDidLayoutSubviews
{
    CGFloat horizontalPadding =(SWScreenWidth-2*self.talkBackButton.width)/3;
    CGFloat bottomPadding = 50;
    
     self.view.backgroundColor=[UIColor colorWithPatternImage:[UIImage imageNamed:SWScreenHeight>SWScreenWidth?@"login_bg":@"login_landSpace_bg"]];
    
    self.avatarImageView.position=CGPointMake((SWScreenWidth-self.avatarImageView.width)/2, 100);
    self.userInfoLabel.position = CGPointMake((SWScreenWidth-self.userInfoLabel.width)/2, CGRectGetMaxY(self.avatarImageView.frame)+10);
    self.talkBackButton.position=CGPointMake(horizontalPadding, SWScreenHeight-self.talkBackButton.height-bottomPadding);
    self.joinMeetingButton.position = CGPointMake(CGRectGetMaxX(self.talkBackButton.frame)+horizontalPadding, SWScreenHeight-self.joinMeetingButton.height-bottomPadding);
}

-(void)dealloc
{
    [[NSNotificationCenter defaultCenter]removeObserver:self name:SWChangeUserAvatarNotification object:nil];
}

#pragma mark -system object Delegate

#pragma mark -UIImagePickerControllerDelegate
- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    NSData *imageData = UIImagePNGRepresentation(info[UIImagePickerControllerEditedImage]);
    
    //把图片保存到本地文件夹
    NSString *avatarFilePath =[SWDocumentPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@_avatar.png",[SWUser user].userName]];
    [imageData writeToFile:avatarFilePath atomically:YES];
    

    UIImage *avatarImage = [UIImage imageWithContentsOfFile:avatarFilePath];
    _avatarImageView.image=avatarImage?avatarImage:[UIImage imageNamed:@"avatar"];
    
    [[NSNotificationCenter defaultCenter]postNotificationName:SWChangeUserAvatarNotification object:nil];
    [picker dismissViewControllerAnimated:YES completion:nil];
    
    #warning 上传图片逻辑
}

#pragma mark -custom Delegate

#pragma mark -event respond
-(void)handlePan:(UIPanGestureRecognizer *)recognizer
{

    CGPoint translation = [recognizer translationInView:recognizer.view];
    recognizer.view.transform = CGAffineTransformTranslate(recognizer.view.transform, translation.x, 0);
    [recognizer setTranslation:CGPointZero inView:recognizer.view];
    

    switch (recognizer.state) {
        case UIGestureRecognizerStateBegan:
        {
        
        }
            break;
        case UIGestureRecognizerStateChanged:
        {
            //View随着手势的移动而移动
            CGFloat xPointToWindow = [recognizer.view convertRect:recognizer.view.bounds toView:nil].origin.x;
            self.view.x=xPointToWindow;
            
            
            if (xPointToWindow>[UIScreen mainScreen].bounds.size.width*leftViewScale)
            {
                self.view.x=[UIScreen mainScreen].bounds.size.width*leftViewScale;
            }
            if (xPointToWindow<0)
            {
                self.view.x=0;
            }
        }
            break;
            case UIGestureRecognizerStateEnded:
            case UIGestureRecognizerStateCancelled:
            case UIGestureRecognizerStateFailed:
        {
            //手势结束时判断停留的最终位置
            [UIView animateWithDuration:0.2 animations:^{
                if (self.view.x>[UIScreen mainScreen].bounds.size.width/2)
                {
                    self.view.x=[UIScreen mainScreen].bounds.size.width*leftViewScale;
                }
                else
                {
                    self.view.x=0;
                }
            }];
        }
            break;
        default:
            break;
    }
    recognizer.view.frame=self.view.bounds;
}

-(void)changeAvatar:(UITapGestureRecognizer *)recognizer
{
    UIAlertController *controller = [UIAlertController alertControllerWithTitle:nil message:nil preferredStyle:UIAlertControllerStyleActionSheet];
    
    UIImagePickerController *imagePicker = [[UIImagePickerController alloc] init];
    imagePicker.delegate = self;
    imagePicker.allowsEditing = YES;
    
    [controller addAction:[UIAlertAction actionWithTitle:@"设置本地图片" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        imagePicker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;

        [self presentViewController:imagePicker animated:YES completion:nil];
    }]];
    
    [controller addAction:[UIAlertAction actionWithTitle:@"拍照" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
       imagePicker.sourceType = UIImagePickerControllerSourceTypeCamera;
        [self presentViewController:imagePicker animated:YES completion:nil];
    }]];
    
    [controller addAction:[UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        SWLog(@"取消");
    }]];
    [self presentViewController:controller animated:YES completion:nil];
}

//更新用户信息
-(void)updateUserInfo
{
    self.userInfoLabel.text=[SWUser user].userName;
    NSString *avatarFilePath = [SWDocumentPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@_avatar.png",[SWUser user].userName]];
    UIImage *avatarImage = [UIImage imageWithContentsOfFile:avatarFilePath];
    
    self.avatarImageView.image=avatarImage?avatarImage:[UIImage imageNamed:@"avatar"];
    
    [[NSNotificationCenter defaultCenter]postNotificationName:SWChangeUserAvatarNotification object:nil];
    
}

#pragma mark -private methods


#pragma mark -getter and setter methods
-(UIButton *)backButton
{
    if (!_backButton) {
        _backButton= [[UIButton alloc] initWithFrame:CGRectMake(10, 20, 40, 40)];
        [_backButton setImage:[UIImage imageNamed:@"more"] forState:UIControlStateNormal];
    }
    return _backButton;
}

-(UIButton *)coverButton
{
    if (!_coverButton) {
        _coverButton = [[UIButton alloc] initWithFrame:self.view.bounds];
        _coverButton.backgroundColor = [UIColor clearColor];
        
        //事件点击
        [[_coverButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            [self.delegateSignal sendNext:nil];
        }];

        //拖动手势
        UIPanGestureRecognizer *panGestureRecognizer = [[UIPanGestureRecognizer alloc]
                                                        initWithTarget:self
                                                        action:@selector(handlePan:)];
        [_coverButton addGestureRecognizer:panGestureRecognizer];
    }
    return _coverButton;
}

-(UIImageView *)avatarImageView
{
    if (!_avatarImageView) {
        NSString *avatarFilePath = [SWDocumentPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@_avatar.png",[SWUser user].userName]];
        UIImage *avatarImage = [UIImage imageWithContentsOfFile:avatarFilePath];
        
        _avatarImageView = [[UIImageView alloc] initWithImage:avatarImage?avatarImage:[UIImage imageNamed:@"avatar"]];
        _avatarImageView.frame  =CGRectMake(0, 0, 100, 100);
        _avatarImageView.layer.cornerRadius=_avatarImageView.height/2;
        _avatarImageView.clipsToBounds=YES;
        _avatarImageView.userInteractionEnabled=YES;
        UITapGestureRecognizer *tapGestureRecognizer = [[UITapGestureRecognizer alloc]
                                                        initWithTarget:self
                                                        action:@selector(changeAvatar:)];
        [_avatarImageView addGestureRecognizer:tapGestureRecognizer];
    }
    return _avatarImageView;
}

-(UILabel *)userInfoLabel
{
    if (!_userInfoLabel) {
        _userInfoLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 200, 60)];
        _userInfoLabel.font=[UIFont systemFontOfSize:20];
        NSString *userName = [SWUser user].userName;
        _userInfoLabel.text= userName.length?userName:@"default";
        _userInfoLabel.textColor = [UIColor whiteColor];
        _userInfoLabel.numberOfLines=2;
        _userInfoLabel.textAlignment=NSTextAlignmentCenter;
        
    }
    return _userInfoLabel;
}

-(UIButton *)talkBackButton
{
    if (!_talkBackButton) {
        _talkBackButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 146*meetingScale, 104*meetingScale)];
        [_talkBackButton setBackgroundImage:[UIImage imageNamed:@"call"] forState:UIControlStateNormal];
    }
    return _talkBackButton;
}

-(UIButton *)joinMeetingButton
{
    if (!_joinMeetingButton) {
        _joinMeetingButton = [[UIButton alloc]initWithFrame:CGRectMake(0, 0, 146*meetingScale, 104*meetingScale)];
        [_joinMeetingButton setBackgroundImage:[UIImage imageNamed:@"meeting"] forState:UIControlStateNormal];
    }
    return _joinMeetingButton;
}

@end
