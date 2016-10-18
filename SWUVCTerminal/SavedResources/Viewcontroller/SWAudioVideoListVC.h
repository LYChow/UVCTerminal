//
//  SWAudioVideoListVC.h
//  SWUVCTerminal
//
//  Created by hxf on 9/23/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "SWViewController.h"

typedef enum :NSInteger
{
    SWAudioMediaType,
    SWVideoMeidaType
}SWMediaType;

@interface SWAudioVideoListVC : SWViewController

@property(nonatomic,assign) SWMediaType mediaType;

@end
