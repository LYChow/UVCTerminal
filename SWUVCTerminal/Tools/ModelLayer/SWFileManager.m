//
//  SWFileManager.m
//  SWUVCTerminal
//
//  Created by hxf on 8/10/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWFileManager.h"
#import "SWImage.h"
#import "SWAudio.h"
#import "SWVideo.h"
#import <AVKit/AVKit.h>
#import <AVFoundation/AVFoundation.h>

#define userFileName @"/user.file"
#define settingFileName @"/setting.file"




@interface SWFileManager()

@end


@implementation SWFileManager

static SWFileManager *manager=nil;

+(instancetype)shareManager
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        manager = [[SWFileManager alloc] init];
    });
    return manager;
}

#pragma mark setting related
+(BOOL)saveSetting:(SWSetting *)setting
{
    //先从doc下获取存储用户信息的文件,unarchiver 成数组
    NSString *settingFilePath = [SWDocumentPath stringByAppendingString:settingFileName];
    
    //不存在文件,第一次创建
    if (![[NSFileManager defaultManager] fileExistsAtPath:settingFilePath]) {
        if (![[NSFileManager defaultManager] createFileAtPath:settingFilePath contents:nil attributes:nil]) {
            
            //第一次创建setting 初始化
            return NO;
        }
        
    }
    return [NSKeyedArchiver archiveRootObject:setting toFile:settingFilePath];
}
+(SWSetting *)getSetting
{
   NSString *settingFilePath = [SWDocumentPath stringByAppendingString:settingFileName];
    
  if (![[NSFileManager defaultManager] fileExistsAtPath:settingFilePath]) {
      
    
      
        if ([[NSFileManager defaultManager] createFileAtPath:settingFilePath contents:nil attributes:nil]) {
            
            //第一次创建setting 初始化
            return nil;
        }
        
    }
    if ([[NSKeyedUnarchiver unarchiveObjectWithFile:settingFilePath] isKindOfClass:[SWSetting class]]) {
         return [NSKeyedUnarchiver unarchiveObjectWithFile:settingFilePath];
    }
    return nil;
}


#pragma mark -user related
+(BOOL)saveUser:(SWUser *)user
{
    //先从doc下获取存储用户信息的文件,unarchiver 成数组
    NSString *userFilePath = [SWDocumentPath stringByAppendingString:userFileName];
    
    //不存在文件,第一次创建
    if (![[NSFileManager defaultManager] fileExistsAtPath:userFilePath]) {
        if (![[NSFileManager defaultManager] createFileAtPath:userFilePath contents:nil attributes:nil]) {
            //第一次创建setting 初始化
            return NO;
        }
        
    }
    return [NSKeyedArchiver archiveRootObject:user toFile:userFilePath];

}


+(SWUser *)getUser
{
    NSString *userFilePath = [SWDocumentPath stringByAppendingString:userFileName];
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:userFilePath]) {
        
        
        
        if ([[NSFileManager defaultManager] createFileAtPath:userFilePath contents:nil attributes:nil]) {
        return nil;
        }
        
    }
    
    if ([[NSKeyedUnarchiver unarchiveObjectWithFile:userFilePath] isKindOfClass:[SWUser class]]) {
        return [NSKeyedUnarchiver unarchiveObjectWithFile:userFilePath];
    }
    return nil;
}

