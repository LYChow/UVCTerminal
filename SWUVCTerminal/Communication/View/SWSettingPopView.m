//
//  SWSettingPopView.m
//  SWUVCTerminal
//
//  Created by hxf on 8/23/16.
//  Copyright © 2016 sinowave. All rights reserved.
//


#define SWCheckVideoButtonTag        100
#define SWCheckAudioButtonTag        200

#define SWSubviewImageViewTag        300

#define SWLossDataRatioLabelTag      500

#import "SWSettingPopView.h"
#import "SWSetting.h"

@interface SWSettingPopView()

@property(nonatomic,assign) CGFloat settingPopViewHeight;
@property(nonatomic,assign) CGFloat settingPopViewY;

@property(nonatomic,strong) UIImageView *popSettingViewBg;

@property(nonatomic,strong) UIView *mainView;
@property(nonatomic,strong) UIView *videoSettingView;
@property(nonatomic,strong) UIView *audioSettingView;

@property(nonatomic,strong) UIButton *mainViewVideoImageButton;
@property(nonatomic,strong) UIButton *mainViewAudioImageButton;


@property(nonatomic,strong) NSArray *videoTypeArray;

@property(nonatomic,strong) NSArray *audioTypeArray;

@property(nonatomic,assign) CGFloat equivalenceRatioScale; //popSetting素材的背景height=201,外部传递的高度是显示高度,所有子视图按照这个比例缩放
@end

@implementation SWSettingPopView

#pragma mark- system methods

-(id)initWithFrame:(CGRect)frame andSettingPopViewHeight:(CGFloat)height withPopViewOriginY:(CGFloat)y
{
    if (self = [super initWithFrame:frame]) {
        self.settingPopViewHeight = height;
        self.settingPopViewY = y;
        self.equivalenceRatioScale = height/201;  //等比例缩放
      self.videoTypeArray = [NSArray arrayWithObjects:@"流畅",@"标清",@"高清",nil];
      self.audioTypeArray = [NSArray arrayWithObjects:@"通话",@"媒体",@"其他",nil];
        [self drawPopSettingView];
    }
    return self;
}

-(void)drawPopSettingView
{
    [self addSubview:self.popSettingViewBg];
    [self.popSettingViewBg addSubview:self.mainView];
    
    [self.popSettingViewBg addSubview:self.videoSettingView];
    self.videoSettingView.hidden=YES;
    
    [self.popSettingViewBg addSubview:self.audioSettingView];
    self.audioSettingView.hidden=YES;
    
}


 -(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    //如果属于_mainView范围,不触发事件
    CGPoint point = [[touches anyObject] locationInView:self];
    CGPoint p = [self  convertPoint:point toView:_mainView];
    
    if (![_mainView.layer containsPoint:p]) {
        [self hiddenPopSettingView];
    }
}


#pragma mark -custom event methods
-(void)videoCheckButtonSelected:(UIButton *)btn
{
    SWSetting *setting = [SWSetting setting];
    //按钮状态全部取消,选中当前选择按钮
    for (int i=0;i<self.videoTypeArray.count ; i++) {
        UIButton *btn =[self.videoSettingView viewWithTag:i+SWCheckVideoButtonTag];
        btn.selected=NO;
    }
      btn.selected=YES;
    
    
    //获取imageView通过Tag并他们设置
    
    UIImage *image =[UIImage imageNamed:[NSString stringWithFormat:@"videoFluency%li",btn.tag-SWCheckVideoButtonTag]];
    
    UIImageView *imageView = [self.videoSettingView viewWithTag:SWSubviewImageViewTag];
    imageView.image = image;
    //设置标识
    [_mainViewVideoImageButton setImage:image forState:UIControlStateNormal];
    
    
    //写入设置数据
    setting.videoFluency = [NSNumber numberWithInteger:(btn.tag-SWCheckVideoButtonTag)];
    [setting save];
}

