//
//  SWSetting.h
//  SWUVCTerminal
//
//  Created by hxf on 8/15/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SWSetting : NSObject
{

}
+(SWSetting *)setting;

@property(nonatomic,strong) NSNumber *isAutoLogin;
@property(nonatomic,strong) NSNumber *isAutoAccept;
@property(nonatomic,strong) NSNumber *isColorFix;
@property(nonatomic,strong) NSNumber *audioChannel;
@property(nonatomic,strong) NSNumber *videoFluency;
@property(nonatomic,strong) NSNumber *lostPacketRecovery;
@property(nonatomic,strong) NSNumber *smoothSend;

-(BOOL)save;

@end
