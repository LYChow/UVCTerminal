// VC3RSFEC.h: interface for the CVC3RSFEC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VC3RSFEC_H__21EBE9C1_F1DB_446E_B1D0_C15F4E61D270__INCLUDED_)
#define AFX_VC3RSFEC_H__21EBE9C1_F1DB_446E_B1D0_C15F4E61D270__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SWFECBase.h"

extern "C"
{
#include "RS/jerasure.h"
}

class CVC3RSFEC : public CSWFECBase  
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
	CVC3RSFEC();
	virtual ~CVC3RSFEC();
private:
	static RSFECMatrix m_RSMatrix[SWVC3_RS_FEC_MATRIX_MAX];

	/************************************************************************/
	/* 构造FEC包相关变量                                                    */
	/************************************************************************/
	int m_nBuidSizeMax;								//数据长度+sizeof(VC3FECPayload);
	int m_FECBuildType;
	pRSFECMatrix m_pBuidCurMatrix;					//当前数据包与冗余包对应矩阵
	char *m_pFECBuidPacket[RS_FEC_REDU_COUNT_MAX];	//生成冗余包缓冲区
	char *m_pFECBuidData[RS_FEC_DATA_COUNT];		//生成数据包缓冲区
	char *m_pFECBuidDataSendBuf;					//生成数据包缓冲区
	int m_BuildRSMatrix[RS_FEC_REDU_COUNT_MAX][RS_FEC_DATA_COUNT];

	/************************************************************************/
	/* 恢复数据相关变量                                                     */
	/************************************************************************/
	int m_nHeaderSize;
	int m_nResumeSizeMax;							//数据长度+发送端的VC3FECPayload;
	int m_FECResumeType;							//当前冗余包生成方式
	pRSFECMatrix m_pResCurMatrix;					//当前数据包与冗余包对应矩阵
	char *m_pFECResPacket[RS_FEC_REDU_COUNT_MAX];	//记录冗余包缓冲区
	char *m_pFECResData[RS_FEC_DATA_COUNT];			//记录数据包缓冲区
	int m_ResumeRSMatrix[RS_FEC_REDU_COUNT_MAX][RS_FEC_DATA_COUNT];
	/************************************************************************/
	/* 记录收到的FEC包的状态:                                               */
	/* 0x00:未收到			                                                */
	/* 0x01:已收到,未发送	                                                */
	/* 0x03:已发送                                                          */
	/************************************************************************/
	unsigned char  m_RecvStatus[RS_FEC_DATA_COUNT + RS_FEC_REDU_COUNT_MAX];
private:
	CVC3RSFEC(const CVC3RSFEC &);
	CVC3RSFEC &operator = (const CVC3RSFEC &);
};

#endif // !defined(AFX_VC3RSFEC_H__21EBE9C1_F1DB_446E_B1D0_C15F4E61D270__INCLUDED_)
