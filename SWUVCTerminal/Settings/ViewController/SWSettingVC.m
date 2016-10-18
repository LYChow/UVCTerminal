//
//  SWSettingVC.m
//  SWUVCTerminal
//
//  Created by hxf on 8/15/16.
//  Copyright © 2016 sinowave. All rights reserved.


#import "SWSettingVC.h"
#import "SWSettingCell.h"
#import "SWAboutVC.h"
#import "SWTransmissionOptimizationVC.h"
#import "SWPickView.h"
#import "UIView+Toast.h"
#define SWCellIdentifier @"SWCellIdentifier"

@interface SWSettingVC ()<UITableViewDelegate,UITableViewDataSource,UIAlertViewDelegate>

#pragma mark -property
@property(nonatomic,strong) UITableView *tableView;
@property(nonatomic,strong) NSArray *settingTitleArray;
@property(nonatomic,strong) NSArray *settingIconArray;
@end

@implementation SWSettingVC



#pragma mark -life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setUpNavTheme];
    [self.view addSubview:self.tableView];
}

-(void)setUpNavTheme
{
    [self.navigationController.navigationBar setBackgroundImage:[UIImage imageNamed:@"navigationBar"] forBarMetrics:UIBarMetricsDefault];
    
    self.title=@"设置";
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
#pragma mark -UITableViewDataSource
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    
    SWSetting *setting =[SWSetting setting];
    SWSettingCell *cell = [tableView dequeueReusableCellWithIdentifier:SWCellIdentifier];
    cell.settingIcon.image=[UIImage imageNamed:[[self.settingIconArray objectAtIndex:indexPath.section] objectAtIndex:indexPath.row]];
    cell.settingTitle.text=[[self.settingTitleArray objectAtIndex:indexPath.section] objectAtIndex:indexPath.row];
    
    
    cell.settingSwitch.hidden=indexPath.section>2;
    cell.currentSettingLabel.hidden=!(indexPath.section==3 && indexPath.row<2);
    if (indexPath.section==3 && indexPath.row==0) {
        cell.currentSettingLabel.text=[[audioChannelArray lastObject] objectAtIndex:[setting.audioChannel integerValue]];
    }
    if (indexPath.section==3 && indexPath.row==1) {
        cell.currentSettingLabel.text=[[videoFluncyArray lastObject] objectAtIndex:[setting.videoFluency integerValue]];
    }
    
    //显示状态
    switch (indexPath.section) {
        case 0:
            [cell.settingSwitch setOn:[setting.isAutoLogin boolValue]];
            break;
        case 1:
            [cell.settingSwitch setOn:[setting.isAutoAccept boolValue]];
            break;
        case 2:
            [cell.settingSwitch setOn:[setting.isColorFix boolValue]];
            break;
        default:
            break;
    }
    return cell;
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [[self.settingTitleArray objectAtIndex:section] count];
}

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return self.settingTitleArray.count;
}

#pragma mark -UITableViewDelegate
-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return SWCellHeight;
}

