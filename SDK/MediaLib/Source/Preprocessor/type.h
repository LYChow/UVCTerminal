#ifndef _SWSAG_TRANCODER_TYPE_H_
#define _SWSAG_TRANCODER_TYPE_H_

#include "CaptionDef.h"
#include "SWDef.h"
#pragma pack(1)

const int SW_VC3_SAG_FRAMEHEADER_MAX = 128;
//const int FRAMECAPTIONLENGTHMAX = 63;

//VS转发协议格式（VS Transmit Protocol）
struct VSTP
{
	char szFlag[4];		//特征字符，固定为：VSTP
	unsigned int uSourceIP;	//源IP地址，不因转发而改变
};

typedef struct tagFrameCaptionInfo
{
	FRAMECAPTIONHEADER CaptionHeader;
	char CaptionData[FRAMECAPTIONLENGTHMAX + 1];
	FrameCaptionHeaderEx2 CaptionHeaderEx2;
	tagFrameCaptionInfo() { memset(this, 0x00, sizeof(*this)); }
}FrameCaptionInfo,*pFrameCaptionInfo;

//缓冲区数据及输出数据信息
typedef struct _VEUTranscodeInfo 
{
	WORD  StreamType;	//码流类型(打包类型)
	DWORD dwTimeStamp;	//时间戳
	DWORD flagFOURCC;	//编解码器类型:如MAKEFOURCC('D','I','V','X')等	
	WORD  Width;		//图像宽度
	WORD  Height;		//图像高度
	DWORD CodeSize;		//数据长度
	char  FrameRate;	//输出帧率
	char  dwFlags;		//帧标志
	WORD  BandWidth;	//输出带宽
	DWORD FrameNo;      //帧序号
	DWORD uSourceIP;	//源IP地址，不因转发而改变
	BYTE  HostName[16]; //终端名称
	int   FECMatrixType;//丢包恢复FEC矩阵类型
	BOOL  bInterlaced;	//是否存在交织
	BOOL  bWaitIFrame;  //是否等关键帧
	char  FrameHeader[SW_VC3_SAG_FRAMEHEADER_MAX];	//原始流中的VIDEOFRAMEHEADER
	char  CaptionInfo[sizeof(FrameCaptionInfo)];
}VEUTRANSCODEINFO;

typedef struct _AEUTranscodeInfo
{
	WORD  StreamType;	//码流类型(打包类型)
	DWORD dwTimeStamp;	//时间戳
	char  wSrcFormatTag;//编解码器类型: 私有编码标志：0x00 - AMR; 0x01 - ADPCM(32k); 0x02 - ADPCM(12k); 0x03 - ADPCM(16k); 0x04 - ADPCM(8k);
	char  wDstFormatTag;//转码后编码器类型
	char  GroupParam;	//FEC参数标志：高4位表示FEC组间参数，低4位表示FEC组内参数
	DWORD CodeSize;		//数据长度
	WORD  BandWidth;	//输出带宽
	DWORD FrameNo;		//帧序号
	DWORD uSourceIP;	//源IP地址，不因转发而改变
	BYTE  HostName[16]; //终端名称
}AEUTRANSCODEINFO;

typedef struct _AVEUFrameInfo
{
	DWORD dwSize;		//结构大小
	int   nType;		//数据类型：0-视频；1-音频
	union
	{
		VEUTRANSCODEINFO  vi;   //视频信息
		AEUTRANSCODEINFO  ai;  //音频信息
	};
	BYTE Reversed[32]; //其它信息
}AVEUFRAME_INFO;


