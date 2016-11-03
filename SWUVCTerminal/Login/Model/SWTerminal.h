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
@property(nonatomic,copy) SWTerminal *chairman;
@property(nonatomic,copy) NSString *confInfo;
@property(nonatomic,copy) NSString *bandInfo;
@property(nonatomic,copy) NSObject *userPtr;
@property(nonatomic,strong) NSNumber *dispOrder;
@property(nonatomic,strong) NSNumber *type;
@property(nonatomic,strong) NSNumber *level;
@property(nonatomic,strong) NSNumber *grant;
@property(nonatomic,strong) NSNumber *state;
@property(nonatomic,strong) NSNumber *netId;
@end
