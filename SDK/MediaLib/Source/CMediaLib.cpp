//
//  CMediaLib.cpp
//  SwIMediaLib
//
//  Created by 李招华 on 15/12/29.
//  Copyright © 2015年 华纬讯. All rights reserved.
//

#include <stdio.h>

#include "CMediaLib.h"
#include "CMediaProcessor.h"
#include "MyHandleList.h"

namespace MediaLib{

#pragma mark - Global
IMediaLib* CreateMediaLib(void* pParam/*=0*/)
{
    return new CMediaLib;
}

#pragma mark - public functions constructor/destructor destructor
CMediaLib::CMediaLib()
{
    m_pMediaDeviceChangeObserver = NULL;
    m_bVideoCapDeviceListInited = false;
    m_bAudioCapDeviceListInited = false;
    m_bVideoWindowDeviceListInited = false;
    m_bAudioOutDeviceListInited = false;
    m_lUserData = 0;
}

CMediaLib::~CMediaLib()
{
}

#pragma mark - public functions inherits from IMediaDeviceManager
void CMediaLib::RegisterDeviceChangeObserver(IMediaDeviceChangeObserver* pObserver)
{
    m_pMediaDeviceChangeObserver = pObserver; // 暂不实现 2016年01月11日 星期一
}

int CMediaLib::AddVideoPlayWindow( PWNDOBJ hwnd)
{
    char szName[128] = {0};
#ifdef DEBUG
    static int index = 0;
    if (index == 0) {
        strncpy(szName, "preview", 7);
    }
    index++;
#endif
    CMediaDevice* pWindowDevice = new CMediaDeviceVideoWindow(szName, (long)hwnd);
    if (m_listVideoWindowDevice.AddTailHandle(pWindowDevice))
        return m_listVideoWindowDevice.GetIndex(pWindowDevice);
    
    return -1;
}

void CMediaLib::RemoveVideoPlayWindow(int iDev)
{
    m_listVideoWindowDevice.DeleteHandle(m_listVideoWindowDevice.GetHandleAt(iDev));
}

int CMediaLib::GetDeviceCount(MediaDeviceType DevType)
{
    int nCount = 0;

    switch (DevType) {
        case MediaDeviceUnknown:
            break;

        case MediaDeviceVideoCap:
            return GetVideoCapDeviceCount();
            break;

        case MediaDeviceAudioCap:
            return GetAudioCapDeviceCount();
            break;

        case MediaDeviceVideoWindow:
            return GetVideoWindowDeviceCount();
            break;

        case MediaDeviceAudioOut:
            return GetAudioOutDeviceCount();
            break;

        default:
            break;
    }

    return nCount;
}

IMediaDevice* CMediaLib::GetDevice(MediaDeviceType DevType,int iDev)
{
    IMediaDevice* pDevice = NULL;

    switch (DevType) {
        case MediaDeviceUnknown:
            break;

        case MediaDeviceVideoCap:
            return GetVideoCapDevice(iDev);
            break;

        case MediaDeviceAudioCap:
            return GetAudioCapDevice(iDev);
            break;

        case MediaDeviceVideoWindow:
            return GetVideoWindowDevice(iDev);
            break;

        case MediaDeviceAudioOut:
            return GetAudioOutDevice(iDev);
            break;

        default:
            break;
    }

    return pDevice;
}

#pragma mark - public functions inherits from IMediaLib
IMediaProcessor* CMediaLib::CreateMediaProcessor(int nInCount,int nOutCount,int fmtIn/*=0*/,int fmtOut/*=0*/)
{
    CMediaProcessor* pr = new CMediaProcessor(nInCount, nOutCount, fmtIn, fmtOut);
    return pr;
}

IMediaProcessor* CMediaLib::CreateMediaProcessor(const MediaProcessorType& mp)
{
    return CreateMediaProcessor(mp.nIn, mp.nOut, mp.fmtIn, mp.fmtOut);
}

int CMediaLib::EnumConfig(int* pConfigTypes/*=0*/,int count/*=-1*/)
{ // ?????
    return -1;
}

IMediaConfig* CMediaLib::GetConfig(int cfgType)
{ // ?????
    return NULL;
}


#pragma mark - public functions own

#pragma mark - class CMediaLib - protected functions
int CMediaLib::AsureVideoCapDeviceListInited()
{
    if (!m_bVideoCapDeviceListInited) {
        m_bVideoCapDeviceListInited = true;
        CMediaDevice* pDevice = new CMediaDeviceVideoCap("前置摄像头", __device_id_VideoCap_front);
        if (!m_listVideoCapDevice.AddTailHandle(pDevice)) {
            delete pDevice;
        }
        pDevice = new CMediaDeviceVideoCap("后置摄像头", __device_id_VideoCap_back);
        if (!m_listVideoCapDevice.AddTailHandle(pDevice)) {
            delete pDevice;
        }
    }

    return m_listVideoCapDevice.GetCount();
}

int CMediaLib::AsureAudioCapDeviceListInited()
{
    if (!m_bAudioCapDeviceListInited) {
        m_bAudioCapDeviceListInited = true;
        CMediaDevice* pDevice = new CMediaDeviceAudioCap("系统麦克风", 0);
        if (!m_listAudioCapDevice.AddTailHandle(pDevice)) {
            delete pDevice;
        }
    }

    return m_listAudioCapDevice.GetCount();
}

int CMediaLib::AsureVideoWindowDeviceListInited()
{
    if (!m_bVideoWindowDeviceListInited) {
        m_bVideoWindowDeviceListInited = true;
    }
    return m_listVideoWindowDevice.GetCount();
}

int CMediaLib::AsureAudioOutDeviceListInited()
{
    if (!m_bAudioOutDeviceListInited) {
        m_bAudioOutDeviceListInited = true;
        CMediaDevice* pDevice = new CMediaDeviceAudioOut("系统扬声器", 0);
        if (!m_listAudioOutDevice.AddTailHandle(pDevice)) {
            delete pDevice;
        }
    }

    return m_listAudioOutDevice.GetCount();
}

int CMediaLib::GetVideoCapDeviceCount()
{
    return AsureVideoCapDeviceListInited(); // camera: front/back
}

int CMediaLib::GetAudioCapDeviceCount()
{
    return AsureAudioCapDeviceListInited(); // mic: iOS mic
}

int CMediaLib::GetVideoWindowDeviceCount()
{
    return AsureVideoWindowDeviceListInited(); // window: render
}

int CMediaLib::GetAudioOutDeviceCount()
{
    return AsureAudioOutDeviceListInited(); // speaker: iOS speaker
}

IMediaDevice* CMediaLib::GetVideoCapDevice(int iDev)
{
    AsureVideoCapDeviceListInited();
    return m_listVideoCapDevice.GetHandleAt(iDev); // camera: front/back
}

IMediaDevice* CMediaLib::GetAudioCapDevice(int iDev)
{
    AsureAudioCapDeviceListInited();
    return m_listAudioCapDevice.GetHandleAt(iDev); // mic: iOS mic
}

IMediaDevice* CMediaLib::GetVideoWindowDevice(int iDev)
{
    AsureVideoWindowDeviceListInited();
    return m_listVideoWindowDevice.GetHandleAt(iDev); // window: render
}

IMediaDevice* CMediaLib::GetAudioOutDevice(int iDev)
{
    AsureAudioOutDeviceListInited();
    return m_listAudioOutDevice.GetHandleAt(iDev); // speaker: iOS speaker
}


}//end MediaLib