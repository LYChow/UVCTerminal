//
//  CMediaLib.h
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/29.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#ifndef CMediaLib_h
#define CMediaLib_h

#include <string.h>
#include "IMediaLib.h"
#include "MyHandleList.h"
#include "CMediaDevice.h"


namespace MediaLib{

class CMediaLib : public IMediaLib
{
#pragma mark - public functions constructor/destructor destructor
public:
    CMediaLib();
    virtual ~CMediaLib();

#pragma mark - public functions inherits from IMediaDeviceManager
public:
    virtual void RegisterDeviceChangeObserver(IMediaDeviceChangeObserver* pObserver);
    virtual int AddVideoPlayWindow(PWNDOBJ pWnd);
    virtual void RemoveVideoPlayWindow(int iWnd);
    virtual int GetDeviceCount(MediaDeviceType DevType);
    virtual IMediaDevice* GetDevice(MediaDeviceType DevType,int iDev);

#pragma mark - public functions inherits from IMediaLib
public:
    virtual IMediaProcessor* CreateMediaProcessor(int nInCount,int nOutCount,int fmtIn=0,int fmtOut=0);
    virtual IMediaProcessor* CreateMediaProcessor(const MediaProcessorType& mp);
    virtual int EnumConfig(int* pConfigTypes=0,int count=-1);
    virtual IMediaConfig* GetConfig(int cfgType);

#pragma mark - public functions inherits from IObject
public:
    virtual long SetUserData(long lUserData) {
        m_lUserData = lUserData;
        return 0;
    }
    virtual long GetUserData() {
        return m_lUserData;
    }

#pragma mark - public functions own
public:

#pragma mark - protected functions
protected:
    int AsureVideoCapDeviceListInited();
    int AsureAudioCapDeviceListInited();
    int AsureVideoWindowDeviceListInited();
    int AsureAudioOutDeviceListInited();
    int GetVideoCapDeviceCount();
    int GetAudioCapDeviceCount();
    int GetVideoWindowDeviceCount();
    int GetAudioOutDeviceCount();
    IMediaDevice* GetVideoCapDevice(int iDev);
    IMediaDevice* GetAudioCapDevice(int iDev);
    IMediaDevice* GetVideoWindowDevice(int iDev);
    IMediaDevice* GetAudioOutDevice(int iDev);

#pragma mark - protected members
protected:
    IMediaDeviceChangeObserver* m_pMediaDeviceChangeObserver;
    MyHandleList<CMediaDevice*> m_listVideoCapDevice;
    MyHandleList<CMediaDevice*> m_listAudioCapDevice;
    MyHandleList<CMediaDevice*> m_listVideoWindowDevice;
    MyHandleList<CMediaDevice*> m_listAudioOutDevice;
    bool m_bVideoCapDeviceListInited;
    bool m_bAudioCapDeviceListInited;
    bool m_bVideoWindowDeviceListInited;
    bool m_bAudioOutDeviceListInited;
    long m_lUserData;
};


}//end MediaLib
#endif /* CMediaLib_h */
