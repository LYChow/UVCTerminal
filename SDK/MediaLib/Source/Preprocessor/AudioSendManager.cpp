// AudioSendManager.cpp: implementation of the CAudioSendManager class.
//
//////////////////////////////////////////////////////////////////////
#include "AudioSendManager.h"
#include "VC3FEC.h"
#include "VC3RSFEC.h"
#include "VC3RSFECEx.h"
#include "SwBaseWin.h"

char* PrefixVSTPHeaderIfNeed( char* pBuf, int &nLen, unsigned int dwLocalIP);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudioSendManager::CAudioSendManager(void)
:m_pFECSend(NULL),
m_pSendBuffer(NULL),
m_bUseAudioFEC(TRUE),
m_pOutput(NULL),
m_wFrameNo(0)
{
	m_lpAudioPacketCallBack = NULL;
	m_dwUser = 0;
	m_nFECType = -1;
	m_nOldFECType = -1;
	m_bUseAudioFEC = FALSE;
	m_dwLocalIP = 0;
}

CAudioSendManager::~CAudioSendManager(void)
{
	Release();
}

//初始化
int CAudioSendManager::Init(unsigned int dwLocalIP, int nCodecType, BOOL bUseFEC, int nFECType, LPAUDIOPACKETCALLBACK lpAudioPacketCallBack, LONG dwUser)
{
	//保存音频编码类型
	m_nCodecType = nCodecType;
	m_bUseAudioFEC = bUseFEC;
	m_nFECType = nFECType;
	m_nOldFECType = nFECType;
	if (!bUseFEC)
	{
		m_nFECType = -1;
		m_nOldFECType = -1;
	}
	m_lpAudioPacketCallBack = lpAudioPacketCallBack;
	m_dwUser = dwUser;
	m_dwLocalIP = dwLocalIP;
	//预留音频包头信息
	if (NULL == m_pSendBuffer)
	{
		m_pSendBuffer = new char[2048];
		memset(m_pSendBuffer, 0x00, 2048);
	}

	return 0;
}

//释放资源
int CAudioSendManager::Release(void)
{
	SAFE_DELETE(m_pFECSend);
	SAFE_DELETE_ARRAY(m_pSendBuffer);
	SAFE_DELETE_ARRAY(m_pOutput);
	return 0;
}

//输入数据
int CAudioSendManager::InputData(char* pData, int nLen, int nFrameNo, int nFlag)
{
	//检查数据有效性
	if (pData == NULL || nLen < 0)
		return -1;
	
	int status = 0;
	DWORD WaveSize = nLen;
	
	//构造音频帧头
	VC3AUDIOFRAMEHEADER header;
	header.subtype = ADOST_FRAME_NORMAL;
	header.nextsegment = 0;
	header.formattag = m_nCodecType;
	header.blocks = 1;
	header.FrameNo = nFrameNo;//m_wFrameNo;
	header.CodeSize = (WORD)WaveSize;
	header.CodeOffset = sizeof(header.FrameNo);
	
	//预留128字节(填充其它协议头)
	char* pSendData = m_pSendBuffer+128;
	
	int sp=GETCODERELATIVEPOS(header);
	if (m_nCodecType != CODEC_OPUS)
	{
		memcpy(pSendData,(PVOID)&header,sizeof(header));
		memcpy(&pSendData[sp],pData,WaveSize);
		WaveSize+=sp;
	}
	else
	{
		memcpy(pSendData,(PVOID)&header,sizeof(header));
		*(WORD *)(pSendData + sp) = (WORD)WaveSize;
		memcpy(&pSendData[sp + 2], pData, WaveSize);
		WaveSize+=sp + 2;
	}
	
	m_wFrameNo++;

	//拆包并发送音频数据
	status = PreDealingRepack(pSendData, WaveSize, header.FrameNo, 1);

	return status;
}

