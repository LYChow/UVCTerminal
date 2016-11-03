

#include "__PCH.h"
#include "SWUA.h"
#include "SWApi.h"

namespace SWApi
{
using namespace MediaLib;
//////////////////////////////////////////////////////////////////////////

SWAPI* _pApi=0;


ISWApi* CreateSWApi(ISWApiEventHandler* pHandler,int ver)
{
	TRACEAPI("SWApi-CREATE:pHandler=%p\n",pHandler);
	if(_pApi==0) _pApi=new SWAPI(pHandler);
	return _pApi;
}

SWAPI::SWAPI(ISWApiEventHandler* pHandler)
{
	_pApi=this;
	_pHandler=pHandler;
	_pLib=CreateMediaLib(0);
	_VCaps.SetSize(_pLib->GetDeviceCount(MediaDeviceVideoCap));
	_ACaps.SetSize(_pLib->GetDeviceCount(MediaDeviceAudioCap));
	_APlayers.SetSize(_pLib->GetDeviceCount(MediaDeviceAudioOut));
	_pVSender=new SWNetVideoSender();
	_pVReceiver=new SWNetVideoReceiver();
	_pASender=new SWNetAudioSender();
	_pAReceiver=new SWNetAudioReceiver();
	_pUA=0;
}
SWAPI::~SWAPI()
{
	_pVSender->Release();
	_pVReceiver->Release();
	_pASender->Release();
	_pAReceiver->Release();
	_pLib->Release();
}

ISWUA* SWAPI::CreateUA(ISWUAEventHandler* pEventHandler)
{
	if(!_pUA)
	{
		_pUA=new SWUA(pEventHandler);
		_pUA->SetNatHoleSocket(_pVReceiver->_pSocket, 0);
		_pUA->SetNatHoleSocket(_pAReceiver->_pSocket, 1);
	}
	TRACEAPI("SWApi-CreateUA:pUA=%p\n",_pUA);
	return _pUA;
}

ISWVideoPlayer* SWAPI::CreateVideoPlayer(void* pVideoWindow)
{
	SWVideoPlayer* p=new SWVideoPlayer(pVideoWindow);
	return p;
}

int SWAPI::GetMediaDeviceCount(int nDevType)
{
	return _pLib->GetDeviceCount((MediaDeviceType)nDevType);
}

ISWVideoCapturer* SWAPI::GetVideoCapturer(int iDev)
{
	if (_VCaps[iDev]==0) {
		_VCaps[iDev]=new SWVideoCapturer(iDev);
	}
	return _VCaps[iDev];
}


ISWNetVideoSender* SWAPI::GetNetVideoSender()
{
	return _pVSender;
}

ISWNetVideoReceiver* SWAPI::GetNetVideoReceiver()
{
	return _pVReceiver;
}

ISWAudioPlayer* SWAPI::GetAudioPlayer(int iDev)
{
	if (_APlayers[iDev]==0) {
		_APlayers[iDev]=new SWAudioPlayer(iDev);
	}
	return _APlayers[iDev];
}
ISWAudioCapturer* SWAPI::GetAudioCapturer(int iDev)
{
	if (_ACaps[iDev]==0) {
		_ACaps[iDev]=new SWAudioCapturer(iDev);
	}
	return _ACaps[iDev];
}
ISWNetAudioSender* SWAPI::GetNetAudioSender()
{
	return _pASender;
}
ISWNetAudioReceiver* SWAPI::GetNetAudioReceiver()
{
	return _pAReceiver;
}


//find and create source port
IMediaProcessor* SWAPI::GetSourceMPort(SWMEDIASOURCEID sid,long& lPort)
{
	IMediaProcessor* pProc=0;
	if(sid==0) return 0;
	MediaSourceId Source(sid);
	switch (Source.From)
	{
		case MediaSourceMPort:

			break;
		case MediaSourceCapture:
			{
				if (Source.Type==MediaTypeVideo)
				{
					pProc=_VCaps[Source.iMain]->_pProc;
					lPort=Source.iSub;
				}
				else if(Source.Type==MediaTypeAudio)
				{
					pProc=_ACaps[Source.iMain]->_pProc;
					lPort=Source.iSub;

				}
			}
			break;
		case MediaSourceNet:
			{
				if (Source.Type==MediaTypeVideo)
				{
					pProc=_pVReceiver->_pProc;
					lPort=Source.iSub;
				}
				else if(Source.Type==MediaTypeAudio)
				{
					pProc=_pAReceiver->_pProc;
					lPort=Source.iSub;

				}

			}
			break;
		case MediaSourceFile:
			break;
		default:
			break;
	}
	return pProc;
}

SWMEDIASOURCEID SWAPI::GetMPortSourceId(IMediaProcessor* pProc,long lPort,bool bInput)
{
	MediaSourceId Source;
	do
	{
		if(!pProc) break;
		MediaFormatMask mfIn=bInput?pProc->GetInputFormat(lPort):pProc->GetInputFormat(0);
		MediaFormatMask mfOut=bInput?pProc->GetOutputFormat(0):pProc->GetInputFormat(lPort);
		void* pObj=0;
		long lParam=-1;
		pObj=pProc->GetInputStorageObject(&lParam,lPort);
		if (bInput)
		{
			if(mfIn.Storage==MediaStorageProcessor)
			{
				pProc=(IMediaProcessor*)pObj;
				lPort=lParam;
				bInput=0;
				continue;
			}
#if 0
//the two case below would be unreachable
			else if(mfIn.Storage==MediaStorageDevice)
			{
				Source.From=MediaSourceCapture;
				Source.Type=mfIn.Type;

			}
			else if(mfIn.Storage==MediaStorageNet)
			{
				Source.From=MediaSourceNet;
				Source.Type=mfIn.Type;
			}
#endif
		}
		else //output port
		{
			if(mfIn.Storage==MediaStorageProcessor)
			{
				if (mfIn.Multiple || mfIn.IsComposed())
				{
					Source.From=MediaSourceMPort;
					Source.Type=mfOut.Type;
					Source.iMain=0;
					Source.iSub=lPort;
					break;
				}
				pProc=(IMediaProcessor*)pObj;
				lPort=lParam;
				bInput=0;
				continue;
			}
			else if(mfIn.Storage==MediaStorageDevice)
			{
				Source.From=MediaSourceCapture;
				Source.Type=mfIn.Type;

			}
			else if(mfIn.Storage==MediaStorageNet)
			{
				Source.From=MediaSourceNet;
				Source.Type=mfIn.Type;
			}

		}

	}while (0);

	return Source;
}


int SWAPI::StartMPOutput(IMediaProcessor* pProc,long lPort)
{
	if (!pProc)
	{
		return 0;
	}
	MediaFormatMask mfIn=pProc->GetInputFormat(0);
	do
	{
		pProc->Start();
		if (mfIn.Storage==MediaStorageNet)
		{
			DWORD mid=pProc->GetMediaStreamId(1,lPort);
			TRACEAPI("SWAPI-OpenNetStream:port=%d,MID=%p\n",lPort,mid);
			_pUA->OpenMediaByStreamId(mid, mfIn.Type);
		}
		else if (mfIn.Storage==MediaStorageDevice)
		{
			TRACEAPI("SWAPI-OpenDeviceStream:port=%d,\n",lPort);
		}
		else if (mfIn.Storage==MediaStorageProcessor)
		{
			pProc=(IMediaProcessor*)pProc->GetInputStorageObject(&lPort,lPort);
			continue;
		}
	}while(0);

	return 0;
}
int SWAPI::StopMPOutput(IMediaProcessor* pProc,long lPort)
{
	if (!pProc)
	{
		return 0;
	}
	do
	{
		pProc->Stop();
		MediaFormatMask mfIn=pProc->GetInputFormat(0);
		if (mfIn.Storage==MediaStorageNet)
		{
			DWORD mid=pProc->GetMediaStreamId(1,lPort);
			TRACEAPI("SWAPI-CloseNetStream:port=%d,MID=%p\n",lPort,mid);
			_pUA->CloseMediaByStreamId(mid, mfIn.Type);
		}
		else if (mfIn.Storage==MediaStorageDevice)
		{

		}
		else if (mfIn.Storage==MediaStorageProcessor)
		{
			pProc=(IMediaProcessor*)pProc->GetInputStorageObject(&lPort,lPort);
			continue;
		}
	}while(0);

	return 0;

}
int SWAPI::FindMPortByMediaId(IMediaProcessor* pProc,long mid,bool bInput,bool bAdd)
{
	long port=-1;
	long portIdle=-1;
	int cnt=bInput?pProc->GetInputCount():pProc->GetOutputCount();
	for (int i=0; i<cnt; i++) {
		long MediaId=pProc->GetMediaStreamId(!bInput);
		if (MediaId==0)
		{
			if(portIdle<0) portIdle=i;
		}
		else if(MediaId==mid) return i;
	}
	if (bAdd) {
		if (portIdle<0) {
			portIdle=bInput?pProc->AddInput():pProc->AddOutput();
		}
		port=portIdle;
		pProc->SetMediaStreamId(mid, !bInput,port);
	}

	return (int)port;

}
/*
source from:
 capture
 mixer
 net

*/


int SWNetVideoSender::StratSend(SWMEDIASOURCEID sid,DWORD NetId,const char* szPeerAddr)
{
	TRACEAPI("VideoSender-StartSend:NetId=%p,to=%s\n",NetId,szPeerAddr);

	if(sid==0)
	{
		return -1;
	}
	long port=-1;
	//already connect processor
	if (_pProc) {
		return 0;
#if 0
		//already sending
		if(_pApi->GetMPOutputSourceId(_pProc,0)==sid)
		{
			return 0;
		}
		//add input,create mux if necessory
#endif
	}
	_pProc=_pApi->GetSourceMPort(sid, port);
	//setinput stream id
	long addr=NetId;//_pApi->GetSourceMediaIdForNet(sid);
	_pProc->SetMediaStreamId(addr, 0,0);
	_pProc->SetOutputFormat(MediaFormatMask(1,MediaStorageNet,1,VC3PackedVideo,MediaTypeVideo), port);
	_pSocket->SetPeerAddr(szPeerAddr, 7100);
	_pProc->SetOutputStorageObject(_pSocket, 0, port);
	_pApi->StartMPOutput(_pProc,port);

	return 0;
}
int SWNetVideoSender::StopSend(SWMEDIASOURCEID sid,DWORD NetId)
{
	if (_pProc==0)
	{
		return 0;
	}
	long port=_pApi->FindMPortByMediaId(_pProc, NetId,1,0);
	_pApi->StopMPOutput(_pProc, port);
	_pProc->SetOutputStorageObject(0, 0, port);
	_pProc=0;
	
	return 0;
}


SWMEDIASOURCEID SWNetVideoReceiver::AddSource(DWORD NetId)
{
	if(_pProc==0) return 0;
	long port=_pApi->FindMPortByMediaId(_pProc, NetId,0,1);
	MediaSourceId Source(MediaSourceNet,MediaTypeVideo,0,port);
	return Source;

};
SWMEDIASOURCEID SWNetVideoReceiver::GetSource(DWORD NetId)
{
	if(_pProc==0) return 0;
	long port=_pApi->FindMPortByMediaId(_pProc, NetId,0,0);
	if(port<0) return 0;
	MediaSourceId Source(MediaSourceNet,MediaTypeVideo,0,port);
	return Source;
};
int SWNetVideoReceiver::RemoveSource(DWORD NetId)
{
	int port=_pApi->FindMPortByMediaId(_pProc,NetId,0,0);
	if(port>-1)
	{
		_pProc->SetMediaStreamId(0, 1);
		_pProc->SetOutputStorageObject(0, 0, port);
		_pProc->RemoveOutput(port);
	}
	return 0;
};

