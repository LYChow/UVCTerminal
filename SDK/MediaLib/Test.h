//
//  Test.h
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/29.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#ifndef Test_h
#define Test_h

#ifdef xcode_backup_code

_BEGIN_NAMESPACE_PROJ
_END_NAMESPACE


namespace MediaLib{
}//end MediaLib


#pragma mark - 测试代码
#pragma mark 测试代码结束

namespace MediaLib{
#pragma mark - public functions constructor/destructor destructor
#pragma mark - public functions inherits from
#pragma mark - public functions own
}//end MediaLib

namespace MediaLib{
#pragma mark - public functions constructor/destructor destructor
public:

#pragma mark - public functions inherits from
public:

#pragma mark - public functions inherits from
public:

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
}//end MediaLib


#endif //#ifdef xcode_backup_code

#ifdef DEBUG

#endif //#ifdef DEBUG

//#ifdef DEBUG

enum TestFunIndex {
    TFI_RESET = 0,
    TFI_PREPROCESS_VIDEO,
    TFI_PREPROCESS_AUDIO,
    TFI_CAPTURE,
    TFI_ENCODE,
    TFI_MEDIALIBOBJ,
    TFI_MEDIALIBOBJ_audio,

    TFI_MAX
};

void TestFunPreprocessor(int n, long p1, long p2);
void TestFunCapture(int n, long p1, long p2);
void TestFunEncode(int n, long p1, long p2);
void TestFunMediaLibObj(int n, long p1, long p2);
void TestFunMediaLibObjAudo(int n, long p1, long p2);
void TestFun(int n, long p1=0, long p2=0);


//#endif //#ifdef DEBUG

#endif /* Test_h */
