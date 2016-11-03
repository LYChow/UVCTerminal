// VideoSender.h: interface for the CVideoSender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOSENDER_H__3D0C0AD6_05AA_404D_99C6_C04B094B0083__INCLUDED_)
#define AFX_VIDEOSENDER_H__3D0C0AD6_05AA_404D_99C6_C04B094B0083__INCLUDED_

#include "type.h"
#include "FEC.h"
#include "VC3RSFEC.h"
#include "VC3RSFECEx.h"
#include "JitterRecover.h"

#define MAX_SMOOTH_SEND_LEN		(1024*2)

typedef enum tagSWVideoSendMask
{
	SWVideoSendMask_NULL	= 0,
	SWVideoSendMask_Smooth	= 0x00000001,
	SWVideoSendMask_FEC		= 0x00000010, 
	SWVideoSendMask_Jitter	= 0x00000100,
}SWVideoSendMask;

#pragma pack(push, 8)

class CVideoSender  
{
public:
	/*发送数据回调函数指针定义*/
	typedef long (__stdcall *LPVS_NETDATACALLBACK) (const char *pbufsend, int sendlen, LONG dwUser);
public:
	CVideoSender();
	virtual ~CVideoSender();
public:
	/*
	*功  能：初始化
	*参  数：
	*	dwLocalIP: 登陆到MVS的IP地址
    *	dwMask:	SWVideoSendMask_FEC - 启用FEC, SWVideoSendMask_Jitter - 启用抖动, 按位运算
	*   LPVS_NETDATACALLBACK lpVideoDataCB 数据回调函数指针
	*   dwUser: 用户私有数据
	*返回值：无
	*/
	void Init(unsigned int dwLocalIP, DWORD dwMask, int nFECType, const char *szHostName, DWORD CodecFOURCC, LPVS_NETDATACALLBACK lpVideoDataCB, LONG dwUser);
	/*
	*功  能：输入数据
	*参  数：
	*	pData		发送数据
	*   nLen		数据长度
	*   nWidth		图像宽度
	*   nHeight		图像高度
	*   FrameNo		帧序号
	*   nFrameType	0 - 帧, 1 - I帧
	*   nFrameRate	帧率
	*   nBandWidth  编码带宽
	*   dwTimestamp 时间戳
	*返回值：返回值0表示成功，其它值表示失败
	*/
	int InputData(const char* pData, int nLen, int nWidth, int nHeight, DWORD FrameNo, int nFrameType, int nFrameRate, int nBandWidth, DWORD dwTimestamp);

	//改变FEC矩阵类型 -1表示不启用FEC
	void SetFECType(int nFECType) { m_nFECType = nFECType; }
private:
	BOOL  m_bUseSmooth;
	BOOL  m_bUseJitter;
	BOOL  m_bUseFEC;
	int   m_nFECType;
	int	  m_nLastFECType;
	int   m_nStreamType;
	int	  m_nSmoothBandPercent;
	DWORD m_dwPacknum;
	DWORD m_dwCodecFOURCC;
	char  m_szHostName[32];
	LONG  m_dwUser;
	int	  m_nFrameBufLen;
	unsigned int m_dwLocalIP;
	char* m_pFrameBuf;
	char* m_pSmoothSndBuf;
	char *m_pFECBuf;
	char *m_pJitterBuf;
	char *m_pVSTPBuffer;
//	CSmoothSendNew* m_pSmoothSend;
	CSWFECBase *m_pFECSend;
	CJitterRecover *m_pJitter;
	LPVS_NETDATACALLBACK m_lpVideoDataCB;
private:
	int InputData(const char* pData, int nLen, VIDEOFRAMEHEADER* pHdr);
	//添加帧头、字幕等信息到帧头帧尾
	int AddInfoToFrame(const char* pData, int nLen, VIDEOFRAMEHEADER* pHdr);

	//拆包发送优化后的小包264数据并且添加帧头、字幕等信息到帧头帧尾
	int AddInfoToFrameX264(const char* pData, int nLen, VIDEOFRAMEHEADER* pHdr);

	//264数据预处理
	int PreDealingH264(const char* pData, int nLen, VIDEOFRAMEHEADER* pHdr);

	//拆包发送
	int RepackSendTo(const char * buf, int len, int flags, BYTE frameno, int nStreamType);
	//拆包发送
	int RepackSendToEx(const char *p264Buf, const char * buf, int len, int flags, BYTE frameno, int nStreamType);

	//平滑发送预处理
	int PreDealingSmoothSend(const char *pbufsend, int sendlen, int flags);

	//FEC预处理
	int PreDealingFEC(const char *pbufsend, int sendlen, int flags);
	int PreDealingJitter(const char *pbufsend, int sendlen, int flags);
private:
	//平滑发送回调函数
//	static long __stdcall SmoothPacketCallBack(char* pSendData, int nSendDatalen, LONG dwUser);
	//FEC回调函数
	static int __stdcall FECSendCallBack(const char* pSendData, int nSendDatalen, LONG dwUser);
};

#pragma pack(pop)

#endif // !defined(AFX_VIDEOSENDER_H__3D0C0AD6_05AA_404D_99C6_C04B094B0083__INCLUDED_)
