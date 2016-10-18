//
//  Marco.h
//  SWUVCTerminal
//
//  Created by hxf on 8/10/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#ifndef Marco_h
#define Marco_h


#endif /* Marco_h */

/*文件操作*/
#define SWImageFolderName       @"images"
#define SWVideoFolderName       @"videos"
#define SWRecordFolderName      @"records"
#define SWFavouriteFolderName   @"favourites"


#define SWDocumentPath [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject]

//根据图片文件名获取图片的绝对路径
#define SWGetAbsolutelyImagePathWithFileName(fileName) [[SWDocumentPath stringByAppendingPathComponent:SWImageFolderName] stringByAppendingPathComponent:fileName]
//根据视频文件名获取图片的绝对路径
#define SWGetAbsolutelyVideoPathWithFileName(fileName)  [[SWDocumentPath stringByAppendingPathComponent:SWVideoFolderName] stringByAppendingPathComponent:fileName]
//根据录音文件名获取图片的绝对路径
#define SWGetAbsolutelyRecordPathWithFileName(fileName) [[SWDocumentPath stringByAppendingPathComponent:SWRecordFolderName] stringByAppendingPathComponent:fileName]


/*收到登录状态的通知*/
#define SWOnLoginNotification            @"SWOnLoginNotification"
/*头像改变的通知*/
#define SWChangeUserAvatarNotification   @"SWChangeUserAvatarNotification"
/*peer stop call notification*/
#define SWPeerStopCallNotification       @"SWPeerStopCallNotification"
#define SWUserInfoUpdateNotification     @"SWUserInfoUpdateNotification"


#pragma Mark -宽高比例尺寸等相关的设置

#define SWScreenWidth [UIScreen mainScreen].bounds.size.width
#define SWScreenHeight [UIScreen mainScreen].bounds.size.height

#define SWCellHeight  66

/*leftView所占window的比例*/
#define leftViewScale 0.75
/*popSettingView宽高比例,目的是对素材等比例缩放*/
#define SWPopSettingViewAspectRatio  1.8607
/*popSettingView距离左边缘的距离*/
#define SWPopSettingViewPaddingToLeftEdge  30


#pragma mark -系统相关设置

/*** 当前系统版本判断 ***/
#define iOS(version) ([UIDevice currentDevice].systemVersion.doubleValue >= (version))

/*打印日志*/
#ifdef DEBUG
#define SWLog(...) NSLog(__VA_ARGS__)
#else
#define SWLog(...)
#endif

#define SWLogFunc(args) SWLog(@"%s--%@",__func__,args);

#pragma mark -颜色值

/*** 颜色 ***/
#define SWColorA(r, g, b, a) [UIColor colorWithRed:(r)/255.0 green:(g)/255.0 blue:(b)/255.0 alpha:(a)/255.0]

#define SWColor(r, g, b) SWColorA((r), (g), (b), 255)

#define SWRandomColor SWColorA(arc4random_uniform(255), arc4random_uniform(255), arc4random_uniform(255))
