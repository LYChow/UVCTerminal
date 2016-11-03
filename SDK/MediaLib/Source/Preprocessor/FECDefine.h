#ifndef _FECDEFIME_H_2010_08_03_
#define _FECDEFIME_H_2010_08_03_

#include "SWVC3TypeDefine.h"

#pragma pack(push ,1)
//RS jerasure 丢包恢复相关
const int SWVC3_RS_FEC_MATRIX_MAX = 101;

const int FEC_DATA_COUNT_MAX = 34;		//数据包最大个数
const int FEC_REDU_COUNT_MAX = 6;		//冗余包最大个数
const int FEC_PACKET_MAX = 1024*2;		//做FEC数据最大长度
const int FEC_PACKET_HEAD_MAX = 256;	//FEC头最大长度
const int FEC_PACKET_TOTAL_LEN_MAX = FEC_PACKET_HEAD_MAX + FEC_PACKET_MAX; //EFC包最大长度
const int FEC_TIMEOUT = 60000;

const int FEC_PACKET_HEAD_RES = 256;	//FEC头最大长度
const int RS_FEC_DATA_COUNT_MAX = 128;	//数据包最大个数
const int RS_FEC_REDU_COUNT_MAX = 96;	//冗余包最大个数
const int RS_FEC_DATA_COUNT = 16;		//数据包默认值
const int FEC_PACKET_PREFIX = 0x09;		//FEC前导字符

/* 编码端抗丢包的矩阵类型 */
typedef enum tagFECMatrixIndex
{
    FECMatrix_2_1  = 0, /*  2-1矩阵，数据包 2个，校验包1个 */
    FECMatrix_4_3  = 1, /*  4-3矩阵，数据包 4个，校验包3个 */
    FECMatrix_4_4  = 2, /*  4-4矩阵，数据包 4个，校验包4个 */
    FECMatrix_6_4  = 3, /*  6-4矩阵，数据包 6个，校验包4个 */
	FECMatrix_9_3  = 4, /*  9-3矩阵，数据包 9个，校验包3个 */
    FECMatrix_9_4  = 5, /*  9-4矩阵，数据包 9个，校验包4个 */
    FECMatrix_10_4 = 6, /* 10-4矩阵，数据包10个，校验包4个 */
    FECMatrix_10_5 = 7, /* 10-5矩阵，数据包10个，校验包5个 */
    FECMatrix_15_5 = 8, /* 15-5矩阵，数据包15个，校验包5个 */
    FECMatrix_20_5 = 9, /* 20-5矩阵，数据包20个，校验包5个 */
    FECMatrix_24_6 = 10,/* 24-6矩阵，数据包24个，校验包6个 */
    FECMatrix_29_6 = 11,/* 29-6矩阵，数据包29个，校验包6个 */
    FECMatrix_34_6 = 12,/* 34-6矩阵，数据包34个，校验包6个 */
    FECMatrix_MAX,      /* 枚举值的最大值                  */

	FECMatrix_RS_Base = 1000,
	FECMatrix_RS_0 = FECMatrix_RS_Base,
	FECMatrix_RS_1,
	FECMatrix_RS_100 = FECMatrix_RS_Base + SWVC3_RS_FEC_MATRIX_MAX - 1,
	FECMatrix_RS_Max,
}FECMatrixIndex;

typedef enum tagFECPacketType
{
	FEC_Type_Video = 0x0,
	FEC_Type_Audio = 0x1,
	FEC_Type_Data  = 0x2,
	FEC_Type_MAX   = 0x7
}FECPacketType;

typedef enum tagFECType
{
	FEC_Data = 0x0, //数据包
	FEC_Redu = 0x1	//冗余包
}FECType;

/* FEC矩阵的结构体 */
typedef struct tagFECMatrix
{
    WORD wNumDataCnt;									 /* 数据包个数 */
    WORD wNumReduCnt;									 /* 冗余包个数 */
    WORD wValue[FEC_REDU_COUNT_MAX][FEC_DATA_COUNT_MAX]; /* 数据包和冗余包的对应关系 */
}FECMatrix, *pFECMatrix;

/* RS FEC矩阵的结构体 */
typedef struct tagRSFECMatrix
{
    WORD wNumDataCnt;									 /* 数据包个数 */
    WORD wNumReduCnt;									 /* 冗余包个数 */
	void Reset(void) { wNumDataCnt =0; wNumReduCnt = 0; }
//	tagRSFECMatrix(void) { Reset(); }
}RSFECMatrix, *pRSFECMatrix;

/*FEC结构子类型*/
typedef struct tagFECSubType 
{
	unsigned char PacketType:3; //FECPacketType类型
	unsigned char FECType   :1;	//0-FEC数据包  1-FEC冗余包
	unsigned char version   :4;	//版本号
}FECSubType;

/* FEC包头结构 */
typedef struct tagFECPacketHeader
{
	char  prefix;		//前导字符，固定为PREFIX_SINOWAVE
	char  type;			/*bit3-bit0
						0x06 - 音频FEC音频包(老版本) ；
						0x07 - 老版FEC冗余包；
						0x08 - 音频FEC冗余包(按网络包计算FEC)；
						0x09 - FEC包(音视频通用);
						0x0A - Jitter包；		
						0x0B - 未定义*/
	FECSubType  subtype;//子类型 高4位表示版本号 低4位表示数据类型
	char  GroupParam;	//FEC参数标志：矩阵索引
	DWORD dwGroupNo;	//组号
	DWORD dwPacketNo;	//包序号
	int   nCodeSize;	//数据长度
	char  GroupIntraNo;	//分组内序号
	char  CodeOffset;	//帧数据开始偏移量,下一字节为基准
	char  tagOffset;    //结构偏移量,下一字节为基准
	char  Reserve;

	tagFECPacketHeader(){memset(this, 0x00, sizeof(*this));}
}FECPacketHeader, *pFECPacketHeader;

//用于存储做FEC数据的真实长度,与真实数据一起做异或运算,丢包恢复后可以得知此长度
typedef struct tagFECPayload 
{
	DWORD dwPayloadlen;
	tagFECPayload() {memset(this, 0, sizeof(*this));}
}FECPayload, *pFECPayload;
#pragma pack(pop)
#endif // _FECDEFIME_H_2010_08_03_