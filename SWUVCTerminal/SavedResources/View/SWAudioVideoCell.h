//
//  SWVideoCell.h
//  SWUVCTerminal
//
//  Created by hxf on 8/17/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SWAudio.h"
#import "SWVideo.h"

@class SWAudioVideoCell;
@protocol SWAVCellDelegate <NSObject>

@optional
- (void)avCellCheckingStateDidChange:(SWAudioVideoCell *)cell;

@end

@interface SWAudioVideoCell : UITableViewCell

@property(nonatomic,strong) SWAudio *audio;
@property(nonatomic,strong) SWVideo *video;
@property (nonatomic, weak) id<SWAVCellDelegate> delegate;

@end