//获取所有登录成功的User
+(NSArray *)getUsers
{
    //    //先从doc下获取存储用户信息的文件,unarchiver 成数组
    //    NSString *userFilePath = [documentPath stringByAppendingString:userFileName];
    //
    //    //不存在文件,第一次创建
    //    if (![[NSFileManager defaultManager] fileExistsAtPath:userFilePath]) {
    //        if ([[NSFileManager defaultManager] createDirectoryAtPath:userFilePath withIntermediateDirectories:NO attributes:nil error:nil]) {
    //            //把模型加入可变数组中,写入文件
    //            NSMutableArray *array = [NSMutableArray array];
    //            [array addObject:user];
    //
    //            [SWFileManager writeDataToFile:userFilePath data:array];
    //        }
    //
    //    }
    //    else
    //    {
    //        if ([[SWFileManager getDataFromFile:userFilePath] isKindOfClass:[NSMutableArray class]]) {
    //            NSMutableArray *array = [SWFileManager getDataFromFile:userFilePath];
    //            //获取所有用户与当前用户做比较
    //
    //            BOOL isNewUser =YES;
    //            for (SWUser *u in array) {
    //                if ([u.userName isEqualToString:user.userName] &&![u.password isEqualToString:user.password]) {
    //                    isNewUser = NO;
    //                    //修改密码,更新本地存储的用户信息
    //                    u.password=user.password;
    //                    return [SWFileManager writeDataToFile:userFilePath data:array];
    //                }
    //            }
    //            if (isNewUser) {
    //                [array addObject:user];
    //                return [SWFileManager writeDataToFile:userFilePath data:array];
    //            }
    //        }
    //    }
    //    
    //    return YES;
    return nil;
}

#pragma mark - video related
+(NSDictionary *)generateVideoInfoWithFile:(NSString *)fileName
{
    NSString *filePath =  SWGetAbsolutelyVideoPathWithFileName(fileName);
    NSURL *url =  [NSURL fileURLWithPath:filePath];
    UIImage *coverImage =[self videoFrameWithFilePath:url AtSecond:1];
    NSMutableDictionary *videoInfos = [NSMutableDictionary dictionary];

    //文件的绝对路径
    [videoInfos setObject:filePath forKey:@"filePath"];
    
    //获取视频文件显示的名字
    [videoInfos setObject:[[fileName componentsSeparatedByString:@"."] firstObject] forKey:@"name"];
    
    //获取视频封面图片
    [videoInfos setObject:coverImage forKey:@"coverPictureImage"];
    
    //视频的大小
    [videoInfos setObject:[self fileSizeAtPath:filePath] forKey:@"fileSize"];
    
    //视频的长度 格式化
    [videoInfos setObject:[self formatFileTotalDurationWithFile:url] forKey:@"duration"];
    
    
    
    return videoInfos;
}


+(BOOL)deleteVideoFiles:(NSArray *)videos
{
    BOOL isDeleteSuccess = YES;
    for (SWVideo *video in videos)  {
        NSString *filePath = video.filePath;
        
        if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
            NSError *error;
          BOOL result =  [[NSFileManager defaultManager] removeItemAtPath:filePath error:&error];
            if (error) SWLogFunc(error);
            if (result==NO) isDeleteSuccess= NO;
        }
    }
    return isDeleteSuccess;
}

#pragma mark audio related
+(NSDictionary *)generateAudioInfoWithFile:(NSString *)fileName
{
    NSString *filePath =  SWGetAbsolutelyRecordPathWithFileName(fileName);
    NSURL *url =  [NSURL fileURLWithPath:filePath];
    UIImage *coverImage =[UIImage imageNamed:@"record_Placehoder"];
    NSMutableDictionary *audioInfos = [NSMutableDictionary dictionary];
    
    //文件的绝对路径
    [audioInfos setObject:filePath forKey:@"filePath"];
    
    //获取视频文件显示的名字
    [audioInfos setObject:[[fileName componentsSeparatedByString:@"."] firstObject] forKey:@"name"];
    
    //获取视频封面图片
    [audioInfos setObject:coverImage forKey:@"coverPictureImage"];
    
    //视频的大小
    [audioInfos setObject:[self fileSizeAtPath:filePath] forKey:@"fileSize"];
    
    //视频的长度 格式化
    [audioInfos setObject:[self formatFileTotalDurationWithFile:url] forKey:@"duration"];
    
    
    
    return audioInfos;
}

