#ifndef __SWAPI__H__
#define __SWAPI__H__

#include "ISWApi.h"
#include "DataSet.h"
#include "IMediaLib.h"
#include "NetLib.h"
#include "SWUA.h"

#define TRACEAPI TRACE

namespace SWApi
{
using namespace MediaLib;
/*
媒体组合类型:

	S =>	A,V
	C =>	SS						{AA+,AV+,VV+}
	M =>	S/S,					{A/A,A/V}
			S/C,					{A/AA+,V/AA+,A/AV+,V/AV+,A/VV+,V/VV+}
			S/M,
			C/C,
			C/M,
			M/M



媒体源ID:
	1.用来表示一个媒体流,用于获取媒体的来源
	2.媒体ID的产生:
		1)网络(DEMUX)
		2)采集设备(CAPTURE)
		3)组合设备:
			复用(MUX) -- 把不同的流叠加成一个流组,赋予流组ID,对方根据媒体Id分离流
			复合(COMPOSER) -- 把不同的流按需求混合成一个不可分隔的流,赋予新的ID
		4) 分离设备
			解复用
			解复合
			分支

	M->S,S->M,S->C

内部Id和外部Id:
	1.内部Id用于本域内部区分不同来源的ID
	2.外部Id用于在网络上传输时标识不同域的ID

媒体源Id转换
	1.当发送到网络时要由内部ID转为外部ID
	2.从网络接收到数据后转为内部ID
	



源参数:
	From:
		Mixer		--  混合器
		Capture		--	采集设备源
		Net			--	网络源
		File		--  文件
	

	mid:
		Mixer		--  混合器号
		Device		--	采集设备号
		Net			--	网络源号

	sid:
		Mixer		--  端口
		Device		--	no
		Net			--	no

	Type:				音频/视频/数据




*/

/*

ControllerType:<Player,Capture,Mux,Demux,NetPlayer,NetSource,Mixer,Spliter>

SourceController:<Capture,Demux,NetSource,Mixer>
RenderController:<Player,Mux,NetPlayer,Mixer,Spliter>



*/

enum MediaSourceType
{
	MediaSourceMPort=0,
	MediaSourceCapture,
	MediaSourceNet,
	MediaSourceFile,
};

struct MediaSourceId
{
	DWORD iSub:16;//sub id
	DWORD iMain:8;//main id
	DWORD Type:4;//<unknown,fmtVideo,fmtAudio,fmtData>
	DWORD From:4;//<Mixer,Capture,(Demux/Net),File>
	MediaSourceId(SWMEDIASOURCEID mid=0)
	{
		*LPSOURCEID(this)=mid;
	}
	MediaSourceId(int from,int type,int idxMain,int idxSub):From(from),Type(type),iMain(idxMain),iSub(idxSub){};
	operator SWMEDIASOURCEID()
	{
		return *LPSOURCEID(this);
	}
};

#define __IMPLMC \
	IMediaProcessor* _pProc; \
	long _lUserData; \
	virtual void SetUserData(long lUserData){_lUserData=lUserData;}; \
	virtual long GetUserData(){return _lUserData;}; \
	virtual void Release(){delete this;}; \

struct SWVideoPlayer;
struct SWVideoCapturer;
struct SWNetVideoSender;
struct SWNetVideoReceiver;

struct SWAudioPlayer;
struct SWAudioCapturer;
struct SWNetAudioSender;
struct SWNetAudioReceiver;

struct SWAPI:public ISWApi
{
	ISWApiEventHandler* _pHandler;
	IMediaLib* _pLib;
//	BaseArray<SWVideoPlayer*> _VPlayers;
	BaseArray<SWVideoCapturer*> _VCaps;
	SWNetVideoSender* _pVSender;
	SWNetVideoReceiver* _pVReceiver;

	BaseArray<SWAudioPlayer*> _APlayers;
	BaseArray<SWAudioCapturer*> _ACaps;
	SWNetAudioSender* _pASender;
	SWNetAudioReceiver* _pAReceiver;


	//single UA
	SWUA* _pUA;
	//multi UA
//	BaseArray<SWUA*> _UAs;
public:
	SWAPI(ISWApiEventHandler* pHandler=0);
	~SWAPI();
public:
	virtual void Release(){delete this;};
	virtual ISWUA* CreateUA(ISWUAEventHandler* pEventHandler=0);
	virtual ISWVideoPlayer* CreateVideoPlayer(void* pVideoWindow);
	virtual int GetMediaDeviceCount(int nDevType);
	virtual ISWVideoCapturer* GetVideoCapturer(int iDev);
	virtual ISWNetVideoSender* GetNetVideoSender();
	virtual ISWNetVideoReceiver* GetNetVideoReceiver();

