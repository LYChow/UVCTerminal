//
//  ViewController.m
//  SWUVCTerminal
//
//  Created by hxf on 8/9/16.
//  Copyright © 2016 sinowave. All rights reserved.
//


/*继承要点:
 1.父类只是给子类提供服务(内存计数,初始化服务)，并不涉及子类的业务逻辑
 2.层级关系明显，功能划分清晰,Object并不参与MVC的管理中，那些都只是各自派生类自己要处理的事情
 3.父类的所有变化，都需要在子类中体现，也就是说此时耦合已经成为需求,内存引用计数 网络请求的发起，网络状态的判断，是所有派生类都需要的
 */

#import "ViewController.h"
#import "AppDelegate.h"
#import "SWAppDemo.h"
#import "SWLoginViewController.h"
#import "SWLeftViewController.h"
#import "SWRightViewController.h"
#import "SWUser.h"
#import "SWSetting.h"
#import "UIApplication+topmostViewController.h"

#import "SWCommunicationVC.h"
@interface ViewController ()

#pragma mark property
@property(nonatomic,strong) SWLeftViewController *leftVC;
@property(nonatomic,strong) SWRightViewController *rightVC;

@end

@implementation ViewController

#pragma mark -life cycle


+(void)load
{
//使用NSObject的load函数，在应用启动时自动监听。
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self addChildViewController:self.leftVC];
    [self addChildViewController:self.rightVC];
    
    [self.view addSubview:self.leftVC.view];
    [self.view addSubview:self.rightVC.view];
    
  
     
}

-(void)viewDidAppear:(BOOL)animated
{
    
    static BOOL isFirst=YES;
    if (isFirst)
    {
        SWLoginViewController *loginViewController = [[SWLoginViewController alloc] init];
        //判断设置里是否实现自动登录
        loginViewController.isAutoLogin =[[SWSetting setting].isAutoLogin boolValue];
        [self presentViewController:loginViewController animated:YES completion:nil];
        isFirst = NO;
    }
    
#warning 自动登录无提示版,勿删
    //    NSString *strIp=[SWUser user].ipAddress;
    //    NSString *strUser = [SWUser user].userName;
    //    NSString *strPass = [SWUser user].password;
    //
    //    if ([[SWSetting setting].isAutoLogin boolValue] &&strIp.length &&strUser.length)
    //    {
    //        GetApp().GetUA()->Login(ToCStr(strIp),ToCStr(strUser),ToCStr(strPass));
    //
    //        //注册一个通知监听
    //        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onLogin:) name:SWOnLoginNotification object:nil];
    //    }
    //    else
    //    {
    //        //非自动登录
    //        SWLoginViewController *loginViewController = [[SWLoginViewController alloc] init];
    //        [self presentViewController:loginViewController animated:YES completion:nil];
    //    }
   

}

-(BOOL)shouldAutorotate
{
 
    UIViewController *topmostVC = [UIApplication topmostViewController];
    //判断当前可视界面是否为communicationVC
    if ([topmostVC isMemberOfClass:[SWCommunicationVC class]]) {
        return YES;
    }
    return YES;
}


-(UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskPortrait;
}

#if 0
-(void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:SWOnLoginNotification object:nil];
}
#warning 自动登录无提示版,勿删
#else
#endif



#pragma mark -system object Delegate


#pragma mark -custom Delegate



#if 0
#warning 自动登录无提示版,勿删
-(void)onLogin:(NSNotification *)noti
{
     int ret =[[noti.object objectForKey:@"ret"] intValue];
    //应该用delegate而不是直接调用,一句话太low
     if(ret==0)
    {
        GetApp().GetUA()->UpdateTerminalInfo(0);
        
        GetApp().GetUA()->SendCommand(GetApp().GetUA()->GetSelf(), 121, 0);
    }
    else if (ret==1)
    {
        
    }
    else
    {
        //自动登录失败时弹出主界面
        SWLoginViewController *loginViewController = [[SWLoginViewController alloc] init];
        [self presentViewController:loginViewController animated:YES completion:nil];
    }
}
#else
#endif

#pragma mark -event respond


#pragma mark -private methods



#pragma mark -getter and setter methods
//所有的属性都使用getter和setter进行初始化
-(SWLeftViewController *)leftVC
{
    
    if (!_leftVC) {
        _leftVC = [[SWLeftViewController alloc] init];
        _leftVC.delegateSignal = [RACSubject subject];
        //点击leftView所在的cell,进入详情界面,返回时rightView在原点位置
        [_leftVC.delegateSignal subscribeNext:^(id x) {
            _rightVC.view.x=0;
        }];
        //初始位置
        _leftVC.view.frame=CGRectMake(0, 0,SWScreenWidth*leftViewScale, SWScreenHeight);
    }
    return _leftVC;
}

-(SWRightViewController *)rightVC
{
    if (!_rightVC) {
        _rightVC = [[SWRightViewController alloc] init];
        _rightVC.delegateSignal = [RACSubject subject];
        //rightView的返回按钮接受点击事件时,进行位置切换
        [_rightVC.delegateSignal subscribeNext:^(id x) {
            [UIView animateWithDuration:0.2 animations:^{
                _rightVC.view.x==0?(_rightVC.view.x=SWScreenWidth*leftViewScale):(_rightVC.view.x=0);
            }];
        }];

        //仿照QQ效果 rightView的移动速度是leftView的两倍
        [[_rightVC.view rac_valuesAndChangesForKeyPath:@"x" options:NSKeyValueObservingOptionNew observer:nil] subscribeNext:^(id x) {
            CGFloat xPoint =[[[x allObjects] firstObject] integerValue];
            self.leftVC.view.x=(xPoint-leftViewScale*SWScreenWidth)/2;
        }];
        
        //初始位置
        _rightVC.view.frame=CGRectMake(SWScreenWidth*leftViewScale, 0, SWScreenWidth, SWScreenHeight);
    }
    return _rightVC;
}

@end