-(void)audioCheckButtonSelected:(UIButton *)btn
{
      SWSetting *setting = [SWSetting setting];
    //按钮状态全部取消,选中当前选择按钮
    for (int i=0;i<self.audioTypeArray.count ; i++) {
        UIButton *btn =[self.audioSettingView viewWithTag:i+SWCheckAudioButtonTag];
        btn.selected=NO;
    }
    btn.selected=YES;
    
    
    //获取imageView通过Tag并他们设置
    
    UIImage *image =[UIImage imageNamed:[NSString stringWithFormat:@"voiceChanel%li",btn.tag-SWCheckAudioButtonTag]];
    
    UIImageView *imageView = [self.audioSettingView viewWithTag:SWSubviewImageViewTag];
    imageView.image = image;
    //设置标识
    [_mainViewAudioImageButton setImage:image forState:UIControlStateNormal];
    
    
    //写入设置数据
    setting.audioChannel = [NSNumber numberWithInteger:(btn.tag-SWCheckAudioButtonTag)];
    [setting save];
}

//转屏/touchBegan/mainViewBackButton
-(void)hiddenPopSettingView
{
    [UIView animateWithDuration:0.5 animations:^{
        self.x=SWScreenWidth;
    } completion:^(BOOL finished) {
        [self removeFromSuperview];
    }];
}


//UISlider的进度条变化
-(void)sliderProgressChange:(UISlider *)slider
{
    UILabel *lossDataRatioLabel =  [self.mainView viewWithTag:SWLossDataRatioLabelTag];
    lossDataRatioLabel.text=[[NSString stringWithFormat:@"%i",(int)slider.value] stringByAppendingString:@"%"];
    
    SWSetting *setting =[SWSetting setting];
    setting.lostPacketRecovery=[NSNumber numberWithInt:(int)slider.value];
    [setting save];
}



#pragma mark -getter methods
-(UIImageView *)popSettingViewBg
{
    if (!_popSettingViewBg) {
        _popSettingViewBg = [[UIImageView alloc] initWithFrame:CGRectMake(SWScreenWidth-self.settingPopViewHeight*SWPopSettingViewAspectRatio, self.settingPopViewY, self.settingPopViewHeight*SWPopSettingViewAspectRatio, self.settingPopViewHeight)];
        _popSettingViewBg.userInteractionEnabled =YES;
        [_popSettingViewBg setImage:[UIImage imageNamed:@"popSettingBg"]];
        
    }
    return _popSettingViewBg;
}

