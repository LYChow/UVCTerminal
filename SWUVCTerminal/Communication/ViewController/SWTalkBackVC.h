//
//  SWTalkBackVC.h
//  SWUVCTerminal
//
//  Created by hxf on 8/12/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWViewController.h"
#import "SWAppDemo.h"

@interface SWTalkBackVC : SWViewController

@property(nonatomic,copy) NSString *peerTerminalName;

-(int)ShowConfirmBox:(const char*)szMessage withBtn1Text:(const char*)szBtn1Text andBtn2Text:(const char*)szBtn2Text andTitle:(const char*)szTitle;
-(int)ShowMsgBox:(const char*)szMessage withButtonText:(const char*)szBtnText andTitle:(const char*)szTitle;
-(int)CancelInfoBox;

@end
