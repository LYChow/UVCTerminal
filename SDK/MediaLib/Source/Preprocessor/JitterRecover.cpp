// CJitterRecover.cpp: implementation of the CRecoverJitter class.
//
//////////////////////////////////////////////////////////////////////

#include "JitterRecover.h"
#include "MyPrivateTypeDefine.h"
#pragma comment(lib, "winmm.lib")
//#include "cs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJitterRecover::CJitterRecover()
{
	m_nErrorData = 0;
	m_dwFlag = 0;
	m_dwPacketNum = 1;
	m_bSendThread = FALSE;
	m_dwCallBackUser = 0;
	m_dwRecveTick = 0;

	m_pSendBuffer = NULL;
	m_pDataInBuf = NULL;
	m_pCallBackBuf = NULL;
	m_pPacketCallBack = NULL;
}

CJitterRecover::~CJitterRecover()
{
	m_bSendThread = FALSE;
	m_pPacketCallBack = NULL;

	SafeDeleteArray(m_pSendBuffer);
	SafeDeleteArray(m_pDataInBuf);
	SafeDeleteArray(m_pCallBackBuf);

	if (NULL != m_JitterGroup.pJitterData) 
	{
		for (int i = 0; i < m_JitterGroup.nJitterPacketMax; i++) 
		{
			SafeDeleteArray((m_JitterGroup.pJitterData + i)->pData);
		}
		SafeDeleteArray(m_JitterGroup.pJitterData);
	}
}

char *CJitterRecover::AddHeader(const char *pData, int nDatalen, int &nOutlen)
{
	if (NULL == pData || nDatalen < 0 || nDatalen + sizeof(JitterPacketHeader) > JITTER_BUFFER_LEN) 
	{
		return NULL;
	}

	if (NULL == m_pSendBuffer) 
	{
		m_pSendBuffer = new char[JITTER_BUFFER_LEN];
	}

	JitterPacketHeader &Header = *(pJitterPacketHeader)m_pSendBuffer;

//	DWORD tmp = m_dwPacketNum;
//	const int mod =20;
//	tmp = m_dwPacketNum/mod*mod + mod - m_dwPacketNum%mod;
//	TRACE2("Addhead packetnum = %d, packetnum = %d\n", tmp, m_dwPacketNum);

	Header.prefix = PREFIX_SINOWAVE;
	Header.type = 0x0A;
	Header.subtype = 0x10 + 0x00;
	Header.dwPacketNo = m_dwPacketNum;
	Header.dwTimeStamp = GetTickCount();
	Header.nCodeSize = nDatalen;
	Header.CodeOffset = GetCodeOffset(Header);

	memcpy(m_pSendBuffer + sizeof(JitterPacketHeader), pData, nDatalen);
	
	nOutlen = nDatalen + sizeof(JitterPacketHeader);

	m_dwPacketNum++;

	return m_pSendBuffer;
}

int CJitterRecover::InitRecover(pSendPacketCallBack SendCallback, LONG dwUser, int nRecoverBufSize, int nType)
{
	if (NULL == SendCallback)
	{
		return -1;
	}

	m_pPacketCallBack = SendCallback;
	m_dwCallBackUser = dwUser;

	m_bSendThread = TRUE;

	if (nRecoverBufSize > 0 && nRecoverBufSize < JITTER_PACKET_MAX) 
	{
		m_JitterGroup.nJitterPacketMax = nRecoverBufSize;
		m_JitterGroup.nJitterPacketTTL = nRecoverBufSize*2;
	}
	if (NULL == m_JitterGroup.pJitterData) 
	{
		m_JitterGroup.pJitterData = new JitterPacket[m_JitterGroup.nJitterPacketMax]; 
	}

	return 0;
}

void CJitterRecover::Reset()
{
	m_nErrorData = 0;
	m_dwFlag = 0;
//	m_dwPacketNum = 1;
	m_JitterGroup.Reset();
	m_dwRecveTick = 0;

	return;
}

int CJitterRecover::GetMixNumPos(DWORD &dwMixPcaketNo) const
{
	int nPos = -1;
	
	//遍历查询重排序列中的最小序号
	for (int i = 0; i < m_JitterGroup.nJitterPacketMax; i++) 
	{
		if ((m_JitterGroup.pJitterData + i)->bAvailData) 
		{
			if (dwMixPcaketNo >= (m_JitterGroup.pJitterData + i)->dwPacketNo) 
			{
				dwMixPcaketNo = (m_JitterGroup.pJitterData + i)->dwPacketNo;
				nPos = i;
			}
		}
	}

	return nPos;
}

