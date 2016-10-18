//
//  SWBottomView.m
//  SWUVCTerminal
//
//  Created by hxf on 9/12/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWBottomView.h"
#define SWBottomViewHeight 44
@interface SWBottomView()

@property(nonatomic,strong) UILabel *label;
@property(nonatomic,strong) UIButton *deleteButton;
@end

@implementation SWBottomView



-(instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame]) {
        self.frame=CGRectMake(0, SWScreenHeight, SWScreenWidth, SWBottomViewHeight);
        self.backgroundColor =  SWColor(229, 229, 229);

        [self addSubview:self.checkButton];
        [self addSubview:self.label];
        [self addSubview:self.deleteButton];
    }
    return self;
}

#pragma mark -event methods
-(void)checkButtonSelected:(UIButton *)btn
{
    btn.selected=!btn.selected;
    
    if (self.delegate && [self.delegate respondsToSelector:@selector(callBackEventType:)]) {
        [self.delegate callBackEventType:btn.selected?SWEventTypeSelected:SWEventTypeUnselected];
    }
}

#pragma mark - getter methods

-(UIButton *)deleteButton
{
    if (!_deleteButton) {
        _deleteButton = [[UIButton alloc] initWithFrame:CGRectMake(SWScreenWidth-100, 4, 71, 37)];
        [_deleteButton setImage:[UIImage imageNamed:@"selectedAll_Delete"] forState:UIControlStateNormal];
        [[_deleteButton rac_signalForControlEvents:UIControlEventTouchUpInside]subscribeNext:^(id x) {
            if (self.delegate && [self.delegate respondsToSelector:@selector(callBackEventType:)]) {
                [self.delegate callBackEventType:SWEventTypeDelete];
            }
        }];
    }
    return _deleteButton;
}

-(UILabel *)label
{
    if (!_label) {
        _label=[[UILabel alloc] initWithFrame:CGRectMake(40,7 , 60, 30)];
        _label.text=@"全选";
        _label.textAlignment=NSTextAlignmentLeft;
        _label.font = [UIFont systemFontOfSize:12];
        _label.textColor = [UIColor whiteColor];
    }
    return _label;
}

-(UIButton *)checkButton
{
    if (!_checkButton) {
        _checkButton =[[UIButton alloc] initWithFrame:CGRectMake(10, 7, 30, 30)];
        [_checkButton setImage:[UIImage imageNamed:@"selectedAll_unselected"] forState:UIControlStateNormal];
        [_checkButton setImage:[UIImage imageNamed:@"selectedAll_selected"] forState:UIControlStateSelected];
        _checkButton.selected=NO;
        [_checkButton addTarget:self action:@selector(checkButtonSelected:) forControlEvents:UIControlEventTouchUpInside];
    }
    return _checkButton;
}

@end