-(CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
{
    return 10;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    SWSetting *setting =[SWSetting setting];
    
    if (indexPath.section==3) {
        switch (indexPath.row)
        {
            case 0:
            {
  
                    SWPickView *pickView = [SWPickView pickView];
                    pickView.delegateSignal = [RACSubject subject];
                    [pickView.delegateSignal subscribeNext:^(NSDictionary *dict) {
                        
                        //存入到setting中
                        NSString *selectedTitle = [dict objectForKey:@"selectedTitle"];
                        if ([[audioChannelArray lastObject] containsObject:selectedTitle])
                        {
                            setting.audioChannel=[NSNumber numberWithInteger:[[audioChannelArray lastObject] indexOfObject:selectedTitle]];
                        }
                        [setting save];
                        //刷新tableView
                        [self.tableView reloadData];
                        
                    }];
                    
                    
                    
                    [pickView setPickViewType:SWPickViewAudioChannelType defaultSelectedTitle:[[audioChannelArray lastObject] objectAtIndex:[setting.audioChannel integerValue]]];
                    [pickView show];
                    
                }
                break;
            case 1:
            {
                SWPickView *pickView = [SWPickView pickView];
                pickView.delegateSignal = [RACSubject subject];
                [pickView.delegateSignal subscribeNext:^(NSDictionary *dict) {
                    
                    //存入到setting中
                    NSString *selectedTitle = [dict objectForKey:@"selectedTitle"];
                    if ([[videoFluncyArray lastObject] containsObject:selectedTitle])
                    {
                        setting.videoFluency=[NSNumber numberWithInteger:[[videoFluncyArray lastObject] indexOfObject:selectedTitle]];
                    }
                    [setting save];
                    //刷新tableView
                    [self.tableView reloadData];
                }];
                
                [pickView setPickViewType:SWPickViewVideoFluncyType defaultSelectedTitle:[[videoFluncyArray lastObject] objectAtIndex:[setting.videoFluency integerValue]]];
                [pickView show];
            }
                break;
            case 2:
            {
                //传输优化 a.丢包恢复(1.不启用2.1%~25%) b.平滑发送(1.不启用 2. 1.25倍 3. 1.5倍  4. 2倍 5.4倍)
                SWTransmissionOptimizationVC *transmissionOptmizationVC =[[SWTransmissionOptimizationVC alloc] init];
                [self.navigationController pushViewController:transmissionOptmizationVC animated:YES];
            }
                break;
            default:
                break;
        }
    }
    else if(indexPath.section == 4)
    {
      
        switch (indexPath.row) {
            case 0:
            {
                //更新 有则跳转 无则提示
                NSString *lastVersion =@"1.0";
                // app版本
                NSString *app_Version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
                
                if ([app_Version isEqualToString:lastVersion]) {
                    [self.tableView makeToast:@"暂无最新版本"];
                    
                }
                else
                {
                    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"更新提示" message:@"发现最新版本可供更新" delegate:self cancelButtonTitle:@"取消" otherButtonTitles:@"现在更新", nil];
                    [alertView show];
                }
            }
                break;
            case 1:
            {
                //关于
                SWAboutVC *aboutVC =[[SWAboutVC alloc] initWithNibName:@"SWAboutVC" bundle:nil];
                [self.navigationController pushViewController:aboutVC animated:YES];
            }
                break;
            default:
                break;
        }

    }
}

#pragma mark  -UIAlertViewDelegate
-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == 1) {
        NSString *appUrl = @"https://itunes.apple.com/app/apple-store/id566752651?mt=8";
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:appUrl]];
    }

}

#pragma mark -custom Delegate

#pragma mark -event respond
//button、gestureRecognizer的响应事件都放在这个区域里面

#pragma mark -private methods
//private methods一般是用于日期换算、图片裁剪啥的这种小功能,小功能要么把它写成一个category，要么把他做成一个模块，哪怕这个模块只有一个函数也行。//dry独立出来,代码复用


#pragma mark -getter and setter methods
//所有的属性都使用getter和setter进行初始化
-(UITableView *)tableView
{
    if (!_tableView) {
        _tableView  =[[UITableView alloc] initWithFrame:CGRectMake(0,0, SWScreenWidth, SWScreenHeight-64) style:UITableViewStylePlain];
        _tableView.delegate=self;
        _tableView.dataSource=self;
        [_tableView registerNib:[UINib nibWithNibName:NSStringFromClass([SWSettingCell class]) bundle:nil] forCellReuseIdentifier:SWCellIdentifier];
    }
    return _tableView;
}

-(NSArray *)settingTitleArray
{
    if (!_settingTitleArray) {
        _settingTitleArray = [NSArray arrayWithObjects:@[@"自动登录"],@[@"自动应答"],@[@"色彩修正"],@[@"音频通道",@"流畅度",@"传输优化"],@[@"更新",@"关于"],nil];
    }
    return _settingTitleArray;
}

-(NSArray *)settingIconArray
{
    if (!_settingIconArray) {
        _settingIconArray = [NSArray arrayWithObjects:@[@"login"],@[@"answer"],@[@"color"],@[@"voice",@"screen",@"transfer"],@[@"update",@"about"],nil];
    }
    return _settingIconArray;
}

@end
