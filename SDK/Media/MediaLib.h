#ifndef __MEDIALIB__H__
#define __MEDIALIB__H__

//
#include "MPWorker.h"
namespace MediaLib{


typedef MediaPort MediaPortIn,MediaPortOut;

//template<typename TIN=MediaPort,typename TOUT=MediaPort>
struct MediaProcessor:public IMP
{
	BaseArray<MediaPortIn> _Inputs;
	BaseArray<MediaPortOut>_Outputs;
	MediaFormatMask _fmtIn;
	MediaFormatMask _fmtOut;
	int _state;
	long _userData;
public:
	MediaProcessor(int nIn=1,int nOut=1,int fmtIn=0,int fmtOut=0)
	{
		_state=0;
		_userData=0;
		_fmtIn=fmtIn;
		_fmtOut=fmtOut;
		SetInputCount(nIn);
		SetOutputCount(nOut);

	}
	~MediaProcessor()
	{
	}
	MediaPortIn& GetInPort(int i){return _Inputs[i];}
	MediaPortOut& GetOutPort(int i){return _Outputs[i];}

	virtual long SetUserData(long lUserData)
	{
		long pre=_userData;
		_userData=lUserData;
		return pre;
	}
	virtual long GetUserData(){return _userData;}
	virtual void SetMediaStreamId(long mid,bool bOutput,int iPort=0)
	{
		if(!bOutput) _Inputs[iPort].sid=mid;
		else _Outputs[iPort].sid=mid;
	}
	virtual long GetMediaStreamId(bool bOutput,int iPort=0)
	{
		return bOutput==0?_Inputs[iPort].sid:_Outputs[iPort].sid;
	}


	virtual void OnInputData(int iPort,IDataSource* pData)
	{
		char buf[2048];
		int len=2048;
		int i=0;
		while (len>=2048)
		{
			len=pData->ReadData(buf,2048);
			TRACEMEDIA("MP_ProcessInputdata(%d):size=%d\n",i,len);
			i++;
		}
	}

	//virtual void OnInputCanRead(int code,int iInPort,void* pObj,long lParam)
	virtual void OnSourceCanRead(int nErrCode,IDataSource* pSource,long userParam)
	{
		OnInputData((int)userParam,pSource);
	}

	virtual int SetInputStroageObject(void* pStrogeObject,long lParam,int iPort)
	{
#if 0
		_Inputs[iPort].SetConnObj(pStrogeObject,lParam);
		if(!pStrogeObject) return iPort;
		MediaFormatMask mf=_Inputs[iPort].fmt;
		switch(mf.Storage)
		{
		case MediaStorageDevice:
			{
				IMediaCapDevice* pDev=(IMediaCapDevice*)((IMediaDevice*)pStrogeObject);
				IVideoCapture* pCap=ConvertToDerivedPtr<IVideoCapture>(pDev);
				VIDEOFMT vf;
				pCap->GetFormat(-1,&vf);
				mf.Format=FCCToIndex(vf.cc);
				_Inputs[iPort].fmt=mf;
				_Inputs[iPort].vf=vf;
				pDev->RegisterObserver(this,iPort);
				mf=_Outputs[0].fmt;
				mf.Format=FCCToIndex(vf.cc);
				_Outputs[0].fmt=mf;
				_Outputs[0].vf=vf;
			}
			break;
		case MediaStorageProcessor:
			{
				IMediaProcessor* pProc=ConvertToDerivedPtr<IMediaProcessor>((IMediaProcessor*)pStrogeObject);
/*
				mf=pProc->GetOutputFormat(lParam);
				MediaFormatMask mfIn=_Inputs[iPort].fmt;
				mfIn.Format=mf.Format;
				_Inputs[iPort].fmt=mfIn;
				_Inputs[iPort].vf=pProc->_Outputs[lParam].vf;
*/
				pProc->SetOutputStroageObject(this,iPort,lParam);
			}
			break;
		case MediaStorageNet:
			{
				ISocket* pSock=(ISocket*)pStrogeObject;
				pSock->RegisterObserver(this,iPort,SocketObserverRead|SocketObserverThread);
			}
			break;

		}
#endif
		return iPort;
	}
	virtual void* GetInputStorageObject(long* lParam=0,int iPort=0)
	{
		return _Inputs[iPort].GetConnObj(lParam);
	}
	virtual int SetOutputStroageObject(void* pStrogeObject,long lParam,int iPort)
	{
		_Outputs[iPort].SetConnObj(pStrogeObject,lParam);
		return iPort;
	}
	virtual void* GetOutputStorageObject(long* lParam=0,int iPort=0)
	{
		return _Outputs[iPort].GetConnObj(lParam);
	}