//解包状态表：纠错时使用（支持多个通道）
struct UNPACKSTATUS
{
	struct STATUS { DWORD offset; DWORD len; } status[2048];
	int count;
	UNPACKSTATUS() { memset(this, 0x00, sizeof(UNPACKSTATUS)); }
	BOOL IsFirstPacketRecved(void) 
	{
		BOOL bRecv = FALSE;
		for (int i = 0; i < count; i++)
		{
			if (status[i].offset == 0 && status->len > 0)
			{
				bRecv = TRUE;
				break;
			}
		}
		return bRecv;
	}
	int FindMaxOffset(DWORD &dwMaxOffset)
	{
		int nMaxOffsetCount = 0;
		dwMaxOffset = 0;
		for (int i = 0; i < count; i++)
		{
			if (dwMaxOffset < status[i].offset)
			{
				dwMaxOffset = status[i].offset;
				nMaxOffsetCount = i;
			}
		}
		return nMaxOffsetCount;
	}
};

typedef struct _REPACKHEADER
{
	char prefix;	//前导字符，固定为PREFIX_SINOWAVE
	char type;		//bit3-bit0包类型：1 - 视频包；2 - 音频包；3 - 文本包；4 - 命令（系统）包；5 - 其它包
					//bit4是否重新打包：1 - 未重新打包；0 - 已重新打包
	WORD offset;	//当前有效数据包在帧数据中的偏移量(不含本头部）
	BYTE packcounter;	//剩余包记数，随着包的到达而减小，形成完整帧数据时减小为0，只对帧的第一个包有效
	BYTE FrameNoLowByte;	//帧编号最低字节
} REPACKHEADER;		//由于拆包需要附加到每个小包的头部的结构

//扩展REPACKHEADER,满足CIF大数据流量要求
typedef struct _REPACKHEADEREX
{
	char prefix;
	char type;		//0x00 表示新版本音频包；0x21表示新版本FEC包；0x06表示老版本音频包；0x11表示老版本FEC包
	DWORD offset;
	WORD  packcounter;
	BYTE  FrameNoLowByte;
} REPACKHEADEREX;

//编码视频时间戳
typedef struct tagVIDEOTIMESTAMP 
{
	struct 
	{
		DWORD dwLowPart;//CaptureTime
		DWORD dwHighPart; //SendTime(FinishEncodeTime)
	};
	DWORD dwStartEncodeTime;
	WORD wBandWidth;
	WORD bOptimizeNalu:1; //小包264包头是否优化 0 - 普通版本; 1 - 优化版本
	WORD bTransFlag:1;   //单屏广播是否启用透传模式 0 - 未启用; 1 - 启用 启用模式下VIDEOFRAMEHEADER结构的HostName前四个字节表示源IP
	WORD bEncodeMult:1;	//编码端编码多路视频输出 0 - 单码流; 1 - 复合流(3路码流)
	WORD wResevred:13;
	//	ULONGLONG QuadPart;
}VIDEOTIMESTAMP;

//Lichensys视频时间结构
typedef struct tagLICHENSYSTIME
{
	WORD wYear;
	WORD wMon;
	WORD wDay;
	WORD wHour;
	WORD wMin;
	WORD wSec;
	WORD wMillionSec;
	WORD wReserved;
}LICHENSYSTIME, *pLICHENSYSTIME;

//视频时间结构
typedef union VIDEOFRAMETIME
{
	LICHENSYSTIME   LichensysTime;
	VIDEOTIMESTAMP  VideoTimeStamp;
	VIDEOFRAMETIME(){ memset(this, 0x00, sizeof(*this)); }
} *LPVIDEOFRAMETIME;

