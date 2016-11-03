// UnpackAudio.h: interface for the CUnpackAudio class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNPACKAUDIO_H__EFEF379F_CF64_4A13_B8B2_FFA8140A782B__INCLUDED_)
#define AFX_UNPACKAUDIO_H__EFEF379F_CF64_4A13_B8B2_FFA8140A782B__INCLUDED_

#include "type.h"
#include "SWFECBase.h"
#include "SwBaseWin.h"

class CUnpackAudio  
{
public:
	//帧数据回调函数f
	typedef int (__stdcall *LPFRAMEDATACALLBACK)(char* pData, int nLength, int nAudioCodeCType, DWORD dwFrameNo, DWORD addr_remote, int nChannel, LONG dwUser);

public:
	CUnpackAudio(int nChannel = 0);
	virtual ~CUnpackAudio();

	//设置帧数据回调输出函数
	int SetFrameDataCallBack(LPFRAMEDATACALLBACK pFrameDataCallBack, LONG dwUser);
	//输入数据
	int InputData(char* pData, int nLength, DWORD RemoteIP = 0);

	BOOL CheckRemoteIP(DWORD RemoteIP) { return RemoteIP == m_dwRemoteIP; }
private:
	//老版本音频传输协议
	int DealOldPacket(char* pData, int nLength, DWORD RemoteIP);
	//新版本音频传输协议
	int DealNewPacket(char* pData, int nLength, DWORD RemoteIP);
	//拼帧
	char *UnpackToFrame(char* RecvBuf, char* pPack, int nPackSize, int &nOutSize);
	//处理音频帧数据
	int DealFrame(char *pData, int nLength, DWORD addr_remote);
	//FEC预处理
	int PreDealFECPacket(char* pPack, int nLen, DWORD addr_remote);
	//处理音频数据
	int PreDealRawAudioData(char* pPack, int nLen, DWORD addr_remote, BOOL bUseFEC=FALSE);
	//组包
	char* UnpackToFrame(char *RecvPackData, char* OutPackData, char* RecvPackHeader, char* pPack, int nPackSize);
	//组包
	char* UnpackToFrameEx(char *RecvPackData, char* OutPackData, char* RecvPackHeader, char* pPack, int nPackSize, int &nTotalLen);
	//去掉音频拆包帧头
	int RepackFrameHeader(char* pData, int nLen, char*pOutData, int &nOutLen);
	//分离拼接在一起的音频帧
	int UnJoinMultiBlocksToFrame(char* pData, int nLen, char* pOutData, int &nOutLen);
	//FEC数据回调函数
	static int __stdcall FECReceiveCallBack(const char* pRecvData, int nRecvDatalen, LONG dwUser, int nChannel, DWORD flag1, DWORD flag2);
private:
	//组包缓冲区
	char *RcvBufDat;
	//输出缓冲区
	char *OutBufDat;
	//输出缓冲区
	char *RcvBufHdr;
	int m_nFlag;
	//音频重构缓冲区
	char *RcvBufRepack;
	int m_nFlag1;
	//音频帧拼接缓冲区
	char *RcvBufUnJoin;
	char *m_pAudioBuf;

	//拆包头结构
	char *m_pRcvBufHdrEx;
	char *m_pRcvBufDatEx;
	char *m_pOutBufDatEx;

	int m_nFrameLen;
	int m_nFECVersion;
	//音频通道号
	int m_nChannel;
	//音频帧序号
	WORD m_wPreviousFrameNo;
	WORD m_wNewFrameNo;
	//用户定义数据
	LONG m_dwUser;
	DWORD m_dwRemoteIP;
	//帧数据回调函数指针
	LPFRAMEDATACALLBACK m_pFrameCallBack;
	
	CSWFECBase* m_pFECRecive;
//	CVC3JitterRecover* m_pJitter;
};

#endif // !defined(AFX_UNPACKAUDIO_H__EFEF379F_CF64_4A13_B8B2_FFA8140A782B__INCLUDED_)
