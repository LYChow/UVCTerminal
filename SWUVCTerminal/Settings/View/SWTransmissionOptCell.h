//
//  SWTransmissionOptCell.h
//  SWUVCTerminal
//
//  Created by hxf on 8/16/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SWTransmissionOptCell : UITableViewCell
@property (weak, nonatomic) IBOutlet UIImageView *transmissionOptIcon;
@property (weak, nonatomic) IBOutlet UILabel *transmissionOptTitle;
@property (weak, nonatomic) IBOutlet UILabel *recoveryPercentLabel;
@property (weak, nonatomic) IBOutlet UISlider *transmissionOptSlider;
@property (weak, nonatomic) IBOutlet UILabel *currentSettingLabel;


@end
