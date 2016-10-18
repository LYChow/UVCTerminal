//
//  SWTerminalListVC.m
//  SWUVCTerminal
//
//  Created by hxf on 8/22/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWTerminalListVC.h"
#import "SWCommunicationCell.h"
#import "SWCommunicationVC.h"
#import "SWTalkBackVC.h"
#import "SWJoinMeetingVC.h"
#import "BMChineseSort.h"
#import "SWTerminal.h"
#import "UIImage+resizableImage.h"
#define SWCellIdentifier @"SWCellIdentifier"
@interface SWTerminalListVC ()<UITableViewDelegate,UITableViewDataSource,UISearchBarDelegate>

#pragma mark -propertyType

@property(nonatomic,strong)UITableView *communicationTableView;
@property(nonatomic,strong)UIView *searchBarView;

@property(nonatomic,strong) NSMutableArray *totalArray; //存放所有的terminal对象
@property(nonatomic,strong) NSArray *indexArray;  //索引数组
@property(nonatomic,strong) NSArray *letterResultArray;  //每个索引元素对应的数组
@property (nonatomic, strong)  UIImageView *noDataView;


@end

@implementation SWTerminalListVC

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setUpNavTheme];
    [self loadDataFiterWithKeyWord:nil];

    [self.view addSubview:self.searchBarView];
    [self.view addSubview:self.communicationTableView];
    [self.view addSubview:self.noDataView];
}

//simulation request MVS Server Data
-(void)loadDataFiterWithKeyWord:(NSString *)keyWord
{
    NSArray *requestArray = [NSArray arrayWithObjects:@{@"name":@"zly1",@"ipAddress":@"192.168.2.112",@"type":@"1"}, @{@"name":@"廊坊市文化路",@"ipAddress":@"211.99.241.243",@"type":@"2"},@{@"name":@"zly",@"ipAddress":@"192.168.2.112",@"type":@"1"},@{@"name":@"xf",@"ipAddress":@"192.168.2.112",@"type":@"2"},@{@"name":@"WFF",@"ipAddress":@"192.168.2.112",@"type":@"1"},@{@"name":@"fwy3",@"ipAddress":@"192.168.2.112",@"type":@"1"},@{@"name":@"Vivo9528_v5",@"ipAddress":@"192.168.2.112",@"type":@"2"},@{@"name":@"lc292112",@"ipAddress":@"192.168.2.112",@"type":@"1"},@{@"name":@"wyq",@"ipAddress":@"211.99.241.243",@"type":@"2"},@{@"name":@"Smart022",@"ipAddress":@"192.168.2.112",@"type":@"1"},@{@"name":@"fwy5",@"ipAddress":@"192.168.2.112",@"type":@"2"},nil];

    
    [self.totalArray removeAllObjects];
    for (int i = 0; i<requestArray.count; i++) {
        SWTerminal *terminal = [[SWTerminal alloc] init];
        
        NSDictionary *terminalInfo =[requestArray objectAtIndex:i];
        terminal.name = [terminalInfo objectForKey:@"name"];
        terminal.ipAddress= [terminalInfo objectForKey:@"ipAddress"];
        terminal.type = [terminalInfo objectForKey:@"type"];
        [self.totalArray addObject:terminal];
        
    }
    
    if (keyWord.length) {
       //predicate key pharse
        NSPredicate *predicate =[NSPredicate predicateWithFormat:@"name CONTAINS %@",keyWord];
       [self.totalArray filterUsingPredicate:predicate];
    }
    
    self.indexArray = [BMChineseSort IndexWithArray:self.totalArray Key:@"name"];
    self.letterResultArray = [BMChineseSort sortObjectArray:self.totalArray Key:@"name"];
    
    [self.communicationTableView reloadData];
}


-(void)setUpNavTheme
{
    [self.navigationController.navigationBar setBackgroundImage:[UIImage imageNamed:@"navigationBar"] forBarMetrics:UIBarMetricsDefault];
    
    UIButton *backButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [backButton setImage:[UIImage imageNamed:@"back"] forState:UIControlStateNormal];
    [backButton sizeToFit];
    backButton.contentEdgeInsets = UIEdgeInsetsMake(0, -20, 0, 0);
    
    [[backButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
        [self dismissViewControllerAnimated:YES completion:nil];
    }];
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:backButton];
    
    
    UIButton *refreshButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [refreshButton setImage:[UIImage imageNamed:@"refresh"] forState:UIControlStateNormal];
    [refreshButton sizeToFit];
    refreshButton.contentEdgeInsets = UIEdgeInsetsMake(0, 0, 0, 0);
    [[refreshButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
        [MBProgressHUD showMessage:@"正在拉取数据..."];
        
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [MBProgressHUD hideHUD];
        });
    }];
    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:refreshButton];
}

#pragma mark -system object Delegate

#pragma mark -UISearchBarDelegate
- (void)searchBar:(UISearchBar *)searchBar textDidChange:(NSString *)searchText
{
    [self loadDataFiterWithKeyWord:searchText];
}