-(UIView *)mainView
{
    if (!_mainView) {
        
        SWSetting *setting =[SWSetting setting];
        
        _mainView = [[UIView alloc] initWithFrame:CGRectMake(0, 0,self.popSettingViewBg.width, self.popSettingViewBg.height)];
        
        CGFloat horizationPadding =20;
        CGFloat verticalMargin =30;
        
        //音频通道
        _mainViewAudioImageButton=[[UIButton alloc] initWithFrame:CGRectMake(_mainView.width-horizationPadding-60*self.equivalenceRatioScale, verticalMargin, 60*self.equivalenceRatioScale, 56*self.equivalenceRatioScale)];
        [_mainViewAudioImageButton setImage:[UIImage imageNamed:[NSString stringWithFormat:@"voiceChanel%i",[setting.audioChannel intValue]]] forState:UIControlStateNormal];
        
        [[_mainViewAudioImageButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            //进入流畅度设置界面
            self.mainView.hidden=YES;
            self.audioSettingView.hidden=NO;
        }];
        [_mainView addSubview:_mainViewAudioImageButton];
        
        //视频通道
        _mainViewVideoImageButton = [[UIButton alloc] initWithFrame:CGRectMake(_mainView.width/3, _mainViewAudioImageButton.y, 74*self.equivalenceRatioScale, 56*self.equivalenceRatioScale)];
        [_mainViewVideoImageButton setImage:[UIImage imageNamed:[NSString stringWithFormat:@"videoFluency%i",[setting.videoFluency intValue]]] forState:UIControlStateNormal];
        [[_mainViewVideoImageButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            //进入音频通道设置界面
            self.mainView.hidden=YES;
            self.videoSettingView.hidden=NO;
        }];
        [_mainView addSubview:_mainViewVideoImageButton];
        
        CGFloat sideLength =42*self.equivalenceRatioScale;
        UIButton *hiddenPopSettingViewButton = [[UIButton alloc] initWithFrame:CGRectMake(horizationPadding, _mainView.center.y-sideLength/2, sideLength,sideLength)];
        [hiddenPopSettingViewButton setImage:[UIImage imageNamed:@"popSettingHidden"] forState:UIControlStateNormal];
        [hiddenPopSettingViewButton addTarget:self action:@selector(hiddenPopSettingView) forControlEvents:UIControlEventTouchUpInside];
        [_mainView addSubview:hiddenPopSettingViewButton];
        
        UILabel *lossDataSignLabel=[[UILabel alloc] initWithFrame:CGRectMake(_mainViewVideoImageButton.x, _mainView.height-verticalMargin-30, 30, 30)];
        lossDataSignLabel.text=@"丢包:";
        lossDataSignLabel.textAlignment=NSTextAlignmentLeft;
        lossDataSignLabel.font = [UIFont systemFontOfSize:12];
        lossDataSignLabel.textColor = [UIColor whiteColor];
        [_mainView addSubview:lossDataSignLabel];
        
        
        UILabel *lossDataRatioLabel = [[UILabel alloc] initWithFrame:CGRectMake(_mainView.width-horizationPadding-30, lossDataSignLabel.y, 30, 30)];
        lossDataRatioLabel.tag=SWLossDataRatioLabelTag;
        lossDataRatioLabel.text=[[NSString stringWithFormat:@"%i", [[SWSetting setting].lostPacketRecovery intValue]] stringByAppendingString:@"%"];
        lossDataRatioLabel.textAlignment=NSTextAlignmentRight;
        lossDataRatioLabel.font = [UIFont systemFontOfSize:12];
        lossDataRatioLabel.textColor = [UIColor whiteColor];
        [_mainView addSubview:lossDataRatioLabel];
        
        
        UISlider *lossDataSlider=[[UISlider alloc] initWithFrame:CGRectMake(CGRectGetMaxX(lossDataSignLabel.frame), lossDataSignLabel.y,CGRectGetMinX(lossDataRatioLabel.frame)-CGRectGetMaxX(lossDataSignLabel.frame), 30)];
        [lossDataSlider addTarget:self action:@selector(sliderProgressChange:) forControlEvents:UIControlEventValueChanged];
        [lossDataSlider setThumbImage:[UIImage imageNamed:@"TrackImage"] forState:UIControlStateNormal];
        [lossDataSlider setMinimumTrackImage:[UIImage imageNamed:@"minimumTrackImage"] forState:UIControlStateNormal];
        [lossDataSlider setMaximumTrackImage:[UIImage imageNamed:@"maximumTrackImage"] forState:UIControlStateNormal];
        lossDataSlider.minimumValue=0;
        lossDataSlider.maximumValue=25;
        lossDataSlider.value = [setting.lostPacketRecovery intValue];
        [_mainView addSubview:lossDataSlider];
    }
    return _mainView;
}

-(UIView *)videoSettingView
{
    if (!_videoSettingView) {
        
         SWSetting *setting =[SWSetting setting];
        
        _videoSettingView = [[UIView alloc] initWithFrame:CGRectMake(0, 0,self.popSettingViewBg.width, self.popSettingViewBg.height)];
        
        CGFloat horizationPadding =20;
        CGFloat verticalPadding = 20;
        
        CGFloat sideLength =42*self.equivalenceRatioScale;
        UIButton *backButton = [[UIButton alloc] initWithFrame:CGRectMake(horizationPadding, _mainView.center.y-sideLength/2, sideLength,sideLength)];
        [backButton setImage:[UIImage imageNamed:@"popSettingBack"] forState:UIControlStateNormal];
        [[backButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            self.videoSettingView.hidden=YES;
            self.mainView.hidden=NO;
        }];
        [_videoSettingView addSubview:backButton];
        
        
        
        CGFloat startX = CGRectGetMaxX(backButton.frame)+horizationPadding;
        CGFloat endX = _videoSettingView.width-horizationPadding;
        CGFloat totalLength = endX-startX;  //流畅,标清,高清及checkBtn的总长度
        CGFloat margin  = totalLength /self.videoTypeArray.count;
        
        
        for (int i=0; i<self.videoTypeArray.count; i++) {
            UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(startX+margin*i, _videoSettingView.height-verticalPadding-30, 30, 30)];
            label.text=[self.videoTypeArray objectAtIndex:i];
            label.font = [UIFont systemFontOfSize:12];
            label.textColor = [UIColor whiteColor];
            [_videoSettingView addSubview:label];
            
            
            UIButton *checkButton =[[UIButton alloc] initWithFrame:CGRectMake(CGRectGetMaxX(label.frame)-5, label.y, 30, 30)];
            [checkButton setImage:[UIImage imageNamed:@"checkButton"] forState:UIControlStateNormal];
            [checkButton setImage:[UIImage imageNamed:@"checkButtonSelected"] forState:UIControlStateSelected];
            checkButton.selected=NO;
            checkButton.tag = i+SWCheckVideoButtonTag;
            [checkButton addTarget:self action:@selector(videoCheckButtonSelected:) forControlEvents:UIControlEventTouchUpInside];
            [_videoSettingView addSubview:checkButton];
        }
        //从设置中获取状态
        UIButton *selectedButton =[_videoSettingView viewWithTag:[setting.videoFluency integerValue]+SWCheckVideoButtonTag];
        selectedButton.selected=YES;
        
        
        
        UIImageView *videoImageView =[[UIImageView alloc] initWithFrame:CGRectMake(startX+totalLength/2-74/2, verticalPadding, 74, 56)];
        videoImageView.tag = SWSubviewImageViewTag;
        videoImageView.image=[UIImage imageNamed:[NSString stringWithFormat:@"videoFluency%i",[setting.videoFluency intValue]]];
        [_videoSettingView addSubview:videoImageView];
        
    }
    return _videoSettingView;
}