/*
*视频帧头结构
*
*原始数据
*	视频包：VIDEOFRAMEHEADE+Data
*无FEC数据
*	视频包：REPACKHEADEREX(0x01或0x07)+拆包后视频数据
*带FEC数据
*	视频包：FECGROUPHEADER(0x0B)+REPACKHEADEREX(0x1或0x07)+拆包后视频数据
*	冗余包：FECGROUPHEADER(0x0A)+FECData
*/
typedef struct tagVIDEOFRAMEHEADER {
		char  prefix;	//前导字符，固定为PREFIX_SINOWAVE
		char  type;		//bit3-bit0包类型：1 - 视频包；2 - 音频包；3 - 文本包；4 - 命令（系统）包；5 - 其它包； 7-大视频包
						//bit4是否重新打包：1 - 未重新打包；0 - 已重新打包
						//bit5数据包叠加字幕信息：1-该数据不需要在解码叠加字幕， 0-该数据需要在解码时叠加字幕
						//bit6表示该视频数据是否是按场处: 1-按场处理, 0-按帧处理
		DWORD flagFOURCC;	//MAKEFOURCC('M','P','4','3'), MAKEFOURCC('D','I','V','X')等  
		DWORD WidthHeight;	//视频解析度：高16位为宽度，低16位为高度，例如0x02D00240表示720x576
        DWORD FrameNo;		//帧序号
		char  FrameRate;	//帧律
		char  dwFlags;		//帧标志
        int   CodeSize;		//帧数据大小
		char  CodeOffset;	//帧数据开始偏移量，下一字节为基准
		unsigned char HostName[16];		//
		BYTE  ChecksumLowByte;	//帧数据按4字节(int)校验求和（不足4字节舍弃）后经BYTE类型转换后的校验值
		VIDEOFRAMETIME timestamp;
		unsigned char HostNameEx[32];//扩展终端名称
} VIDEOFRAMEHEADER; //视频帧头结构

#define GETCODERELATIVEPOS(h) ((char*)&h.CodeOffset-(char*)&h+sizeof(h.CodeOffset)+h.CodeOffset)
#define GETSCREENCODERELATIVEPOS(h) ((char*)&h.videoheader.CodeOffset-(char*)&h+sizeof(h.videoheader.CodeOffset)+h.videoheader.CodeOffset)

#define GETOFFSETOFVDOHDRV1(h) ((char*)&h.timestamp -(char*)&h.HostName+sizeof(h.timestamp))
#define GETOFFSETOFVDOHDRV2(h) ((char*)&h.HostNameEx-(char*)&h.HostName+sizeof(h.HostNameEx))


/////////////////////////////////////////////////
// 特别说明(2006-03-21)：
//      为了实现双模式混音，对音频的UDP帧数据包进行了扩展，现在完整的一个帧数据包可能是这样一个格式：
//		AUDIOFRAMEHEADER + 编码数据包(通道1) + AUDIOPACKHEADER_EX + 编码数据包(通道2) + AUDIOPACKHEADER_EX + 编码数据包(通道3) + ...
////////////////////////////////////////////////////

typedef struct tagFECGROUPHEADER {
		char  prefix;	//前导字符，固定为PREFIX_SINOWAVE
		char  type;		//0x06 - 音频包；0x07 - FEC冗余包, 0x08 - 新版FEC冗余包(按网络包计算FEC)
		char  GroupParam;	//FEC参数标志：高4位表示FEC组间参数，低4位表示FEC组内参数
		int   CodeSize;		//数据长度
        DWORD GroupNo;		//组号
		char  SubGroupNo;	//分组组号
		char  GroupInnerNo;	//分组内序号
} FECGROUPHEADER; //音频帧头结构

/* 
* modify by hxf 09-02-01
* 扩展音频头结构(兼容老版本),新增uSrcIP
* 因为以路数分帧，所以当音频包丢失只是丢某路数据，不会影响其它路的音频，所以该结构中应该包含详细帧信息
*/
typedef struct tagAUDIOFRAMEHEADER {
		char  prefix;	//前导字符，固定为PREFIX_SINOWAVE
		char  type;		//bit3-bit0包类型：1 - 视频包；2 - 音频包(ADPCM(12k16b))；3 - 扩展音频包(amr,ADPCM(32k16b))；
		char  wFormatTag;	//私有编码标志： 0x00 - AMR; 0x01 - ADPCM(32); 0x02 - ADPCM(12); 0x03 - ADPCM(16)
        DWORD FrameNo;
        int  CodeSize;		//编码数据包（单个通道）的长度
		char  CodeOffset;	//编码数据包开始偏移量，下一字节为基准
		char  NextSegmentFlag;	//帧内编码数据包后续段标志，2006-03-21双混音模式扩充，非0值表示帧内有附加的编码数据包（多通道）
		DWORD uSrcIP;		//该编码数据包最原始的来源IP地址
} AUDIOFRAMEHEADER; //音频帧头结构

