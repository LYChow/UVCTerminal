#ifndef __ISWAPI__H__
#define __ISWAPI__H__

#include "ISWMediaController.h"
#include "ISWUA.h"
#include "IMediaDef.h"

namespace SWApi
{
    struct ISWApiEventHandler
    {
        virtual void OnSWMediaDeviceStateChange(int nDevType,int iDev,int state)=0;
    };
    struct ISWApi
    {
        virtual ~ISWApi(){};
        virtual void Release()=0;
        virtual ISWUA* CreateUA(ISWUAEventHandler* pEventHandler=0)=0;
        virtual ISWVideoPlayer* CreateVideoPlayer(void* pVideoWindow)=0;
        //0=vcap,1=acap,2=aout
        virtual int GetMediaDeviceCount(int nDevType)=0;
        virtual ISWVideoCapturer* GetVideoCapturer(int iDev)=0;
        virtual ISWNetVideoSender* GetNetVideoSender()=0;
        virtual ISWNetVideoReceiver* GetNetVideoReceiver()=0;

		virtual ISWAudioPlayer* GetAudioPlayer(int iDev)=0;
		virtual ISWAudioCapturer* GetAudioCapturer(int iDev)=0;
        virtual ISWNetAudioSender* GetNetAudioSender()=0;
        virtual ISWNetAudioReceiver* GetNetAudioReceiver()=0;
//        virtual ISWVideoMixer* CreateVideoMixer()=0;
//        virtual ISWAudioMixer* CreateAudioMixer();
//        virtual IMediaComposer* CreateComposer();
    };
    
    ISWApi* CreateSWApi(ISWApiEventHandler* pEventHandler,int ver=0);
    
 }

#endif
