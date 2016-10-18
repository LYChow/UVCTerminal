//
//  SWSettingWheel.h
//  WheelUseUIBerzier
//
//  Created by hxf on 8/31/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <UIKit/UIKit.h>

#define SWRecordVideo         @"SWRecordVideo"
#define SWHiddenMineVideo     @"SWHiddenMineVideo"
#define SWCaptureImage        @"SWCaptureImage"
#define SWPopSetting          @"SWPopSetting"
#define SWSwitchFrontCapture  @"SWSwitchFrontCapture"
#define SWRecordAudio         @"SWRecordAudio"


@protocol SWSettingWheelDelegate <NSObject>
//回调点击的item的标题
-(void)callBackClilkItemTitle:(NSString *)title;

//手指在触摸settingWheel
-(void)touchingSettingWheelView;

@end

@interface SWSettingWheel : UIView

@property(nonatomic,weak) id <SWSettingWheelDelegate> delegate;

@end
