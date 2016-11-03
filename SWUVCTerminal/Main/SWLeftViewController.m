//
//  SWLeftViewController.m
//  SWUVCTerminal
//
//  Created by hxf on 8/9/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWLeftViewController.h"
#import "SWLeftTableViewCell.h"

#import "SWImageListVC.h"
#import "SWAudioVideoListVC.h"
#import "SWFavouriteListVC.h"
#import "SWSettingVC.h"
#import "AppDelegate.h"

#import "SWLoginViewController.h"
#import "SWAppDemo.h"
#import "SWNavigationController.h"
#import "SWUser.h"
#define SWCellIdentifier @"SWCellIdentifier"
@interface SWLeftViewController ()<UITableViewDelegate,UITableViewDataSource,UIAlertViewDelegate>

#pragma mark -property
@property(nonatomic,strong) UIImageView *navgationBarView;
@property(nonatomic,strong) UIImageView *avatarImageView;
@property(nonatomic,strong) UITableView *tableView;
@property(nonatomic,strong) UIButton *logoutButton;
@property(nonatomic,strong) NSArray *titleArray;
@property(nonatomic,strong) NSArray *iconArray;

@end

@implementation SWLeftViewController

#pragma mark -life cycle


- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor =[UIColor colorWithPatternImage:[UIImage imageNamed:@"loginLogo"]];
    
    NSArray *firstSectionTitleArray =[NSArray arrayWithObjects:@"picture.png",@"video.png",@"record.png",@"favourite.png", nil];
    NSArray *secondSectionTitleArray = [NSArray arrayWithObjects:@"setting.png", nil];
    self.iconArray =[NSArray arrayWithObjects:firstSectionTitleArray,secondSectionTitleArray,nil];
    
    NSArray *firstSectionIconArray =[NSArray arrayWithObjects:@"图片",@"视频",@"录音",@"收藏", nil];
    NSArray *secondSectionIconArray = [NSArray arrayWithObjects:@"设置", nil];
    self.titleArray=[NSArray arrayWithObjects:firstSectionIconArray,secondSectionIconArray, nil];
    
    [self.view addSubview:self.navgationBarView];
    [self.navgationBarView addSubview:self.avatarImageView];
    [self.view addSubview:self.tableView];
    [self.view addSubview:self.logoutButton];
    [[self.logoutButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"退出登录" message:@"你确定要退出登录吗" delegate:self cancelButtonTitle:@"取消" otherButtonTitles:@"确定", nil];
        [alertView show];
    }];
}

-(void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    //弹出的Nav消失时,rightView.x =0
    [self.delegateSignal sendNext:nil];
}

-(void)viewDidLayoutSubviews
{
    
    self.navgationBarView.frame=CGRectMake(0, 0,  SWScreenWidth*leftViewScale, 64);
    self.tableView.frame=CGRectMake(0, 64, SWScreenWidth*leftViewScale, SWScreenHeight-(64+CGRectGetMaxY(self.navgationBarView.frame)));
    self.logoutButton.frame= CGRectMake((SWScreenWidth*leftViewScale-self.logoutButton.width)/2, SWScreenHeight-60, SWScreenWidth*leftViewScale-80, 40);
}

-(void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:SWChangeUserAvatarNotification object:nil];
}

#pragma mark -system object Delegate

#pragma mark -UITableViewDataSource
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    SWLeftTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:SWCellIdentifier];
    cell.cellImageView.image=[UIImage imageNamed:[[self.iconArray objectAtIndex:indexPath.section] objectAtIndex:indexPath.row]];
    cell.cellTitleLabel.text=[[self.titleArray objectAtIndex:indexPath.section] objectAtIndex:indexPath.row];
    return cell;

}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [[self.titleArray objectAtIndex:section] count];
}

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return self.titleArray.count;
}

#pragma mark -UITableViewDelegate

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return SWCellHeight;
}

