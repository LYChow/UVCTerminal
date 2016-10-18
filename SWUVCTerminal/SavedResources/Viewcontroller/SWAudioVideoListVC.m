//
//  SWAudioVideoListVC.m
//  SWUVCTerminal
//
//  Created by hxf on 9/23/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWAudioVideoListVC.h"
#import "SWAudioVideoCell.h"
#import "SWVideo.h"
#import "SWAudio.h"
#import <MediaPlayer/MediaPlayer.h>
#import "SWBottomView.h"
#import "SWFileManager.h"

NSString *const SWDone = @"完成";
NSString *const SWEdit = @"编辑";

#define SWCellIdentifier @"SWCellIdentifier"

@interface SWAudioVideoListVC ()<UITableViewDelegate,UITableViewDataSource,SWBottomViewDelegate,SWAVCellDelegate>


#pragma mark -property
@property(nonatomic,strong) UITableView *tableView;

@property (nonatomic, strong) SWBottomView *bottomView;
@property (nonatomic, strong) NSMutableArray *videos;
@property (nonatomic, strong) NSMutableArray *audios;
@property (nonatomic, strong)  UIImageView *noDataView;
@property (nonatomic, strong) UIBarButtonItem *editButtonItem;


@end

@implementation SWAudioVideoListVC

#pragma mark -life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title=_mediaType==SWVideoMeidaType?@"视频":@"录音";
    [self setUpNavTheme];
    
    [self.view addSubview:self.tableView];
    
    [self.view addSubview:self.noDataView];
    
    [self refreshData];
    
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
    
    
    UIButton *editButton =[[UIButton alloc] initWithFrame:CGRectMake(0, 0, 60, 30)];
    [editButton setTitle:SWEdit forState:UIControlStateNormal];
    [editButton addTarget:self action:@selector(edit:) forControlEvents:UIControlEventTouchUpInside];
    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:editButton];
}


#pragma mark -system object Delegate

#pragma mark -UITableViewDataSource
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    SWAudioVideoCell *cell = [tableView dequeueReusableCellWithIdentifier:SWCellIdentifier];
    cell.delegate =self;
    
    if (_mediaType == SWVideoMeidaType)
    {
        cell.video=[self.videos objectAtIndex:indexPath.row];
    }
    else
    {
        cell.audio=[self.audios objectAtIndex:indexPath.row];
    }
    return cell;
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    NSInteger numberOfRow = (_mediaType==SWVideoMeidaType)?self.videos.count:self.audios.count;
    
    self.navigationItem.rightBarButtonItem=numberOfRow?self.editButtonItem:nil;
    self.noDataView.hidden=numberOfRow;
    
    //数据清空时隐藏bottomView
    if (_bottomView && numberOfRow==0) {
        [_bottomView removeFromSuperview];
        self.tableView.height = SWScreenHeight-64;
    }
    
    return  numberOfRow;
}

#pragma mark -UITableViewDelegate

