#ifndef _SAG_TRANSCODER_SWDEF_
#define _SAG_TRANSCODER_SWDEF_

#include "SWVC3TypeDefine.h"

#define SAFE_DELETE(p)		 { if(p) { delete (p);		(p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);	(p)=NULL; } }
#define SAFE_RELEASE(p)		 { if(p) { (p)->Release();  (p)=NULL; } }
#define SAFE_CloseHandle(p)  { if(p) { CloseHandle(p);  (p)=NULL; } }

#define MAX_TRANSCODER_NUM	64

#define	VIDEO_BUFFER		10
#define AUDIO_BUFFER		20

#define VIDEO_SITE			16
#define VIDEO_SITE_CARD		16
#define AUDIO_SITE			17

#define REPACK_HEADER		0
#define REPACK_HEADER_X		1

#define MAXUDPPACKSIZE		1000	//网络UDP数据包最大值
#define MAXAUDPACKSIZE		800  	//网络UDP音频包最大值

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
		((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
		((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

//视频编码器类型
#define CODEC_XVID	MAKEFOURCC('X','V','I','D')
#define CODEC_MP43	MAKEFOURCC('M','P','4','3')
#define CODEC_SVID  MAKEFOURCC('S','V','I','D')
#define CODEC_JPEG	MAKEFOURCC('J','P','E','G')
#define CODEC_WMV3	MAKEFOURCC('W','M','V','3')
#define CODEC_MSS2	MAKEFOURCC('M','S','S','2')
#define CODEC_H264	MAKEFOURCC('H','2','6','4')
#define CODEC_X264	MAKEFOURCC('X','2','6','4')
#define CODEC_H263  mmioFOURCC('H','2','6','3')
#define CODEC_H265  mmioFOURCC('H','2','6','5')

//内部定义颜色空间类型
typedef enum {
	CODEC_CSP_UNKNOWN,		/* error/unspecified */
	CODEC_CSP_GRAYSCALE,		/* monochrome */
	CODEC_CSP_RGB,			/* red/green/blue */
	CODEC_CSP_YV12,			/* yv12 */
	CODEC_CSP_YUY2,			/* yuy2 */
	CODEC_CSP_I420			/* I420 */
} SW_COLOR_SPACE;

#define AVIIF_KEYFRAME	0x00000010L

//音频编解码器
typedef enum tagAudioCodeC
{
	CODEC_AMR			= 0,//12.8k
	CODEC_ADPCM_32K,		//32k16b
	CODEC_ADPCM_12K,		//12k16b
	CODEC_ADPCM_16K,		//16k16b
	CODEC_ADPCM_8K,			//8k16b
	CODEC_AMR_5K,			//5.6k
	CODEC_UNUSED,			//为了保持兼容性，不使用此值
	CODEC_OPUS,				//opus编码
	CODEC_G711U,			//g711u
	MAX_AUDIO_CODECS
}AudioCodeC;

const int SW_OPUS_ENCODE_BITRATE = 20;

//码流类型(决定解包打包方式)
#define STREAM_SW_SINOWAVE			0   //VC3系统XVID格式
#define STREAM_SW_CODEC			1	//VC3系统Codec格式
#define STREAM_SW_H264			2	//VC3系统H264格式
#define STREAM_SW_AUDIO			10	//VC3系统音频格式(包括指挥会议叠加音频数据)
#define STREAM_AUDIO_PCM		11	//VC3系统Codec音频格式(PCM数据)
#define STREAM_TRANSDIRC		20	//透传数据

//音频原始数据长度
#define PACK_SIZE_AMR			640
#define PACK_SIZE_ADPCM8		640
#define PACK_SIZE_ADPCM12		960
#define PACK_SIZE_ADPCM16		1280
#define PACK_SIZE_ADPCM32		2560
#define PACK_SIZE_OPUS			1280
#define PACK_SIZE_G711U			640

#define VIDEO_WIDTH				352
#define VIDEO_HEIGHT			288

const DWORD SWH264NALU_HEADER   = 0x01000000;
const int VIDEOBUFFERSIZEINIT	= 1024*256;
const int VIDEOBUFFERSIZEMAX	= 1024*1024*2;
const int VIDEOFRAMESSIZEMAX	= (1920*1088*3/2+1920);
const int AUDIOBUFFERSIZEMAX	= 10240;
const int AUDIODECOUTSIZEMAX	= 2560*16;

//帧数据缓冲最大值
const int TOTALVIDEOBUFFERLENGTH = VIDEOBUFFERSIZEMAX*VIDEO_SITE;
const int TOTALAUDIOBUFFERLENGTH = AUDIOBUFFERSIZEMAX*AUDIO_SITE;
const int SCREENBUFFERSIZEMAX	 = 1920*1080*3;//数据会议最大1024x768

const int CODEC_SET_OUT_DIRECTMODE_DECOE = 100020; //透传并且解码模式

typedef enum tagSWMAJORDATAYPE
{
	SW_MAJOR_DATA_UNKNOWN = 0,	//未知类型
	SW_MAJOR_DATA_VIDEO = 1,	//视频流
	SW_MAJOR_DATA_AUDIO = 2,	//音频流
	SW_MAJOR_DATA_DATA  = 3,	//数据流
}SWMAJORDATAYPE;

enum VideoFrameHeaderNewFlag
{
	VideoFrameHeaderNewFlag_Optimizex264	= 0x0001,//小包264包头是否优化 0 - 普通版本; 1 - 优化版本
	VideoFrameHeaderNewFlag_TransFlag		= 0x0002,//单屏广播是否启用透传模式 0 - 未启用; 1 - 启用 启用模式下VideoFrameHeaderNew dwBroadSrcAddr IP
	VideoFrameHeaderNewFlag_FrameType		= 0x0004,//帧类型 0 - P帧; 1 - I帧
	VideoFrameHeaderNewFlag_FakeBand		= 0x0008,//虚拟码流 0 - 不启用; 1 - 启用
	VideoFrameHeaderNewFlag_264HP			= 0x0010,//是否为H.264 HP编码 0 - baseline; 1 - HP
	VideoFrameHeaderNewFlag_MultVideo		= 0x0020,//编码数据是否含有标题 0 - 没有标题; 1 - 含有标题
	VideoFrameHeaderNewFlag_ScanVideoByScene= 0x0040,//是否按场扫描 0 - 帧; 1 - 场
	VideoFrameHeaderNewFlag_EncodeMult		= 0x0080,//编码端编码多路视频输出 0 - 单码流; 1 - 复合流(3路码流)
};


//网络传输协议
typedef enum tagVC3TransmitProtocol
{
	VC3_TransmitProtocol_Normal = 0,	//老协议
	VC3_TransmitProtocol_Optimize,		//优化后的协议
	VC3_TransmitProtocol_Max
}VC3TransmitProtocol;

//VC3终端主数据类型定义
typedef enum tagVC3MajorType
{
	VC3MT_NULL	  = 0,	//未定义
	VC3MT_AUDIO	  = 1,	//音频
	VC3MT_VIDEO	  = 2,	//视频
	VC3MT_DESKTOP = 3,  //桌面数据
	VC3MT_DATA	  = 4,	//数据会议
	VC3MT_CMD     = 5,	//信令
	VC3MT_MAX
}VC3MAJORTYPE;

//音频子类型定义
typedef enum tagVC3AudioSubType
{
	ADOST_FRAME_NORMAL	= 0,//音频帧数据
	ADOST_FRAME_APPEND	= 1,//音频叠加帧数据
	ADOST_FRAME_REPACK	= 2,//音频拆包帧数据
	ADOST_REPACK		= 3,//拆包数据(发端拆包、收端组包)
	ADOST_FEC			= 4,//FEC数据
	ADOST_JITTER		= 5,//防抖
	ADOST_REPACK_EX,		//拆包数据(发端拆包、收端组包,单路音频包长度大于800)
}VC3AUDIOSUBTYPE;

//视频传输子类型定义
typedef enum tagVC3VideoPackSubType
{
	VideoSubType_Raw = 0,	//原始数据
	VideoSubType_Frame,		//加帧头的帧数据
	VideoSubType_Repack,	//拆包后的数据
	VideoSubType_FEC,		//FEC数据
	VideoSubType_Jitter,	//抖动包
	VideoSubType_Max
}VC3VideoPackSubType;

#define PREFIX_SINOWAVE	0x53

#endif /*_SAG_TRANSCODER_SWDEF_*/