	virtual ISWAudioPlayer* GetAudioPlayer(int iDev);
	virtual ISWAudioCapturer* GetAudioCapturer(int iDev);
	virtual ISWNetAudioSender* GetNetAudioSender();
	virtual ISWNetAudioReceiver* GetNetAudioReceiver();

public:

	IMediaLib* GetLib(){return _pLib;}
	IMediaProcessor* GetSourceMPort(SWMEDIASOURCEID sid,long& lPort);
	SWMEDIASOURCEID GetMPortSourceId(IMediaProcessor* pProc,long lPort,bool bInput=1);
	int StartMPOutput(IMediaProcessor* pProc,long lPort);
	int StopMPOutput(IMediaProcessor* pProc,long lPort);
	int FindMPortByMediaId(IMediaProcessor* pProc,long mid,bool bInput=0,bool bAdd=1);
	long GetSourceMediaIdForNet(SWMEDIASOURCEID sid)
	{
		if(_pUA)
		{
			return (long)((DWORD)_pUA->m_localIp);
		}
		return -1;
	}

};
extern SWAPI* _pApi;


struct SWVideoPlayer:ISWVideoPlayer
{
	__IMPLMC;
	long _lPort;
	IMediaDevice* _pDev;
	int _iDev;
public:
	SWVideoPlayer(void* pWnd)
	{
		_pProc=0;
		_lPort=0;

		_pDev=0;
		int iWnd=_pApi->GetLib()->AddVideoPlayWindow(pWnd);
		_iDev=iWnd;
		_pDev=_pApi->GetLib()->GetDevice(MediaDeviceVideoWindow,iWnd);
	}
	~SWVideoPlayer()
	{
		_pApi->GetLib()->RemoveVideoPlayWindow(_iDev);
	}
	virtual int SetSource(SWMEDIASOURCEID sid)
	{
		if(sid==0)
		{
			if(_pProc)
			{
				_pApi->StopMPOutput(_pProc,_lPort);
				_pProc->SetOutputStorageObject(0, 0, _lPort);
				_pProc=0;
			}
		}
		else
		{
			long port=-1;
			_pProc=_pApi->GetSourceMPort(sid, port);
			_pProc->SetOutputFormat(MediaFormatMask(1,MediaStorageDevice,0,RawVideo,MediaTypeVideo), port);
			_pProc->SetOutputStorageObject(_pDev, 0, port);
			_pApi->StartMPOutput(_pProc,port);
		}
		return 0;

	}
	virtual int Play()
	{
		if(_pProc)
		{
			_pApi->StartMPOutput(_pProc,_lPort);
		}
		return 0;
	}
	virtual IVideoRenderConfig* GetRenderConfig()
	{
		if(_pProc)
		{
			return (IVideoRenderConfig*)_pProc->GetConfig(mcfgVideoRender);
		}
		return 0;
	};

	


};



struct SWVideoCapturer:ISWVideoCapturer
{
	__IMPLMC;

public:
	SWVideoCapturer(int iDev)
	{
		IMediaDevice* pDev=_pApi->GetLib()->GetDevice(MediaDeviceVideoCap);

		_pProc=_pApi->GetLib()->CreateMediaProcessor(1, 0,fmtDeviceVideo,fmtVideo);
		_pProc->SetInputStorageObject(pDev,iDev,0);
	}
	~SWVideoCapturer()
	{
		_pProc->Release();
	}
public:
	virtual int GetSupportedFormatCount(){return 0;};
	virtual int GetFormatType(int iFmt=-1){return 0;};
	virtual int GetFrameWidht(int iFmt=-1){return 0;};
	virtual int GetFrameHeight(int iFmt=-1){return 0;};
	virtual int SetFrameRate(int q,int n=1){return 0;};
	virtual void GetFrameRate(int& q,int& n,int iFmt=-1){};
	virtual int GetCurrentFormat(){return 0;};
	virtual int SetCurrentFormat(int iFmt=0){return 0;};
	virtual SWMEDIASOURCEID AddSource(DWORD mid)
	{
		int port=_pApi->FindMPortByMediaId(_pProc, mid,0,1);
		MediaSourceId Source(MediaSourceCapture,MediaTypeVideo,GetDeviceIndex(),port);
		return Source;
	}
	virtual SWMEDIASOURCEID GetSource(DWORD Id)
	{
		int port=_pApi->FindMPortByMediaId(_pProc,Id,0,0);
		if(port<0)
		{
			return 0;
		}
		MediaSourceId Source(MediaSourceCapture,MediaTypeVideo,GetDeviceIndex(),port);
		return Source;

	}