typedef struct tagAudioMixInfo
{
	DWORD SrcIPAddr;
	WORD wFrameNo;
	tagAudioMixInfo() { memset(this, 0x00, sizeof(tagAudioMixInfo)); }
}AudioMixInfo, *pAudioMixInfo;

//音频主席混音扩展结构
typedef struct tagAUDIOMIXAPPENDHEADER
{
	char  prefix;			//前导字符，固定为PREFIX_SINOWAVE
	char  type;				//bit3-bit0包类型：1 - 视频包；2 - 音频包(ADPCM(12k16b))；3 - 扩展音频包(amr,ADPCM(32k16b))；
	char  wFormatTag;		//私有编码标志： 0x00 - AMR; 0x01 - ADPCM(32); 0x02 - ADPCM(12); 0x03 - ADPCM(16)
	DWORD nTotalMixCount;	//混音总通道数
	int   CodeSize;			//为兼容老版本,此位必须为0
	char  CodeOffset;		//编码数据包开始偏移量，下一字节为基准
	char  NextSegmentFlag;	//为兼容老版本,此位必须为0		
	AudioMixInfo MixInfo[AUDIO_SITE];
	tagAUDIOMIXAPPENDHEADER() { memset(this, 0x00, sizeof(tagAUDIOMIXAPPENDHEADER)); }
	int GetCodeOffset(void) { return 1 + nTotalMixCount*sizeof(AudioMixInfo); }
	int GetNetHeaderSize(void) { return 12 + CodeOffset; }
}AUDIOMIXAPPENDHEADER, *pAUDIOMIXAPPENDHEADER;

#define MAX_GROUP_OUTER		10
#define MAX_GROUP_INNER		5
#define MAX_AUDIO_BUFFER	3072

//解包状态表：纠错时使用（支持多个通道）
typedef struct tag_FECGROUPSTATUS
{
	struct	STATUS { WORD offset; WORD len; } status[128];	//音频帧状态(偏移量、长度)
	char	RcvBuf[MAX_GROUP_OUTER*MAX_AUDIO_BUFFER];		//音频接收缓存
	char*	FramePack[MAX_GROUP_OUTER];					    //音频帧指针(每个组中最大包数)
	int		nCount;		//处理音频包计数
	int     nLostCount; //丢包计数(只能恢复丢一个包，若丢两个包则不处理)
	int     nLostPos;   //丢失数据包的位置
	BOOL    bHasRcvFEC[MAX_GROUP_INNER]; //是否收到FEC包(最大分组数)
}FECGROUPSTATUS;

typedef struct tag_STACKAPTSTATUS
{
	int	 nSendPacketIndex; //已发送包序号
	BOOL bHasRcvPack[MAX_GROUP_OUTER*MAX_GROUP_INNER];
	char* FramePack[MAX_GROUP_OUTER*MAX_GROUP_INNER];
	int   nFrameLen[MAX_GROUP_OUTER*MAX_GROUP_INNER];
}STACKAPTSTATUS;

typedef struct tag_AUDIOUNPACKSTATUS
{
	int	nGroupOuterParam; //组间间隔参数
	int	nGroupInnerParam; //组内间隔参数
	int nGroupNo;		  //组序号
	int nGroupPacketIndex;//组中包序号(递增)
	STACKAPTSTATUS stacks;//剩余包索引
	FECGROUPSTATUS status[MAX_GROUP_INNER]; //组结构信息
	DWORD dwLastPackRcvTicks;//上一个数据包到达时间
}AUDIOUNPACKSTATUS;