	int SWNetVideoSender::InsertKeyFrame(const char* szPeerAddr)
	{
		return 0;
	}


//audio

	int SWNetAudioSender::StratSend(SWMEDIASOURCEID sid,DWORD NetId,const char* szPeerAddr)
	{
		TRACEAPI("AudioSender-StartSend:NetId=%p,to=%s\n",NetId,szPeerAddr);

		if(sid==0)
		{
			return -1;
		}
		long port=-1;
		//already connect processor
		if (_pProc) {
			return 0;
#if 0
			//already sending
			if(_pApi->GetMPOutputSourceId(_pProc,0)==sid)
			{
				return 0;
			}
			//add input,create mux if necessory
#endif
		}
		_pProc=_pApi->GetSourceMPort(sid, port);
		//setinput stream id
		long addr=NetId;//_pApi->GetSourceMediaIdForNet(sid);
		_pProc->SetMediaStreamId(addr, 0,0);
		_pProc->SetOutputFormat(MediaFormatMask(1,MediaStorageNet,1,VC3PackedAudio,MediaTypeAudio), port);
		_pSocket->SetPeerAddr(szPeerAddr, 7101);
		_pProc->SetOutputStorageObject(_pSocket, 0, port);
		_pApi->StartMPOutput(_pProc,port);

		return 0;
	}
	int SWNetAudioSender::StopSend(SWMEDIASOURCEID sid,DWORD NetId)
	{
		if (_pProc==0)
		{
			return 0;
		}
		long port=_pApi->FindMPortByMediaId(_pProc, NetId);
		_pApi->StopMPOutput(_pProc, port);
		_pProc->SetOutputStorageObject(0, 0, port);
		_pProc=0;

		return 0;
	}


	SWMEDIASOURCEID SWNetAudioReceiver::AddSource(DWORD NetId)
	{
		if(_pProc==0) return 0;
		long port=_pApi->FindMPortByMediaId(_pProc, NetId,0,1);
		MediaSourceId Source(MediaSourceNet,MediaTypeAudio,0,port);
		return Source;

	};
	SWMEDIASOURCEID SWNetAudioReceiver::GetSource(DWORD NetId)
	{
		if(_pProc==0) return 0;
		long port=_pApi->FindMPortByMediaId(_pProc, NetId,0,0);
		if(port<0) return 0;
		MediaSourceId Source(MediaSourceNet,MediaTypeAudio,0,port);
		return Source;
	};
	int SWNetAudioReceiver::RemoveSource(DWORD NetId)
	{
		int port=_pApi->FindMPortByMediaId(_pProc,NetId,0,0);
		if(port>-1)
		{
			_pProc->SetMediaStreamId(0, 1);
			_pProc->SetOutputStorageObject(0, 0, port);
			_pProc->RemoveOutput(port);
		}
		return 0;
	};
	


//////////////////////////////////////////////////////////////////////////
};//end name space SWApi