	virtual void RemoveSource(DWORD Id)
	{
		int port=_pApi->FindMPortByMediaId(_pProc,Id,0,0);
		if(port>-1)
		{
			_pProc->SetMediaStreamId(0, 1);
			_pProc->SetOutputStorageObject(0, 0, port);
			_pProc->RemoveOutput(port);
		}
	}
public:
	int GetDeviceIndex()
	{
		long index=-1;
		_pProc->GetInputStorageObject(&index,0);
		return (int)index;
	}
	virtual IVideoCaptureConfig* GetCaptureConfig()
	{
		if(_pProc)
		{
			return (IVideoCaptureConfig*)_pProc->GetConfig(mcfgVideoCap);
		}
		return 0;
	};


};

struct SWNetVideoSender:public ISWNetVideoSender
{
	__IMPLMC;
	IAsyncSocket* _pSocket;

public:
	SWNetVideoSender()
	{
		int err=0;
		_pSocket=(IAsyncSocket*)NetLib_CreateSocketObject(SOCK_DGRAM);
		err=_pSocket->OpenSock("", 7100);
//		err=_pSocket->SetPeerAddr("192.168.2.151", 7100);
	//	char buf[256];
	//	err=_pSocket->WriteData(buf, 256);
		_pProc=0;
	};
	~SWNetVideoSender()
	{
		_pSocket->Release();
	};
	virtual int StratSend(SWMEDIASOURCEID sid,DWORD NetId,const char* szPeerAddr);
	virtual int StopSend(SWMEDIASOURCEID sid,DWORD NetId);
	virtual int InsertKeyFrame(const char* szPeerAddr=0);

public:
	void SetPeerIp(SocketIpAddr addr)
	{
		_pSocket->ISocket::SetPeerAddr(addr, 7100);
	}
	virtual INetSendConfig* GetNetSendConfig()
	{
		if(_pProc)
		{
			return (INetSendConfig*)_pProc->GetConfig(mcfgNetSend);
		}
		return 0;
	};


};

struct SWNetVideoReceiver:public ISWNetVideoReceiver
{
	__IMPLMC;
	IAsyncSocket* _pSocket;
public:
	SWNetVideoReceiver()
	{
		int err=0;
		_pSocket=(IAsyncSocket*)NetLib_CreateSocketObject(SOCK_DGRAM);
//		_pProc=_pApi->GetLib()->CreateMediaProcessor(1, 0);
		_pProc=_pApi->GetLib()->CreateMediaProcessor(1, 0,fmtVC3MultiNetVideo,fmtVideo);
		_pProc->SetInputFormat(fmtVC3MultiNetVideo);
		_pProc->SetInputStorageObject(_pSocket,0,0);
		err=_pSocket->OpenSock("", 0,"",7100);


	};
	~SWNetVideoReceiver(){};
	virtual SWMEDIASOURCEID AddSource(DWORD NetId);
	virtual SWMEDIASOURCEID GetSource(DWORD NetId);
	virtual int RemoveSource(DWORD NetId);
	virtual INetRecvConfig* GetNetRecvConfig()
	{
		if(_pProc)
		{
			return (INetRecvConfig*)_pProc->GetConfig(mcfgNetRecv);
		}
		return 0;
	};


};

//audio

	struct SWAudioPlayer:ISWAudioPlayer
	{
		__IMPLMC;
		long _lPort;
		IMediaDevice* _pDev;
		int _iDev;
	public:
		SWAudioPlayer(int iDev)
		{
			_pProc=0;
			_lPort=0;

			_pDev=0;
			_iDev=iDev;
			_pDev=_pApi->GetLib()->GetDevice(MediaDeviceAudioOut,iDev);
		}
		~SWAudioPlayer()
		{
		}
		virtual int SetSource(SWMEDIASOURCEID sid)
		{
			if(sid==0)
			{
				if(_pProc)
				{
					_pApi->StopMPOutput(_pProc,_lPort);
					_pProc->SetOutputStorageObject(0, 0, _lPort);
					_pProc=0;
				}
			}
			else
			{
				long port=-1;
				_pProc=_pApi->GetSourceMPort(sid, port);
				_pProc->SetOutputFormat(MediaFormatMask(1,MediaStorageDevice,0,RawAudio,MediaTypeAudio), port);
				_pProc->SetOutputStorageObject(_pDev, 0, port);
				_pApi->StartMPOutput(_pProc,port);
			}
			return 0;

		}
		virtual int Play()
		{
			if(_pProc)
			{
				_pApi->StartMPOutput(_pProc,_lPort);
			}
			return 0;
		}
		virtual IAudioRenderConfig* GetAudioRenderConfig()
		{
			if(_pProc)
			{
				return (IAudioRenderConfig*)_pProc->GetConfig(mcfgAudioRender);
			}
			return 0;
		};



	};



