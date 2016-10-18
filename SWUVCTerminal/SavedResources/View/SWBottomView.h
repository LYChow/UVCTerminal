//
//  SWBottomView.h
//  SWUVCTerminal
//
//  Created by hxf on 9/12/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef enum : NSUInteger {
    SWEventTypeSelected,
    SWEventTypeUnselected,
    SWEventTypeDelete,
} SWEventType;

@protocol SWBottomViewDelegate <NSObject>

-(void)callBackEventType:(SWEventType)eventType;

@end

@interface SWBottomView : UIView


@property(nonatomic,assign,getter=isSelectedAll) BOOL selectedAll;

@property(nonatomic,strong) UIButton *checkButton;

@property(nonatomic,weak) id <SWBottomViewDelegate> delegate;


@end
