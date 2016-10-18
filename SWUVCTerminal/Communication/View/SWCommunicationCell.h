//
//  SWCommunicationCell.h
//  SWUVCTerminal
//
//  Created by hxf on 8/12/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SWTerminal.h"

@interface SWCommunicationCell : UITableViewCell

@property (weak, nonatomic) IBOutlet UIImageView *terminalTypeImage;

@property(nonatomic,strong) SWTerminal *terminalModel;

@end
