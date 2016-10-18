//
//  SWNavigationController.m
//  SWUVCTerminal
//
//  Created by hxf on 8/31/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWNavigationController.h"

@interface SWNavigationController ()

@end

@implementation SWNavigationController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // 设置导航控制器为手势识别器的代理
}

+(void)initialize{
    
    UINavigationBar *navBar = [UINavigationBar appearance
                               ];
    [navBar setBackgroundImage:[UIImage imageNamed:@"navigationBar"] forBarMetrics:UIBarMetricsDefault];
    
    // 2.设置栏的字体
    NSMutableDictionary *att = [NSMutableDictionary dictionary];
    att[NSForegroundColorAttributeName] = [UIColor whiteColor];
    att[NSFontAttributeName] = [UIFont systemFontOfSize:20];
    
    [navBar setTitleTextAttributes:att];
    
    [UIApplication sharedApplication].statusBarStyle = UIStatusBarStyleLightContent;

    navBar.translucent = NO;
}


-(BOOL)shouldAutorotate
{
    return NO;
}

-(UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskPortrait;
}
@end