#pragma mark -UITableViewDataSource

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    SWTerminal *teminal =[[self.letterResultArray objectAtIndex:indexPath.section] objectAtIndex:indexPath.row];
    SWCommunicationCell *cell = [tableView dequeueReusableCellWithIdentifier:SWCellIdentifier];
    cell.terminalModel=teminal;
    if(_communicationType==SWMeetingCommunicationType)
    {
        cell.terminalTypeImage.image=[UIImage imageNamed:@"meetingIcon"];
    }
    return cell;
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [[self.letterResultArray objectAtIndex:section] count];
}

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    self.noDataView.hidden=self.indexArray.count;
    return self.indexArray.count;
}

- (NSArray *)sectionIndexTitlesForTableView:(UITableView *)tableView
{
    return self.indexArray;
}

-(NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    return [self.indexArray objectAtIndex:section];
}


#pragma mark -UITableViewDelegate
-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return SWCellHeight;
}

-(CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    return 20;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    SWTerminal *terminalModel =[[self.letterResultArray objectAtIndex:indexPath.section] objectAtIndex:indexPath.row];
    
    
    if ([terminalModel.type isEqualToString:@"1"])
    {
        //SWTalkBackCommunicationType
        SWTerminal *terminal =[[self.letterResultArray objectAtIndex:indexPath.section] objectAtIndex:indexPath.row];
        SWTalkBackVC *talkBackVC =[[SWTalkBackVC alloc] init];
        talkBackVC.peerTerminalName=terminal.name;
        [self.navigationController pushViewController:talkBackVC animated:YES];
    }else if ([terminalModel.type isEqualToString:@"2"])
    {
        //SWMonitorCommunicationType
        SWCommunicationVC *communicationVC =[[SWCommunicationVC alloc] init];
        communicationVC.mainViewType=SWVideoMainViewType;
        communicationVC.videoType=SWMonitorVideoType;
        
        [self presentViewController:communicationVC animated:NO completion:nil];

    }else if ([terminalModel.type isEqualToString:@"3"])
    {
        //SWMeetingCommunicationType
        SWJoinMeetingVC *joinMeetingVC = [[SWJoinMeetingVC alloc] init];
        [self.navigationController pushViewController:joinMeetingVC animated:YES];
    }
}

#pragma mark -custom Delegate

#pragma mark -event respond


#pragma mark -private methods


#pragma mark setter methods

-(void)setCommunicationType:(SWCommunicationType)type
{
    _communicationType = type;
    
    
    if (_communicationType==SWMeetingCommunicationType)
    {
        self.title=@"选择参加会议";
    }
    else
    {
        self.title=@"选择对讲成员";
    }
}

#pragma mark getter methods
- (UIImageView *)noDataView
{
    if (!_noDataView) {
        UIImageView *noDataView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"nodataTips"]];
        noDataView.frame=CGRectMake(0, 0, 124, 107);
        noDataView.center=self.view.center;
        self.noDataView = noDataView;
    }
    return _noDataView;
}



-(UITableView *)communicationTableView
{
    if (!_communicationTableView) {
        _communicationTableView  =[[UITableView alloc] initWithFrame:CGRectMake(0,CGRectGetMaxY(self.searchBarView.frame), SWScreenWidth, SWScreenHeight-(64+CGRectGetMaxY(self.searchBarView.frame))) style:UITableViewStyleGrouped];
        _communicationTableView.delegate=self;
        _communicationTableView.dataSource=self;
        
        [_communicationTableView registerNib:[UINib nibWithNibName:NSStringFromClass([SWCommunicationCell class]) bundle:nil] forCellReuseIdentifier:SWCellIdentifier];
        
    }
    return _communicationTableView;
}

-(UIView *)searchBarView
{
    if (!_searchBarView) {
        _searchBarView =[[UIView alloc] initWithFrame:CGRectMake(0, 0, SWScreenWidth, 44)];
        _searchBarView.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"navigationBar"]];
        
        UISearchBar *searchBar = [[UISearchBar alloc] initWithFrame:CGRectMake(20, (_searchBarView.height-32)/2, SWScreenWidth-40, 32)];
        //保护部分区域进行拉伸
        searchBar.backgroundImage = [UIImage resizableImageNamed:@"searchBar_bg"];
        searchBar.delegate = self;
        
        [searchBar setPlaceholder:@"请输入关键字"];
        UIImageView *searImage = [[UIImageView alloc]initWithImage:[UIImage imageNamed:@"Search_Icon_Copy"]];
        searImage.frame = CGRectMake(0, 0, 14, 13);
        [searchBar setImage:searImage.image forSearchBarIcon:UISearchBarIconSearch state:0];
        
        UITextField *searchField = [searchBar valueForKey:@"_searchField"];
        searchField.borderStyle = UITextBorderStyleNone;
        searchField.layer.borderWidth = 0.5;
        searchField.layer.borderColor = SWColorA(255, 255, 255, 0.8).CGColor;
        searchField.layer.cornerRadius = 15;
        searchField.layer.masksToBounds = YES;
        //    改变搜索框文字颜色
        UIColor *color = SWColorA(215, 215, 215, 1);
        [searchField setValue:color  forKeyPath:@"_placeholderLabel.textColor"];
        searchField.backgroundColor = SWColorA(255, 255, 255, 0.8);
        
        [_searchBarView addSubview:searchBar];
    }
    return _searchBarView;
}

-(NSMutableArray *)totalArray
{
    if (!_totalArray) {
        _totalArray = [NSMutableArray array];
    }
    return _totalArray;
}
@end
