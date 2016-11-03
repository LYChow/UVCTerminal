//
//  SwCameraController.h
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/30.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#ifndef SwCameraController_h
#define SwCameraController_h

//struct COM_DataCallbackFrame {
//    unsigned char* buf;
//    int size;
//    int width;
//    int height;
//};
enum COM_DataCallbackCode {
    COM_DCC_CAPTUREFRAME = 0,   // param: CVImageBufferRef
    COM_DCC_FINISHPICKIMAGE,    // param: UIImage*
};
typedef void (*PCSwCameraOperationManager_DataCallback)(int code, long param, long user);

class CSwCameraOperationManager
{
#pragma mark - public functions
public:
    CSwCameraOperationManager();
    virtual ~CSwCameraOperationManager();

    bool Init(int w, int h, int frameduration_value, int frameduration_timescale, PCSwCameraOperationManager_DataCallback cb, long cbuser, void* UIImageView_preview, bool back=FALSE);
    void SetMinFrameDuration(int value, int timescale);
    bool SwitchCamera();

    void OnFinishPickImage(void* UIImage);
    void OnCaptureFrame(void* _CVImageBufferRef);

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    void* m_pSwCameraOperationManager;
    PCSwCameraOperationManager_DataCallback m_pDataCallback;
    long m_lDataCallbackUser;
};


#endif /* SwCameraController_h */
