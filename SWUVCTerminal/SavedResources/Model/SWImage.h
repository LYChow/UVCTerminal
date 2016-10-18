//
//  SWImage.h
//  SWUVCTerminal
//
//  Created by hxf on 8/17/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SWImage : NSObject
@property(nonatomic,copy) NSString *name;
@property(nonatomic,assign,getter=isChecking) BOOL checking;
@property(nonatomic,assign,getter=isEditing) BOOL editing;
@end
