#ifndef _VC3_FECDEFIME_H_2012_1_11_
#define _VC3_FECDEFIME_H_2012_1_11_
#include "FECDefine.h"
#pragma pack(push,1)

//const int FEC_TIMEOUT = 1000;

// typedef enum tagSWFECType
// {
// 	FEC_Data = 0x0, //数据包
// 	FEC_Redu = 0x1	//冗余包
// }SWFECType;

typedef enum tagSWFECVersion
{
	SWFECVersion_Nor = 0x1,
	SWFECVersion_RS,
	SWFECVersion_RSEx,
	SWFECVersion_Max
}SWFECVersion;

/*FEC结构子类型*/
// typedef struct tagFECSubType 
// {
// 	unsigned char PacketType:3; //FECPacketType类型
// 	unsigned char FECType   :1;	//0-FEC数据包  1-FEC冗余包
// 	unsigned char version   :4;	//版本号
// }FECSubType;

typedef enum tagVC3FECPacketType
{
	VC3FEC_Type_Normal  = 0x0,	//正常数据量 如音频
	VC3FEC_Type_Special = 0x1,	//大数据量 如264小包
	VC3FEC_Type_MAX
}VC3FECPacketType;

/* FEC包头结构 */
typedef struct tagVC3FECPacketHeader
{
	unsigned char datatype;		//数据类型，参见VC3SUBTYPE定义
	FECSubType  subtype;//子类型 高4位表示版本号 低4位表示数据类型
	unsigned char  GroupParam;	//FEC参数标志：矩阵索引
	WORD  dwGroupNo;	//组号
	WORD  dwPacketNo;	//包序号
	WORD  nCodeSize;	//数据长度
	unsigned char  GroupIntraNo;	//分组内序号
	unsigned char  CodeOffset;	//帧数据开始偏移量,下一字节为基准
	unsigned char  tagOffset;    //结构偏移量,下一字节为基准

	tagVC3FECPacketHeader(){memset(this, 0x00, sizeof(*this));}
}VC3FECPacketHeader, *pVC3FECPacketHeader;

typedef struct tagVC3FECPacketHeaderEx
{
	char  prefix;		//前导字符，固定为PREFIX_SINOWAVE
	unsigned char datatype;		//数据类型，参见VC3SUBTYPE定义
	FECSubType  subtype;//子类型 高4位表示版本号 低4位表示数据类型
	unsigned char  GroupParam;	//FEC参数标志：矩阵索引
	DWORD dwGroupNo;	//组号
	DWORD dwPacketNo;	//包序号
	int  nCodeSize;	//数据长度
	unsigned char  GroupIntraNo;	//分组内序号
	unsigned char  CodeOffset;	//帧数据开始偏移量,下一字节为基准
	unsigned char  tagOffset;    //结构偏移量,下一字节为基准

	tagVC3FECPacketHeaderEx(){memset(this, 0x00, sizeof(*this));}
}VC3FECPacketHeaderEx, *pVC3FECPacketHeaderEx;

/* FEC包头结构 */
typedef struct tagVC3RSFECHeader
{
	unsigned char datatype;		//数据类型，参见VC3SUBTYPE定义
	FECSubType  subtype;//子类型 高4位表示版本号 低4位表示数据类型
	WORD  GroupParam;	//FEC参数标志：矩阵索引,MAKEWORD(数据包个数,冗余包个数)
	WORD  dwGroupNo;	//组号
	WORD  dwPacketNo;	//包序号
	WORD  nCodeSize;	//数据长度
	unsigned char  CodeOffset;	//帧数据开始偏移量,下一字节为基准
	unsigned char  tagOffset;    //结构偏移量,下一字节为基准
	unsigned char  GroupIntraNo;	//分组内序号

	tagVC3RSFECHeader(){memset(this, 0x00, sizeof(*this));}
}VC3RSFECHeader, *pVC3RSFECHeader;

typedef struct tagVC3RSFECHeaderEx
{
	char  prefix;		//前导字符，固定为PREFIX_SINOWAVE
	unsigned char datatype;		//数据类型，参见VC3SUBTYPE定义
	FECSubType  subtype;//子类型 高4位表示版本号 低4位表示数据类型
	WORD  GroupParam;	//FEC参数标志：矩阵索引, MAKEWORD(数据包个数,冗余包个数)
	DWORD dwGroupNo;	//组号
	DWORD dwPacketNo;	//包序号
	int  nCodeSize;	//数据长度
	unsigned char  CodeOffset;	//帧数据开始偏移量,下一字节为基准
	unsigned char  tagOffset;    //结构偏移量,下一字节为基准
	unsigned char  GroupIntraNo;	//分组内序号

	tagVC3RSFECHeaderEx(){memset(this, 0x00, sizeof(*this));}
}VC3RSFECHeaderEx, *pVC3RSFECHeaderEx;

//用于存储做FEC数据的真实长度,与真实数据一起做异或运算,丢包恢复后可以得知此长度
typedef struct tagVC3FECPayload 
{
	WORD dwPayloadlen;
	tagVC3FECPayload() {memset(this, 0x00, sizeof(*this));}
}VC3FECPayload, *pVC3FECPayload;

#pragma pack(pop)
#endif // _VC3_FECDEFIME_H_2012_1_11_