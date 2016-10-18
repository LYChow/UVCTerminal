//
//  SWPTZControlView.h
//  SWUVCTerminal
//
//  Created by hxf on 8/24/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <UIKit/UIKit.h>
typedef enum
{
    SWPTZControlTypeMoveToUp=1,
    SWPTZControlTypeMoveToLeft,
    SWPTZControlTypeMoveToDown,
    SWPTZControlTypeMoveToRight,
    
    SWPTZControlTypeZoomIn,
    SWPTZControlTypeZoomOut
}SWPTZControlType;

@protocol SWPTZControlViewDelegate <NSObject>

-(void)callBackPtzControllEventType:(SWPTZControlType)eventType;

@end

@interface SWPTZControlView : UIView

//设置横竖屏布局
@property(nonatomic,assign,getter=isLandScape) BOOL landScape;

@property(nonatomic,weak) id <SWPTZControlViewDelegate> delegate;

@end