-(CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
{
    return 1;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 88;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (_mediaType == SWAudioMediaType)
    {
        SWAudio *audio =[self.audios objectAtIndex:indexPath.row];
        NSString *filePath =  audio.filePath;
        NSURL *url =  [NSURL fileURLWithPath:filePath];
        MPMoviePlayerViewController *playerVc = [[MPMoviePlayerViewController alloc] initWithContentURL:url];
        [self presentViewController:playerVc animated:YES completion:nil];

    }
    else
    {
        SWVideo *video =[self.videos objectAtIndex:indexPath.row];
        NSString *filePath = video.filePath;
        NSURL *url =  [NSURL fileURLWithPath:filePath];
        MPMoviePlayerViewController *playerVc = [[MPMoviePlayerViewController alloc] initWithContentURL:url];
        [self presentViewController:playerVc animated:YES completion:nil];
    }
}


#pragma mark -custom Delegate

#pragma mark -SWBottomViewDelegate (selectedAll/unSelectecAll/Delete)
-(void)callBackEventType:(SWEventType)eventType
{
    if (_mediaType==SWAudioMediaType)
    {
        switch (eventType) {
            case SWEventTypeUnselected:
                for (SWAudio *audio in self.audios) {
                    audio.checking = NO;
                }
                break;
            case SWEventTypeSelected:
                for (SWAudio *audio in self.audios) {
                    audio.checking = YES;
                }
                break;
            case SWEventTypeDelete:
            {
                //Enumeration all selected Model
                NSMutableArray *tempArray = [NSMutableArray array];
                for (SWAudio *audio in self.audios)  {
                    if (audio.isChecking) {
                        [tempArray addObject:audio];
                    }
                }
                //delete from disk
                [SWFileManager deleteAudioFiles:tempArray];
                //delete from memory
                [self.audios removeObjectsInArray:tempArray];
            }
                break;
            default:
                break;
        }
    }
    else
    {
        switch (eventType) {
            case SWEventTypeUnselected:
                for (SWVideo *video in self.videos) {
                    video.checking = NO;
                }
                break;
            case SWEventTypeSelected:
                for (SWVideo *video in self.videos) {
                    video.checking = YES;
                }
                break;
            case SWEventTypeDelete:
            {
                //Enumeration all selected Model
                NSMutableArray *tempArray = [NSMutableArray array];
                for (SWVideo *video in self.videos)  {
                    if (video.isChecking) {
                        [tempArray addObject:video];
                    }
                }
                //delete from disk
                [SWFileManager deleteVideoFiles:tempArray];
                //delete from memory
                [self.videos removeObjectsInArray:tempArray];
            }
                break;
            default:
                break;
        }
    }
    [self.tableView reloadData];
}

#pragma mark  -SWAVCellDelegate  (detecte SelecteAll Status When CheckButton Selected Status Change)
- (void)avCellCheckingStateDidChange:(SWAudioVideoCell *)cell
{
    BOOL isSelectedAll = YES;
    if (_mediaType==SWVideoMeidaType)
    {
        for (SWVideo *video in self.videos) {
            if (video.isChecking==NO) {
                isSelectedAll = NO;
                break;
            }
        }
    }
    else
    {
        for (SWAudio *audio in self.audios) {
            if (audio.isChecking==NO) {
                isSelectedAll = NO;
                break;
            }
        }
    }
    _bottomView.checkButton.selected=isSelectedAll;
}

#pragma mark -event respond
-(void)edit:(UIButton *)editButton
{
    //tableView will become Edit Status
    if ([editButton.currentTitle isEqualToString:SWEdit]) {
        //default all unselected ,edit status
        if (_mediaType==SWAudioMediaType)
        {
            for (SWAudio *audio in self.audios)  {
                audio.checking=NO;
            }
            for (SWAudio *audio in self.audios) {
                audio.editing = YES;
            }
        }
        else
        {
            for (SWVideo *video in self.videos)  {
                video.checking=NO;
            }
            for (SWVideo *video in self.videos) {
                video.editing = YES;
            }
        }
        
        
        
        [editButton setTitle:SWDone forState:UIControlStateNormal];
        
        //bottomView Appear
        _bottomView =[[SWBottomView alloc] initWithFrame:CGRectMake(0, SWScreenHeight-64,SWScreenWidth,44)];
        _bottomView.delegate =self;
        [self.view addSubview:_bottomView];
        
        //bottomView appear animatation,tableView.height - =44
        [UIView animateWithDuration:0.3 animations:^{
            _bottomView.y=SWScreenHeight-64-44;
        } completion:^(BOOL finished) {
            self.tableView.height = SWScreenHeight-64-44;
        }];
    } else
    {
        //tableView will become Done Status,done status
        if (_mediaType==SWAudioMediaType)
        {
            for (SWAudio *audio in self.audios) {
                audio.editing = NO;
            }
        }
        else
        {
            for (SWVideo *video in self.videos) {
                video.editing = NO;
            }
        }

        [editButton setTitle:SWEdit forState:UIControlStateNormal];
        
        self.tableView.height += 44;
        
        //bottomView disappear
        [UIView animateWithDuration:0.3 animations:^{
            _bottomView.y=SWScreenHeight-64;
        } completion:^(BOOL finished) {
            
            [_bottomView removeFromSuperview];
        }];
    }
    [self.tableView reloadData];
}

#warning simulation record video && record audio
- (void)downloadFile
{
    
    [MBProgressHUD showMessage:@"正在下载..."];
    if (_mediaType==SWAudioMediaType)
    {
        //http://rainymood.com/audio1110/0.ogg
        NSURL *url = [NSURL URLWithString:@"http://rainymood.com/audio1110/0.m4a"];
        NSURLRequest *request = [NSURLRequest requestWithURL:url];
        [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse *response, NSData *data, NSError *connectionError) {
            
            
            NSString *directoriesPath =[SWDocumentPath stringByAppendingPathComponent:SWRecordFolderName];
            if (![[NSFileManager defaultManager] fileExistsAtPath:directoriesPath]) {
                NSError *error;
                [[NSFileManager defaultManager] createDirectoryAtPath:directoriesPath withIntermediateDirectories:NO attributes:nil error:&error];
                if (error) SWLogFunc(error);
            }
            NSString *filePath = [directoriesPath stringByAppendingPathComponent:@"0.m4a"];
            
            [data writeToFile:filePath atomically:YES];
            [MBProgressHUD hideHUD];
            [MBProgressHUD showSuccess:data?@"下载完成":@"下载失败"];
            [self refreshData];
        }];
    }
    else
    {
        NSURL *url = [NSURL URLWithString:@"http://192.168.11.185:8080/MJServer/resources/videos/minion_01.mp4"];
        NSURLRequest *request = [NSURLRequest requestWithURL:url];
        [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse *response, NSData *data, NSError *connectionError) {
            NSLog(@"%d", data.length);
            
            
            
            NSString *directoriesPath =[SWDocumentPath stringByAppendingPathComponent:SWVideoFolderName];
            if (![[NSFileManager defaultManager] fileExistsAtPath:directoriesPath]) {
                NSError *error;
                [[NSFileManager defaultManager] createDirectoryAtPath:directoriesPath withIntermediateDirectories:NO attributes:nil error:&error];
                if (error) SWLogFunc(error);
            }
            NSString *filePath = [directoriesPath stringByAppendingPathComponent:@"minion_01.mp4"];
            
            [data writeToFile:filePath atomically:YES];
            
            [MBProgressHUD hideHUD];
            [MBProgressHUD showSuccess:data?@"下载完成":@"下载失败"];
            [self refreshData];
        }];
    }
    
    
}

