//
//  SWLoginViewController.m
//  SWUVCTerminal
//
//  Created by hxf on 8/18/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWLoginViewController.h"
#import "SWAppDemo.h"
#import "SWUser.h"
#import "UIImage+resizableImage.h"
#import "SWSetting.h"
#import "SWFileManager.h"
#define SWVerticalSpace 20
#define SWPaddingToEdge 40
@interface SWLoginViewController ()<UITextFieldDelegate>

#pragma mark -property
@property(nonatomic,strong)UIImageView *userNameIcon;
@property(nonatomic,strong)UIImageView *ipAddressIcon;
@property(nonatomic,strong)UIImageView *passcodeIcon;

@property(nonatomic,strong)UIImageView *logoImageView;

@property (strong, nonatomic)  UITextField *userNameTextField;
@property (strong, nonatomic)  UITextField *serverAddressTextField;
@property (strong, nonatomic)  UITextField *passcodeTextField;
@property (strong, nonatomic)  UIButton *loginButton;
@property (strong, nonatomic)  UIButton *rememberPasscodeButton;
@property (strong, nonatomic)  UILabel  *rememberPasscodeLabel;

@property(strong,nonatomic) UITextField *focusTextField;

@end

@implementation SWLoginViewController

#pragma mark -life cycle


- (void)viewDidLoad {
    [super viewDidLoad];
    SWUser *user =[SWUser user];
    
    //给appDemo的loginView对象赋值
    GetApp().SetLoginView(self);

    [self.view addSubview:self.logoImageView];
    
    [self.view addSubview:self.userNameTextField];
    self.userNameTextField.text=user.userName;
    
    [self.view addSubview:self.serverAddressTextField];
    self.serverAddressTextField.text=user.ipAddress;
    
    [self.view addSubview:self.passcodeTextField];
    
    [self.view addSubview:self.rememberPasscodeButton];
    [self.view addSubview:self.rememberPasscodeLabel];
    [self.view addSubview:self.loginButton];
    
    
    self.rememberPasscodeButton.selected=[user.isRememberPassword boolValue];
    
    
    if (_isAutoLogin) [self login];
   
    
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardWillChange:)
                                                 name:UIKeyboardWillChangeFrameNotification
                                               object:nil];
    
 
}

- (void)viewDidDisappear:(BOOL)animated
{
    //给appDemo的loginView对象置为nil
    GetApp().SetLoginView(nil);
    
    [super viewDidDisappear:animated];
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:UIKeyboardWillChangeFrameNotification
                                                  object:nil];
}


-(void)viewDidLayoutSubviews
{
    [super viewDidLayoutSubviews];
     self.view.backgroundColor=[UIColor colorWithPatternImage:[UIImage imageNamed:SWScreenHeight>SWScreenWidth?@"login_bg":@"login_landSpace_bg"]];
   
    //内容总高度 =CGRectGetMaxY(self.loginButton.frame)-50 //contentHeight=378
    self.logoImageView.frame=CGRectMake((SWScreenWidth-self.logoImageView.width)/2, (SWScreenHeight-378)/3, 202, 58);
    self.userNameTextField.frame = CGRectMake(SWPaddingToEdge, CGRectGetMaxY(self.logoImageView.frame)+50, SWScreenWidth-2*SWPaddingToEdge, 40);
    self.serverAddressTextField.frame = CGRectMake(self.userNameTextField.x, CGRectGetMaxY(self.userNameTextField.frame)+SWVerticalSpace,  SWScreenWidth-2*SWPaddingToEdge, 40);
     self.passcodeTextField.frame= CGRectMake(self.userNameTextField.x,CGRectGetMaxY(self.serverAddressTextField.frame)+SWVerticalSpace,   SWScreenWidth-2*SWPaddingToEdge, 40);
    self.rememberPasscodeLabel.frame = CGRectMake(CGRectGetMaxX(self.userNameTextField.frame)-self.rememberPasscodeLabel.width, CGRectGetMaxY(self.passcodeTextField.frame)+SWVerticalSpace,  70, 30);
    self.rememberPasscodeButton.frame = CGRectMake(CGRectGetMinX(self.rememberPasscodeLabel.frame)-self.rememberPasscodeButton.width, CGRectGetMaxY(self.passcodeTextField.frame)+SWVerticalSpace-5,40, 40);
    
    self.loginButton.frame=CGRectMake(self.userNameTextField.x,CGRectGetMaxY(self.rememberPasscodeLabel.frame)+SWVerticalSpace,SWScreenWidth-2*SWPaddingToEdge, 40);
}

#pragma mark -system object Delegate

#pragma mark -UITextFieldDelegate
-(BOOL)textFieldShouldBeginEditing:(UITextField *)textField
{
  self.focusTextField=textField;
  return YES;
}



#pragma mark -custom Delegate