	virtual int SetInputCount(int count)
	{
		_Inputs.SetSize(count);
		for (int i=0;i<count;i++)
		{
			SetInputFormat(_fmtIn,i);
		}

		return count;
	};
	virtual int GetInputCount()
	{
		return _Inputs.GetSize();
	}
	virtual int AddInput(int fmt=0)
	{
		if(fmt==0)
		{
			fmt=_fmtIn;
		}
		return _Inputs.Add(MediaPortIn());
	};
	virtual int RemoveInput(int iPort)
	{
		//lock
		int curstate=_state;
		_state=0;
		_Inputs.RemoveAt(iPort);
		_state=1;

		return iPort;
	}
	virtual int SetInputFormat(int fmt,int iPort=-1)
	{
		if (iPort>=0)
		{
			_Inputs[iPort].fmt=fmt;
		}
		else
		{
			for (int i=0;i<_Inputs.GetSize();i++)
			{
				SetInputFormat(fmt,i);
			}

		}
		return iPort;

	}
	virtual int GetInputFormat(int iPort=0)
	{
		return _Inputs[iPort].fmt;
	}
	virtual int SetOutputCount(int count)
	{
		_Outputs.SetSize(count);
		for (int i=0;i<count;i++)
		{
			SetOutputFormat(_fmtOut,i);
		}

		return count;
	}
	virtual int GetOutputCount()
	{
		return _Outputs.GetSize();
	}
	virtual int AddOutput(int fmt=0)
	{
		if(fmt==0)
		{
			fmt=_fmtOut;
		}
		int i= _Outputs.Add(MediaPortOut(fmt));
		return i;
	}
	virtual int RemoveOutput(int iPort)
	{
		_Outputs.RemoveAt(iPort);
		return iPort;
	}
	virtual int SetOutputFormat(int fmt,int iPort=-1)
	{
		if (iPort>=0)
		{
			_Outputs[iPort].fmt=fmt;
		}
		else
		{
			for (int i=0;i<_Outputs.GetSize();i++)
			{
				_Outputs[i].fmt=fmt;
			}

		}
		return iPort;

	}
	virtual int GetOutputFormat(int iPort=0)
	{
		return _Outputs[iPort].fmt;
	}
	int FindPortByMeidaId(bool bOutput,long mid,bool bAdd=0)
	{
		int cnt=bOutput?_Outputs.GetSize():_Inputs.GetSize();
		int port=-1;
		int i=0;
		int iUnused=-1;
		for (i=0;i<cnt;i++)
		{
			MediaPort& p=(bOutput?(MediaPort&)_Outputs[i]:(MediaPort&)_Inputs[i]);
			if(iUnused<0 && p.sid==0) iUnused=i;
			if(p.sid==mid)
			{
				return i;
			}
		}
		if (bAdd)
		{
			if(iUnused>-1) port=iUnused;
			else port=(bOutput?AddOutput():AddInput());
			if(bOutput) _Outputs[port].sid=mid;
			else _Inputs[port].sid=mid;
		}
		return port;
	}

	virtual int Start(){return 0;};
	virtual int Stop(){return 0;}
	virtual int GetStatus(){return _state;}// 0 idle, 1 working
	virtual int EnumConfig(int* pConfigTypes=0){return 0;}
	virtual IMediaConfig* GetConfig(int configType){return 0;}

public:
	int CreateStreamePath(int iIn,int iOut);
	int Create();

};



typedef BaseArray<void*> VideoCaptureArray;
typedef BaseArray<void*> VideoWindowArray;

struct MediaLib:public IMediaLib
{
	IMediaDeviceChangeObserver* _pObserver;
	VideoCaptureArray _vcaps;
	VideoWindowArray _vwnds;
	//AudioCaptureArray _acaps;
	//AudioOutArray _aouts;

public:
	MediaLib();

public:
	virtual void RegisterDeviceChangeObserver(IMediaDeviceChangeObserver* pObserver){_pObserver=pObserver;};
	virtual int AddVideoPlayWindow(PWNDOBJ pWnd)
	{
		//return _vwnds.AddVideoWindow((HWND)pWnd);
		return _vwnds.Add(pWnd);
	};
	virtual void RemoveVideoPlayWindow(int iWnd)
	{
		_vwnds.RemoveAt(iWnd);
	};
	virtual int GetDeviceCount(MediaDeviceType DevType)
	{
		int cnt=0;
		switch (DevType)
		{
		case MediaDeviceVideoCap:
			cnt=_vcaps.GetSize();
			break;
		case MediaDeviceVideoWindow:
			cnt=_vwnds.GetSize();
			break;
		}
		return cnt;

	}
	virtual IMediaDevice* GetDevice(MediaDeviceType DevType,int iDev);
	virtual IMediaProcessor* CreateMediaProcessor(int nInCount,int nOutCount,int fmtIn=0,int fmtOut=0);
	virtual IMediaProcessor* CreateMediaProcessor(const MediaProcessorType& mp)
	{
		return CreateMediaProcessor(mp.nIn,mp.nOut,mp.fmtIn,mp.fmtOut);
	}
	virtual int EnumConfig(int* pConfigTypes=0,int cnt=0){return 0;}
	virtual IMediaConfig* GetConfig(int cfgType){return 0;}

};


};//end namespace


#endif
