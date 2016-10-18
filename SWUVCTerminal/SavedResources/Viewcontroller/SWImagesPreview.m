//
//  SWImagesPreview.m
//  SWUVCTerminal
//
//  Created by hxf on 9/14/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWImagesPreview.h"
#import "SWImagePreviewCollectionCell.h"
#import "UIBarButtonItem+Extension.h"
#define SWItemWidth SWScreenWidth

@interface SWImagesPreview()

@property (nonatomic, strong) NSArray *images;

@property(nonatomic,strong) UIView *toolBarView;
@property (nonatomic, strong) UIBarButtonItem *backItem;
@end


@implementation SWImagesPreview
#pragma mark -life cycle


static NSString * const ImageCellReuseIdentifier = @"ImageCellReuseIdentifier";
- (instancetype)init
{
    UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
    layout.scrollDirection=UICollectionViewScrollDirectionHorizontal;
    layout.minimumLineSpacing=0;
    return [self initWithCollectionViewLayout:layout];
}

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title=[NSString stringWithFormat:@"1/%lu",(unsigned long)self.images.count];
    // Register cell from nib
    [self.collectionView registerNib:[UINib nibWithNibName:@"SWImagePreviewCollectionCell" bundle:nil] forCellWithReuseIdentifier:ImageCellReuseIdentifier];
    self.collectionView.showsHorizontalScrollIndicator=NO;
    self.collectionView.decelerationRate =  UIScrollViewDecelerationRateNormal;
    self.collectionView.alwaysBounceVertical = NO;
    self.collectionView.pagingEnabled=YES;
    
    [self.view addSubview:self.toolBarView];
    
    self.navigationItem.leftBarButtonItem =self.backItem;
}

#pragma mark - public methods

-(void)disSelectedCollectionCell:(SWImage *)image withImageList:(NSArray *)images
{
    self.images=images;
    if ([images containsObject:image]) {
         NSInteger index =[images indexOfObject:image];
        //将collectionView滑动到制定的位置
    }
    [self.collectionView reloadData];
}

#pragma mark -system object Delegate
#pragma mark -UICollectionViewDataSource
- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    // 计算一遍内边距
//    [self viewWillTransitionToSize:CGSizeMake(collectionView.width, 0) withTransitionCoordinator:nil];
    
    return self.images.count;
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    
    SWImagePreviewCollectionCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:ImageCellReuseIdentifier forIndexPath:indexPath];
    cell.imageModel  = [self.images objectAtIndex:indexPath.item];
    return cell;
}

#pragma mark -UICollectionViewDelegate
- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    SWLog(@"item被点击,展示item大图");
}

- (CGSize)collectionView:(nonnull UICollectionView *)collectionView layout:(nonnull UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(nonnull NSIndexPath *)indexPath {
    return CGSizeMake(SWItemWidth, SWScreenHeight-80*2);
}


#pragma mark -UIScrollViewDelegate
-(void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView
{
    NSInteger index = MAX(1, (scrollView.contentOffset.x/SWScreenWidth)+1);
    self.title=[NSString stringWithFormat:@"%li/%lu",(long)index,(unsigned long)self.images.count];
}

#pragma mark - custome event
-(void)back
{
    [self.navigationController popViewControllerAnimated:YES];
}

#pragma mark -getter

- (UIBarButtonItem *)backItem
{
    if (!_backItem) {
        self.backItem = [[UIBarButtonItem alloc] initWithImage:[[UIImage imageNamed:@"back"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] style:UIBarButtonItemStylePlain target:self action:@selector(back)];
    }
    return _backItem;
}

-(UIView *)toolBarView
{
    if (!_toolBarView) {
        _toolBarView = [[UIView alloc] initWithFrame:CGRectMake(0, SWScreenHeight-64-44, SWScreenWidth, 44)];
        _toolBarView.backgroundColor= [UIColor darkGrayColor];
  
        UIButton *favouriteBtn = [[UIButton alloc] initWithFrame:CGRectMake(SWScreenWidth/2-80, 4, 60, 36)];
        [favouriteBtn setTitle:@"收藏" forState:UIControlStateNormal];
        favouriteBtn.backgroundColor=[UIColor orangeColor];
        
        [[favouriteBtn rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            SWLogFunc(@"收藏");
        }];
        
        UIButton *sendBtn = [[UIButton alloc] initWithFrame:CGRectMake(SWScreenWidth/2, 4, 60, 36)];
        [sendBtn setTitle:@"发送" forState:UIControlStateNormal];
        sendBtn.backgroundColor=[UIColor orangeColor];
        
        [[sendBtn rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            SWLogFunc(@"发送");
        }];
        [_toolBarView addSubview:favouriteBtn];
        [_toolBarView addSubview:sendBtn];
    }
    return _toolBarView;
}



@end
