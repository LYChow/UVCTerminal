//
//  SWSettingWheel.m
//  WheelUseUIBerzier
//
//  Created by hxf on 8/31/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWSettingWheel.h"
#define numberOfItem 6

#define   DEGREES_TO_RADIANS(degrees)  ((M_PI * degrees)/ 180)

#define padding 20



@interface SWSettingWheel()
{
    UIImageView *containerView;
    CAShapeLayer* clockWiseLayer;
    
    CGFloat startAngle;
    CGFloat endAngle;
    
    CGPoint centerPoint;
    float radius;
    
    UIButton *iconBtn;
    
    float touchRadians; //刚滑时的弧度
    float deltaRadians;
    float btnWidth;
    
    float contentWidth;
}
@end

@implementation SWSettingWheel

#pragma mark- system methods

-(void)drawRect:(CGRect)rect
{
    //外界设置错误时,width!=height  此时,width=height = max
    if (rect.size.width!=rect.size.height) {
        contentWidth=rect.size.width>rect.size.height?rect.size.width:rect.size.height;
    }
    else
    {
        contentWidth =rect.size.height;
    }
  
   
    touchRadians =0.f;
    deltaRadians =0.f;
    
   radius = (contentWidth-padding)*0.4;
    
    
    
    
    
    startAngle = DEGREES_TO_RADIANS(240);
    endAngle = startAngle + 2*M_PI;
    
    
    
    containerView =[[UIImageView alloc] initWithFrame:CGRectMake(0, 0,contentWidth, contentWidth)];
    containerView.userInteractionEnabled=YES;
    containerView.image=[UIImage imageNamed:@"bgbg"];
    
    UIPanGestureRecognizer *pan =[[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipe:)];
    [containerView addGestureRecognizer:pan];
    [self addSubview:containerView];
    
    
    centerPoint = CGPointMake(containerView.frame.size.width/2, containerView.frame.size.height/2);
    clockWiseLayer = [[CAShapeLayer alloc] init];
    clockWiseLayer.fillColor = [UIColor clearColor].CGColor;
    [containerView.layer addSublayer:clockWiseLayer];
    
    for (int i=1; i<numberOfItem+1; i++) {
        iconBtn = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, radius/2, radius/2)];
        [iconBtn setImage:[UIImage imageNamed:[NSString stringWithFormat:@"icon_setting%i.png", i-1]]forState:UIControlStateNormal];
        iconBtn.tag=i;
        [iconBtn addTarget:self action:@selector(itemClick:) forControlEvents:UIControlEventTouchUpInside];
        [containerView addSubview:iconBtn];
    }
    [self setPropress:startAngle];
}


-(void)handleSwipe:(UIPanGestureRecognizer *)recognizer
{
    UITouch *touch = [[recognizer valueForKeyPath:@"touches"] lastObject];
    
    CGPoint touchPoint = [touch locationInView:containerView];
    
    if ((_delegate && [_delegate respondsToSelector:@selector(touchingSettingWheelView)]))
    {
        [_delegate touchingSettingWheelView];
    }
    
    switch (recognizer.state) {
        case UIGestureRecognizerStateBegan:
            //begin
        {
            //初始点的弧度
            float dx = touchPoint.x  - containerView.center.x;
            float dy = touchPoint.y  - containerView.center.y;
            deltaRadians=0;
            touchRadians = atan2(dy,dx);
            
        }
            break;
        case UIGestureRecognizerStateChanged:
            //change
            //计算出coverView在rootView的相对位置
        {
            float dx = touchPoint.x  - containerView.center.x;
            float dy = touchPoint.y  - containerView.center.y;
            float ang = atan2(dy,dx);
            
            deltaRadians = 2*(ang - touchRadians);
            
            
            [self setPropress:startAngle+deltaRadians];
            
        }
            break;
        case UIGestureRecognizerStateEnded:
        case UIGestureRecognizerStateCancelled:
        case UIGestureRecognizerStateFailed:
            //end 判断rightView.x所在的位置 决定最终目标位置
        {
            //实际停留位置
            
            
            startAngle=startAngle+deltaRadians;
            NSLog(@"实际位置--%f",startAngle);
            
            
            while (startAngle>2*M_PI) {
                startAngle -= 2*M_PI;
            }
            while (startAngle<0) {
                startAngle+=2*M_PI;
            }
            //判断结束点的停留位置
            if ((2*M_PI-M_PI/numberOfItem)<=startAngle  || startAngle< M_PI/numberOfItem){
                
                startAngle=0 * M_PI/numberOfItem;
            }
            else if (M_PI/numberOfItem<=startAngle && startAngle<M_PI/numberOfItem*3)
            {
                startAngle=M_PI/numberOfItem*2;
                
            }
            else if (M_PI/numberOfItem*3<=startAngle && startAngle<M_PI/numberOfItem*5)
            {
                startAngle=M_PI/numberOfItem*4;
                
            }
            else if (M_PI/numberOfItem*5<=startAngle && startAngle<M_PI/numberOfItem*7)
            {
                startAngle=M_PI/numberOfItem*6;
                
            }
            else if (M_PI/numberOfItem*7<=startAngle && startAngle<M_PI/numberOfItem*9)
            {
                startAngle=startAngle=M_PI/numberOfItem*8;
                
            }
            else if (M_PI/numberOfItem*9<=startAngle && startAngle<M_PI/numberOfItem*11)
            {
                startAngle=startAngle=M_PI/numberOfItem*10;
                
            }
            
            
            [UIView beginAnimations:nil context:NULL];
            [UIView setAnimationDuration:0.2];
            //最终停留位置
            [self setPropress:startAngle];
            [UIView commitAnimations];
            
        }
            break;
        default:
            break;
    }
    
}

#pragma mark -setter methods

-(void)setPropress:(float)radians
{
    UIBezierPath *bezierPath =[UIBezierPath bezierPathWithArcCenter:centerPoint
                                                             radius:radius
                                                         startAngle:startAngle
                                                           endAngle:radians
                                                          clockwise:YES];
    
    clockWiseLayer.path = bezierPath.CGPath;
    
    for (int i=1; i<numberOfItem+1; i++) {
        UIButton *b = [containerView viewWithTag:i];
        b.center = [self getPointFromEndAngle:radians+(i-1)*M_PI/3];
    }
}

#pragma mark -custom methods

-(CGPoint)getPointFromEndAngle:(float)ridians
{
    UIBezierPath *bezierPath =[UIBezierPath bezierPathWithArcCenter:centerPoint
                                                             radius:radius
                                                         startAngle:startAngle
                                                           endAngle:ridians
                                                          clockwise:YES];
    return bezierPath.currentPoint;
    
    
}

-(void)itemClick:(UIButton *)btn
{
    
    if (!(_delegate && [_delegate respondsToSelector:@selector(callBackClilkItemTitle:)])) {
        return;
    }
    switch (btn.tag)
    {
        case 1:
            [_delegate callBackClilkItemTitle:SWRecordVideo];
            break;
        case 2:
            [_delegate callBackClilkItemTitle:SWHiddenMineVideo];
            break;
        case 3:
            [_delegate callBackClilkItemTitle:SWCaptureImage];
            break;
        case 4:
            [_delegate callBackClilkItemTitle:SWPopSetting];
            break;
        case 5:
            [_delegate callBackClilkItemTitle:SWSwitchFrontCapture];
            break;
        case 6:
            [_delegate callBackClilkItemTitle:SWRecordAudio];
            break;
        default:
            break;
    }
  
}


@end
