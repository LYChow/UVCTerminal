#ifndef _H264_DEFINE_H_20100518_
#define _H264_DEFINE_H_20100518_

#include "SWVC3TypeDefine.h"

#pragma pack(push ,1)

typedef enum tagColorSpace264 // 颜色空间
{
	ColorSpace_264_YUY2,
	ColorSpace_264_RGB24,
	ColorSpace_264_YV12,
	ColorSpace_264_I420,
	ColorSpace_264_UYVY
}ColorSpace264;

const int PAYLOAD_264_MAX = 1024*2;
const int VIDEOBUFFERSIZEMAX264 = 2048000;//409600;

typedef struct tagPacket264
{
	DWORD dwFrameNum;
	WORD  dwPacketNum;
	WORD  dwPacketTotalNum;
	WORD  dwFrameType;
	WORD  dwNalutype;
	int   nPacketSize;
	DWORD dwReserve;
	DWORD dwReserve1;
	BYTE  Payload[PAYLOAD_264_MAX];
	tagPacket264(void) { Reset(); }
	void Reset(void) { memset(this, 0, sizeof(*this)); }
	static int GetHeadSize(void) { return sizeof(tagPacket264) - PAYLOAD_264_MAX; }
}Packet264, *pPacket264;

typedef struct tagH263PictureHeader
{	
	DWORD PSC1 : 16; //固定为0x0000
	
	DWORD RT1 :2;
	DWORD PSC2 : 6;	//固定为0x20
	
	DWORD PType2Distinct261 :1; //固定为0
	DWORD PType1StartCode	:1; //固定  1
	DWORD RT2 :6;
	
	DWORD PType10MotionVector :1;
	DWORD PType9Type :1;	//I帧 - 0, P帧 - 1
	DWORD PType6Fmt :3;
	DWORD PType5FullPictureFreeze :1;
	DWORD PType4DocumentCamera :1;
	DWORD PType3SplitScreen :1;
	
	DWORD dwReserve :5;
	DWORD PType13PBFramesMode :1;
	DWORD PType12AdvancedPredictionMode :1;
	DWORD PType11ArithmeticCoding :1;
	
	DWORD dwReserve1 :16;
	
	tagH263PictureHeader(void) { Reset(); }
	int GetRT(void) { return (RT1<<6)|RT2; }
	void Reset(void) { memset(this, 0x00, sizeof(*this)); }
	BOOL CheckPSC(void) { return (PSC1 == 0x00) && (PSC2 == 0x20) && (PType1StartCode == 1) && (PType2Distinct261 == 0); }
}H263PictureHeader, *pH263PictureHeader;

/* 网络顺序
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|F|P|SBIT |EBIT | SRC |I|U|S|A|R      |DBQ| TRB |    TR         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
typedef struct tagRFC2190ModeA
{
	BYTE EBIT	:3;
	BYTE SBIT	:3;
	BYTE P		:1;
	BYTE F		:1;
	
	BYTE R1		:1;
	BYTE A		:1;
	BYTE S		:1;
	BYTE U		:1;
	BYTE I		:1;
	BYTE SRC	:3;

	BYTE TRB	:3;
	BYTE DBQ	:2;
	BYTE R2		:3;	

	BYTE TR;
	tagRFC2190ModeA() { Reset(); }
	void Reset(void) { memset(this, 0x00, sizeof(*this)); }
}RFC2190ModeA, *pRFC2190ModeA;

#define SAFE_DELETE_Context(p) { if(p) { if(p->codec){ avcodec_close(p); } av_free(p); (p)=NULL; } }
#define SAFE_DELETE_Frame(p) { if(p) { av_free(p); (p)=NULL; } }
#define SAFE_FREE(p) { if(p) { free(p); (p)=NULL; } }
#define SAFE_DESTROY(p, pCoder) { if(p) { p->Destroy(pCoder); (pCoder) = NULL;} }

const int ENCODE_FRAME_MAX = 99;
const int H264_DECODER_BUFLEN = 1024*1024*2;

const int H264_DECODER_MAX = 36 + 1 + 1 + 2;//最大支持36分屏+网采+双流+预留

#define H264_TYPE_PVOP  0
#define H264_TYPE_IVOP	1

typedef enum tagSWDecode264Type
{
	SWDecode264Type_SofeWare = 0,
	SWDecode264Type_Intel,
	SWDecode264Type_DXVA,
	SWDecode264Type_Max
}SWDecode264Type;

#pragma pack(pop)

#endif//_H264_DEFINE_H_20100518_