//音频拆包发送函数(假设叠加的各路音频数据都相等,且暂不处理单帧超过拆包上限的数据)
int CAudioSendManager::PreDealingRepack(char* pData, int nLen, WORD wFrameNo, DWORD dwTotalSegments, int nPackSizeMax)
{
	//音频头信息
	VC3AUDIOFRAMEHEADER &header = *(VC3AUDIOFRAMEHEADER*)pData;

	int sp = GETCODERELATIVEPOS(header);
	
	//无需拆包
	if (nLen < MAXAUDPACKSIZE)
	{
		return PreDealingFEC(pData, nLen);
	}
	else
	{
		//单个音频数据包大于800的情况
		//计算拆包数
		int nRepackCount = nLen/MAXAUDPACKSIZE;
		if (nLen%MAXAUDPACKSIZE != 0)
		{
			nRepackCount++;
		}
		int nOffset = 0;
		int nRemainLen = nLen;
		int nSendLen = 0;
		char *pSendBuf = pData;
		for(int i=0; i<nRepackCount; i++)
		{
			pSendBuf -= sizeof(VC3REPACKHEADER);
			VC3REPACKHEADER &rph = *(VC3REPACKHEADER *)pSendBuf;
			//填充拆包后的包头
			rph.subtype = ADOST_REPACK_EX;
			rph.offset = nOffset;
			rph.packcounter = nRepackCount;
			rph.FrameNoLowByte = (BYTE)wFrameNo;

			nSendLen = nRemainLen < MAXAUDPACKSIZE? nRemainLen:MAXAUDPACKSIZE;
			nSendLen += sizeof(VC3REPACKHEADER);

			PreDealingFEC((char*)pSendBuf, nSendLen);
			pSendBuf += nSendLen;
			nOffset += MAXAUDPACKSIZE;
			nRemainLen -= MAXAUDPACKSIZE;
			if (nRemainLen <= 0)
			{
				break;
			}
		}
	}

	return 0;
}

int CAudioSendManager::PreDealingFEC(char* pData, int nLen)
{
	if (m_nFECType == -1 && m_bUseAudioFEC)
	{
		m_bUseAudioFEC = FALSE;
	}
	//不做FEC数据处理
	if (!m_bUseAudioFEC)
	{
		return PreDealingSendToNet(pData, nLen);
	}
	//FEC数据处理
	try
	{
		if (m_nFECType != m_nOldFECType)
		{
			SAFE_DELETE(m_pFECSend);
		}
		if (NULL == m_pFECSend) 
		{
			int nFECType = m_nFECType;
			if (m_nFECType >= FECMatrix_MAX)
			{
				if (nFECType < FECMatrix_RS_Base)
				{
					nFECType = FECMatrix_RS_Base + 1;
				}
				if (nFECType <= FECMatrix_RS_100)
				{
					m_pFECSend = new CVC3RSFEC();
				}
				else
				{
					nFECType -= FECMatrix_RS_100;
					m_pFECSend = new CVC3RSFECEx();
				}
			}
			else
			{
				m_pFECSend = new CVC3FEC();
			}

		//	TRACE("CAudioSendManager FECType = %d\n", m_nFECType);

			m_pFECSend->InitBuildFEC(ADOST_FEC, (FECMatrixIndex)m_nFECType, FECSendCallBack, (LONG)this);
			m_nOldFECType = m_nFECType;
		}
		m_pFECSend->BuildPacket(pData, nLen, 0);
	}
	catch (...)
	{
	//	TRACE("CAudioSendManager::PreDealingFEC catch error!");
	}

	return 0;
}

int CAudioSendManager::PreDealingSendToNet(char* pData, int nLen)
{
	if (NULL == m_lpAudioPacketCallBack)
	{
		return -1;
	}

	char sendbuf[1024];
	memset(sendbuf, 0, 1024);

	char *pSend = sendbuf + 32;
	VC3COMMONHEADER &header = *(VC3COMMONHEADER*)pSend;
	header.prefix = PREFIX_SINOWAVE;
	header.version = 0x01;
	header.fixtype = 0x0F;
	header.majortype = VC3MT_AUDIO;
	memcpy(pSend + sizeof(VC3COMMONHEADER), pData, nLen);

	//重新计算包大小
	int sendlen = nLen + sizeof(VC3COMMONHEADER);
	
	char *pSendBuf = PrefixVSTPHeaderIfNeed(pSend, sendlen, m_dwLocalIP);
	return m_lpAudioPacketCallBack(pSendBuf, sendlen, m_dwUser);
}

int __stdcall CAudioSendManager::FECSendCallBack(const char* pSendData, int nSendDatalen, LONG dwUser)
{
	if (nSendDatalen < 0 || NULL == pSendData)
	{
		return -1;
	}

	CAudioSendManager& This = *(CAudioSendManager*)dwUser;

	return This.OnFECSendCallBack(pSendData, nSendDatalen);
}

int CAudioSendManager::OnFECSendCallBack(const char* pSendData, int nSendDatalen)
{
	return PreDealingSendToNet((char*)pSendData, nSendDatalen);
}