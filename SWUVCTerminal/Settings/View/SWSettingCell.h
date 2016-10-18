//
//  SWSettingCell.h
//  SWUVCTerminal
//
//  Created by hxf on 8/15/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SWSetting.h"

@interface SWSettingCell : UITableViewCell
@property (weak, nonatomic) IBOutlet UIImageView *settingIcon;
@property (weak, nonatomic) IBOutlet UILabel *settingTitle;
@property (weak, nonatomic) IBOutlet UISwitch *settingSwitch;
@property (weak, nonatomic) IBOutlet UILabel *currentSettingLabel;


@end
