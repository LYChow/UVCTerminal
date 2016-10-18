//
//  SWLoginViewController.h
//  SWUVCTerminal
//
//  Created by hxf on 8/18/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWViewController.h"

@interface SWLoginViewController : SWViewController

@property(nonatomic,assign) BOOL isAutoLogin;

-(void)OnLogin:(int)ret;

@end