#define GETQOSCMDDATAPOS(h) ((char*)&h.wCodeOffset-(char*)&h+sizeof(h.wCodeOffset)+h.wCodeOffset) 
//////////////////////////////////////////////////////////////////////////
//音频相关概念描述
//  音频块( block )：指40ms的音频数据块，在一个音频帧内存放两个或多个40ms的音频数据块以提高音频传输效率。
//  音频段(segment): 指一路音频数据通道，在指挥或会议模式下，主席叠加多路音频数据到本地音频数据后时使用。
//////////////////////////////////////////////////////////////////////////
//VC3通用头结构信息
typedef struct tagVC3COMMONHEADER
{
	char prefix;			//前导字符，固定为PREFIX_SINOWAVE
	BYTE fixtype:4;			//固定值(0x0F)
	BYTE version:4;			//版本号(0x01)
	char majortype;			//主类型，参见VC3MAJORTYPE定义
}VC3COMMONHEADER;

//拆包结构
typedef struct tagVC3RepackHeader
{
	char subtype;			//子类型，参见VC3SUBTYPE定义
	DWORD offset;			//当前有效数据包在帧数据中的偏移量(不含本头部）
	WORD  packcounter;		//剩余包记数，随着包的到达而减小，形成完整帧数据时减小为0，只对帧的第一个包有效
	BYTE  FrameNoLowByte;	//帧编号最低字节
} VC3REPACKHEADER;

//音频帧通用结构
typedef struct tagVC3AudioCommon
{
	char subtype;		//子类型，参见VC3SUBTYPE定义
	BYTE nextsegment:1;	//是否存在扩展数据(0-不存在；1-存在)
	BYTE blocks:3;		//帧内音频帧块数(默认为1，帧内最多可存储5个块，每个片断40ms，总延时200ms)
	BYTE formattag:4;	//编码类型(0-AMR(12.8k); 1-ADPCM(32k16b); 2-ADPCM(12k16b); 3-ADPCM(16k16b); 4 ADPCM(8k16b))
	WORD CodeSize;		//编码数据包（单个通道）的长度
	char CodeOffset;	//编码数据包开始偏移量，下一字节为基准
}VC3AUDIOCOMMON;

//音频帧结构
typedef struct tagVC3AUDIOFRAMEHEADER : public VC3AUDIOCOMMON
{
	WORD FrameNo;		//帧序号(44分钟循环)
}VC3AUDIOFRAMEHEADER;

//音频帧叠加扩展结构
typedef struct tagVC3AudioAppendFrameHeader : public VC3AUDIOCOMMON
{
	DWORD uSrcIP;		//该编码数据包最原始的来源IP地址
}VC3AUDIOAPPENDFRAMEHEADER;

//音频拆包首帧结构（此时有效数据较大，所以在首包的首帧中多增加uSrcIP对整体带宽影响不大）
typedef struct tagVC3AudioRepackHeader : public VC3AUDIOCOMMON
{
	WORD FrameNo;		//帧序号(44分钟循环)
	DWORD uSrcIP;		//该编码数据包最原始的来源IP地址
}VC3AUDIOREPACKHEADER;

//音频主席混音附加包结构
typedef struct tagVC3AudioMixAppendHeader : public VC3AUDIOCOMMON
{
	BYTE nTotalMixCount;	//总混音通道数
	AudioMixInfo MixInfo[AUDIO_SITE];
	tagVC3AudioMixAppendHeader() { memset(this, 0x00, sizeof(*this)); }
	int GetCodeOffset(void) { return 1 + nTotalMixCount*sizeof(AudioMixInfo); }
	int GetNetHeaderSize(void) { return 5 + CodeOffset; }
}VC3AudioMixAppendHeader, *pVC3AudioMixAppendHeader;

typedef struct tagLICHENSYSOSD
{
	char  Prefix[8];	//特征字符，固定为：LICH_OSD
	BYTE  HostName[16];	//终端名称
	int   FrameRate;	//帧率
	VIDEOFRAMETIME timestamp;
}LICHENSYSOSD;

typedef struct tagSCREENINFO
{
	DWORD				dwBitCount;
	DWORD				dwBandWidth;
	DWORD				dwCursorPos;
	DWORD				dwCursorTypeInfo;
} SCREENINFO;

