// UnpackVideo.h: interface for the CUnpackVideo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNPACKVIDEO_H__76C27EFC_8674_43D9_BA9B_249BB3142720__INCLUDED_)
#define AFX_UNPACKVIDEO_H__76C27EFC_8674_43D9_BA9B_249BB3142720__INCLUDED_

#include "type.h"
#include "FEC.h"
#include "JitterRecover.h"
#include "VC3RSFEC.h"
#include "VC3RSFECEx.h"
#include "UnpackVideoX264.h"

class CUnpackVideo
{
public:
	//帧数据回调函数
	typedef int (__stdcall *LPFRAMEDATACALLBACK)(char* pFrame, int nFrameLen, VIDEOFRAMEHEADER *pFrameHeader, FRAMECAPTIONHEADER *pCaptionHeader, 
													char *szCaption, DWORD addr_remote, LONG dwUser);

public:
	CUnpackVideo(int nChannel = 0);
	virtual ~CUnpackVideo();

	//设置帧数据回调输出函数
	int SetFrameDataCallBack(LPFRAMEDATACALLBACK pFrameDataCallBack, LONG dwUser);

	//输入数据
	int InputData(char* pData, int nLength, DWORD addr_remote = 0);

	BOOL CheckRemoteIP(DWORD addr_remote) { return m_dwRemoteIP == addr_remote; }

	int GetMatrixType(void) { return m_nMatrixType; }
private:
	//FEC预处理
	int PreDealFECPacket(char* pPack, int nLen, DWORD addr_remote);

	//处理视频数据
	int PreDealRawVideoData(char* pPack, int nLen, DWORD addr_remote, BOOL bUseFEC=FALSE);
	
	//组包预处理函数(把REPACKHEADER头转换成REPACKHEADEREX头结构，从而使UnpackToFrame()只处理REPACKHEADEREX这种情况)
	char* PreUnpackToFrame(char* RecvPackHeader, char* pPack, int nPackSize, int &nFrameLen);
	
	//组包函数
	char* UnpackToFrame(char* RecvPackHeader, char* pPack, int nPackSize, int &nFrameLen);

	int CallBackFrame(char *RcvBuf, int nFrameLen, DWORD addr_remote, BOOL bLostPacket = FALSE);

	//抖动数据回调函数
	static int __stdcall JitterCallBack(const char* pSendData, int nSendDatalen, LONG dwUser, DWORD dwflag);

	//FEC数据回调函数
	static int __stdcall FECReceiveCallBack(const char* pRecvData, int nRecvDatalen, LONG dwUser, int nChannel, DWORD flag1, DWORD flag2);
private:
	//组包缓冲区
	char *RcvBufDat;
	//组包临时缓冲区
	char *RcvBufTmp;
	//输出缓冲区
	char *RcvBufHdr;
	char *m_pOptimizex264Buf;
	char *m_pX264FrameBuf;
	//帧序号
	DWORD m_dwOldFrameNo;
	//源地址
	DWORD m_dwRemoteIP;
	//通道号
	int m_nChannel;
	int m_nFECVersion;
	int m_nRcvBufDatLen;
	int m_nMatrixType;
	LONG m_dwUser;
	//组包状态表
	UNPACKSTATUS UnpackStatus;

	//帧数据回调函数指针
	LPFRAMEDATACALLBACK m_pFrameCallBack;

	CSWFECBase* m_pFECRecive;
	CJitterRecover* m_pJitter;
	CUnpackVideoX264 *m_pUnpackVideoX264;
};

#endif // !defined(AFX_UNPACKVIDEO_H__76C27EFC_8674_43D9_BA9B_249BB3142720__INCLUDED_)
