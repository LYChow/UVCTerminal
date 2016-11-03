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
    
    //2 	指挥/会议终端
    //3 	监控前端
    unsigned int type = [terminalModel.type unsignedIntValue];
    if (type == 2)
    {
    self.terminalTypeImage.image=[UIImage imageNamed:@"user"];
    }
    else if(type == 3)
    {
    self.terminalTypeImage.image=[UIImage imageNamed:@"monitor"];
    }
}

@end