-(CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
{
    return 30;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section==0) {
        switch (indexPath.row) {
            case 0:
            {
                SWImageListVC  *imageVC = [[SWImageListVC alloc] init];
                [self presentViewController:[[SWNavigationController alloc]initWithRootViewController:imageVC] animated:YES completion:nil];
            }
                break;
            case 1:
            {
                SWAudioVideoListVC  *videoVC = [[SWAudioVideoListVC alloc] init];
                videoVC.mediaType=SWVideoMeidaType;
                [self presentViewController:[[SWNavigationController alloc]initWithRootViewController:videoVC] animated:YES completion:nil];
            }
                break;
            case 2:
            {
                SWAudioVideoListVC  *audioVC = [[SWAudioVideoListVC alloc] init];
                audioVC.mediaType=SWAudioMediaType;
                [self presentViewController:[[SWNavigationController alloc]initWithRootViewController:audioVC] animated:YES completion:nil];
            }
                break;
            case 3:
            {
                SWFavouriteListVC  *favouriteVC = [[SWFavouriteListVC alloc] init];
                [self presentViewController:[[SWNavigationController alloc]initWithRootViewController:favouriteVC] animated:YES completion:nil];
            }
                break;
            default:
                break;
        }
        
    }
    else
    {
        SWSettingVC  *settingVC = [[SWSettingVC alloc] init];
        [self presentViewController:[[SWNavigationController alloc]initWithRootViewController:settingVC] animated:YES completion:nil];
    }
}

#pragma -mark UIAlertViewDelegate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex==1) {
        //切换到登录界面 发送信令到MVS退出登录
        GetApp().GetUA()->Logout();
        
        SWLoginViewController *loginVC = [[SWLoginViewController alloc] init];
        loginVC.isAutoLogin=NO;
        AppDelegate *app=(AppDelegate *)[UIApplication sharedApplication].delegate;
        [app.window.rootViewController presentViewController:loginVC animated:YES completion:nil];
    }
}


#pragma mark -custom Delegate


#pragma mark -custom NSNotification
-(void)changeUserAvatar:(NSNotification *)noti
{
    UIImage *avatar = [UIImage imageWithContentsOfFile:[SWDocumentPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@_avatar.png",[SWUser user].userName]]];
    _avatarImageView.image=avatar?avatar:[UIImage imageNamed:@"avatar"];
}

#pragma mark -event respond


#pragma mark -private methods



#pragma mark -getter and setter methods
-(UIImageView *)navgationBarView
{
    if (!_navgationBarView) {
        _navgationBarView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, SWScreenWidth*leftViewScale, 64)];
        _navgationBarView.image= [UIImage imageNamed:@"navigationBar"];
    }
    return _navgationBarView;
}

-(UITableView *)tableView
{
    if (!_tableView) {
        _tableView  =[[UITableView alloc] initWithFrame:CGRectMake(0,CGRectGetMaxY(self.navgationBarView.frame), SWScreenWidth*leftViewScale, SWScreenHeight-(64+CGRectGetMaxY(self.navgationBarView.frame))) style:UITableViewStylePlain];
        _tableView.delegate=self;
        _tableView.dataSource=self;
        
        
        [_tableView registerNib:[UINib nibWithNibName:NSStringFromClass([SWLeftTableViewCell class]) bundle:nil] forCellReuseIdentifier:SWCellIdentifier];
    }
    return _tableView;
}

-(UIButton *)logoutButton
{
    if (!_logoutButton) {
        _logoutButton  = [UIButton buttonWithType:UIButtonTypeCustom];
        _logoutButton.frame=CGRectMake(0, 0, SWScreenWidth*leftViewScale-80, 40);
        _logoutButton.layer.borderColor=[UIColor grayColor].CGColor;
        _logoutButton.layer.borderWidth=1.0;
        _logoutButton.layer.cornerRadius=8;
        [_logoutButton setTitle:@"退出登录" forState:UIControlStateNormal];
        [_logoutButton setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
    }
    return _logoutButton;
}

-(UIImageView *)avatarImageView
{
    if (!_avatarImageView) {
        
        UIImage *avatar = [UIImage imageWithContentsOfFile:[SWDocumentPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@_avatar.png",[SWUser user].userName]]];
        _avatarImageView = [[UIImageView alloc] initWithImage:avatar?avatar:[UIImage imageNamed:@"avatar"]];
        _avatarImageView.frame  =CGRectMake(10, 25, 34, 34);
        _avatarImageView.layer.cornerRadius=_avatarImageView.height/2;
        _avatarImageView.clipsToBounds=YES;
        
        [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(changeUserAvatar:) name:SWChangeUserAvatarNotification object:nil];
        
    }
    return _avatarImageView;
}

@end