#pragma mark -event respond
//登录
-(void)login
{
    //进行判空操作
    if(self.userNameTextField.text.length &&self.serverAddressTextField.text.length)
    {
        NSString* strIp=self.serverAddressTextField.text;
        NSString* strUser=self.userNameTextField.text;
        NSString* strPass=self.passcodeTextField.text;
        
        GetApp().GetUA()->Login(ToCStr(strIp),ToCStr(strUser),ToCStr(strPass));
    }
    else
    {
        [MBProgressHUD showError:@"用户名或者密码为空"];
    }
}

-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self.userNameTextField resignFirstResponder];
    [self.serverAddressTextField resignFirstResponder];
    [self.passcodeTextField resignFirstResponder];
}

- (void)keyboardWillChange:(NSNotification *)notification
{
    [self.view layoutIfNeeded];
    
    NSDictionary* info = notification.userInfo;
    CGRect kbRect = [info[UIKeyboardFrameEndUserInfoKey] CGRectValue];
    
    NSValue * animationDurationValue = info[UIKeyboardAnimationDurationUserInfoKey];
    NSTimeInterval duration;
    [animationDurationValue getValue:&duration];
    
    //判断keyBoard.y与LoginBtn的相对位置

    CGFloat focusTextFieldMaxY = CGRectGetMaxY(self.focusTextField.frame)+80;
    
    if (focusTextFieldMaxY>kbRect.origin.y)
    {
        [UIView animateWithDuration:duration animations:^{
            //上移self.view的Frame
            self.view.y=-(focusTextFieldMaxY - kbRect.origin.y);
        }];
    }
    else
    {
        [UIView animateWithDuration:duration animations:^{
            //上移self.view的Frame
            self.view.y= 0;
        }];
        
    }
}

-(void)OnLogin:(int)ret
{
     [MBProgressHUD hideHUDForView:self.view];
    
    
    /*
     LoginConnectFail = -4,
     LoginGetRegInfoFail,
     LoginGetRegInfoLoginedElsewhere,
     LoginRegNodeFail,
     LoginSucess,
     LoginConnecting,
     LoginGetRegInfo,
     LoginRegNode,
     */
    //详细提示版本
//    NSString* msg[]={
//        @"网络有问题!",
//        @"获取注册信息失败!",
//        @"该账号在其他地方登录!",
//        @"注册节点失败",
//        @"登录成功",
//        @"正在登录....",
//        @"获取节点信息...",
//        @"注册节点..."
//    };
    
    //简化版
    NSString* msg[]={
        @"网络有问题!",
        @"获取注册信息失败!",
        @"该账号在其他地方登录!",
        @"登录失败!",
        @"登录成功!",
        @"正在登录....",
        @"正在登录....",
        @"正在登录...."
    };
     if(ret==0)
    {
         [MBProgressHUD showSuccess:msg[ret+4] toView:self.view];
    }
    else if(ret>0)
    {
        [MBProgressHUD showMessage:msg[ret+4] toView:self.view];
    }
    else
    {
        [MBProgressHUD showError:msg[ret+4] toView:self.view];
    }
  
    
    if(ret==0)
    {
        //存储user属性
        SWUser *user =[SWUser user];
        user.userName=self.userNameTextField.text;
        user.ipAddress=self.serverAddressTextField.text;
        user.password=self.rememberPasscodeButton.selected?self.passcodeTextField.text:@"";
        user.isRememberPassword=[NSNumber numberWithBool:self.rememberPasscodeButton.selected];
        [user save];
        
        [[NSNotificationCenter defaultCenter] postNotificationName:SWUserInfoUpdateNotification object:nil];
        
        GetApp().GetUA()->UpdateTerminalInfo(0);
        
        GetApp().GetUA()->SendCommand(GetApp().GetUA()->GetSelf(), 121, 0);
        
        [self dismissViewControllerAnimated:YES completion:nil];
    }
    
}

#pragma mark -private methods



#pragma mark -getter methods

-(UIImageView *)logoImageView
{
    if (!_logoImageView) {
        _logoImageView = [[UIImageView alloc]initWithImage:[UIImage imageNamed:@"login_logo"]];
        _logoImageView.frame=CGRectMake(0, 0, 202, 58);
        _logoImageView.contentMode=UIViewContentModeCenter;
    }
    return _logoImageView;
}

-(UITextField *)userNameTextField
{
    if (!_userNameTextField) {
        _userNameTextField = [[UITextField alloc] initWithFrame:CGRectMake(0, 0, SWScreenWidth-2*SWPaddingToEdge, 40)];
        _userNameTextField.textColor=[UIColor whiteColor];
        _userNameTextField.borderStyle=UITextBorderStyleNone;
        _userNameTextField.delegate=self;
        _userNameTextField.background=[UIImage imageNamed:@"login_write"];
        _userNameTextField.clearButtonMode=UITextFieldViewModeAlways;
        _userNameTextField.leftViewMode = UITextFieldViewModeAlways;
        _userNameTextField.leftView =self.userNameIcon;
    }
    return _userNameTextField;
}

