#ifndef __IMEDIALIB__H__
#define __IMEDIALIB__H__

#include "IMediaDef.h"


namespace MediaLib{

#define RawVideo		vfRaw
#define RawAudio		afRaw
#define RawData			dfRaw

#define VC3PackedVideo	vfSWVC3
#define VC3PackedAudio	afSWVC3
#define VC3PackedData	dfSWVC3

#pragma warning(disable: 4200)

struct VideoCapData
{
	DWORD dwTimeStamp;
	int Stride;
	int width;
	int height;
	char pDataBits[0];
};


struct MediaFormatMask 
{
	DWORD Format:24;//[fmtVideo|fmtAudio|fmtData|fmtOther],
	DWORD Coded:1;//Plained,Coded
	DWORD Storage:2;//<MediaProcessor,Device,File,Net>
	DWORD Multiple:1;//Single,Multi
	DWORD Type:4;//[Video|Audio|Data|Other]

	MediaFormatMask(int Multiple,int Storage,int Coded,int Format,int type=0)
	{
		this->Multiple=Multiple;
		this->Storage=Storage;
		this->Coded=Coded;
		this->Format=Format;
		this->Type=type;
	}
	MediaFormatMask(int fmt=0)
	{
		*((int*)this)=fmt;
	}
	operator int(){return *((int*)this);}
	MediaFormatMask& operator=(int fmt)
	{
		*((int*)this)=fmt;
		return *this;
	}
	DWORD GetMeidaType()
	{
		return Type;
	}
	bool HasType(MediaType mt)
	{
		return (Type&mt)!=0;
	}
	bool IsComposed()
	{
		return Type!=1&&Type!=2&&Type!=4&&Type!=8;
	}
	bool IsCoded()
	{
		if(IsComposed()) return 1;
		if(Format<256) return 1;
		return 0;
	}
	int GetStreamType()
	{
		if (Multiple) return MediaStreamMuxed;
		if (IsComposed()) return MediaStreamComposed;
		return MediaStreamSingle;
	}
};

//test
const int fmtVC3MultiNetVideo=MediaFormatMask(1,MediaStorageNet,1,VC3PackedVideo,MediaTypeVideo);

//const int fmtVC3MultiNetVideo=MediaFormatMask(1,MediaStorageNet,1,muxVSTP,MediaTypeVideo);
const int fmtVC3MultiNetAudio=MediaFormatMask(1,MediaStorageNet,1,muxVSTP,MediaTypeAudio);
const int fmtVC3MultiNetData=MediaFormatMask(1,MediaStorageNet,1,muxVSTP,MediaTypeData);

const int fmtVC3NetVideo=MediaFormatMask(0,MediaStorageNet,1,VC3PackedVideo,MediaTypeVideo);
const int fmtVC3NetAudio=MediaFormatMask(0,MediaStorageNet,1,VC3PackedAudio,MediaTypeAudio);
const int fmtVC3NetData=MediaFormatMask(0,MediaStorageNet,1,VC3PackedData,MediaTypeData);

const int fmtCodedComposeAV=MediaFormatMask(0,MediaStorageProcessor,1,cmpsRaw,MediaTypeVideo|MediaTypeAudio);
const int fmtDeviceVideo=MediaFormatMask(0,MediaStorageDevice,0,RawVideo,MediaTypeVideo);
const int fmtDeviceAudio=MediaFormatMask(0,MediaStorageDevice,0,RawAudio,MediaTypeAudio);
const int fmtVideo=MediaFormatMask(0,MediaStorageProcessor,0,RawVideo,MediaTypeVideo);
const int fmtAudio=MediaFormatMask(0,MediaStorageProcessor,0,RawAudio,MediaTypeAudio);
const int fmtUnknown=MediaFormatMask(0,0,0,0,0);


struct MediaProcessorType
{
	int nIn;
	int nOut;
	int fmtIn;
	int fmtOut;
	MediaProcessorType(	int nIn,int nOut,int fmtIn,int fmtOut)
	{
		this->nIn=nIn;this->nOut=nOut;this->fmtIn=fmtIn;this->fmtOut=fmtOut;
	};
};

const MediaProcessorType mpVC3CapVideoToNet=MediaProcessorType(1,1,fmtDeviceVideo,fmtVC3NetVideo);
const MediaProcessorType mpVC3CapAudioToNet=MediaProcessorType(1,1,fmtDeviceAudio,fmtVC3NetAudio);
const MediaProcessorType mpVC3NetVideoDisplay=MediaProcessorType(1,1,fmtVC3NetVideo,fmtDeviceVideo);
const MediaProcessorType mpVC3NetAudioPlayer=MediaProcessorType(1,1,fmtVC3NetAudio,fmtDeviceAudio);


const MediaProcessorType mpVC3NetVideoSpliter=MediaProcessorType(1,1,fmtVC3MultiNetVideo,fmtVideo);
const MediaProcessorType mpVC3NetAudioSpliter=MediaProcessorType(1,1,fmtVC3MultiNetAudio,fmtAudio);
const MediaProcessorType mpVC3NetVideoMixer=MediaProcessorType(1,1,fmtVideo,fmtVC3MultiNetVideo);
const MediaProcessorType mpVC3NetAudioMixer=MediaProcessorType(1,1,fmtAudio,fmtVC3MultiNetAudio);
const MediaProcessorType mpVideoPlayer=MediaProcessorType(1,1,fmtVideo,fmtDeviceVideo);
const MediaProcessorType mpAudioPlayer=MediaProcessorType(1,1,fmtAudio,fmtDeviceAudio);
const MediaProcessorType mpVideoCapture=MediaProcessorType(1,1,fmtDeviceVideo,fmtVideo);
const MediaProcessorType mpAudioCapture=MediaProcessorType(1,1,fmtDeviceAudio,fmtAudio);
const MediaProcessorType mpCopySpliter=MediaProcessorType(1,1,fmtUnknown,fmtUnknown);





//IMediaProcessor* MediaLib_CreateMeidaProcessor(int nIn,int fmtIn,int nOut,int fmtOut);


/*
StorageObject:
	Device			-- 	IMdeiaDevice
	Processor		--	IMediaProcessor
	Net				--	ISocket

*/
struct IMediaConfig;
struct IMediaProcessor:public IObject,public IDataSockObserver//public IDataSourceObserver,public IDataSinkObserver
{
	virtual int SetInputCount(int count){return 0;};
	virtual int GetInputCount()=0;
	virtual int AddInput(int fmt=0){return -1;};
	virtual int RemoveInput(int iPort){return -1;};
	virtual int SetInputFormat(int fmt,int iPort=-1){return -1;};
	virtual int GetInputFormat(int iPort=0)=0;
	virtual int SetInputStorageObject(void* pStrogeObject,long lParam,int iPort)=0;
	virtual void* GetInputStorageObject(long* lParam=0,int iPort=0)=0;
	virtual int SetOutputCount(int count){return -1;}
	virtual int GetOutputCount()=0;
	virtual int AddOutput(int fmt=0){return -1;};
	virtual int RemoveOutput(int iPort){return -1;};
	virtual int SetOutputFormat(int fmt,int iPort=-1)=0;
	virtual int GetOutputFormat(int iPort=0)=0;
	virtual int SetOutputStorageObject(void* pStrogeObject,long lParam,int iPort)=0;
	virtual void* GetOutputStorageObject(long* lParam=0,int iPort=0)=0;
	virtual void SetMediaStreamId(long mid,bool bOutput,int iPort=0)=0;
	virtual long GetMediaStreamId(bool bOutput,int iPort=0)=0;
	virtual IDataSource* GetOutputSource(int iPort=0){return 0;};
	virtual IDataSink* GetInputSink(int iPort=0){return 0;};

	virtual int Start(){return 0;};
	virtual int Stop(){return 0;}
	virtual int GetStatus()=0; // 0 idle, 1 working
	virtual int EnumConfig(int* pConfigTypes=0,int count=-1){return 0;}; // return: all config type count, count: buf count
	virtual IMediaConfig* GetConfig(int configType){return 0;}
	virtual long SetUserData(long lUserData)=0;
	virtual long GetUserData()=0;

};

struct IMediaCapDevice:public IMediaDevice,public IDataSource
{
	virtual void RegisterObserver(IDataSourceObserver* pObserver,long userParam)=0;
};


struct IMediaLib:public IMediaDeviceManager
{
	virtual IMediaProcessor* CreateMediaProcessor(int nInCount,int nOutCount,int fmtIn=0,int fmtOut=0)=0;
	virtual IMediaProcessor* CreateMediaProcessor(const MediaProcessorType& mp)=0;
	virtual int EnumConfig(int* pConfigTypes=0,int count=-1)=0; // return: all config type count, count: buf count
	virtual IMediaConfig* GetConfig(int cfgType)=0;

};

IMediaLib* CreateMediaLib(void* pParam=0);

}//end MediaLib

#endif