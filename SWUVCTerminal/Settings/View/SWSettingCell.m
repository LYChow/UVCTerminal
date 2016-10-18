//
//  SWSettingCell.m
//  SWUVCTerminal
//
//  Created by hxf on 8/15/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWSettingCell.h"

@implementation SWSettingCell

- (void)awakeFromNib {
    [super awakeFromNib];

    self.selectionStyle=UITableViewCellSelectionStyleNone;
    [self.settingSwitch addTarget:self action:@selector(switchStatusChange:) forControlEvents:UIControlEventValueChanged];
}

-(void)switchStatusChange:(UISwitch *)settingSwitch
{
    SWSetting *setting = [SWSetting setting];
    
    if ([self.settingTitle.text isEqualToString:@"自动登录"])
    {
        setting.isAutoLogin=settingSwitch.isOn?@1:@0;
    }
    else if ([self.settingTitle.text isEqualToString:@"自动应答"])
    {
       setting.isAutoAccept=settingSwitch.isOn?@1:@0;
    }
    else if ([self.settingTitle.text isEqualToString:@"色彩修正"])
    {
         setting.isColorFix=settingSwitch.isOn?@1:@0;
    }
    [setting save];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
