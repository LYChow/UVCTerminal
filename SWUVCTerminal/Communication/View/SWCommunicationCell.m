//
//  SWCommunicationCell.m
//  SWUVCTerminal
//
//  Created by hxf on 8/12/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWCommunicationCell.h"

@interface SWCommunicationCell()

@property (weak, nonatomic) IBOutlet UILabel *teminalNameLabel;

@end

@implementation SWCommunicationCell

-(void)setTerminalModel:(SWTerminal *)terminalModel
{
    self.teminalNameLabel.text=terminalModel.name;
    if ([terminalModel.type isEqualToString:@"1"])
    {
        self.terminalTypeImage.image=[UIImage imageNamed:@"user"];
    }else if ([terminalModel.type isEqualToString:@"2"])
    {
    self.terminalTypeImage.image=[UIImage imageNamed:@"monitor"];
    }else if ([terminalModel.type isEqualToString:@"3"])
    {
     self.terminalTypeImage.image=[UIImage imageNamed:@"monitor"];
    }
}

@end
