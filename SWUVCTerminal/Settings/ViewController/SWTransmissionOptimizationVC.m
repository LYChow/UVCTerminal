//
//  SWTransmissionOptimizationVC.m
//  SWUVCTerminal
//
//  Created by hxf on 8/15/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWTransmissionOptimizationVC.h"
#import "SWTransmissionOptCell.h"
#import "SWPickView.h"
#import "SWSetting.h"

#define SWCellIdentifier @"SWCellIdentifier"

@interface SWTransmissionOptimizationVC ()<UITableViewDelegate,UITableViewDataSource>
#pragma mark -property
@property(nonatomic,strong) UITableView *tableView;
@property(nonatomic,strong) NSArray *settingTitleArray;
@property(nonatomic,strong) NSArray *settingIconArray;
@end

@implementation SWTransmissionOptimizationVC

#pragma mark -life cycle
- (void)viewDidLoad
{
    [super viewDidLoad];
    [self setUpNavTheme];
    [self.view addSubview:self.tableView];
}

-(void)setUpNavTheme
{
    self.title=@"传输优化";
    UIButton *backButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [backButton setImage:[UIImage imageNamed:@"back"] forState:UIControlStateNormal];
    [backButton sizeToFit];
    backButton.contentEdgeInsets = UIEdgeInsetsMake(0, -20, 0, 0);
    
    [[backButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
        [self.navigationController popViewControllerAnimated:YES];
    }];
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:backButton];
    
    [self.navigationController.navigationBar setBackgroundImage:[UIImage imageNamed:@"navigationBar"] forBarMetrics:UIBarMetricsDefault];
}

#pragma mark -system object Delegate
#pragma mark -UITableViewDataSource
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    SWSetting *setting =[SWSetting setting];
    
    SWTransmissionOptCell *cell = [tableView dequeueReusableCellWithIdentifier:SWCellIdentifier];
    cell.transmissionOptIcon.image=[UIImage imageNamed:[self.settingIconArray objectAtIndex:indexPath.row]];
    cell.transmissionOptTitle.text=[self.settingTitleArray objectAtIndex:indexPath.row];
    
    if (indexPath.row==0)
    {
        cell.currentSettingLabel.text=[[lossPacketRecoveryArray lastObject] objectAtIndex:[setting.lostPacketRecovery integerValue]];
    }
    else if (indexPath.row==1)
    {
        cell.currentSettingLabel.text=[[smoothSendArray lastObject] objectAtIndex:[setting.smoothSend integerValue]];
    }
    return cell;
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [self.settingTitleArray count];
}

#pragma mark -UITableViewDelegate

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return SWCellHeight;
}

-(CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
{
    return 1;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
        SWSetting *setting =[SWSetting setting];

        SWPickView *pickView = [SWPickView pickView];
        
         pickView.delegateSignal = [RACSubject subject];
        [pickView.delegateSignal subscribeNext:^(NSDictionary *dict) {

            NSString *selectedTitle = [dict objectForKey:@"selectedTitle"];
            if (indexPath.row==0)
            {
                if ([[lossPacketRecoveryArray lastObject] containsObject:selectedTitle]) {
                    setting.lostPacketRecovery=[NSNumber numberWithInteger:[[lossPacketRecoveryArray lastObject] indexOfObject:selectedTitle]];
                }
            }
            else if (indexPath.row==1)
            {
                if ([[smoothSendArray lastObject] containsObject:selectedTitle]) {
                    setting.smoothSend=[NSNumber numberWithInteger:[[smoothSendArray lastObject] indexOfObject:selectedTitle]];
                }
            }
            
            [setting save];
            [self.tableView reloadData];
            
        }];
        
    if (indexPath.row == 0)
    {
        [pickView setPickViewType:SWPickViewLossPacketRecoveryType defaultSelectedTitle:[[lossPacketRecoveryArray lastObject] objectAtIndex:[setting.lostPacketRecovery integerValue]]];
    }
    else if (indexPath.row == 1)
    {
        [pickView setPickViewType:SWPickViewSmoothSendType defaultSelectedTitle:[[smoothSendArray lastObject] objectAtIndex:[setting.smoothSend integerValue]]];
    }
        
    
    [pickView show];

}

#pragma mark -custom Delegate

#pragma mark -event respond


#pragma mark -private methods



#pragma mark -getter and setter methods
-(UITableView *)tableView
{
    if (!_tableView) {
        _tableView  =[[UITableView alloc] initWithFrame:CGRectMake(0,0, SWScreenWidth, SWScreenHeight-64) style:UITableViewStylePlain];
        _tableView.delegate=self;
        _tableView.dataSource=self;
        
        
        [_tableView registerNib:[UINib nibWithNibName:NSStringFromClass([SWTransmissionOptCell class]) bundle:nil] forCellReuseIdentifier:SWCellIdentifier];
    }
    return _tableView;
}

-(NSArray *)settingTitleArray
{
    if (!_settingTitleArray) {
        _settingTitleArray = [NSArray arrayWithObjects:@"丢包恢复",@"平滑发送",nil];
    }
    return _settingTitleArray;
}

-(NSArray *)settingIconArray
{
    if (!_settingIconArray) {
        _settingIconArray = [NSArray arrayWithObjects:@"update",@"about",nil];
    }
    return _settingIconArray;
}

@end