-(void)refreshData
{
    if (_mediaType==SWVideoMeidaType)
    {
        NSArray *fileNames = [SWFileManager getAllFilesFromFileType:SWVideoFileType];
        [self.videos removeAllObjects];
        for (int i=0; i<fileNames.count; i++) {
            
            NSString *filePathName=[fileNames objectAtIndex:i];
            //get file Info eg:(name,fileSize,duration,filePath,coverImage...)
            NSDictionary *videoInfo = [SWFileManager generateVideoInfoWithFile:filePathName];
            SWVideo *video= [[SWVideo alloc] initWithVideoInfo:videoInfo];
            [self.videos addObject:video];
        }
    }
    else
    {
        NSArray *fileNames = [SWFileManager getAllFilesFromFileType:SWRecordFileType];
        [self.audios removeAllObjects];
        for (int i=0; i<fileNames.count; i++) {
            
            NSString *filePathName=[fileNames objectAtIndex:i];
            //get file Info eg:(name,fileSize,duration,filePath...)
            NSDictionary *audioInfo = [SWFileManager generateAudioInfoWithFile:filePathName];
            SWAudio *audio= [[SWAudio alloc] initWithAudioInfo:audioInfo];
            [self.audios addObject:audio];
        }
    }
    [self.tableView reloadData];
}

#pragma mark -private methods



#pragma mark -getter
-(UITableView *)tableView
{
    
    if (!_tableView) {
        _tableView  =[[UITableView alloc] initWithFrame:CGRectMake(0,0, SWScreenWidth, SWScreenHeight-64) style:UITableViewStylePlain];
        _tableView.delegate=self;
        _tableView.dataSource=self;
        
        
        [_tableView registerNib:[UINib nibWithNibName:NSStringFromClass([SWAudioVideoCell class]) bundle:nil] forCellReuseIdentifier:SWCellIdentifier];
    }
    return _tableView;
}

-(NSMutableArray *)videos
{
    if (!_videos) {
        _videos = [NSMutableArray array];
    }
    return _videos;
}

-(NSMutableArray *)audios
{
    if (!_audios) {
        _audios = [NSMutableArray array];
    }
    return _audios;
}

- (UIImageView *)noDataView
{
    if (!_noDataView) {
        UIImageView *noDataView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"nodataTips"]];
        noDataView.frame=CGRectMake(0, 0, 124, 107);
        noDataView.center=self.view.center;
        self.noDataView = noDataView;
        _noDataView.userInteractionEnabled=YES;
        UITapGestureRecognizer *recognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(downloadFile)];
        [_noDataView addGestureRecognizer:recognizer];
    }
    return _noDataView;
}

-(UIBarButtonItem *)editButtonItem
{
    if (!_editButtonItem) {
        UIButton *editButton =[[UIButton alloc] initWithFrame:CGRectMake(0, 0, 60, 30)];
        [editButton setTitle:SWEdit forState:UIControlStateNormal];
        [editButton addTarget:self action:@selector(edit:) forControlEvents:UIControlEventTouchUpInside];
        _editButtonItem = [[UIBarButtonItem alloc] initWithCustomView:editButton];
    }
    return _editButtonItem;
}

-(void)setMediaType:(SWMediaType)mediaType
{
    _mediaType=mediaType;
    if (_mediaType==SWVideoMeidaType)
    {
#warning simulatiom test data
        //        for (int i=0; i<16; i++) {
        
        //            SWVideo *video =[[SWVideo alloc] init];
        //            video.filePathName=[NSString stringWithFormat:@"minion_0%i.mp4",i+1];
        //            video.coverPictureImage=[UIImage imageNamed:[NSString stringWithFormat:@"video_Placehoder"]];
        //            video.videoName=[NSString stringWithFormat:@"minion%i",i+1];
        //            video.videoDuration=@"02:32";
        //            video.videoFileSize=@"228K";
        //            video.checking=i%2;
        //            video.editing=NO;
        //            [self.videos addObject:video];
        //        }
    }
    else if (_mediaType==SWAudioMediaType)
    {
#warning simulatiom test data
        //        for (int i=0; i<8; i++) {
        //            SWAudio  *audio =[[SWAudio alloc] init];
        //            audio.filePath=[NSString stringWithFormat:@"%i.mp3",i];
        //            audio.coverPicturei=[NSString stringWithFormat:@"record_Placehoder"];
        //            audio.audioName=[NSString stringWithFormat:@"音频%i",i+1];
        //            audio.audioDuration=@"42:32";
        //            audio.audioFileSize=@"228K";
        //            audio.checking=i%2;
        //            audio.editing=NO;
        //            [self.audios addObject:audio];
        //        }
    }
}

@end
