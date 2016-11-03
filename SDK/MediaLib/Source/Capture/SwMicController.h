//
//  SwMicController.h
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/30.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#ifndef SwMicController_h
#define SwMicController_h

struct MOM_DataCallbackFrame {
    unsigned char* pcm8k16bit;
    int len;
};
enum MOM_DataCallbackCode {
    MOM_DCC_CAPTUREFRAME = 0,   // param: MOM_DataCallbackFrame*
};
typedef void (*PCSwMicOperationManager_DataCallback)(int code, long param, long user);

class CSwMicOperationManager
{
#pragma mark - public functions
public:
    CSwMicOperationManager();
    virtual ~CSwMicOperationManager();

    bool Init(PCSwMicOperationManager_DataCallback cb, long cbuser);
	bool Fini();

    void OnCaptureFrame(unsigned char* pcm8k16bit, int len);

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    void* m_pSwMicOperationManager;
    PCSwMicOperationManager_DataCallback m_pDataCallback;
    long m_lDataCallbackUser;
};


#endif /* SwMicController_h */
