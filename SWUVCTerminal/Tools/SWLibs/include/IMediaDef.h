#ifndef __MEDIADEF__H__
#define __MEDIADEF__H__
#include "IDataObject.h"


namespace MediaLib{

typedef void* PWNDOBJ;
typedef DWORD SWMEDIASOURCEID,*LPSOURCEID;
enum MediaStorageType
{
	MediaStorageProcessor,//
	MediaStorageDevice,//
	MediaStorageNet,//
	MediaStorageFile,//

};

enum MediaType
{
	MediaTypeNone = 0,
	MediaTypeVideo = 0x1,
	MediaTypeAudio = 0x2,
	MediaTypeData	= 0x4,

};


enum MediaDeviceType
{
	MediaDeviceUnknown=0,
	MediaDeviceVideoCap=0x0001,
	MediaDeviceAudioCap=0x0002,
	MediaDeviceVideoWindow=0x0101,//
	MediaDeviceAudioOut=0x0102,
};

enum MediaStreamDirType
{
	MediaStreamDirUnknown =0,
	MediaStreamDirInput,
	MediaStreamDirOutput,
	MediaStreamDirDuplex,
};

enum MediaVideoFormatType
{
	vfRaw=0,
	vfSWVC3,
	vfH264,

	vfRGB32	=	256,//			--	[RGB0]...
	vfRGB24,//			--	[RGB]...
	vfYUY2,//YUYV422	--	[YUYV]...
	vfUYVY,//UYVY422	--  [UYVY]...
	vfNV12,//			--	[YY/YY]..,[UV]...	
	vfNV21,//			--	[YY/YY]..,[VU]...
	vfI420,//YUV420	--	[YY/YY]...,[U]...,[V]...
	//	vfYV12,//YVU420	--	[YY/YY]...,[V]...,[U]...


};
enum MediaAudioFormatType
{
	afRaw=0,
	afSWVC3,


