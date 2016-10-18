//
//  SWVideo.h
//  SWUVCTerminal
//
//  Created by hxf on 8/17/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SWVideo : NSObject
@property(nonatomic,copy) NSString *filePath;
@property(nonatomic,copy) UIImage *coverPictureImage;
@property(nonatomic,copy) NSString *name;
@property(nonatomic,copy) NSString *duration;
@property(nonatomic,copy) NSString *fileSize;
@property(nonatomic,assign,getter=isChecking) BOOL checking;
@property(nonatomic,assign,getter=isEditing) BOOL editing;

-(SWVideo *)initWithVideoInfo:(NSDictionary *)videoInfo;

@end
