// AudioSendManager.h: interface for the CAudioSendManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOSENDMANAGER_H__17DEE525_3B26_49CA_BEC2_B2CC1351A993__INCLUDED_)
#define AFX_AUDIOSENDMANAGER_H__17DEE525_3B26_49CA_BEC2_B2CC1351A993__INCLUDED_

#include "type.h"
#include "SWFECBase.h"
#include "SwBaseWin.h"

class CAudioSendManager  
{
public:
	/*发送数据回调函数指针定义*/
	typedef long (__stdcall *LPAUDIOPACKETCALLBACK) (const char *pData, int nLen, LONG dwUser);
public:
	CAudioSendManager(void);
	virtual ~CAudioSendManager(void);
public:
	//初始化
	int Init(unsigned int dwLocalIP, int nCodecType, BOOL bUseFEC, int nFECType, LPAUDIOPACKETCALLBACK lpAudioPacketCallBack, LONG dwUser);
	//释放资源
	int Release(void);	
	//输入数据
	int InputData(char* pData, int nLen, int nFrameNo, int nFlag = 0);

	//改变FEC矩阵类型 -1表示不启用FEC
	void SetFECType(int nFECType) { m_nFECType = nFECType; }
private:
	//音频拆包发送处理
	int PreDealingRepack(char* pData, int nLen, WORD wFrameNo, DWORD dwTotalSegments, int nPackSizeMax=MAXAUDPACKSIZE);
	//FEC处理
	int PreDealingFEC(char* pData, int nLen);
	//发送数据上网
	int PreDealingSendToNet(char* pData, int nLen);
	//FEC数据回调函数
	static int __stdcall FECSendCallBack(const char* pSendData, int nSendDatalen, LONG dwUser);
	
	//FEC回调数据处理函数
	int OnFECSendCallBack(const char* pSendData, int nSendDatalen);
private:
	//音频编码类型
	int m_nCodecType;
	//是否启用音频FEC
	BOOL m_bUseAudioFEC;
	//纠错处理对象
	CSWFECBase* m_pFECSend;
	//帧序号(47分钟循环)
	WORD m_wFrameNo;
	//发送缓冲区
	char *m_pSendBuffer;
	BYTE *m_pOutput;
	LPAUDIOPACKETCALLBACK m_lpAudioPacketCallBack;
	LONG m_dwUser;
	int  m_nFECType;
	int  m_nOldFECType;
	unsigned int m_dwLocalIP;
};

#endif // !defined(AFX_AUDIOSENDMANAGER_H__17DEE525_3B26_49CA_BEC2_B2CC1351A993__INCLUDED_)