-(UIView *)audioSettingView
{
    if (!_audioSettingView) {
        SWSetting *setting =[SWSetting setting];
        
        _audioSettingView =[[UIView alloc] initWithFrame:CGRectMake(0, 0,self.popSettingViewBg.width, self.popSettingViewBg.height)];
        
        
        CGFloat horizationPadding =20;
        CGFloat verticalPadding = 20;
        
        CGFloat sideLength =42*self.equivalenceRatioScale;
        UIButton *backButton =  [[UIButton alloc] initWithFrame:CGRectMake(horizationPadding, _mainView.center.y-sideLength/2, sideLength,sideLength)];
        [backButton setImage:[UIImage imageNamed:@"popSettingBack"] forState:UIControlStateNormal];
        
        [[backButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            self.audioSettingView.hidden=YES;
            self.mainView.hidden=NO;
        }];
        [_audioSettingView addSubview:backButton];
        
        CGFloat startX = CGRectGetMaxX(backButton.frame)+horizationPadding;
        CGFloat endX = _audioSettingView.width-horizationPadding;
        CGFloat totalLength = endX-startX;  //流畅,标清,高清及checkBtn的总长度
        CGFloat margin  = totalLength /self.audioTypeArray.count;
        
        
        for (int i=0; i<self.audioTypeArray.count; i++) {
            UILabel *label=[[UILabel alloc] initWithFrame:CGRectMake(startX+margin*i, _videoSettingView.height-verticalPadding-30, 30, 30)];
            label.text=[self.audioTypeArray objectAtIndex:i];
            label.font = [UIFont systemFontOfSize:12];
            label.textColor = [UIColor whiteColor];
            [_audioSettingView addSubview:label];
            
            
            UIButton *checkButton=[[UIButton alloc] initWithFrame:CGRectMake(CGRectGetMaxX(label.frame)-5, label.y, 30, 30)];
            [checkButton setImage:[UIImage imageNamed:@"checkButton"] forState:UIControlStateNormal];
            [checkButton setImage:[UIImage imageNamed:@"checkButtonSelected"] forState:UIControlStateSelected];
            checkButton.selected=NO;
            checkButton.tag = i+SWCheckAudioButtonTag;
            [checkButton addTarget:self action:@selector(audioCheckButtonSelected:) forControlEvents:UIControlEventTouchUpInside];
            [_audioSettingView addSubview: checkButton];
        }
        
        //从设置中获取状态
        UIButton *selectedButton =[_audioSettingView viewWithTag:[setting.audioChannel integerValue]+SWCheckAudioButtonTag];
        selectedButton.selected=YES;
        
        
        UIImageView *audioImageView =[[UIImageView alloc] initWithFrame:CGRectMake(startX+totalLength/2-74/2, verticalPadding, 60, 56)];
        audioImageView.image=[UIImage imageNamed:[NSString stringWithFormat:@"voiceChanel%i",[setting.audioChannel intValue]]];
        audioImageView.tag = SWSubviewImageViewTag;
         [_audioSettingView addSubview:audioImageView];
    }
    return _audioSettingView;
}


@end
