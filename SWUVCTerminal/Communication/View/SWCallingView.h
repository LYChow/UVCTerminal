//
//  SWCallingView.h
//  SWUVCTerminal
//
//  Created by hxf on 8/23/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <UIKit/UIKit.h>

#define SWAccept   @"Accept"
#define SWHandUp   @"HandUp"
#define SWCancled  @"Cancled"

typedef enum
{
SWCallOutcomingDirectType,
SWCallIncomingDirectType
}SWCallDirectType;
@interface SWCallingView : UIView

@property(nonatomic,copy) NSString *peerTerminalName;

@property(nonatomic,assign) SWCallDirectType  callDirectType; //calling or OnCalling

@property(nonatomic,strong) RACSubject *callBackSignal; //回调的事件类型

+(instancetype)shareManager;
-(void)show;
-(void)hidden;

@end
