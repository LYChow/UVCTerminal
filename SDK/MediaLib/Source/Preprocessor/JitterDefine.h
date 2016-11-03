#ifndef _JITTER_DEFINE_H_2010_08_18_
#define _JITTER_DEFINE_H_2010_08_18_

#include "SwBaseWin.h"

#pragma pack(push ,1)

const int JITTER_BUFFER_LEN = 2048;
const int JITTER_BUFFER_LEN_MAX = 1024*64;
const int JITTER_PACKET_MAX = 100;
const int JITTER_PACKET_TTL = JITTER_PACKET_MAX*2;
const int JITTER_RECVER_TIMEOUT = 500;

//乱序恢复模式
typedef enum tagSWJitterRecverType
{
	SWJitterRecverType_Asyn = 0, //异步恢复
	SWJitterRecverType_Syn		 //同步恢复
}SWJitterRecverType;

/* Jitter包头结构 */
typedef struct tagJitterPacketHeader
{
	char  prefix;		//前导字符，固定为PREFIX_SINOWAVE
	char  type;			/*bit3-bit0
						0x06 - 音频FEC音频包(老版本) ；
						0x07 - 老版FEC冗余包；
						0x08 - 音频FEC冗余包(按网络包计算FEC)；
						0x09 - FEC包(音视频通用);
						0x0A - Jitter包；		
						0x0B - 未定义*/
	char  subtype;      //子类型 高4位表示版本号 低4位表示数据类型
	char  Reserve;	
	DWORD dwPacketNo;	//包序号
	DWORD dwTimeStamp;	//时间戳
	int   nCodeSize;	//数据长度
	char  CodeOffset;	//帧数据开始偏移量,下一字节为基准
	char  Reserve1[3];

	tagJitterPacketHeader(){memset(this, 0x00, sizeof(*this));}
}JitterPacketHeader, *pJitterPacketHeader;

//恢复网络抖动引起丢包信息
typedef struct tagJitterPacket
{
	BOOL  bAvailData;
	DWORD dwPacketNo;
	int   nTTL;
	char *pData;
	int   nBufferLen;
	int   nDatalen;
	DWORD dwFlag1;
	DWORD dwFlag2;

	tagJitterPacket() { memset(this, 0x00, sizeof(*this)); }

	void Reset(void)
	{
		bAvailData = FALSE;
		dwPacketNo = 0;
		dwFlag1 = 0;
		dwFlag2 = 0;
	}
}JitterPacket, *pJitterPacket;

typedef struct tagJitterGroup 
{
	DWORD nSendPacketNo;
	DWORD nPacketCount;
	int nJitterPacketMax;
	int nJitterPacketTTL;
	pJitterPacket pJitterData;

	tagJitterGroup() { memset(this, 0x00, sizeof(*this)); nJitterPacketMax = JITTER_PACKET_MAX; nJitterPacketTTL = JITTER_PACKET_TTL;}
	void Reset()
	{
		nSendPacketNo = 0; 
		nPacketCount = 0;
		if (NULL != pJitterData)
		{
			for (int i = 0; i < nJitterPacketMax; i++) 
			{
				(pJitterData + i)->Reset();
			}
		}
	}

	BOOL IsBufFull(void) {return nPacketCount >= (DWORD)nJitterPacketMax;}
}JitterGroup, *pJitterGroup;

typedef struct tagJitterSDHead
{
	char prefix[4]; //数据头
	int  nLength;	//长度
	tagJitterSDHead() {memset(this, 0, sizeof(*this));}
}JitterSDHead,*pJitterSDHead;
#pragma pack(pop)
#endif // _JITTER_DEFINE_H_2010_08_18_

