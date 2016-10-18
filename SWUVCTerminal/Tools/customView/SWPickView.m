//
//  SWPickView.m
//  SWUVCTerminal
//
//  Created by hxf on 8/16/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWPickView.h"

@interface SWPickView()<UIPickerViewDataSource,UIPickerViewDelegate>
{
    NSInteger selectedComponentIndex;
    pickViewDataType dataType;
    NSArray *pickViewArray;
}
@property(nonatomic,strong)UIView *toolBarView;
@property(nonatomic,strong)UIPickerView *pickView;
@property(nonatomic,strong)UIWindow *keyWindow;
@property(nonatomic,strong)UILabel *toolBarTitleLabel;
@end
@implementation SWPickView
static SWPickView *customPickView=nil;
static NSInteger animationDuration=0.8;
static CGFloat toolBarHeight =44;
static CGFloat pickViewHeight=216;

+(SWPickView *)pickView
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        customPickView = [[SWPickView alloc] init];

    });
    return customPickView;
}


-(void)layoutSubviews
{
    [super layoutSubviews];
    [self.keyWindow addSubview:self];
    self.frame=self.keyWindow.bounds;
    
    
    [self addSubview:self.pickView];
    [self addSubview:self.toolBarView];
    
    UITapGestureRecognizer * tap  =  [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(hidden)];
    [self addGestureRecognizer:tap];
    self.y=SWScreenHeight;
    
    [UIView animateWithDuration:animationDuration animations:^{
        self.y=0;
    }];
    
}

#pragma mark -publicMethods
-(void)setPickViewType:(pickViewDataType)type defaultSelectedTitle:(NSString *)selectedTitle
{
   
    switch (type) {
        case SWPickViewAudioChannelType:
        {
            [self setUpPickViewTitle:@"音频通道" withDataSource:audioChannelArray defaultSelectedTitle:selectedTitle];
        }
            break;
        case SWPickViewVideoFluncyType:
        {
        [self setUpPickViewTitle:@"流畅度" withDataSource:videoFluncyArray defaultSelectedTitle:selectedTitle];
        }
            break;
        case SWPickViewSmoothSendType:
        {
        [self setUpPickViewTitle:@"平滑发送" withDataSource:smoothSendArray defaultSelectedTitle:selectedTitle];
        }
             break;
        case SWPickViewLossPacketRecoveryType:
        {
            [self setUpPickViewTitle:@"丢包恢复" withDataSource:lossPacketRecoveryArray defaultSelectedTitle:selectedTitle];
        }
            break;
        default:
            break;
    }
}

-(void)setUpPickViewTitle:(NSString *)title withDataSource:(NSArray *)array defaultSelectedTitle:(NSString *)selectedTitle
{
    [self layoutSubviews];
    
    _toolBarTitleLabel.text=title;
    pickViewArray=array;
    [self.pickView reloadAllComponents];
    
#warning 现在暂时支持一个component的情况
    selectedComponentIndex  =[[array lastObject] indexOfObject:selectedTitle];
    [self.pickView selectRow:selectedComponentIndex inComponent:0 animated:YES];
    
}







#pragma mark -system object Delegate

//UIPickViewDataSource
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
  return  pickViewArray.count;
}

// returns the # of rows in each component..
- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    return [[pickViewArray objectAtIndex:component] count];
}
//UIPickViewDelegate
- (nullable NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return [[pickViewArray objectAtIndex:component] objectAtIndex:row];
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    if (component==0) {
        selectedComponentIndex=row;
    }
}



#pragma mark -custom Delegate

#pragma mark -event respond
//移动到用户看得见的位置
-(void)show
{
    [UIView animateWithDuration:animationDuration animations:^{
    self.y=0;
    }];
}
//移动到用户看不见的位置
-(void)hidden
{
    [UIView animateWithDuration:animationDuration animations:^{
        self.y=SWScreenHeight;
    }];
}

#pragma mark -getter and setter methods

-(UIView *)toolBarView
{
    if (!_toolBarView) {
        _toolBarView = [[UIView alloc]initWithFrame:CGRectMake(0,SWScreenHeight - self.pickView.height -toolBarHeight, SWScreenWidth, toolBarHeight)];
        _toolBarView.backgroundColor = SWColor(73, 184, 182);

        UIButton *cancelBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        cancelBtn.frame = CGRectMake(5, 0, toolBarHeight, toolBarHeight);
        [cancelBtn setImage:[UIImage imageNamed:@"pickerView_Cancel"] forState:UIControlStateNormal];
        [[cancelBtn rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            [self hidden];
        }];
        
        UIButton *confirmBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        confirmBtn.frame = CGRectMake(_toolBarView.frame.size.width-65, 0, (4/3)*toolBarHeight, toolBarHeight);
        [confirmBtn setImage:[UIImage imageNamed:@"pickerView_Confirm"] forState:UIControlStateNormal];
        [[confirmBtn rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            [self hidden];
            
            //发送信号
            if (self.delegateSignal) {
                [self.delegateSignal sendNext:@{@"pickViewDataType":[NSNumber numberWithInteger:dataType],@"selectedTitle":[[pickViewArray lastObject]objectAtIndex:selectedComponentIndex]}];
            }

        }];
        
        _toolBarTitleLabel=[[UILabel alloc] initWithFrame:CGRectMake(CGRectGetMaxX(cancelBtn.frame), 0, confirmBtn.x-CGRectGetMaxX(cancelBtn.frame), toolBarHeight)];
        _toolBarTitleLabel.textColor=[UIColor whiteColor];
        _toolBarTitleLabel.textAlignment = NSTextAlignmentCenter;
        
        [_toolBarView addSubview:_toolBarTitleLabel];
        [_toolBarView addSubview:cancelBtn];
        [_toolBarView addSubview:confirmBtn];
    }
    return _toolBarView;
}

-(UIWindow *)keyWindow
{
    if (!_keyWindow) {
        _keyWindow = [UIApplication sharedApplication].keyWindow;
        if (!_keyWindow)
        {
            _keyWindow = [[UIApplication sharedApplication].windows objectAtIndex:0];
        }
    }
    return _keyWindow;
}

-(UIPickerView *)pickView
{
    if (!_pickView) {
        _pickView = [[UIPickerView alloc] init];
        _pickView.backgroundColor = [UIColor whiteColor];
        _pickView.delegate=self;
        _pickView.dataSource=self;
        _pickView.frame = CGRectMake(0, SWScreenHeight-pickViewHeight, SWScreenWidth, pickViewHeight);
    }
    return _pickView;
}

@end