typedef struct tagSCREENFRAMEHEADER
{
	VIDEOFRAMEHEADER	videoheader;
	DWORD				dwBitCount;
	DWORD				dwBandWidth;
	DWORD				dwCursorPos;
	DWORD				dwflag;		//预留 0x01 - 解码需要flip一次
} SCREENFRAMEHEADER;


/*
* 旧版本主席叠加后续音频包头
*/
typedef struct tagAUDIOPACKHEADER_EX {
	int   CodeSize;		//编码数据包（单个通道）的长度
	char  CodeOffset;	//编码数据包开始偏移量，下一字节为基准
	char  NextSegmentFlag;	//帧内编码数据包后续段标志，非0值表示帧内有更多的编码数据包附加在后面
	DWORD uSrcIP;		//该编码数据包最原始的来源IP地址
} AUDIOPACKHEADER_EX;	//音频帧扩展包结构，该结构及与其对应的编码数据包，

typedef struct HOSTName {
	char HostName[16+1];
	HOSTName(){ memset(this,0,sizeof(*this)); }
} HOSTNAME;

#define REPACK_HEADER		0
#define REPACK_HEADER_X		1

typedef enum tagSWSTREAMTYPE
{
	SW_STREAM_UNKNOWN = 0,  //未知类型
	SW_STREAM_RECORD = 1,	//录像扩展结构
	SW_STREAM_COMMON = 2,	//网络扩展结构
}SWSTREAMTYPE;

//码流录像结构头
typedef struct tagSWSStreamRecHeader
{
	char prefix;		//前导字符，固定为PREFIX_HIKVISION=0x48
	char type;			//0x0F(扩展结构)
	char subtype;		//子结构类型: 1 录像扩展结构；2 网络扩展结构
	BYTE flags;		//保留（扩展为majortype）
	int   CodeSize;		//编码数据包（单个通道）的长度
	DWORD CodeOffset;	//当前有效数据包在帧数据中的偏移量(不含本头部）
	DWORD dwTimeStamp;	//时间戳
	DWORD dwSrourceIP;	//源地址
}SWSTREAMRECHEADER;

//接收数据头描述信息出
typedef struct tagSWSStreamNetHeader
{
	char prefix;		//前导字符，固定为PREFIX_HIKVISION=0x48
	char type;			//0x0F(扩展结构)
	char subtype;		//子结构类型: 1 录像扩展结构；2 网络扩展结构
	char majortype;		//数据类型，参考SWSTREAMTYPE定义
	int   CodeSize;		//编码数据包（单个通道）的长度
	DWORD CodeOffset;	//当前有效数据包在帧数据中的偏移量(不含本头部）
	DWORD dwTimeStamp;	//时间戳
}SWSTREAMRNETHEADER;

#define VIDEO_BUFFER_SIZE			100*4096
#define FRAME_BUFFER_SIZE			5120 //5k/伪帧

//帧数据缓冲最大值
//const int SCREENBUFFERSIZEMAX		= 1024*768*3;    //数据会议最大1024x768

//屏幕帧数据类型
#define SCREEN_NORMALFRAME				0
#define SCREEN_KEYFRAME					1
#define SCREEN_FRAME_PRIVATEDATA		2
#define SCREEN_FRAME_MOUSE				3
#define SCREEN_FRAME_ALIVEFRAME			4
#define SCREEN_FRAME_NOSIGNAL			5
#define SCREEN_FRAME_OUTOFRANGE			6
#define SCREEN_FRAME_UNRECOGNISABLE		7
#define SCREEN_FRAME_PLEASEWAIT			8  //等待关键帧
#define SCREEN_FRAME_CREATEDECOD_ERROR	9  //创建解码器失败
#define SCREEN_FRAME_DECODERDATA_ERROR	10 //解码失败

