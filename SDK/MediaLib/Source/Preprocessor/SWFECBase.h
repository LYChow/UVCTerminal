/******************************************************************************
  文 件 名   : CSWFECBase.h
  功    能	 : FEC前向纠错类基类
  创 建 者   : zds
  创建日期   : 2013年3月25日

******************************************************************************/

#if !defined(AFX_SWFECBASE_H__CA6B61D2_AED9_44C0_970E_0B0618217D27__INCLUDED_)
#define AFX_SWFECBASE_H__CA6B61D2_AED9_44C0_970E_0B0618217D27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "FECDefine.h"
//#include "VC3FEC.h"
//#include "FEC.h"
#include "VC3FECDefine.h"

class CSWFECBase  
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
	virtual int InitBuildFEC(int nDataType, int FECType, pPacketCallBack BuildCallBack, 
							LONG dwUser = 0, VC3FECPacketType nPacketType = VC3FEC_Type_Normal) = 0;
	
	//生成FEC包
	virtual int BuildPacket(const char *pData, int nDatalen, DWORD flag = 0) = 0;
	
	//释放构造FEC包资源
	virtual int ReleaseBuild() = 0;
	
	/************************************************************************/
	/*FEC数据恢复相关函数                                                   */
	/************************************************************************/
	//初始化 ResumeCallBack为恢复FEC包后的回调函数指针
	virtual int InitResume(pResumeCallBack ResumeCallBack, pFrameNoCallBack FrameNoCallBack, LONG dwUser) = 0;
	
	//根据FEC包恢复数据
	virtual int ResumePacket(const char *pData, int nDatalen, DWORD flag1 = 0, DWORD flag2 = 0) = 0;
	
	//释放恢复FEC包资源
	virtual int ReleaseResume() = 0;
	
	//设置、获取通道号
	virtual void SetChannel(int nChannel) { m_nChannel = nChannel;}
	virtual int  GetChannel() const { return m_nChannel;}

	/************************************************************************/
	/*类的全局函数                                                          */
	/************************************************************************/
	//恢复FEC包时,用于获取构建FEC包的头长度,构建FEC包端FECPacketHeader与FECPayload结构大小之和
	static int GetRealPosition(const VC3FECPacketHeader &PacketHeader);
	
	//恢复FEC包时,用于获取构建FEC包的头长度,为构建FEC包端FECPacketHeader结构大小
	static int GetRealTagSize(const VC3FECPacketHeader &PacketHeader);
	
	//构建FEC包时,用于标识FECPacketHeader结构CodeOffset之后结构剩余部分的大小,便于扩展
	static int GetCodeOffset(const VC3FECPacketHeader &PacketHeader);
	
	static int GetRealPosition(const VC3FECPacketHeaderEx &PacketHeader);
	static int GetRealTagSize(const VC3FECPacketHeaderEx &PacketHeader);
	static int GetCodeOffset(const VC3FECPacketHeaderEx &PacketHeader);
protected:
	int m_nChannel;

	/************************************************************************/
	/* 构造FEC包相关变量                                                    */
	/************************************************************************/
	int   m_nEFCDataType;						//数据类型(视频、音频)
	int   m_nPacketType;						//包类型
	DWORD m_dwBuidPacketNo;						//数据包序号(不包含冗余包)
	DWORD m_dwBuidGroupNo;						//FEC组号
	int   m_nBuidGroupIntraNo;					//组内序号
	LONG  m_dwBuildUser;						//回调函数m_BuildCallBack的调用者,如果m_BuildCallBack为全局函数m_dwBuildUser可以为NULL
	int   m_nBuidEvent;						//标识 m_BuildCallBack 是否正在执行
	DWORD m_dwBuildFlag;						//标识数据源是否发生变化
	DWORD m_dwSendTick;							//发送上一个包的时间
	pPacketCallBack m_BuildCallBack;			//生成FEC包后回调函数

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
	pResumeCallBack m_ResumeCallBack;			//恢复FEC包后回调函数
	pFrameNoCallBack m_FrameNoCallBack;			//帧序号回调函数
public:
	CSWFECBase();
	virtual ~CSWFECBase();
private:
	CSWFECBase(const CSWFECBase &);//拷贝构造函数
	CSWFECBase &operator = (const CSWFECBase &);//重载赋值
};

#endif // !defined(AFX_SWFECBASE_H__CA6B61D2_AED9_44C0_970E_0B0618217D27__INCLUDED_)
