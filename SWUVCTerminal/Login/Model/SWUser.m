//
//  SWUser.m
//  SWUVCTerminal
//
//  Created by hxf on 8/10/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWUser.h"
#import "SWFileManager.h"
#import <objc/runtime.h>
@interface SWUser()<NSCoding>

@end

@implementation SWUser

static SWUser *_user = nil;
+(SWUser *)user
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        
        //从本地文件unarchiverModel,无数据时使用默认值
        _user = [SWFileManager getUser];
        
        if (!_user) {
            //默认设置
            _user = [[SWUser alloc] init];
            _user.avatarFileName=@"";
            _user.userName =@"";
            _user.ipAddress=@"";
            _user.password=@"";
            _user.isRememberPassword=@0;
            
        }
    });
    return _user;
}


-(void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:self.avatarFileName forKey:@"avatarFileName"];
    [coder encodeObject:self.userName forKey:@"userName"];
    [coder encodeObject:self.ipAddress forKey:@"ipAddress"];
    [coder encodeObject:self.password forKey:@"password"];
    [coder encodeObject:self.isRememberPassword forKey:@"isRememberPassword"];
    
}

- (nullable instancetype)initWithCoder:(NSCoder *)decode
{
    if (self = [super init]) {
        self.avatarFileName = [decode decodeObjectForKey:@"avatarFileName"];
        self.userName = [decode decodeObjectForKey:@"userName"];
        self.ipAddress = [decode decodeObjectForKey:@"ipAddress"];
        self.password = [decode decodeObjectForKey:@"password"];
        self.isRememberPassword = [decode decodeObjectForKey:@"isRememberPassword"];
    }

    
    return self;
}

-(BOOL)save
{
    return [SWFileManager saveUser:self];
}

@end
