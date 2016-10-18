//
//  SWPTZControlView.m
//  SWUVCTerminal
//
//  Created by hxf on 8/24/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#define SWPadding 20

#define SWPortaitScale 0.6

#import "SWPTZControlView.h"
@interface SWPTZControlView()
@property(nonatomic,strong) UIButton *backButton;
@property(nonatomic,strong) UIImageView *directWheel;
@property(nonatomic,strong) UIButton *zoomInButton;
@property(nonatomic,strong) UIButton *zoomOutButton;

@end

@implementation SWPTZControlView
#pragma mark -system Methods

-(void)drawRect:(CGRect)rect
{
    [self addSubview:self.backButton];
    [self addSubview:self.directWheel];
    [self addSubview:self.zoomInButton];
    [self addSubview:self.zoomOutButton];
}

-(void)layoutSubviews
{
    [super layoutSubviews];
    
    
    
    self.backButton.position=CGPointMake(SWPadding, SWScreenHeight-SWPadding-self.backButton.height-self.directWheel.height);
    
    self.directWheel.position=CGPointMake(CGRectGetMaxX(self.backButton.frame), CGRectGetMaxY(self.backButton.frame));
    
    self.zoomOutButton.position = CGPointMake(SWScreenWidth-SWPadding-self.zoomOutButton.width, self.directWheel.center.y);
    self.zoomInButton.position = CGPointMake(CGRectGetMinX(self.zoomOutButton.frame)-SWPadding-self.zoomInButton.width, self.zoomOutButton.y);
    
}

#pragma mark -event Methods
-(void)ptzControlButtonClick:(UIButton *)btn
{
    if (!(_delegate && [_delegate respondsToSelector:@selector(callBackPtzControllEventType:)])) return;
    
    switch (btn.tag) {
        case 1:
            [_delegate callBackPtzControllEventType:SWPTZControlTypeMoveToUp];
            break;
        case 2:
            [_delegate callBackPtzControllEventType:SWPTZControlTypeMoveToLeft];
            break;
        case 3:
            [_delegate callBackPtzControllEventType:SWPTZControlTypeMoveToDown];
            break;
        case 4:
            [_delegate callBackPtzControllEventType:SWPTZControlTypeMoveToRight];
            break;
        case 5:
            [_delegate callBackPtzControllEventType:SWPTZControlTypeZoomIn];
            break;
        case 6:
            [_delegate callBackPtzControllEventType:SWPTZControlTypeZoomOut];
            break;
        default:
            break;
    }
    
}


#pragma mark system Delegate
//-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
//{
//        self.backButton.hidden=self.backButton.hidden?NO:YES;
//        self.directWheel.hidden=self.directWheel.hidden?NO:YES;
//        self.zoomInButton.hidden=self.zoomInButton.hidden?NO:YES;
//        self.zoomOutButton.hidden=self.zoomOutButton.hidden?NO:YES;
//}


#pragma mark getter method
-(UIButton *)backButton
{
    if (!_backButton) {
        _backButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 44, 44)];
        [_backButton setImage:[UIImage imageNamed:@"ptzControlBack"] forState:UIControlStateNormal];
        [[_backButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            [self removeFromSuperview];
        }];
    }
    return _backButton;
}

-(UIImageView *)directWheel
{
    if (!_directWheel) {
        
        float directButtonWidth  =_landScape?52:52*SWPortaitScale;
        float directButtonHeight = _landScape?31:31*SWPortaitScale;
        float directWheelWidth =_landScape?172:172*SWPortaitScale;
        
        _directWheel =[[UIImageView alloc] initWithFrame:CGRectMake(0, 0,directWheelWidth,directWheelWidth)];
        _directWheel.image = [UIImage imageNamed:@"ptzControlWheelBg"];
        
        UIButton *up =[[UIButton alloc] initWithFrame:CGRectMake(_directWheel.center.x-directButtonWidth/2, 0, directButtonWidth, directButtonHeight)];
        [up setImage:[UIImage imageNamed:@"up"] forState:UIControlStateNormal];
        [up setImage:[UIImage imageNamed:@"up_Highlighted"] forState:UIControlStateHighlighted];
        up.tag =1;
        [up addTarget:self action:@selector(ptzControlButtonClick:) forControlEvents:UIControlEventTouchUpInside];
        [_directWheel addSubview:up];
        
        UIButton *left =[[UIButton alloc] initWithFrame:CGRectMake(0, _directWheel.height/2-directButtonWidth/2, directButtonHeight, directButtonWidth)];
        [left setImage:[UIImage imageNamed:@"left"] forState:UIControlStateNormal];
        [left setImage:[UIImage imageNamed:@"left_Highlighted"] forState:UIControlStateHighlighted];
        left.tag =2;
        [left addTarget:self action:@selector(ptzControlButtonClick:) forControlEvents:UIControlEventTouchUpInside];
        [_directWheel addSubview:left];
        
       
        UIButton *down =[[UIButton alloc] initWithFrame:CGRectMake(up.x, _directWheel.height-directButtonHeight, directButtonWidth, directButtonHeight)];
        [down setImage:[UIImage imageNamed:@"down"] forState:UIControlStateNormal];
        [down setImage:[UIImage imageNamed:@"down_Highlighted"] forState:UIControlStateHighlighted];
        down.tag =3;
        [down addTarget:self action:@selector(ptzControlButtonClick:) forControlEvents:UIControlEventTouchUpInside];
        [_directWheel addSubview:down];
        
        
        UIButton *right =[[UIButton alloc] initWithFrame:CGRectMake(_directWheel.width-directButtonHeight, left.y, directButtonHeight, directButtonWidth)];
        [right setImage:[UIImage imageNamed:@"right"] forState:UIControlStateNormal];
        [right setImage:[UIImage imageNamed:@"right_Highlighted"] forState:UIControlStateHighlighted];
        right.tag =4;
        [right addTarget:self action:@selector(ptzControlButtonClick:) forControlEvents:UIControlEventTouchUpInside];
        [_directWheel addSubview:right];
        _directWheel.userInteractionEnabled=YES;
    }
    return _directWheel;
}

-(UIButton *)zoomInButton
{
    if (!_zoomInButton) {
        _zoomInButton =[[UIButton alloc] initWithFrame:CGRectMake(0, 0, 44, 44)];
        [_zoomInButton setImage:[UIImage imageNamed:@"zoomIn"] forState:UIControlStateNormal];
        [_zoomInButton setImage:[UIImage imageNamed:@"zoomIn_Highlighted"] forState:UIControlStateHighlighted];
        _zoomInButton.tag =5;
        [_zoomInButton addTarget:self action:@selector(ptzControlButtonClick:) forControlEvents:UIControlEventTouchUpInside];
    }
    return _zoomInButton;
}

-(UIButton *)zoomOutButton
{
    if (!_zoomOutButton) {
        _zoomOutButton =[[UIButton alloc] initWithFrame:CGRectMake(0, 0, 44, 44)];
        [_zoomOutButton setImage:[UIImage imageNamed:@"zoomOut"] forState:UIControlStateNormal];
        [_zoomOutButton setImage:[UIImage imageNamed:@"zoomOut_Highlighted"] forState:UIControlStateHighlighted];
        _zoomOutButton.tag =6;
        [_zoomOutButton addTarget:self action:@selector(ptzControlButtonClick:) forControlEvents:UIControlEventTouchUpInside];
    }
    return _zoomOutButton;
}



@end
