//
//  SWAboutVC.m
//  SWUVCTerminal
//
//  Created by hxf on 8/15/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWAboutVC.h"

@interface SWAboutVC ()

@end

@implementation SWAboutVC

#pragma mark -life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setUpNavTheme];
}


-(void)setUpNavTheme
{
    [self.navigationController.navigationBar setBackgroundImage:[UIImage imageNamed:@"navigationBar"] forBarMetrics:UIBarMetricsDefault];
    
    self.title=@"关于";
    UIButton *backButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [backButton setImage:[UIImage imageNamed:@"back"] forState:UIControlStateNormal];
    [backButton sizeToFit];
    backButton.contentEdgeInsets = UIEdgeInsetsMake(0, -20, 0, 0);
    
    [[backButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
        [self.navigationController popViewControllerAnimated:YES];
    }];
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:backButton];
}


#pragma mark -system object Delegate


#pragma mark -custom Delegate

#pragma mark -event respond

#warning 拨打电话
//-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
//    if (buttonIndex == 1) {
//        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"tel://4006-135-135"]];
// [[UIApplication sharedApplication] openURL:[NSURL URLWithString:_appDownloadPath]];
//    }
//}
#pragma mark -private methods



#pragma mark -getter and setter methods
//所有的属性都使用getter和setter进行初始化


@end