+(BOOL)deleteAudioFiles:(NSArray *)videos
{
    BOOL isDeleteSuccess = YES;
    for (SWAudio *audio in videos)  {
        NSString *filePath = audio.filePath;
        
        if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
            NSError *error;
            BOOL result =  [[NSFileManager defaultManager] removeItemAtPath:filePath error:&error];
            if (error) SWLogFunc(error);
            if (result==NO) isDeleteSuccess= NO;
        }
    }
    return isDeleteSuccess;
}


#pragma mark -图片/录音/视频 操作
+(BOOL)saveFileFromFileType:(SWFileType)type andFileName:(NSString *)fileName
{

    //主目录document
    //二级目录  用户id
    //三级目录  fileType
    //四级目录  fileName
    return YES;
    
}

//delete
+(BOOL)deleteFileWithFileType:(SWFileType)type fromFileList:(NSArray *)filesModel
{
    BOOL deleteAllFilesSuccess = YES;
    switch (type) {
        case SWImageFileType:
            for (SWImage *imageModel in filesModel) {
                //根据模型找出路径,根据路径删除文件
                NSString *filePath =[[SWDocumentPath stringByAppendingPathComponent:SWImageFolderName] stringByAppendingPathComponent:imageModel.name];
                
                 BOOL result =[self deleteFileWithAbsoutelyPath:filePath];
                 if (result==NO) deleteAllFilesSuccess = NO;
                
            }
            break;
        case SWVideoFileType:
            for (SWVideo *videoModel in filesModel) {
                //根据模型找出路径,根据路径删除文件
                NSString *filePath =videoModel.filePath;
                
                BOOL result =[self deleteFileWithAbsoutelyPath:filePath];
                if (result==NO) deleteAllFilesSuccess = NO;
                
            }
            break;
        case SWRecordFileType:
            for (SWAudio *audioModel in filesModel) {
                //根据模型找出路径,根据路径删除文件
                NSString *filePath =audioModel.filePath;
                
                BOOL result =[self deleteFileWithAbsoutelyPath:filePath];
                if (result==NO) deleteAllFilesSuccess = NO;
                
            }
            break;
        default:
            break;
    }
    return deleteAllFilesSuccess;
}

+(BOOL)deleteFileWithAbsoutelyPath:(NSString *)filePath
{
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        NSError *error;
        BOOL result =  [[NSFileManager defaultManager]removeItemAtPath:filePath error:&error];
        if (error) {
            SWLogFunc(error);
        }
        return result;
    }
    return NO;
}


// query
+(NSArray *)getAllFilesFromFileType:(SWFileType)fileType
{
    //获取文件所在文件夹目录
    NSString *fileDirectoriesPath;
    NSError *error;
    switch (fileType) {
        case SWImageFileType:
            fileDirectoriesPath = [SWDocumentPath stringByAppendingPathComponent:SWImageFolderName];
            break;
        case SWVideoFileType:
            fileDirectoriesPath = [SWDocumentPath stringByAppendingPathComponent:SWVideoFolderName];
            break;
        case SWRecordFileType:
            fileDirectoriesPath = [SWDocumentPath stringByAppendingPathComponent:SWRecordFolderName];
            break;
        case SWFavouriteFileType:
            fileDirectoriesPath = [SWDocumentPath stringByAppendingPathComponent:SWFavouriteFolderName];
            break;
        default:
            break;
    }
    
    //遍历文件夹下文件
    NSArray *items = [[NSFileManager defaultManager]contentsOfDirectoryAtPath:fileDirectoriesPath error:&error];
    if (error) SWLogFunc(error);
    
    //把文件夹目录下指定文件类型名添加到数组中
    NSMutableArray *filterArray =[NSMutableArray array];
    for (NSString *fileName in items) {
        
        switch (fileType) {
            case SWImageFileType:
                if ([fileName hasSuffix:@".jpg"]) [filterArray addObject:fileName];
                break;
            case SWVideoFileType:
                if ([fileName hasSuffix:@".mp4"]) [filterArray addObject:fileName];
                break;
            case SWRecordFileType:
                if ([fileName hasSuffix:@".m4a"]) [filterArray addObject:fileName];
                break;
            case SWFavouriteFileType:
                fileDirectoriesPath = [SWDocumentPath stringByAppendingPathComponent:SWFavouriteFolderName];
                break;
            default:
                break;
        }
    }
    return filterArray;
}


