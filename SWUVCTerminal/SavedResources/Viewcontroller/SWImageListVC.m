//
//  SWImageListVC.m
//  SWUVCTerminal
//
//  Created by hxf on 8/15/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWImageListVC.h"
#import "UIView+Additions.h"
#import "UIBarButtonItem+Extension.h"
#import "SWImageCell.h"
#import "SWImage.h"
#import "SWBottomView.h"
#import "SWFileManager.h"
#import "SWImagesPreview.h"
NSString *const SWDone = @"完成";
NSString *const SWEdit = @"编辑";

#define SWItemWidth 100

#define SWString(str) [NSString stringWithFormat:@"  %@  ",str]

@interface SWImageListVC ()<SWImageCellDelegate,SWBottomViewDelegate>

#pragma mark property
@property (nonatomic, strong) NSMutableArray *images;
@property (nonatomic, strong) UIImageView *noDataView;
@property (nonatomic, strong) UIBarButtonItem *backItem;
@property (nonatomic, strong) SWBottomView *bottomView;
@property (nonatomic, strong) UIBarButtonItem *editButtonItem;

//ipad
@property (nonatomic, strong) UIBarButtonItem *selectAllItem;
@property (nonatomic, strong) UIBarButtonItem *unselectAllItem;
@property (nonatomic, strong) UIBarButtonItem *removeItem;
@end

@implementation SWImageListVC

#pragma mark -life cycle

static NSString * const reuseIdentifier = @"images";
- (instancetype)init
{
    UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
    // cell的大小
    layout.itemSize = CGSizeMake(SWItemWidth, SWItemWidth*(SWScreenHeight/SWScreenWidth));
    return [self initWithCollectionViewLayout:layout];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.collectionView.backgroundColor=[UIColor whiteColor];
    self.title=@"图片";
    [self setUpNavTheme];
    
    [self.collectionView registerNib:[UINib nibWithNibName:@"SWImageCell" bundle:nil] forCellWithReuseIdentifier:reuseIdentifier];
    self.collectionView.alwaysBounceVertical = YES;
    
    [self.view addSubview:self.noDataView];
  
    //遍历本地文件夹下文件,转换成模型加入数组
    NSArray *fileNames = [SWFileManager getAllFilesFromFileType:SWImageFileType];
    for (int i=0; i<fileNames.count; i++) {
        SWImage *image= [[SWImage alloc] init];
        image.name=[fileNames objectAtIndex:i];
        image.checking=NO;
        image.editing=NO;
        [self.images addObject:image];
    }
}

-(void)setUpNavTheme
{
    
    UIButton *backButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [backButton setImage:[UIImage imageNamed:@"back"] forState:UIControlStateNormal];
    [backButton sizeToFit];
    // 这句代码放在sizeToFit后面
    backButton.contentEdgeInsets = UIEdgeInsetsMake(0, -20, 0, 0);
    
    [[backButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
        [self.navigationController dismissViewControllerAnimated:NO completion:nil];
    }];
    backButton.size=backButton.currentImage.size;
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:backButton];
    
    
    
    self.navigationItem.rightBarButtonItem = self.editButtonItem;
    
    
    
    [self.navigationController.navigationBar setBackgroundImage:[UIImage imageNamed:@"navigationBar"] forBarMetrics:UIBarMetricsDefault];
    
}

#pragma mark -system object Delegate
#pragma mark -UICollectionViewDataSource
- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    // 计算一遍内边距
    [self viewWillTransitionToSize:CGSizeMake(collectionView.width, 0) withTransitionCoordinator:nil];
    
    self.noDataView.hidden = (self.images.count > 0);
    self.navigationItem.rightBarButtonItem=self.images.count?self.editButtonItem:nil;
    
    //数据清空时隐藏bottomView
    if (_bottomView && self.images.count==0) {
        self.collectionView.height += 44;
        
        //bottomView dismiss
        [UIView animateWithDuration:0.3 animations:^{
            _bottomView.y=SWScreenHeight-64;
        } completion:^(BOOL finished) {
            
            [_bottomView removeFromSuperview];
        }];
    }
    return self.images.count;
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    SWImageCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:reuseIdentifier forIndexPath:indexPath];
    cell.delegate = self;
    cell.image = self.images[indexPath.item];
    
    return cell;
}

#pragma mark -UICollectionViewDelegate
- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    SWImagesPreview *previewImageVC =[[SWImagesPreview alloc] init];
    [previewImageVC disSelectedCollectionCell:[self.images objectAtIndex:indexPath.item] withImageList:self.images];
    [self.navigationController pushViewController:previewImageVC animated:YES];
}


#pragma mark -custom Delegate
#pragma mark -SWImageCellDelegate 复选按钮选中时检查是否为全选
- (void)imageCellCheckingStateDidChange:(SWImageCell *)cell
{
    BOOL isSelectedAll = YES;
    for (SWImage *image in self.images) {
        if (image.isChecking==NO) {
            isSelectedAll = NO;
            break;
        }
    }
    // 按钮是否处于全选状态
    _bottomView.checkButton.selected=isSelectedAll;
}