int CJitterRecover::DealJitterDataEx( const char *pData, int nDatalen, DWORD dwFlag )
{
	if (NULL == pData || nDatalen < 0 || nDatalen > JITTER_BUFFER_LEN)
	{
		return -1;
	}

	if (NULL == m_pPacketCallBack || !m_bSendThread) 
	{
		return -2;
	}

	if (dwFlag != m_dwFlag) 
	{
		Reset();
		m_dwFlag = dwFlag;
	}
	
	if (m_dwRecveTick > 0 && GetTickCount() - m_dwRecveTick > JITTER_RECVER_TIMEOUT) 
	{
	//	Reset();
	}
	m_dwRecveTick = GetTickCount();

	JitterPacketHeader &Header = *(pJitterPacketHeader)pData;

	if (Header.type != 0x0A || (Header.subtype & 0xF0) != 0x10 || Header.prefix != PREFIX_SINOWAVE) 
	{
	//	TRACE0("CJitterRecover DealJitterData type or subtype not support!\n");
		return -3;
	}

	DWORD dwPacketNo = Header.dwPacketNo;
//	TRACE("jitter Rev packetno = %d\n", dwPacketNo);

    int i = 0;
	for (i = 0; i < m_JitterGroup.nJitterPacketMax; i++)
	{
		if ((m_JitterGroup.pJitterData + i)->bAvailData) 
		{
			//有效数据包的TTL减1
			(m_JitterGroup.pJitterData + i)->nTTL--;
			if ((m_JitterGroup.pJitterData + i)->nTTL <= 0 || (m_JitterGroup.pJitterData + i)->nTTL > m_JitterGroup.nJitterPacketTTL) 
			{
				(m_JitterGroup.pJitterData + i)->bAvailData = FALSE;
				m_JitterGroup.nPacketCount--;
			}
		}
	}
	
	DWORD dwMixPcaketNo = 0xffffffff;	
	int nPos = GetMixNumPos(dwMixPcaketNo);
	//缓冲区无数据
	if (0xffffffff == dwMixPcaketNo) 
	{
		dwMixPcaketNo = 0;
	}

	//收到的包序号小于已发送序号
	if (dwPacketNo <= m_JitterGroup.nSendPacketNo)
	{
		m_nErrorData++;
		if (m_nErrorData >= m_JitterGroup.nJitterPacketTTL) 
		{
			Reset();
		}
		return -4;
	}

	if (0 == m_JitterGroup.nSendPacketNo)
	{
		m_JitterGroup.nSendPacketNo = dwPacketNo - 1;
	}
	
	m_nErrorData = 0;

	//包序号连续，直接发送，不进入缓存队列，或收到的包序号小于缓冲区中数据的最小值
	if (m_JitterGroup.nSendPacketNo + 1 == dwPacketNo)
	{
		int nOffset = GetRealSize(Header);
		m_pPacketCallBack(pData + nOffset, Header.nCodeSize, m_dwCallBackUser, m_dwFlag);
		m_JitterGroup.nSendPacketNo = dwPacketNo;
	}
	else
	{
		for (i = 0; i < m_JitterGroup.nJitterPacketMax; i++) 
		{
			if (!(m_JitterGroup.pJitterData + i)->bAvailData)
			{
				nPos = i;
				break;
			}
			
			if (m_JitterGroup.nJitterPacketMax - 1 == i) 
			{
				m_JitterGroup.Reset();
				nPos = 0;
			}
		}
		JitterPacket &JitterData = *(m_JitterGroup.pJitterData + nPos);
		
		JitterData.bAvailData = TRUE;
		JitterData.dwPacketNo = dwPacketNo;
		JitterData.nTTL = m_JitterGroup.nJitterPacketTTL;
		JitterData.nDatalen = nDatalen;
		JitterData.dwFlag1 = dwFlag;
		
		if (JitterData.nDatalen < nDatalen) 
		{
			SafeDeleteArray(JitterData.pData);
		}
		
		if (NULL == JitterData.pData) 
		{
			while (nDatalen > JitterData.nBufferLen) 
			{
				if (0 <= JitterData.nBufferLen) 
				{
					JitterData.nBufferLen = JITTER_BUFFER_LEN;
				}
				else
				{
					JitterData.nBufferLen = JitterData.nBufferLen*2;
				}
			}
			
			JitterData.pData = new char[JitterData.nBufferLen];
			memset(JitterData.pData, 0x00, JitterData.nBufferLen);
		}
		
		memcpy(JitterData.pData, pData, nDatalen);	
		m_JitterGroup.nPacketCount++;
	}
	RecoverAndSendPacket();

	return 0;	
}

int CJitterRecover::RecoverAndSendPacket( void )
{
	DWORD dwMixPcaketNo = 0xffffffff;
	int nPos = 0;
	BOOL bSendAll = m_JitterGroup.IsBufFull();
	BOOL bSend = TRUE;
	do 
	{
		dwMixPcaketNo = 0xffffffff;
		nPos = GetMixNumPos(dwMixPcaketNo);
		if (nPos == -1)
		{
			break;
		}
		//缓冲区无数据
		if (0xffffffff == dwMixPcaketNo) 
		{
			dwMixPcaketNo = 0;
		}
		bSend = FALSE;
		if ((dwMixPcaketNo == m_JitterGroup.nSendPacketNo + 1) || bSendAll)
		{
			JitterPacket &JitterData = *(m_JitterGroup.pJitterData + nPos);
			//	TRACE("PutDataIn dwMixPcaketNo = %d, m_JitterGroup.nSendPacketNo = %d\n", dwMixPcaketNo, m_JitterGroup.nSendPacketNo);
			JitterPacketHeader &Header = *(pJitterPacketHeader)JitterData.pData;
			int nOffset = GetRealSize(Header);
			char SendDataBuf[1024*3] = {0};
			memcpy(SendDataBuf + 1024, JitterData.pData + nOffset, Header.nCodeSize);
			m_pPacketCallBack(SendDataBuf + 1024/*JitterData.pData + nOffset*/, Header.nCodeSize, m_dwCallBackUser, m_dwFlag);
			m_JitterGroup.nSendPacketNo = dwMixPcaketNo;

			JitterData.bAvailData = FALSE;
			if (m_JitterGroup.nPacketCount > 0) 
			{
				m_JitterGroup.nPacketCount--;
			}
			bSend = TRUE;
		}
	}while(bSend);

	return 0;
}

inline int CJitterRecover::GetRealSize(const JitterPacketHeader &PacketHeader)
{
	return (char *)&PacketHeader.CodeOffset - (char *)&PacketHeader + sizeof(PacketHeader.CodeOffset) + PacketHeader.CodeOffset;
}

int CJitterRecover::GetCodeOffset(const JitterPacketHeader &PacketHeader)
{
	return sizeof(PacketHeader.Reserve1);
}