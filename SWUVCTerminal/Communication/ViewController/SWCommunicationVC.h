//
//  SWCommunicationVC.h
//  SWUVCTerminal
//
//  Created by hxf on 8/23/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWViewController.h"
#define SWVoiceHandUp    @"VoiceHandUp"
#define SWVideoHandUp    @"VideoHandUp"
#define SWMonitorHandUp  @"MonitorHandUp"

typedef enum
{
    SWAudioMainViewType,
    SWVideoMainViewType
} SWMainViewType;

typedef enum
{
    SWChatVideoType,
    SWMonitorVideoType
}SWVideoType;

@interface SWCommunicationVC : SWViewController

@property(nonatomic,assign) SWMainViewType mainViewType;
@property(nonatomic,assign) SWVideoType    videoType;

@property(nonatomic,strong) RACSubject *handUpSignal;

@end
