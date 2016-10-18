//
//  SWAudio.m
//  SWUVCTerminal
//
//  Created by hxf on 8/17/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWAudio.h"

@implementation SWAudio

-(SWAudio *)initWithAudioInfo:(NSDictionary *)audioInfo
{
    if (self = [super init]) {
        self.filePath = [audioInfo objectForKey:@"filePath"];
        self.coverPictureImage = [audioInfo objectForKey:@"coverPictureImage"];
        self.name = [audioInfo objectForKey:@"name"];
        self.duration =[audioInfo objectForKey:@"duration"];
        self.fileSize = [audioInfo objectForKey:@"fileSize"];
    }
    return self;
}

@end
