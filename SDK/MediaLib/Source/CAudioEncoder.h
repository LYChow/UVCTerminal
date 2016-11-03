//
//  CAudioEncoder.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/1/21.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef CAudioEncoder_h
#define CAudioEncoder_h

#include "IDataDelegate.h"
#include "AudioConverter.h"

namespace MediaLib{

class CAudioEncoder : public IDataDelegate {
#pragma mark - public functions constructor/destructor destructor
public:
    CAudioEncoder();
    virtual ~CAudioEncoder();

#pragma mark - public functions inherits from IDataDelegate
public:
    virtual int PutData(const char* data, int len, long mediaid, int format);

#pragma mark - public functions own
public:
    static void CAudioConverter_DataCallback(int code, long param, long user);
    void OnCAudioConverter_DataCallback(int code, long param);

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    CAudioConverter* m_pCAudioConverter;
};

}//end MediaLib

#endif /* CAudioEncoder_h */