#pragma mark -快照保存
//保存到制定文件路径
+(BOOL)saveSnapShots:(UIView *)snapShots toFilePath:(NSString *)filePath
{
    UIGraphicsBeginImageContextWithOptions(snapShots.bounds.size, NO, 0);
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    [snapShots.layer renderInContext:ctx];
    UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    NSData *data = UIImagePNGRepresentation(newImage);
    BOOL success =[data writeToFile:filePath atomically:YES];

  
    return success;
}

//默认路径,默认命名
+(BOOL)saveSnapShots:(UIView *)snapShots
{
    UIGraphicsBeginImageContextWithOptions(snapShots.bounds.size, NO, 0);
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    [snapShots.layer renderInContext:ctx];
    UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    NSData *data = UIImageJPEGRepresentation(newImage, 0.8);
    
    //命名规则:SW-PeerTerminalName-time.jpg(其中time精确到毫秒形如:20160909142538213)
    
    NSString *peerTeminalName =@"fwy3";
    NSString *dateString = [self getCurrentDateStringFromDateFormat:@"YYYYMMddhhmmssSSS"];
    NSString *fileName=[NSString stringWithFormat:@"SW-%@-%@.jpg",peerTeminalName,dateString];
    
    
    NSString *directoriesPath =[SWDocumentPath stringByAppendingPathComponent:SWImageFolderName];
    if (![[NSFileManager defaultManager] fileExistsAtPath:directoriesPath]) {
        NSError *error;
        [[NSFileManager defaultManager] createDirectoryAtPath:directoriesPath withIntermediateDirectories:NO attributes:nil error:&error];
        if (error) SWLogFunc(error);
    }
    NSString *filePath = [directoriesPath stringByAppendingPathComponent:fileName];
    
    return [data writeToFile:filePath atomically:YES];
}



#pragma mark -common write/read file operation
+(BOOL)writeDataToFile:(NSString *)path data:(id)data
{
    if ([[NSFileManager defaultManager] fileExistsAtPath:path])
    {
        [[NSFileManager defaultManager] removeItemAtPath:path error:nil];
    }
    
    //Array
    if ([data isKindOfClass:[NSMutableArray class]])
    {
        NSMutableArray * arrData = (NSMutableArray *)data;
        NSMutableArray * writeArr = [NSMutableArray arrayWithCapacity:arrData.count];
        for (id object in arrData)
        {
            if (![object respondsToSelector:@selector(encodeWithCoder:)])
            {
                return NO;
            }
            NSData * mdata = [NSKeyedArchiver archivedDataWithRootObject:object];
            [writeArr addObject:mdata];
        }
        return [writeArr writeToFile:path atomically:NO];
    }
    //Dictionary
    if ([data isKindOfClass:[NSDictionary class]] || [data isKindOfClass:[NSMutableDictionary class]])
    {
        NSDictionary * dict = (NSDictionary *)data;
        NSMutableDictionary * newDict = [NSMutableDictionary dictionary];
        NSArray * dictKeys = [dict allKeys];
        for (id key in dictKeys)
        {
            id object = [dict objectForKey:key];
            if (![object respondsToSelector:@selector(encodeWithCoder:)])
            {
                return NO;
            }
            NSData * mdata = [NSKeyedArchiver archivedDataWithRootObject:object];
            [newDict setValue:mdata forKey:key];
        }
        return [newDict writeToFile:path atomically:YES];
    }
    //model
    if (![data respondsToSelector:@selector(encodeWithCoder:)])
    {
        return NO;
    }
    NSData * mdata = [NSKeyedArchiver archivedDataWithRootObject:data];
    return [mdata writeToFile:path atomically:YES];
}

