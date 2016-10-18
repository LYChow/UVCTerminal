//
//  SWUser.h
//  SWUVCTerminal
//
//  Created by hxf on 8/10/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SWUser : NSObject

+(SWUser *)user;

@property(nonatomic,copy) NSString *userName;
@property(nonatomic,copy) NSString *ipAddress;
@property(nonatomic,copy) NSString *password;
@property(nonatomic,strong) NSNumber *isRememberPassword;
@property(nonatomic,copy)  NSString *avatarFileName;

-(BOOL)save;
@end