#pragma mark -SWBottomViewDelegate
-(void)callBackEventType:(SWEventType)eventType
{
    switch (eventType) {
        case SWEventTypeUnselected:
            for (SWImage *image in self.images) {
                image.checking = NO;
            }
            break;
        case SWEventTypeSelected:
            for (SWImage *image in self.images) {
                image.checking = YES;
            }
            break;
        case SWEventTypeDelete:
        {
            NSMutableArray *tempArray = [NSMutableArray array];
            for (SWImage *image in self.images)  {
                if (image.isChecking) {
                    [tempArray addObject:image];
                }
            }
            // 删除内存中所有打钩的模型
            [self.images removeObjectsInArray:tempArray];
            
            //删除document文件夹下对应的图片
            [SWFileManager deleteFileWithFileType:SWImageFileType fromFileList:tempArray];
        }
            break;
        default:
            break;
    }
    
    [self.collectionView reloadData];
}

#pragma mark -event respond

-(void)edit:(UIButton *)editButton
{
    //即将进入编辑状态
    if ([editButton.currentTitle isEqualToString:SWEdit]) {
        [editButton setTitle:SWDone forState:UIControlStateNormal];
       //默认全不选中
        for (SWImage *image in self.images)  {
            image.checking=NO;
        }
        
        //bottomView pop
        _bottomView =[[SWBottomView alloc] initWithFrame:CGRectMake(0, SWScreenHeight-64,SWScreenWidth,44)];
        _bottomView.delegate =self;
        [self.view addSubview:_bottomView];
        
        
        [UIView animateWithDuration:0.3 animations:^{
            _bottomView.y=SWScreenHeight-64-44;
        } completion:^(BOOL finished) {
            self.collectionView.height -= 44;
        }];
        
        
        for (SWImage *image in self.images) {
            image.editing = YES;
        }
    } else
    {
        //即将进入完成状态
        [editButton setTitle:SWEdit forState:UIControlStateNormal];
        
        self.collectionView.height += 44;
        //bottomView dismiss
        [UIView animateWithDuration:0.3 animations:^{
            _bottomView.y=SWScreenHeight-64;
        } completion:^(BOOL finished) {
            
            [_bottomView removeFromSuperview];
        }];

        // 完成状态
        for (SWImage *image in self.images) {
            image.editing = NO;
        }
    }
    

    [self.collectionView reloadData];
}

-(void)loadData
{
    SWLog(@"刷新数据");
}

#pragma mark -system methods
/**
 当屏幕旋转,控制器view的尺寸发生改变调用
 */


#pragma mark -UIContentContainer
- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    // 根据屏幕宽度决定列数
    int columnSpace = (SWScreenWidth<SWScreenHeight) ? 2 : 3;
    
    // 根据列数计算内边距
    UICollectionViewFlowLayout *layout = (UICollectionViewFlowLayout *)self.collectionViewLayout;
    CGFloat inset = (size.width - columnSpace * layout.itemSize.width) / (columnSpace + 1);
    layout.sectionInset = UIEdgeInsetsMake(inset, inset , inset, inset);
    
    // 设置每一行之间的间距
    layout.minimumLineSpacing = 30;
}



#pragma mark -getter
//所有的属性都使用getter和setter进行初始化

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

#warning 以下注释部分为ipad所用形式
//- (UIBarButtonItem *)backItem
//{
//    if (!_backItem) {
//        self.backItem = [UIBarButtonItem itemWithTarget:self action:@selector(back) image:@"icon_back" highImage:@"icon_back_highlighted"];
//    }
//    return _backItem;
//}
//
//- (UIBarButtonItem *)selectAllItem
//{
//    if (!_selectAllItem) {
//        self.selectAllItem = [[UIBarButtonItem alloc] initWithTitle:SWString(@"全选") style:UIBarButtonItemStyleDone target:self action:@selector(selectAll)];
//    }
//    return _selectAllItem;
//}
//
//- (UIBarButtonItem *)unselectAllItem
//{
//    if (!_unselectAllItem) {
//        self.unselectAllItem = [[UIBarButtonItem alloc] initWithTitle:SWString(@"全不选") style:UIBarButtonItemStyleDone target:self action:@selector(unselectAll)];
//    }
//    return _unselectAllItem;
//}
//
//- (UIBarButtonItem *)removeItem
//{
//    if (!_removeItem) {
//        self.removeItem = [[UIBarButtonItem alloc] initWithTitle:SWString(@"删除") style:UIBarButtonItemStyleDone target:self action:@selector(remove)];
//        self.removeItem.enabled = NO;
//    }
//    return _removeItem;
//}

- (UIImageView *)noDataView
{
    if (!_noDataView) {
        // 添加一个"没有数据"的提醒
        _noDataView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"nodataTips"]];
        _noDataView.frame=CGRectMake(0, 0, 124, 107);
        _noDataView.center=self.view.center;
        _noDataView.userInteractionEnabled=YES;
        UITapGestureRecognizer *recognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(loadData)];
        [_noDataView addGestureRecognizer:recognizer];
    }
    return _noDataView;
}

-(NSMutableArray *)images
{
    if (!_images) {
        _images = [NSMutableArray array];
    }
    return _images;
}

@end
