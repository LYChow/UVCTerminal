//
//  SWVideo.m
//  SWUVCTerminal
//
//  Created by hxf on 8/17/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWVideo.h"

@implementation SWVideo

-(SWVideo *)initWithVideoInfo:(NSDictionary *)videoInfo
{
    if (self = [super init]) {
        self.filePath = [videoInfo objectForKey:@"filePath"];
        self.coverPictureImage = [videoInfo objectForKey:@"coverPictureImage"];
        self.name = [videoInfo objectForKey:@"name"];
        self.duration =[videoInfo objectForKey:@"duration"];
        self.fileSize = [videoInfo objectForKey:@"fileSize"];
    }
    return self;
}

@end
