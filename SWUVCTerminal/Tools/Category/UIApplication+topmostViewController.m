//
//  UIApplication+topmostViewController.m
//  SWUVCTerminal
//
//  Created by hxf on 9/6/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "UIApplication+topmostViewController.h"

@implementation UIApplication (topmostViewController)
+ (UIViewController *)topmostViewController {
    UIWindow *keyWindow = [UIApplication sharedApplication].keyWindow;
    UIViewController *vc = keyWindow.rootViewController;
    while (vc.presentedViewController) {
        vc = vc.presentedViewController;
        
        if ([vc isKindOfClass:[UINavigationController class]]) {
            vc = [(UINavigationController *)vc visibleViewController];
        } else if ([vc isKindOfClass:[UITabBarController class]]) {
            vc = [(UITabBarController *)vc selectedViewController];
        }
    }
    return vc;
}
@end
