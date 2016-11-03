/******************************************************************************
  文 件 名   : FEC.h
  功    能	 : FEC前向纠错类
  创 建 者   : zds
  创建日期   : 2010年8月3日

******************************************************************************/

#if !defined(AFX_FEC_H__0CB62FA6_B1E9_4683_9055_41FBE6EB77D6__INCLUDED_)
#define AFX_FEC_H__0CB62FA6_B1E9_4683_9055_41FBE6EB77D6__INCLUDED_

#include "FECDefine.h"
//#include "264include/SmoothSendDefine.h"
#include "SWFECBase.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFEC: public CSWFECBase
{
public:
	/*FEC构造数据回调函数指针定义*/
	typedef int (__stdcall *pPacketCallBack) (const char* pSendData, int nSendDatalen, LONG dwUser);

	/*FEC构恢复后数据回调函数指针定义*/
	typedef int (__stdcall *pResumeCallBack) (const char* pSendData, int nSendDatalen, LONG dwUser, int nChannel, DWORD flag1, DWORD flag2);

	/*FEC帧序号回调函数指针定义*/
	typedef int (__stdcall *pFrameNoCallBack) (DWORD dwFrameNo, LONG dwUser);
public:
	/************************************************************************/
	/*FEC包构造相关函数:													*/
	/************************************************************************/
	//初始化 BuildCallBack为构建FEC包后的回调函数指针
//	int InitBuildFEC(FECPacketType nType, FECMatrixIndex FECType, pPacketCallBack BuildCallBack, DWORD dwUser);
	int InitBuildFEC(int nDataType, int FECType, pPacketCallBack BuildCallBack, 
		LONG dwUser = 0, VC3FECPacketType nPacketType = VC3FEC_Type_Normal);

	//生成FEC包
	int BuildPacket(const char *pData, int nDatalen, DWORD flag = 0);
	
	//释放构造FEC包资源
	int ReleaseBuild();

	/************************************************************************/
	/*FEC数据恢复相关函数                                                   */
	/************************************************************************/
	//初始化 ResumeCallBack为恢复FEC包后的回调函数指针
	int InitResume(pResumeCallBack ResumeCallBack, pFrameNoCallBack FrameNoCallBack, LONG dwUser);

	//根据FEC包恢复数据
	int ResumePacket(const char *pData, int nDatalen, DWORD flag1 = 0, DWORD flag2 = 0);

	//释放恢复FEC包资源
	int ReleaseResume();

	//设置、获取通道号
//	void SetChannel(int nChannel) { m_nChannel = nChannel;}
//	int  GetChannel() const { return m_nChannel;};
	/************************************************************************/
	/*类的全局函数                                                          */
	/************************************************************************/
	//恢复FEC包时,用于获取构建FEC包的头长度,构建FEC包端FECPacketHeader与FECPayload结构大小之和
	static int GetRealPosition(const FECPacketHeader &PacketHeader);

	//恢复FEC包时,用于获取构建FEC包的头长度,为构建FEC包端FECPacketHeader结构大小
	static int GetRealTagSize(const FECPacketHeader &PacketHeader);

	//构建FEC包时,用于标识FECPacketHeader结构CodeOffset之后结构剩余部分的大小,便于扩展
	static int GetCodeOffset(const FECPacketHeader &PacketHeader);
private:	
	//重新构建FEC包序列
	int BuildReset();

	//重置丢包恢复状态
	int ResumeReset();

	//nGroupIntraNo(数据包)是否为丢包恢复矩阵中nFECPacketNo(冗余包)生成所需的第一个包
	BOOL IsFECFirstPacket(int nGroupIntraNo, int nFECPacketNo, const FECMatrix &CurMatrix) const;

	//获取已收到一组FEC包中数据包的总个数
	int GetPacketNum() const;

	//一组FEC包已经恢复结束,发送所有数据
	int SendResumePacket(DWORD dwGroupNo);

	//根据FEC冗余包恢复数据包
	int ResumeData();
public:
	CFEC();
	virtual ~CFEC();
private:
	//复制控制
	CFEC(const CFEC &OtherObject);//拷贝构造函数
	CFEC &operator = (const CFEC &OtherObject);//重载赋值
private:
	//通道号
	int m_nChannel;

	/************************************************************************/
	/* 构造FEC包相关变量                                                    */
	/************************************************************************/
	int   m_nBuidPacketType;					//包类型(视频、音频)
	DWORD m_dwBuidPacketNo;						//数据包序号(不包含冗余包)
	DWORD m_dwBuidGroupNo;						//FEC组号
	int   m_nBuidGroupIntraNo;					//组内序号
	LONG  m_dwBuildUser;						//回调函数m_BuildCallBack的调用者,如果m_BuildCallBack为全局函数m_dwBuildUser可以为NULL
	int   m_nBuildEvent;						//标识 m_BuildCallBack 是否正在执行
	DWORD m_dwBuildFlag;						//标识数据源是否发生变化
	DWORD m_dwSendTick;							//发送上一个包的时间
	FECMatrixIndex m_FECBuildType;				//当前冗余包生成方式
	pPacketCallBack m_BuildCallBack;			//生成FEC包后回调函数
	pFECMatrix m_pBuidCurMatrix;				//当前数据包与冗余包对应矩阵
	char *m_pFECBuidPacket[FEC_REDU_COUNT_MAX];	//生成冗余包缓冲区
	char *m_pFECBuidData;						//生成数据包缓冲区

	/************************************************************************/
	/* 恢复数据相关变量                                                     */
	/************************************************************************/
	int   m_nResPacketType;						//包类型(视频、音频)
	DWORD m_dwResPacketNo;						//数据包序号(不包含冗余包)
	DWORD m_dwResGroupNo;						//FEC组号
	DWORD m_dwResFinGopNo;						//已经发送FEC的组号
	int   m_nResGroupIntraNo;					//组内序号
	LONG  m_dwResumeUser;						//回调函数m_BuildCallBack的调用者,如果m_BuildCallBack为全局函数m_dwBuildUser可以为NULL
	BOOL  m_bLostPacket;						//数据包是否连续(不包含冗余包)
	int   m_nResumeEvent;						//标识 m_ResumeCallBack 是否正在执行
	DWORD m_dwResumeFlag1;						//标识数据源是否发生变化
	DWORD m_dwResumeFlag2;
	DWORD m_dwRecveTick;						//收到上一个包的时间
	FECMatrixIndex m_FECResumeType;				//当前冗余包生成方式
	pResumeCallBack m_ResumeCallBack;			//恢复FEC包后回调函数
	pFrameNoCallBack m_FrameNoCallBack;			//帧序号回调函数
	pFECMatrix m_pResCurMatrix;					//当前数据包与冗余包对应矩阵
	char *m_pFECResPacket[FEC_REDU_COUNT_MAX];  //记录冗余包缓冲区
	char *m_pFECResData[FEC_DATA_COUNT_MAX];    //记录数据包缓冲区

	/************************************************************************/
	/* 记录收到的FEC包的状态:                                               */
	/* 0x00:未收到			                                                */
	/* 0x01:已收到,未发送	                                                */
	/* 0x03:已发送                                                          */
	/************************************************************************/
	char  m_RecvStatus[FEC_DATA_COUNT_MAX + FEC_REDU_COUNT_MAX];


	/************************************************************************/
	/*类的全局变量                                                          */
	/************************************************************************/
	//数据包与冗余包关系矩阵
	static FECMatrix m_TriBudMatStu[FECMatrix_MAX];
};

#endif // !defined(AFX_FEC_H__0CB62FA6_B1E9_4683_9055_41FBE6EB77D6__INCLUDED_)