+(id)getDataFromFile:(NSString *)path
{
    if (![[NSFileManager defaultManager] fileExistsAtPath:path])
    {
        return nil;
    }
    NSData * data = [NSData dataWithContentsOfFile:path];
    id objcet = [NSKeyedUnarchiver unarchiveObjectWithData:data];
    return objcet;
}


#pragma mark private Methods
//获取视频的封面图片
+(UIImage *)videoFrameWithFilePath:(NSURL *)fileUrl AtSecond:(NSTimeInterval)timeBySecond
{
    AVURLAsset *urlAsset = [AVURLAsset assetWithURL:fileUrl];
    AVAssetImageGenerator *imageGenerator = [AVAssetImageGenerator assetImageGeneratorWithAsset:urlAsset];
    //创建视频缩略图的时间，第一个参数是视频第几秒，第二个参数是每秒帧数
    CMTime time = CMTimeMake(timeBySecond, 10);
    CMTime actualTime;//实际生成视频缩略图的时间
    NSError *error = nil;//错误信息
    //使用对象方法，生成视频缩略图，注意生成的是CGImageRef类型，如果要在UIImageView上显示，需要转为UIImage
    CGImageRef cgImage = [imageGenerator copyCGImageAtTime:time
                                                actualTime:&actualTime
                                                     error:&error];
    if (error) {
        NSLog(@"截取视频缩略图发生错误，错误信息：%@",error.localizedDescription);
        return nil;
    }
    //CGImageRef转UIImage对象
    UIImage *image = [UIImage imageWithCGImage:cgImage];
    //记得释放CGImageRef
    CGImageRelease(cgImage);
    
    
    return image;
}
//获取视频文件的大小
+(NSString *) fileSizeAtPath:(NSString*) filePath{
    
    NSFileManager* manager = [NSFileManager defaultManager];
    
    if ([manager fileExistsAtPath:filePath]){
        
        long long fileSize =  [[manager attributesOfItemAtPath:filePath error:nil] fileSize];
        if (fileSize>(1024.0*1024.0)) {
            return [NSString stringWithFormat:@"%.lld M",(fileSize/(1024*1024))];
        }
        else
        {
             return [NSString stringWithFormat:@"%.lld KB",(fileSize/1024)];
        }
    }
    
    return @"";
    
}
//根据音视频文件长度格式化成时分秒格式
+(NSString *)formatFileTotalDurationWithFile:(NSURL *)fileUrl
{
    AVURLAsset* audioAsset =[AVURLAsset URLAssetWithURL:fileUrl options:nil];
    CMTime audioDuration = audioAsset.duration;
    NSInteger audioDurationSeconds =CMTimeGetSeconds(audioDuration);
    
    
    NSString *str_hour = [NSString stringWithFormat:@"%02ld",audioDurationSeconds/3600];
    //format of minute
    NSString *str_minute = [NSString stringWithFormat:@"%02ld",(audioDurationSeconds%3600)/60];
    //format of second
    NSString *str_second = [NSString stringWithFormat:@"%02ld",audioDurationSeconds%60];
    //format of time
    NSString *format_time = [NSString stringWithFormat:@"%@:%@:%@",str_hour,str_minute,str_second];
    return format_time;
}

//日期格式化字符串
+(NSString *)getCurrentDateStringFromDateFormat:(NSString *)dateFormat
{
    NSDateFormatter * formatter = [[NSDateFormatter alloc ] init];
    [formatter setDateFormat:dateFormat];
    NSString *date =  [formatter stringFromDate:[NSDate date]];
    NSString *timeLocal = [[NSString alloc] initWithFormat:@"%@", date];
    return timeLocal;
}

@end