// Macro that makes a FOURCC from four characters.
#define wmcFOURCC( ch0, ch1, ch2, ch3 )				\
		( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |	\
		( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

#define WMCFOURCC_WMV1     wmcFOURCC('W','M','V','1')
#define WMCFOURCC_WMV2     wmcFOURCC('W','M','V','2')
#define WMCFOURCC_WMV3     wmcFOURCC('W','M','V','3')
#define WMCFOURCC_WMVA     wmcFOURCC('W','M','V','A')
#define WMCFOURCC_WVC1     wmcFOURCC('W','V','C','1')
#define WMCFOURCC_WMVP     wmcFOURCC('W','M','V','P')
#define WMCFOURCC_WVP2     wmcFOURCC('W','V','P','2')
#define WMCFOURCC_MSS1     wmcFOURCC('M','S','S','1')
#define WMCFOURCC_MSS2     wmcFOURCC('M','S','S','2')

const int SW_NET_PACKET_LEN_MAX = 1024*8;

typedef struct CodecParam
{
	/*
	*关键帧类型
	*	KEY_FIXED_TIME		固定时间间隔，I帧间隔时间由nMaxSecondInterval指定
	*	KEY_FIXED_FRAME		固定帧数间隔，I帧间隔帧数由nMaxFramesInterval指定
	*	KEY_RQUST_IFNEED	按需请求关键帧
	*/
	int nKeyFrameType;		
	int nMaxSecondInterval;			//固定时间间隔值
	int nMaxFramesInterval;			//固定帧数间隔值
	BOOL bDepressKeyFrameQulity;	//降低关键帧质量
	BOOL bUseX264;
	int  nCodecID;
	CodecParam(){ memset( this, 0, sizeof(*this) ); };
}CODECPARAM;

typedef struct tagVC3VideoCommonHeader : public VC3COMMONHEADER
{
	BYTE  VideoType;		//流标志 0 - 主码流; 1 - 子码流1; 2 - 子码流2;
	BYTE  PacketType;		//包类型参见 VC3VideoPackSubType 定义
}VC3VideoCommonHeader, *pVC3VideoCommonHeader;

typedef struct tagVideoFrameHeaderNew : public VC3VideoCommonHeader
{
	BYTE  FrameRate;		//帧率  
	WORD  wBandWidth;		//带宽
	DWORD dwTimeStamp;		//时间戳
	DWORD dwFlag;			//标志位 参见 VideoFrameHeaderNewFlag 定义
    LONG  CodeSize;			//帧数据大小
	BYTE  CodeOffset;		//帧数据开始偏移量，下一字节为基准
	BYTE  ChecksumLowByte;	//帧数据按4字节(int)校验求和（不足4字节舍弃）后经BYTE类型转换后的校验值
	DWORD FrameNo;			//帧序号
	WORD  wWidth;			//图像宽度
	WORD  wHeight;			//图像高度
	DWORD dwFlagFOURCC;		//MAKEFOURCC('H','2','6','4'), MAKEFOURCC('D','I','V','X')等
	char  HostNameEx[32];	//扩展终端名称
	DWORD dwBroadSrcAddr;	//广播透传时的被广播终端IP地址
	tagVideoFrameHeaderNew() { memset(this, 0x00, sizeof(*this)); }
	int GetCodeOffset(void) { return sizeof(*this) - 21; }
	static int GetNatHeadSize(const tagVideoFrameHeaderNew &Header)
	{
		return (char *)&Header.CodeOffset - (char *)&Header + sizeof(Header.CodeOffset) + Header.CodeOffset;
	}
}VideoFrameHeaderNew, *pVideoFrameHeaderNew;

typedef struct tagVC3VideoRepackHeader : public VC3VideoCommonHeader
{
	DWORD offset;			//当前有效数据包在帧数据中的偏移量(不含本头部）
	WORD  packcounter;		//剩余包记数，随着包的到达而减小，形成完整帧数据时减小为0，只对帧的第一个包有效
	BYTE  FrameNoLowByte;	//帧编号最低字节
}VC3VideoRepackHeader, *pVC3VideoRepackHeader;

#pragma pack()

#endif //_SWSAG_TRANCODER_TYPE_H_