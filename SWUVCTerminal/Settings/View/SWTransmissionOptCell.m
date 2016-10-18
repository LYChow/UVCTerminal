//
//  SWTransmissionOptCell.m
//  SWUVCTerminal
//
//  Created by hxf on 8/16/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWTransmissionOptCell.h"
#import "SWSetting.h"

@interface SWTransmissionOptCell()
- (IBAction)lostPacketRecovery:(UISlider *)slider;
@end

@implementation SWTransmissionOptCell

- (void)awakeFromNib {
    [super awakeFromNib];
    self.selectionStyle=UITableViewCellSelectionStyleNone;
    //初始化slider时,从文件中获取初始值
    
    SWSetting *setting = [SWSetting setting];
    
    self.transmissionOptSlider.value=[setting.lostPacketRecovery floatValue];
    self.recoveryPercentLabel.text=[[NSString stringWithFormat:@"%@",setting.lostPacketRecovery] stringByAppendingString:@"%"];
    
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (IBAction)lostPacketRecovery:(UISlider *)slider
{
    NSInteger percent = (int)slider.value;
    self.recoveryPercentLabel.text=[[NSString stringWithFormat:@"%i",percent] stringByAppendingString:@"%"];
    
    SWSetting *setting = [SWSetting setting];
    setting.lostPacketRecovery=[NSNumber numberWithInteger:percent];
    [setting save];
    
}
@end
