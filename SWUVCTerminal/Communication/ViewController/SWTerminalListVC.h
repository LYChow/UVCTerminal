//
//  SWTerminalListVC.h
//  SWUVCTerminal
//
//  Created by hxf on 8/22/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWViewController.h"

typedef enum
{
    SWTalkBackCommunicationType=0,
    SWMonitorCommunicationType,
    SWMeetingCommunicationType
}SWCommunicationType;

@interface SWTerminalListVC : SWViewController

@property(nonatomic,assign) SWCommunicationType communicationType;

@end