	afPCM=256,


};

enum MediaDataFormatType
{
	dfRaw=0,
	dfSWVC3,
};
enum MediaComposedType
{
	cmpsRaw =0,

};

enum MediaMuxType
{
	muxRaw=0,
	muxVSTP,
};

enum MediaStreamTypeEnum
{
	MediaStreamSingle=0,
	MediaStreamComposed,
	MediaStreamMuxed,
};



struct IMdeiaController;
struct IMediaDevice:public IObject
{
	virtual int GetType()=0;//MediaDeviceType
	virtual LPCTSTR GetDeviceName()=0;
	virtual long GetDeviceId()=0;
	virtual void* GetObject()=0;
};


struct MediaConfigType
{
	DWORD Type:2;//<none,video,audio,caption>
	DWORD Dir:2;//<no,in,out,in+out>
	DWORD Class:2;//<no,device,net,converter>
	DWORD SubClass:10;//
	DWORD Reserved:16;//
	MediaConfigType(int mediaType,int dir,int cls,int subCls):Type(mediaType),Dir(dir),Class(cls),SubClass(subCls),Reserved(0){};
	MediaConfigType(const MediaConfigType& mc){*this=mc;};
	MediaConfigType(int mc=0){*LPDWORD(this)=mc;}
	operator int(){return *((int*)this);}

};

const int mcfgStream=MediaConfigType(MediaTypeNone,MediaStreamDirUnknown,0,0);
const int mcfgVideoStream=MediaConfigType(MediaTypeVideo,MediaStreamDirUnknown,0,0);
const int mcfgAudioStream=MediaConfigType(MediaTypeAudio,MediaStreamDirUnknown,0,0);
const int mcfgVideoCap=MediaConfigType(MediaTypeVideo,MediaStreamDirInput,0,0);
const int mcfgAudioCap=MediaConfigType(MediaTypeAudio,MediaStreamDirInput,0,0);
const int mcfgVideoRender=MediaConfigType(MediaTypeVideo,MediaStreamDirOutput,0,0);
const int mcfgAudioRender=MediaConfigType(MediaTypeAudio,MediaStreamDirOutput,0,0);
const int mcfgVideoCoder=MediaConfigType(MediaTypeVideo,MediaStreamDirInput,3,0);
const int mcfgAudioCoder=MediaConfigType(MediaTypeAudio,MediaStreamDirInput,3,0);
const int mcfgVideoDecoder=MediaConfigType(MediaTypeVideo,MediaStreamDirOutput,3,0);
const int mcfgAudioDecoder=MediaConfigType(MediaTypeAudio,MediaStreamDirOutput,3,0);
const int mcfgNetSend=MediaConfigType(MediaTypeNone,MediaStreamDirOutput,2,0);
const int mcfgNetRecv=MediaConfigType(MediaTypeNone,MediaStreamDirInput,2,0);

const int mcfgVideoCaption=MediaConfigType(MediaTypeData,MediaStreamDirUnknown,0,1);





struct MFRATIO
{
	UINT Numerator;
	UINT Denominator;
	MFRATIO(UINT q=0,UINT r=1)
	{
		Numerator=q;Denominator=r;
	}
	MFRATIO(const MFRATIO& ro)
	{
		Numerator=ro.Numerator;
		Denominator=ro.Denominator;
	}
	operator double(){return Numerator*1.0/Denominator;}
};

struct MFSIZE
{
	int width;
	int height;
};



struct MediaFormatDesp
{
	DWORD dwSize;
	int FormatType;
	DWORD dwFlags;
	int BitRate;//Number of bits per second
	int BitRateMax;
	int BitRateMin;
	int AvgBitRate;
};

struct VideoFormatDesp:public MediaFormatDesp
{
	MFSIZE FrameSize;
	MFRATIO FrameRate;
	MFRATIO FrameRateMax;
	MFRATIO FrameRateMin;
	MFRATIO PixelRatio;
	MFRATIO AspectRatio;
	int InterlaceMode;
	int Stride;
};

struct AudioFormatDesp:public MediaFormatDesp
{
	int ChannelCount;
	int SampleSize;//Number of bits per sample in one channel
	MFRATIO SampleRate;
	MFRATIO SampleRateMax;
	MFRATIO SampleRateMin;
	int nBlockAlgin;
};

enum MediaConfigNotifyCode
{
	MCN_VideoCaptureSnapshotPictureEnd =0 ,//szImageFilePath=(LPCTSTR)pParam
};

enum MediaStreamPrivatDataName
{
	MSPD_VideoCoderKeyFrameIntval =0,//nMinKeyFrameIntval=(int)pParam,-1=tempoarily insert,0=auto,
	MSPD_SWVC3PackFormat,//nPackFormat=(int)pParam,0=no Jitter,1=Jitter1,2=Jitter2
};


struct IMediaConfigCallback
{
	virtual void OnMediaConfigCallback(struct IMediaConfig* pCfg,int nNotifyCode,void* pParam)=0;
};


struct IMediaConfig:public IObject
{
	virtual int GetType()=0;
	virtual void SetConfigCallback(IMediaConfigCallback* pCallback=0)=0;
};


struct IMediaStreamConfig:public IMediaConfig
{
    int GetType(){return mcfgStream;}
	virtual int GetSupportedFormatCount()=0;
	virtual int GetCurrentFormat()=0;
	virtual int SetCurrentFormat(int iFmt)=0;
	virtual int GetFormatDesp(MediaFormatDesp* pDesp,int iFmt=-1)=0; // pDesp: VideoFormatDesp/AudioFormatDesp
	virtual int SetStreamPrivateData(int iName,void* pValue=0)=0;
	virtual int GetStreamPrivateData(int iName,void* pValue,int size)=0;
	virtual int SetBitRate(int BitRate)=0;
	virtual int StartRecord(const char* szRecordFilePath,int iRecordFileFormat=0)=0;
	virtual void StopRecord()=0;
};

struct IVideoStreamConfig:public IMediaStreamConfig
{
    int GetType(){return mcfgVideoStream;}
	virtual MFRATIO GetCurrentFrameRate()=0;
	virtual int SetCurrentFrameRate(MFRATIO rate)=0;
	
};

struct IAudioStreamConfig:public IMediaStreamConfig
{
    int GetType(){return mcfgAudioStream;}
	virtual MFRATIO GetCurrentSampleRate()=0;
	virtual int SetCurrentSampleRate(MFRATIO rate)=0;
	virtual int SetGain(int val)=0;
	virtual int GetGain()=0;

};


struct IVideoCaptureConfig:public IVideoStreamConfig
{
	int GetType(){return mcfgVideoCap;}
	virtual int SnapshotPicture(const char* szImageFilePath,int iImageFileType=0)=0; //async call
    virtual int StartPreview(PWNDOBJ pViewWnd)=0;
	virtual void StopPreview()=0;
};

struct IVideoRenderConfig;


struct IAudioCaptureConfig:public IAudioStreamConfig
{
	int GetType(){return mcfgAudioCap;}
	virtual int EnableAEC(bool bEnable=1)=0;
	virtual int EnableNoiseReduction(bool bEnable=1)=0;
	virtual int EnableAGC(bool bEnable=1)=0;

};

struct IAudioRenderConfig:public IMediaConfig
{
	int GetType(){return mcfgAudioRender;}
	virtual int SetVoluem(int volume)=0;
	virtual int GetVoluem()=0;
};

struct INetStreamConfig:public IMediaConfig
{
	virtual int SetMuxFormat(int iMuxFormat)=0;
	virtual int GetMuxFormat()=0;
	virtual int SetFECType(int type,void* pParam)=0;
	virtual int GetFECType(void* pParam)=0;
	virtual int SetNATHost(const char* szHostAddr)=0;
};

struct INetSendConfig:public INetStreamConfig
{
	int GetType(){return mcfgNetSend;}
	virtual int SetSmoothType(int type,void* pParam)=0;
	virtual int GetSmoothType(void* pParam)=0;
	virtual IMediaStreamConfig* GetInputStreamConfig(int iPort=0)=0;
};

struct INetRecvConfig:public INetStreamConfig
{
	int GetType(){return mcfgNetRecv;}
};





struct VideoCaptionStyle
{
	DWORD dwBkColor;
	DWORD dwTextColor;
	char FontName;
	WORD FontSize;
	WORD hAlign:2;//left,center,right
	WORD vAlign:2;//top,center,bottom
	WORD Wrap:2;//no,char,word
	WORD AutoSize:1;//no,yes
	WORD EndElipse:1;//no,yes
};

struct RenderMode 
{
	DWORD hAlign:2;
	DWORD vAlign:2;
	DWORD hStretch:1;
	DWORD vStretch:1;
	DWORD KeepRatio:1;
    DWORD Mirror:1;
    DWORD Flip:1;
	RenderMode()
	{
        *LPDWORD(this)=0;hAlign=1;vAlign=1;hStretch=1;vStretch=1;KeepRatio=0;Mirror=0;Flip=0;
	}
};




struct IVideoRenderConfig:public IMediaConfig
{
	int GetType(){return mcfgVideoRender;}
	virtual int GetSourceSize(int& widht,int& height)=0;
	virtual int SetRenderSize(int widht,int height)=0;
	virtual int GetRenderSize(int& widht,int& height)=0;
	virtual int SetRenderMode(int mode)=0;//RenderMode
	virtual int GetRenderMode()=0;
	virtual int SetIdleBkColor(DWORD dwRGB)=0;
	virtual int SetIdleBitmap(LPVOID pBitmap)=0;
	virtual int SetBrightness(int percent)=0;
	virtual int GetBrightness()=0;
	virtual int CaptureImage(LPCTSTR szFileName,int type=0)=0;

};

struct IVideoCaptionConfig:public IMediaConfig
{
	int GetType(){return mcfgVideoCaption;}
	virtual int GetCaptionCount()=0;
	virtual int AddCaption(int x,int y,int width,int height,VideoCaptionStyle* pStyle,LPCTSTR szCaption)=0;
	virtual int RemoveCaption(int iCaption)=0;
	virtual int SetCaptionText(int iCaption,LPCTSTR szCaption)=0;
	virtual int SetCaptionStyle(int iCaption,VideoCaptionStyle* pStyle)=0;
	virtual int SetCaptionRect(int iCaption,int x,int y,int widht,int height)=0;
	virtual int GetCaptionRect(int iCaption,int& x,int& y,int& widht,int& height)=0;
	virtual VideoCaptionStyle* GetCaptionStyle(int iCaption)=0;
	virtual LPCTSTR GetCaptionText(int iCaption)=0;
	virtual int CalcCaptionSize(VideoCaptionStyle* pStyle,LPCTSTR szText,int& width,int& height)=0;
};



//////////////////////////////////////////////////////////////////////////


struct IMediaDeviceChangeObserver
{
	virtual void OnMediaDeviceStateChange(int DevType,int iDev,int state)=0;
};

struct IMediaDeviceManager:public IObject
{
	virtual void RegisterDeviceChangeObserver(IMediaDeviceChangeObserver* pObserver)=0;
	virtual int AddVideoPlayWindow(PWNDOBJ pWnd)=0;
	virtual void RemoveVideoPlayWindow(int iWnd)=0;
	virtual int GetDeviceCount(MediaDeviceType DevType)=0;
	virtual IMediaDevice* GetDevice(MediaDeviceType DevType,int iDev=0)=0;

};


};//end MediaLib

#endif
