#ifndef __ISWMEDIACONTROLLER__H__
#define __ISWMEDIACONTROLLER__H__

#include "ISWUA.h"
#include "IMediaDef.h"
using namespace MediaLib;
namespace SWApi
{
    typedef DWORD SWMEDIASOURCEID;
    
    struct ISWMediaController
    {
        virtual void SetUserData(long lUserData)=0;
        virtual long GetUserData()=0;
        virtual ~ISWMediaController(){};
        virtual void Release()=0;
    };


    struct ISWVideoPlayer:public ISWMediaController
    {
        virtual int SetSource(SWMEDIASOURCEID sid)=0;
        virtual int Play()=0;
        int PlaySource(SWMEDIASOURCEID sid)
        {
            SetSource(sid);
            Play();
            return 0;
        }
		virtual IVideoRenderConfig* GetRenderConfig()=0;
    };
    
    struct ISWVideoMixer:public ISWMediaController
    {
        virtual SWMEDIASOURCEID GetOutputSourceId()=0;
        virtual int SetInputSourceCount()=0;
        virtual int SetInputSource(int i,SWMEDIASOURCEID sid)=0;
    };
    

    struct ISWVideoCapturer:public ISWMediaController
    {

		virtual SWMEDIASOURCEID AddSource(DWORD Id)=0;
		virtual SWMEDIASOURCEID GetSource(DWORD Id)=0;
		virtual void RemoveSource(DWORD Id)=0;
		virtual IVideoCaptureConfig* GetCaptureConfig()=0;
    };

    struct ISWNetVideoSender:public ISWMediaController
    {
        virtual int StratSend(SWMEDIASOURCEID sid,DWORD NetId,const char* szPeerAddr)=0;
        virtual int StopSend(SWMEDIASOURCEID sid,DWORD NetId)=0;
		virtual int InsertKeyFrame(const char* szPeerAddr=0)=0;
		virtual INetSendConfig* GetNetSendConfig()=0;
    };
    
    struct ISWNetVideoReceiver:public ISWMediaController
    {
        virtual SWMEDIASOURCEID AddSource(DWORD NetId)=0;
		virtual SWMEDIASOURCEID GetSource(DWORD NetId)=0;
        virtual int RemoveSource(DWORD NetId)=0;
		virtual INetRecvConfig* GetNetRecvConfig()=0;
    };

	struct ISWAudioPlayer:public ISWMediaController
	{
		virtual int SetSource(SWMEDIASOURCEID sid)=0;
		virtual int Play()=0;
		int PlaySource(SWMEDIASOURCEID sid)
		{
			SetSource(sid);
			Play();
			return 0;
		}
		virtual IAudioRenderConfig* GetAudioRenderConfig()=0;
	};

	struct ISWAudioCapturer:public ISWMediaController
	{
		virtual SWMEDIASOURCEID AddSource(DWORD Id)=0;
		virtual SWMEDIASOURCEID GetSource(DWORD Id)=0;
		virtual void RemoveSource(DWORD Id)=0;
		virtual IAudioCaptureConfig* GetCaptureConfig()=0;

	};

	struct ISWNetAudioSender:public ISWMediaController
	{
		virtual int StratSend(SWMEDIASOURCEID sid,DWORD NetId,const char* szPeerAddr)=0;
		virtual int StopSend(SWMEDIASOURCEID sid,DWORD NetId)=0;
		virtual INetSendConfig* GetNetSendConfig()=0;

	};

	struct ISWNetAudioReceiver:public ISWMediaController
	{
		virtual SWMEDIASOURCEID AddSource(DWORD NetId)=0;
		virtual SWMEDIASOURCEID GetSource(DWORD NetId)=0;
		virtual int RemoveSource(DWORD NetId)=0;
		virtual INetRecvConfig* GetNetRecvConfig()=0;
	};



}//end

#endif
