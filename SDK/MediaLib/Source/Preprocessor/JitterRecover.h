/******************************************************************************
  文 件 名   : JitterRecover.h
  功    能	 : 乱序重排类
  创 建 者   : zds
  创建日期   : 2010年8月18日

******************************************************************************/

#if !defined(AFX_RECOVERJITTER_H__96ACB9F8_4E29_4FFA_95A1_F557F77E9A19__INCLUDED_)
#define AFX_RECOVERJITTER_H__96ACB9F8_4E29_4FFA_95A1_F557F77E9A19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JitterDefine.h"

#pragma pack(push, 4)

class CJitterRecover  
{
public:
	/************************************************************************/
	/*乱序发送端相关函数:													*/
	/************************************************************************/
	char *AddHeader(const char *pData, int nDatalen, int &nOutlen);	

	/************************************************************************/
	/*乱序恢复端相关函数:													*/
	/************************************************************************/
	/*乱序恢复后数据回调函数指针定义*/
	typedef int (__stdcall *pSendPacketCallBack) (const char* pSendData, int nSendDatalen, LONG dwUser, DWORD dwflag);
	
	//初始化丢包恢复	nTyp: 0 - 异步机制  1 - 同步机制
	int InitRecover(pSendPacketCallBack SendCallback, LONG dwUser, int nRecoverBufSize = 30, int nType = SWJitterRecverType_Syn);

	//处理乱序,不额外开启线程
	int DealJitterDataEx(const char *pData, int nDatalen, DWORD dwFlag);

	//同步模式中使用 乱序恢复并发送数据包
	int RecoverAndSendPacket(void);

	//复位
	void Reset();
	/************************************************************************/
	/*类的全局函数                                                          */
	/************************************************************************/
	//恢复抖动包时,用于获取构建抖动包的头长度,为构建抖动包端JitterPacketHeader结构大小
	static int GetRealSize(const JitterPacketHeader &PacketHeader);

	//构建抖动包时,用于标识JitterPacketHeader结构CodeOffset之后结构剩余部分的大小,便于扩展
	static int GetCodeOffset(const JitterPacketHeader &PacketHeader);
private:
	//获取乱序缓冲区中的最小包序号及对应位置
	int  GetMixNumPos(DWORD &dwMixPcaketNo) const;

	//将排序后的数据放入发送缓冲区
	int  PutDataIn(const char *pData, int nDatalen, int nPos);
private:
	int    m_nErrorData;
	DWORD  m_dwFlag;
	DWORD  m_dwPacketNum;
	BOOL   m_bSendThread;
	LONG   m_dwCallBackUser;
	DWORD  m_dwRecveTick;

	JitterGroup m_JitterGroup;

	char *m_pSendBuffer;//发送加抖动头的缓冲区
	char *m_pDataInBuf;	 //排序后数据进入LoopBuffer之前的缓冲区
	char *m_pCallBackBuf;//回调函数发送缓冲区
	pSendPacketCallBack m_pPacketCallBack;//回调函数
public:
	CJitterRecover();
	virtual ~CJitterRecover();
};
#pragma pack(pop)
#endif // !defined(AFX_RECOVERJITTER_H__96ACB9F8_4E29_4FFA_95A1_F557F77E9A19__INCLUDED_)
