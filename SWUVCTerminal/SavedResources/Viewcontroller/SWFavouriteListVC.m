//
//  SWFavouriteListVC.m
//  SWUVCTerminal
//
//  Created by hxf on 8/15/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWFavouriteListVC.h"

@interface SWFavouriteListVC ()
@property (nonatomic, strong)  UIImageView *noDataView;

@end

@implementation SWFavouriteListVC

#pragma mark -life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setUpNavTheme];
    
    [self.view addSubview:self.noDataView];
}

-(void)viewDidLayoutSubviews
{
    [super viewDidLayoutSubviews];
    
    self.noDataView.center=self.view.center;
    
}

-(void)setUpNavTheme
{
    [self.navigationController.navigationBar setBackgroundImage:[UIImage imageNamed:@"navigationBar"] forBarMetrics:UIBarMetricsDefault];
    
    UIButton *backButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [backButton setImage:[UIImage imageNamed:@"back"] forState:UIControlStateNormal];
    [backButton sizeToFit];
    backButton.contentEdgeInsets = UIEdgeInsetsMake(0, -20, 0, 0);
    
    [[backButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
        [self.navigationController dismissViewControllerAnimated:NO completion:nil];
    }];
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:backButton];
}

#pragma mark -system object Delegate


#pragma mark -custom Delegate

#pragma mark -event respond


#pragma mark -private methods



#pragma mark -getter
- (UIImageView *)noDataView
{
    if (!_noDataView) {
        UIImageView *noDataView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"nodataTips"]];
        noDataView.frame=CGRectMake(0, 0, 124, 107);
        self.noDataView = noDataView;
    }
    return _noDataView;
}



@end