	struct SWAudioCapturer:ISWAudioCapturer
	{
		__IMPLMC;

	public:
		SWAudioCapturer(int iDev)
		{
			IMediaDevice* pDev=_pApi->GetLib()->GetDevice(MediaDeviceAudioCap);

			_pProc=_pApi->GetLib()->CreateMediaProcessor(1, 0,fmtDeviceAudio,fmtAudio);
			_pProc->SetInputStorageObject(pDev,iDev,0);
		}
		~SWAudioCapturer()
		{
			_pProc->Release();
		}
	public:
		virtual int GetSupportedFormatCount(){return 0;};
		virtual int GetFormatType(int iFmt=-1){return 0;};
		virtual int GetCurrentFormat(){return 0;};
		virtual int SetCurrentFormat(int iFmt=0){return 0;};
		virtual SWMEDIASOURCEID AddSource(DWORD mid)
		{
			int port=_pApi->FindMPortByMediaId(_pProc, mid,0,1);
			MediaSourceId Source(MediaSourceCapture,MediaTypeAudio,GetDeviceIndex(),port);
			return Source;
		}
		virtual SWMEDIASOURCEID GetSource(DWORD Id)
		{
			int port=_pApi->FindMPortByMediaId(_pProc,Id,0,0);
			if(port<0)
			{
				return 0;
			}
			MediaSourceId Source(MediaSourceCapture,MediaTypeAudio,GetDeviceIndex(),port);
			return Source;

		}

		virtual void RemoveSource(DWORD Id)
		{
			int port=_pApi->FindMPortByMediaId(_pProc,Id,0,0);
			if(port>-1)
			{
				_pProc->SetMediaStreamId(0, 1);
				_pProc->SetOutputStorageObject(0, 0, port);
				_pProc->RemoveOutput(port);
			}
		}
	public:
		int GetDeviceIndex()
		{
			long index=-1;
			_pProc->GetInputStorageObject(&index,0);
			return (int)index;
		}
		virtual IAudioCaptureConfig* GetCaptureConfig()
		{
			if(_pProc)
			{
				return (IAudioCaptureConfig*)_pProc->GetConfig(mcfgAudioCap);
			}
			return 0;
		};


	};

	struct SWNetAudioSender:public ISWNetAudioSender
	{
		__IMPLMC;
		IAsyncSocket* _pSocket;

	public:
		SWNetAudioSender()
		{
			int err=0;
			_pSocket=(IAsyncSocket*)NetLib_CreateSocketObject(SOCK_DGRAM);
			err=_pSocket->OpenSock("", 7101);
			_pProc=0;
		};
		~SWNetAudioSender()
		{
			_pSocket->Release();
		};
		virtual int StratSend(SWMEDIASOURCEID sid,DWORD NetId,const char* szPeerAddr);
		virtual int StopSend(SWMEDIASOURCEID sid,DWORD NetId);

	public:
		void SetPeerIp(SocketIpAddr addr)
		{
			_pSocket->ISocket::SetPeerAddr(addr, 7101);
		}

		virtual INetSendConfig* GetNetSendConfig()
		{
			if(_pProc)
			{
				return (INetSendConfig*)_pProc->GetConfig(mcfgNetSend);
			}
			return 0;
		};

	};

	struct SWNetAudioReceiver:public ISWNetAudioReceiver
	{
		__IMPLMC;
		IAsyncSocket* _pSocket;
	public:
		SWNetAudioReceiver()
		{
			int err=0;
			_pSocket=(IAsyncSocket*)NetLib_CreateSocketObject(SOCK_DGRAM);
			_pProc=_pApi->GetLib()->CreateMediaProcessor(1, 0,fmtVC3MultiNetAudio,fmtAudio);
			_pProc->SetInputFormat(fmtVC3MultiNetAudio);
			_pProc->SetInputStorageObject(_pSocket,0,0);
			err=_pSocket->OpenSock("", 0,"",7101);

			
		};
		~SWNetAudioReceiver(){};
		virtual SWMEDIASOURCEID AddSource(DWORD NetId);
		virtual SWMEDIASOURCEID GetSource(DWORD NetId);
		virtual int RemoveSource(DWORD NetId);
		virtual INetRecvConfig* GetNetRecvConfig()
		{
			if(_pProc)
			{
				return (INetRecvConfig*)_pProc->GetConfig(mcfgNetRecv);
			}
			return 0;
		};


	};


///////////////////////end name space
};

#endif //end ifdef