-(UITextField *)serverAddressTextField
{
    if (!_serverAddressTextField) {
        _serverAddressTextField = [[UITextField alloc] initWithFrame:CGRectMake(SWPaddingToEdge, 0, SWScreenWidth-2*SWPaddingToEdge, 40)];
        _serverAddressTextField.textColor=[UIColor whiteColor];
        _serverAddressTextField.borderStyle=UITextBorderStyleNone;
        _serverAddressTextField.delegate=self;
        _serverAddressTextField.keyboardType=UIKeyboardTypeNumbersAndPunctuation;
        _serverAddressTextField.background=[UIImage resizableImageNamed:@"login_write"];
        _serverAddressTextField.clearButtonMode=UITextFieldViewModeAlways;
        _serverAddressTextField.leftView =self.ipAddressIcon;
        _serverAddressTextField.leftViewMode = UITextFieldViewModeAlways;
        
    }
    return _serverAddressTextField;
}


-(UITextField *)passcodeTextField
{
    if (!_passcodeTextField) {
        _passcodeTextField = [[UITextField alloc] initWithFrame:CGRectMake(0, 0, SWScreenWidth-2*SWPaddingToEdge, 40)];
        _passcodeTextField.textColor=[UIColor whiteColor];
        _passcodeTextField.secureTextEntry=YES;
        _passcodeTextField.borderStyle=UITextBorderStyleNone;
        _passcodeTextField.delegate=self;
        _passcodeTextField.background=[UIImage resizableImageNamed:@"login_write"];
        _passcodeTextField.clearButtonMode=UITextFieldViewModeAlways;
        _passcodeTextField.leftViewMode = UITextFieldViewModeAlways;
        _passcodeTextField.leftView =self.passcodeIcon
        
        ;
    }
    return _passcodeTextField;
}

-(UIImageView *)userNameIcon
{
    if (!_userNameIcon) {
        _userNameIcon = [[UIImageView alloc]initWithImage:[UIImage imageNamed:@"login_user"]];
        _userNameIcon.frame=CGRectMake(0, 0, 40, 40);
        _userNameIcon.contentMode=UIViewContentModeCenter;
    }
    return _userNameIcon;
}


-(UIImageView *)ipAddressIcon
{
    if (!_ipAddressIcon) {
        _ipAddressIcon = [[UIImageView alloc]initWithImage:[UIImage imageNamed:@"login_server"]];
        _ipAddressIcon.frame=CGRectMake(0, 0,40, 40);
        _ipAddressIcon.contentMode=UIViewContentModeCenter;
    }
    return _ipAddressIcon;
}


-(UIImageView *)passcodeIcon
{
    if (!_passcodeIcon) {
        _passcodeIcon = [[UIImageView alloc]initWithImage:[UIImage imageNamed:@"login_key"]];
        _passcodeIcon.frame=CGRectMake(0, 0,40, 40);
        _passcodeIcon.contentMode=UIViewContentModeCenter;
    }
    return _passcodeIcon;
}

-(UIButton *)rememberPasscodeButton
{
    if (!_rememberPasscodeButton) {
        _rememberPasscodeButton= [UIButton buttonWithType:UIButtonTypeCustom];
        _rememberPasscodeButton.frame=CGRectMake(0, 0,40, 40);
        [_rememberPasscodeButton setImage:[UIImage imageNamed:@"login_button_check"] forState:UIControlStateNormal];
        [_rememberPasscodeButton setImage:[UIImage imageNamed:@"login_button_check2"] forState:UIControlStateSelected];
        [[_rememberPasscodeButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            _rememberPasscodeButton.selected=!_rememberPasscodeButton.selected;
            
            SWUser *user =[SWUser user];
            user.isRememberPassword  = [NSNumber numberWithBool:_rememberPasscodeButton.selected];
            [user save];
        }];
    }
    return _rememberPasscodeButton;
}

-(UILabel *)rememberPasscodeLabel
{
    if (!_rememberPasscodeLabel) {
        _rememberPasscodeLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 70, 30)];
        _rememberPasscodeLabel.text=@"记住密码";
        _rememberPasscodeLabel.textColor=[UIColor whiteColor];
        _rememberPasscodeLabel.textAlignment=NSTextAlignmentRight;
        _rememberPasscodeLabel.font= [UIFont systemFontOfSize:17];
        _rememberPasscodeLabel.backgroundColor=[UIColor clearColor];
    }
    return _rememberPasscodeLabel;

}

-(UIButton *)loginButton
{
    if (!_loginButton) {
        _loginButton= [UIButton buttonWithType:UIButtonTypeCustom];
        _loginButton.frame=CGRectMake(0, 0, SWScreenWidth-2*SWPaddingToEdge, 40);
        _loginButton.layer.cornerRadius=5.0;
        [_loginButton setTitle:@"登录" forState:UIControlStateNormal];
        [_loginButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        _loginButton.backgroundColor = SWColor(73, 211, 111);
        [_loginButton addTarget:self action:@selector(login) forControlEvents:UIControlEventTouchUpInside];
       
    }
    return _loginButton;
}

@end
