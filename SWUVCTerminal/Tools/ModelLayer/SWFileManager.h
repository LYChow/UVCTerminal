//
//  SWFileManager.h
//  SWUVCTerminal
//
//  Created by hxf on 8/10/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SWUser.h"
#import "SWSetting.h"


typedef enum{
SWVideoFileType = 0,
SWImageFileType,
SWRecordFileType,
SWFavouriteFileType
}SWFileType;

@interface SWFileManager : NSObject

+(instancetype)shareManager;


#pragma mark setting

+(BOOL)saveSetting:(SWSetting *)setting;
+(SWSetting *)getSetting;

#pragma mark -user
//保存User
+(BOOL)saveUser:(SWUser *)user;

+(SWUser *)getUser;

//获取所有登录成功的User
+(NSArray *)getUsers;

#pragma mark 图片/录音/视频
// save
+(BOOL)saveFileFromFileType:(SWFileType)type andFileName:(NSString *)fileName;

// delete
+(BOOL)deleteFileWithFileType:(SWFileType)type fromFileList:(NSArray *)filesModel;

// query
+(NSArray *)getAllFilesFromFileType:(SWFileType)fileType;


#pragma mark -屏幕快照

+(BOOL)saveSnapShots:(UIView *)snapShots toFilePath:(NSString *)filePath;

//保存快照到默认路径,使用默认命名
+(BOOL)saveSnapShots:(UIView *)snapShots;

#pragma mark 视频操作相关
//获取视频的信息根据指定视频文件
+(NSDictionary *)generateVideoInfoWithFile:(NSString *)fileName;

+(BOOL)deleteVideoFiles:(NSArray *)videos;

#pragma mark 录音操作相关
+(NSDictionary *)generateAudioInfoWithFile:(NSString *)fileName;

+(BOOL)deleteAudioFiles:(NSArray *)videos;


@end
