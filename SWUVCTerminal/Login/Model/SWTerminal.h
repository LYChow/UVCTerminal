//
//  SWTerminal.h
//  SWUVCTerminal
//
//  Created by hxf on 8/19/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface SWTerminal : NSObject
@property(nonatomic,copy) NSString *name;
@property(nonatomic,copy) NSString *aliase;
@property(nonatomic,copy) NSString *friendlyName;
@property(nonatomic,copy) NSString *department;
@property(nonatomic,copy) NSString *domain;
@property(nonatomic,copy) NSString *ipAddress;
@property(nonatomic,copy) NSString *phoneNumber;
@property(nonatomic,copy) NSObject *chairman;
@property(nonatomic,copy) NSString *confInfo;
@property(nonatomic,copy) NSString *bandInfo;
@property(nonatomic,copy) NSObject *userPtr;
@property(nonatomic,copy) NSString *type;
@property(nonatomic,copy) NSNumber *level;
@property(nonatomic,copy) NSNumber *grant;
@property(nonatomic,copy) NSNumber *netId;

@end
