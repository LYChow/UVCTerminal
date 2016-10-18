//
//  SWPickView.h
//  SWUVCTerminal
//
//  Created by hxf on 8/16/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <UIKit/UIKit.h>
#define audioChannelArray    @[@[@"通话音",@"媒体音",@"系统音"]]
#define videoFluncyArray     @[@[@"流畅",@"标清",@"高清"]]
#define smoothSendArray      @[@[@"不启用",@"1.25倍",@"1.5倍",@"2倍",@"4倍"]]
#define lossPacketRecoveryArray   @[@[@"不启用",@"1%",@"2%",@"3%",@"4%",@"5%",@"6%",@"7%",@"8%",@"9%",@"10%",@"11%",@"12%",@"13%",@"14%",@"15%",@"16%",@"17%",@"18%",@"19%",@"20%",@"21%",@"22%",@"23%",@"24%",@"25%"]]
typedef enum :NSInteger
{
    SWPickViewAudioChannelType=0,
    SWPickViewVideoFluncyType,
    SWPickViewSmoothSendType,
    SWPickViewLossPacketRecoveryType
} pickViewDataType;


@interface SWPickView : UIView 
//把选中的信息 通过发送信号返回给信号订阅者
@property(nonatomic,strong) RACSubject *delegateSignal;

+(SWPickView *)pickView;

-(void)setPickViewType:(pickViewDataType)type defaultSelectedTitle:(NSString *)selectedTitle;

-(void)setUpPickViewTitle:(NSString *)title withDataSource:(NSArray *)array defaultSelectedTitle:(NSString *)selectedTitle;

-(void)show;
-(void)hidden;

@end
