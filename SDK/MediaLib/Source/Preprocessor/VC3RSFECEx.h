// VC3RSFEC.h: interface for the CVC3RSFECEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VC3RSFECEX_H__21EBE9C1_F1DB_446E_B1D0_C15F4E61D270__INCLUDED_)
#define AFX_VC3RSFECEX_H__21EBE9C1_F1DB_446E_B1D0_C15F4E61D270__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SWFECBase.h"

#pragma pack(push ,8)
extern "C"
{
#include "RS/jerasure.h"
}
#pragma pack(pop)

#pragma pack(push ,4)
class CVC3RSFECEx : public CSWFECBase  
{
public:
	/************************************************************************/
	/*FEC包构造相关函数:													*/
	/************************************************************************/
	//初始化 BuildCallBack为构建FEC包后的回调函数指针
	virtual int InitBuildFEC(int nDataType, int FECType, pPacketCallBack BuildCallBack, 
		LONG dwUser = 0, VC3FECPacketType nPacketType = VC3FEC_Type_Normal);
	
	//生成FEC包
	virtual int BuildPacket(const char *pData, int nDatalen, DWORD flag = 0);
	
	//释放构造FEC包资源
	virtual int ReleaseBuild();
	
	/************************************************************************/
	/*FEC数据恢复相关函数                                                   */
	/************************************************************************/
	//初始化 ResumeCallBack为恢复FEC包后的回调函数指针
	virtual int InitResume(pResumeCallBack ResumeCallBack, pFrameNoCallBack FrameNoCallBack, LONG dwUser);
	
	//根据FEC包恢复数据
	virtual int ResumePacket(const char *pData, int nDatalen, DWORD flag1 = 0, DWORD flag2 = 0);
	
	//释放恢复FEC包资源
	virtual int ReleaseResume();
private:
	void BuildReset(void);
	void ResumeReset(void);
	int SendResumePacket(DWORD dwGroupNo);
	int ResumeData(void);
	int GetPacketNum(void) const;
	void InitResumeMatrix(void);
public:
	CVC3RSFECEx();
	virtual ~CVC3RSFECEx();
private:
	/************************************************************************/
	/* 构造FEC包相关变量                                                    */
	/************************************************************************/
	int m_nBuidSizeMax;								//数据长度+sizeof(VC3FECPayload);
	int m_FECBuildType;
	RSFECMatrix m_BuidCurMatrix;					//当前数据包与冗余包对应矩阵
	char *m_pFECBuidPacket[RS_FEC_REDU_COUNT_MAX];	//生成冗余包缓冲区
	char *m_pFECBuidData[RS_FEC_DATA_COUNT_MAX];		//生成数据包缓冲区
	char *m_pFECBuidDataSendBuf;					//生成数据包缓冲区
	int m_BuildRSMatrix[RS_FEC_REDU_COUNT_MAX][RS_FEC_DATA_COUNT_MAX];

	/************************************************************************/
	/* 恢复数据相关变量                                                     */
	/************************************************************************/
	int m_nHeaderSize;
	int m_nResumeSizeMax;							//数据长度+发送端的VC3FECPayload;
	int m_FECResumeType;							//当前冗余包生成方式
	RSFECMatrix m_ResCurMatrix;					//当前数据包与冗余包对应矩阵
	char *m_pFECResPacket[RS_FEC_REDU_COUNT_MAX];	//记录冗余包缓冲区
	char *m_pFECResData[RS_FEC_DATA_COUNT_MAX];			//记录数据包缓冲区
	int m_ResumeRSMatrix[RS_FEC_REDU_COUNT_MAX][RS_FEC_DATA_COUNT_MAX];
	/************************************************************************/
	/* 记录收到的FEC包的状态:                                               */
	/* 0x00:未收到			                                                */
	/* 0x01:已收到,未发送	                                                */
	/* 0x03:已发送                                                          */
	/************************************************************************/
	unsigned char  m_RecvStatus[RS_FEC_DATA_COUNT_MAX + RS_FEC_REDU_COUNT_MAX];

	/************************************************************************/
	/*类的全局函数                                                          */
	/************************************************************************/
	//恢复FEC包时,用于获取构建FEC包的头长度,构建FEC包端FECPacketHeader与FECPayload结构大小之和
	static int GetRealPosition(const VC3RSFECHeader &PacketHeader);
	
	//恢复FEC包时,用于获取构建FEC包的头长度,为构建FEC包端FECPacketHeader结构大小
	static int GetRealTagSize(const VC3RSFECHeader &PacketHeader);
	
	//构建FEC包时,用于标识FECPacketHeader结构CodeOffset之后结构剩余部分的大小,便于扩展
	static int GetCodeOffset(const VC3RSFECHeader &PacketHeader);
	
	static int GetRealPosition(const VC3RSFECHeaderEx &PacketHeader);
	static int GetRealTagSize(const VC3RSFECHeaderEx &PacketHeader);
	static int GetCodeOffset(const VC3RSFECHeaderEx &PacketHeader);
private:
	CVC3RSFECEx(const CVC3RSFECEx &);
	CVC3RSFECEx &operator = (const CVC3RSFECEx &);
};
#pragma pack(pop)
#endif // !defined(AFX_VC3RSFECEX_H__21EBE9C1_F1DB_446E_B1D0_C15F4E61D270__INCLUDED_)
