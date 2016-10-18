//
//  SWSetting.m
//  SWUVCTerminal
//
//  Created by hxf on 8/15/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWSetting.h"
#import "SWFileManager.h"
@interface SWSetting()<NSCoding>

@end

@implementation SWSetting
static SWSetting *setting = nil;
+(SWSetting *)setting
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        //从本地文件unarchiverModel,无数据时使用默认设置
        setting=[SWFileManager getSetting];
        if (!setting) {
            setting = [[SWSetting alloc] init];
            setting.isAutoLogin =@0;
            setting.isAutoAccept =@0;
            setting.isColorFix=@1;
            setting.audioChannel=@1;
            setting.videoFluency=@1;
            setting.lostPacketRecovery=@5;
            setting.smoothSend=@2;
        }
    });
    return setting;
}

-(void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:self.isAutoLogin forKey:@"isAutoLogin"];
    [coder encodeObject:self.isAutoAccept forKey:@"isAutoAccept"];
    [coder encodeObject:self.isColorFix forKey:@"isColorFix"];
    [coder encodeObject:self.audioChannel forKey:@"audioChannel"];
    [coder encodeObject:self.videoFluency forKey:@"videoFluency"];
    [coder encodeObject:self.lostPacketRecovery forKey:@"lostPacketRecovery"];
    [coder encodeObject:self.smoothSend forKey:@"smoothSend"];
}

- (nullable instancetype)initWithCoder:(NSCoder *)decode
{
    if (self = [super init]) {
        self.isAutoLogin = [decode decodeObjectForKey:@"isAutoLogin"];
        self.isAutoAccept = [decode decodeObjectForKey:@"isAutoAccept"];
        self.isColorFix = [decode decodeObjectForKey:@"isColorFix"];
        self.audioChannel = [decode decodeObjectForKey:@"audioChannel"];
        self.videoFluency = [decode decodeObjectForKey:@"videoFluency"];
        self.lostPacketRecovery = [decode decodeObjectForKey:@"lostPacketRecovery"];
        self.smoothSend = [decode decodeObjectForKey:@"smoothSend"];
    }
    return self;
}

-(BOOL)save
{
   return  [SWFileManager saveSetting:self];
}

@end
