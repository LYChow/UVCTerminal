// VC3RSFEC.cpp: implementation of the CVC3RSFECEx class.
//
//////////////////////////////////////////////////////////////////////

#include "VC3RSFECEx.h"
#include "MyPrivateTypeDefine.h"
#include "SwBaseWin.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVC3RSFECEx::CVC3RSFECEx()
{
    int i = 0;
	for (i = 0; i < RS_FEC_REDU_COUNT_MAX; i++)
	{
		m_pFECBuidPacket[i] = NULL;
		m_pFECResPacket[i] = NULL;
	}

	for (i = 0; i < RS_FEC_DATA_COUNT_MAX; i++)
	{
		m_pFECBuidData[i] = NULL;
		m_pFECResData[i] = NULL;
	}

	m_nBuidSizeMax = 0;
	m_FECBuildType = MAKEWORD(16, 5);
	m_pFECBuidDataSendBuf = NULL;
	memset(m_BuildRSMatrix, 0x00, sizeof(m_BuildRSMatrix));

	m_nResumeSizeMax = 0;
	m_FECResumeType = m_FECBuildType;
	m_BuidCurMatrix.Reset();
	memset(m_ResumeRSMatrix, 0x00, sizeof(m_ResumeRSMatrix));
	memset(m_RecvStatus, 0x00, sizeof(m_RecvStatus));
}

CVC3RSFECEx::~CVC3RSFECEx()
{
	ReleaseBuild();
	ReleaseResume();
}

int CVC3RSFECEx::InitBuildFEC( int nDataType, int FECType, pPacketCallBack BuildCallBack, 
							LONG dwUser, VC3FECPacketType nPacketType )
{
	if (NULL == BuildCallBack)
	{
		return -1;
	}
	int nDataNum = LOBYTE(FECType);
	int nResNum = HIBYTE(FECType);
	if (nDataNum <= 0 || nResNum <= 0)
	{
		return -3;
	}
	
	m_nEFCDataType = nDataType;
	m_nPacketType = nPacketType;
	m_BuildCallBack = BuildCallBack;
	m_dwBuildUser = dwUser;

	m_dwSendTick = 0;
	m_dwBuidPacketNo = 0;
	m_dwBuidGroupNo = 0;
	m_nBuidGroupIntraNo = 0;
	m_dwBuildFlag = 0;

	m_FECBuildType = FECType;
	m_BuidCurMatrix.wNumDataCnt = nDataNum;
	m_BuidCurMatrix.wNumReduCnt = nResNum;
	m_nBuidSizeMax = 0;
			
	int *pMatrix = &m_BuildRSMatrix[0][0] ;
	for (int i = 0; i < m_BuidCurMatrix.wNumReduCnt; i++) 
	{
		for (int j = 0; j < m_BuidCurMatrix.wNumDataCnt; j++) 
		{
			pMatrix[i*m_BuidCurMatrix.wNumDataCnt+j] = galois_single_divide(1, i ^ (m_BuidCurMatrix.wNumReduCnt + j), W_COUNT);
		}
	}

	return 0;
}

int CVC3RSFECEx::BuildPacket( const char *pData, int nDatalen, DWORD flag )
{
	if (NULL == pData || nDatalen < 0 || nDatalen > FEC_PACKET_MAX) 
	{
		return -1;
	}
	if (NULL == m_BuildCallBack) 
	{
		return -2;
	}
	if (m_BuidCurMatrix.wNumDataCnt == 0 || m_BuidCurMatrix.wNumDataCnt > RS_FEC_DATA_COUNT_MAX 
		|| m_BuidCurMatrix.wNumReduCnt == 0 || m_BuidCurMatrix.wNumReduCnt > RS_FEC_REDU_COUNT_MAX)
	{
		return -3;
	}

	pVC3FECPayload pFECPayloadHead = NULL;
	
	m_nBuidEvent = 1;
	
	try
	{
		if (m_dwBuildFlag != flag) 
		{
		//	BuildReset();
			m_dwBuildFlag = flag;
		}

		if (m_dwSendTick > 0 && GetTickCount() - m_dwSendTick > FEC_TIMEOUT) 
		{
			BuildReset();
		}
		m_dwSendTick = GetTickCount();

		if (NULL == m_pFECBuidDataSendBuf) 
		{
			m_pFECBuidDataSendBuf = new char[FEC_PACKET_TOTAL_LEN_MAX + FEC_PACKET_HEAD_RES];
			memset(m_pFECBuidDataSendBuf, 0x00, FEC_PACKET_TOTAL_LEN_MAX + FEC_PACKET_HEAD_RES);
		}
		char *pFECBuidDataSendBuf = m_pFECBuidDataSendBuf + FEC_PACKET_HEAD_RES;

		if (VC3FEC_Type_Normal == m_nPacketType)
		{
			pVC3RSFECHeader	pFECHeader = (pVC3RSFECHeader)pFECBuidDataSendBuf;

			pFECHeader->datatype = m_nEFCDataType;
			pFECHeader->subtype.version = SWFECVersion_RSEx;
			pFECHeader->subtype.FECType = FEC_Data;
			pFECHeader->subtype.PacketType = m_nPacketType;
			pFECHeader->GroupParam = (WORD)m_FECBuildType;

			pFECHeader->GroupIntraNo = m_nBuidGroupIntraNo;
			pFECHeader->dwGroupNo = (WORD)m_dwBuidGroupNo;
			pFECHeader->dwPacketNo = (WORD)m_dwBuidPacketNo;
			pFECHeader->nCodeSize = nDatalen;
			pFECHeader->CodeOffset = GetCodeOffset(*pFECHeader) + sizeof(VC3FECPayload);
			pFECHeader->tagOffset = GetCodeOffset(*pFECHeader) - sizeof(pFECHeader->tagOffset);
			
			pFECPayloadHead = (pVC3FECPayload)(pFECBuidDataSendBuf + sizeof(VC3RSFECHeader));
			pFECPayloadHead->dwPayloadlen = nDatalen;

			memcpy(pFECBuidDataSendBuf + sizeof(VC3RSFECHeader) + sizeof(VC3FECPayload), (char *)pData, nDatalen);
		}
		else
		{
			pVC3RSFECHeaderEx pFECHeader = (pVC3RSFECHeaderEx)pFECBuidDataSendBuf;

			pFECHeader->prefix = PREFIX_SINOWAVE;
			pFECHeader->datatype = m_nEFCDataType;
			pFECHeader->subtype.version = SWFECVersion_RSEx;
			pFECHeader->subtype.FECType = FEC_Data;
			pFECHeader->subtype.PacketType = m_nPacketType;
			pFECHeader->GroupParam = (WORD)m_FECBuildType;
			pFECHeader->GroupIntraNo = m_nBuidGroupIntraNo;
			pFECHeader->dwGroupNo = m_dwBuidGroupNo;
			pFECHeader->dwPacketNo = m_dwBuidPacketNo;
			pFECHeader->nCodeSize = nDatalen;
			pFECHeader->CodeOffset = GetCodeOffset(*pFECHeader) + sizeof(VC3FECPayload);
			pFECHeader->tagOffset = GetCodeOffset(*pFECHeader) - sizeof(pFECHeader->tagOffset);
			
			pFECPayloadHead = (pVC3FECPayload)(pFECBuidDataSendBuf + sizeof(VC3RSFECHeaderEx));
			pFECPayloadHead->dwPayloadlen = nDatalen;
		
			memcpy(pFECBuidDataSendBuf + sizeof(VC3RSFECHeaderEx) + sizeof(VC3FECPayload), (char *)pData, nDatalen);
		}

		int nFECPacketHeaderLen = sizeof(VC3RSFECHeader);
		if (VC3FEC_Type_Special == m_nPacketType)
		{
			nFECPacketHeaderLen = sizeof(VC3RSFECHeaderEx);
		}

		if (m_nBuidGroupIntraNo < m_BuidCurMatrix.wNumDataCnt) 
		{
			if (NULL == m_pFECBuidData[m_nBuidGroupIntraNo])
			{
				m_pFECBuidData[m_nBuidGroupIntraNo] = new char[FEC_PACKET_TOTAL_LEN_MAX];
			}
			memset(m_pFECBuidData[m_nBuidGroupIntraNo], 0x00, FEC_PACKET_TOTAL_LEN_MAX);
			memcpy(m_pFECBuidData[m_nBuidGroupIntraNo], pFECBuidDataSendBuf + nFECPacketHeaderLen, 
						nDatalen + sizeof(VC3FECPayload));
		}
		if (0 == m_nBuidGroupIntraNo)
		{
			m_nBuidSizeMax = 0;
		}
		m_nBuidSizeMax = m_nBuidSizeMax < (int)(nDatalen  + sizeof(VC3FECPayload))? nDatalen  + sizeof(VC3FECPayload): m_nBuidSizeMax;
		m_BuildCallBack(pFECBuidDataSendBuf, nFECPacketHeaderLen + sizeof(VC3FECPayload) + nDatalen, m_dwBuildUser);
		m_nBuidGroupIntraNo++;

		//已经收到所有数据,发送		
		if (m_nBuidGroupIntraNo == m_BuidCurMatrix.wNumDataCnt) 
		{
            int i = 0;
			for (i = 0; i < m_BuidCurMatrix.wNumReduCnt; i++)
			{
				if (NULL == m_pFECBuidPacket[i])
				{
					m_pFECBuidPacket[i] = new char[FEC_PACKET_TOTAL_LEN_MAX];
				}
			//	memset(m_pFECBuidPacket[i], 0x00, FEC_PACKET_TOTAL_LEN_MAX);
			}
			m_nBuidSizeMax = (m_nBuidSizeMax + 3) & ~3;

			//生成所有冗余数据
			jerasure_matrix_encode(m_BuidCurMatrix.wNumDataCnt, m_BuidCurMatrix.wNumReduCnt, W_COUNT, 
					(int*)m_BuildRSMatrix, (char**)m_pFECBuidData, (char**)m_pFECBuidPacket, m_nBuidSizeMax);

			for (i = 0; i < m_BuidCurMatrix.wNumReduCnt; i++)
			{
				if (VC3FEC_Type_Normal == m_nPacketType)
				{
					pVC3RSFECHeader pFECHeader = (pVC3RSFECHeader)pFECBuidDataSendBuf;

					pFECHeader->datatype = m_nEFCDataType;
					pFECHeader->subtype.version = SWFECVersion_RSEx;
					pFECHeader->subtype.FECType = FEC_Redu;
					pFECHeader->subtype.PacketType = m_nPacketType;
					pFECHeader->GroupParam = (WORD)m_FECBuildType;
					pFECHeader->GroupIntraNo = m_nBuidGroupIntraNo;
					pFECHeader->dwGroupNo = (WORD)m_dwBuidGroupNo;
					pFECHeader->dwPacketNo = (WORD)m_dwBuidPacketNo;
					pFECHeader->CodeOffset = GetCodeOffset(*pFECHeader) + sizeof(VC3FECPayload);
					pFECHeader->tagOffset = GetCodeOffset(*pFECHeader) - sizeof(pFECHeader->tagOffset);
					pFECHeader->nCodeSize = m_nBuidSizeMax - sizeof(VC3FECPayload);

					memcpy(pFECBuidDataSendBuf + sizeof(VC3RSFECHeader), m_pFECBuidPacket[i], m_nBuidSizeMax);

					m_BuildCallBack(pFECBuidDataSendBuf, sizeof(VC3RSFECHeader) + sizeof(VC3FECPayload) + pFECHeader->nCodeSize, m_dwBuildUser);
				}
				else
				{
					pVC3RSFECHeaderEx pFECHeader = (pVC3RSFECHeaderEx)pFECBuidDataSendBuf;

					pFECHeader->prefix = PREFIX_SINOWAVE;
					pFECHeader->datatype = m_nEFCDataType;
					pFECHeader->subtype.version = SWFECVersion_RSEx;
					pFECHeader->subtype.FECType = FEC_Redu;
					pFECHeader->subtype.PacketType = m_nPacketType;
					pFECHeader->GroupParam = (WORD)m_FECBuildType;
					pFECHeader->GroupIntraNo = m_nBuidGroupIntraNo;
					pFECHeader->dwGroupNo = m_dwBuidGroupNo;
					pFECHeader->dwPacketNo = m_dwBuidPacketNo;
					pFECHeader->CodeOffset = GetCodeOffset(*pFECHeader) + sizeof(VC3FECPayload);
					pFECHeader->tagOffset = GetCodeOffset(*pFECHeader) - sizeof(pFECHeader->tagOffset);
					pFECHeader->nCodeSize = m_nBuidSizeMax - sizeof(VC3FECPayload);

					memcpy(pFECBuidDataSendBuf + sizeof(VC3RSFECHeaderEx), m_pFECBuidPacket[i], m_nBuidSizeMax);
					
					m_BuildCallBack(pFECBuidDataSendBuf, sizeof(VC3RSFECHeaderEx) + sizeof(VC3FECPayload) + pFECHeader->nCodeSize, m_dwBuildUser);
				}
				m_nBuidGroupIntraNo++;
			}
			m_nBuidSizeMax = 0;
			m_nBuidGroupIntraNo = 0;
			m_dwBuidGroupNo++;
		}
		
		m_dwBuidPacketNo++;
		
	}
	catch (...) 
	{
		TRACE0("BuildPacket catch error!\n");
	}

	m_nBuidEvent = 0;

	return 0;
}

void CVC3RSFECEx::BuildReset(void)
{
	m_dwSendTick = 0;
	m_dwBuidPacketNo = 0;
	m_dwBuidGroupNo = 0;
	m_nBuidGroupIntraNo = 0;
}

int CVC3RSFECEx::ReleaseBuild()
{
	try
	{
        int i = 0;
		for (i = 0; i < RS_FEC_REDU_COUNT_MAX; i++)
		{
			SafeDeleteArray(m_pFECBuidPacket[i]);
		}
		
		for (i = 0; i < RS_FEC_DATA_COUNT_MAX; i++)
		{
			SafeDeleteArray(m_pFECBuidData[i]);
		}

		SafeDeleteArray(m_pFECBuidDataSendBuf);
		//防止BuildPacket与ReleaseBuild不在一个线程调用引起冲突
		for (i = 0; i < 100; i++)
		{
			if (m_nBuidEvent != 0)
			{
				Sleep(1);
			}
			else
			{
				break;
			}
		}

		m_BuildCallBack = NULL;
	}
	catch (...) 
	{
		TRACE0("CVC3RSFECEx ReleaseBuild catch error!\n");
	}

	return 0;
}

int CVC3RSFECEx::InitResume( pResumeCallBack ResumeCallBack, pFrameNoCallBack FrameNoCallBack, LONG dwUser )
{
	if (NULL == ResumeCallBack)
	{
		return -1;
	}
	
	m_ResumeCallBack = ResumeCallBack;
	m_FrameNoCallBack = FrameNoCallBack;
	m_dwResumeUser = dwUser;
	
	m_dwRecveTick = 0;
	m_dwResPacketNo = 0;
	m_dwResGroupNo = 0;
	m_dwResFinGopNo = 0xffffffff;
	m_nResGroupIntraNo = 0;
	m_nResPacketType = 0;
	m_bLostPacket = FALSE;
	m_dwResumeFlag1 = 0;
	m_dwResumeFlag2 = 0;
	
	m_nHeaderSize = 0;
	m_nResumeSizeMax = 0;
	m_FECResumeType = 0;
	m_ResCurMatrix.Reset();
	memset(m_ResumeRSMatrix, 0x00, sizeof(m_ResumeRSMatrix));
	memset(m_RecvStatus, 0x00, sizeof(m_RecvStatus));

	return 0;
}

int CVC3RSFECEx::ReleaseResume()
{
	try
	{
        int i = 0;
		for (int i = 0; i < RS_FEC_REDU_COUNT_MAX; i++)
		{
			SafeDeleteArray(m_pFECResPacket[i]);
		}
		
		for (i = 0; i < RS_FEC_DATA_COUNT_MAX; i++)
		{
			SafeDeleteArray(m_pFECResData[i]);
		}

		//防止BuildPacket与ReleaseBuild不在一个线程调用引起冲突
		for (i =0; i < 100; i++)
		{
			if (m_nResumeEvent != 0)
			{
				Sleep(1);
			}
			else
			{
				break;
			}
		}

		m_ResumeCallBack = NULL;
		m_FrameNoCallBack = NULL;
	}
	catch (...) 
	{
		TRACE0("CVC3RSFECEx ReleaseResume catch error!\n");
	}

	return 0;
}

int CVC3RSFECEx::ResumePacket( const char *pData, int nDatalen, DWORD flag1, DWORD flag2 )
{
	if(NULL == pData)
	{
		return -1;
	}

	if (NULL == m_ResumeCallBack) 
	{
		return -3;
	}

	m_nResumeEvent = 1;
	try
	{
		//如果收到的标志与原来的标志不一致,重置丢包恢复状态
		if (m_dwResumeFlag1 != flag1 || m_dwResumeFlag2 != flag2)
		{
			ResumeReset();
			m_dwResumeFlag1 = flag1;
			m_dwResumeFlag2 = flag2;

			TRACE0("FEC Resume reset\n");
		}
		
		if (m_dwRecveTick > 0 && GetTickCount() - m_dwRecveTick > FEC_TIMEOUT) 
		{
			ResumeReset();
		}
		m_dwRecveTick = GetTickCount();

		int nDataCount = 0;
		int nReduCount = 0;
		int nGroupIntraNo = 0;
		int nFECType = FEC_Data;
		DWORD dwPacketNo = 0;
		DWORD dwGroupNo = 0;
		int nOffset = 0;
		if (PREFIX_SINOWAVE == *pData)
		{
			pVC3RSFECHeaderEx pFECHeader = (pVC3RSFECHeaderEx)pData;
			
			nOffset = GetRealPosition(*pFECHeader);
			if (nDatalen < nOffset || nDatalen > nOffset + FEC_PACKET_MAX) 
			{
				m_nResumeEvent = 0;
				return -2;
			}
			
			//	nOffset = GetRealPosition(*pFECHeader);
			if (nOffset < 0 || nOffset > FEC_PACKET_HEAD_MAX) 
			{
				m_nResumeEvent = 0;
				return -4;
			}

			if (pFECHeader->subtype.PacketType != m_nResPacketType) 
			{
				ResumeReset();
			//	SendResumePacket(m_dwResGroupNo);
				m_nResPacketType = pFECHeader->subtype.PacketType;
			}

			if (pFECHeader->subtype.version != SWFECVersion_RSEx) 
			{
				TRACE0("ResumePacket packetsubtype or type not support!\n");
				if (FEC_Data == pFECHeader->subtype.FECType)
				{
					m_ResumeCallBack(pData + nOffset, nDatalen - nOffset, m_dwResumeUser, m_nChannel, m_dwResumeFlag1, m_dwResumeFlag2);
					m_nResumeEvent = 0;
					return 0;
				}
				else
				{
					m_nResumeEvent = 0;
					return -4;
				}
			}
		}
		else
		{
			pVC3RSFECHeader pFECHeader = (pVC3RSFECHeader)pData;

			nOffset = GetRealPosition(*pFECHeader);
			if (nDatalen < nOffset || nDatalen > nOffset + FEC_PACKET_MAX) 
			{
				m_nResumeEvent = 0;
				return -2;
			}
			
			//	nOffset = GetRealPosition(*pFECHeader);
			if (nOffset < 0 || nOffset > FEC_PACKET_HEAD_MAX) 
			{
				m_nResumeEvent = 0;
				return -4;
			}

			if (pFECHeader->subtype.PacketType != m_nResPacketType) 
			{
				ResumeReset();
			//	SendResumePacket(m_dwResGroupNo);
				m_nResPacketType = pFECHeader->subtype.PacketType;
			}
			if (pFECHeader->subtype.version != SWFECVersion_RSEx) 
			{
				TRACE0("ResumePacket packetsubtype or type not support!\n");
				if (FEC_Data == pFECHeader->subtype.FECType)
				{
					m_ResumeCallBack(pData + nOffset, nDatalen - nOffset, m_dwResumeUser, m_nChannel, m_dwResumeFlag1, m_dwResumeFlag2);
					m_nResumeEvent = 0;
					return 0;
				}
				else
				{
					m_nResumeEvent = 0;
					return -4;
				}
			}
		}
		
		if (VC3FEC_Type_Normal == m_nResPacketType)
		{
			pVC3RSFECHeader pFECHeader = (pVC3RSFECHeader)pData;

			nOffset = GetRealPosition(*pFECHeader);
			
			if (m_ResCurMatrix.wNumDataCnt == 0 || m_ResCurMatrix.wNumReduCnt == 0) 
			{
				int nDataNum = LOBYTE(pFECHeader->GroupParam);
				int nResNum = HIBYTE(pFECHeader->GroupParam);
				if (nDataNum == 0 || nDataNum > RS_FEC_DATA_COUNT_MAX 
					|| nResNum == 0 || nResNum > RS_FEC_REDU_COUNT_MAX)
				{
					m_nResumeEvent = 0;
					return -5;
				}
				m_FECResumeType = pFECHeader->GroupParam;
				m_ResCurMatrix.wNumDataCnt = nDataNum;
				m_ResCurMatrix.wNumReduCnt = nResNum;
				m_nResumeSizeMax = 0;	
				InitResumeMatrix();
			}
			else if (m_FECResumeType != pFECHeader->GroupParam)
			{
				SendResumePacket(m_dwResGroupNo);
				int nDataNum = LOBYTE(pFECHeader->GroupParam);
				int nResNum = HIBYTE(pFECHeader->GroupParam);
				if (nDataNum == 0 || nDataNum > RS_FEC_DATA_COUNT_MAX 
					|| nResNum == 0 || nResNum > RS_FEC_REDU_COUNT_MAX)
				{
					m_nResumeEvent = 0;
					return -5;
				}	
				m_FECResumeType = pFECHeader->GroupParam;
				m_ResCurMatrix.wNumDataCnt = nDataNum;
				m_ResCurMatrix.wNumReduCnt = nResNum;
				m_nResumeSizeMax = 0;
				InitResumeMatrix();
			}
			
			nDataCount = m_ResCurMatrix.wNumDataCnt;
			nReduCount = m_ResCurMatrix.wNumReduCnt;
			nGroupIntraNo = pFECHeader->GroupIntraNo;
			nFECType = pFECHeader->subtype.FECType;
			dwPacketNo = pFECHeader->dwPacketNo;
			dwGroupNo = pFECHeader->dwGroupNo;
			
			m_nHeaderSize = GetRealTagSize(*pFECHeader);
			m_nResumeSizeMax = m_nResumeSizeMax < nDatalen - m_nHeaderSize? nDatalen - m_nHeaderSize: m_nResumeSizeMax;
		}
		else
		{
			pVC3RSFECHeaderEx pFECHeader = (pVC3RSFECHeaderEx)pData;
			
			nOffset = GetRealPosition(*pFECHeader);

			//TRACE1("FEC Resume packetno = %d\n", pFECHeader->dwPacketNo);
			//cs<<"packetnum = "<<pFECHeader->dwPacketNo<<" m_bLostPacket = "<<m_bLostPacket<<endl;
			
			if (m_ResCurMatrix.wNumDataCnt == 0 || m_ResCurMatrix.wNumReduCnt == 0) 
			{
				int nDataNum = LOBYTE(pFECHeader->GroupParam);
				int nResNum = HIBYTE(pFECHeader->GroupParam);
				if (nDataNum == 0 || nDataNum > RS_FEC_DATA_COUNT_MAX 
					|| nResNum == 0 || nResNum > RS_FEC_REDU_COUNT_MAX)
				{
					m_nResumeEvent = 0;
					return -5;
				}
				m_FECResumeType = pFECHeader->GroupParam;
				m_ResCurMatrix.wNumDataCnt = nDataNum;
				m_ResCurMatrix.wNumReduCnt = nResNum;
				m_nResumeSizeMax = 0;
				InitResumeMatrix();
			}
			else if (m_FECResumeType != pFECHeader->GroupParam)
			{
				SendResumePacket(m_dwResGroupNo);
				int nDataNum = LOBYTE(pFECHeader->GroupParam);
				int nResNum = HIBYTE(pFECHeader->GroupParam);
				if (nDataNum == 0 || nDataNum > RS_FEC_DATA_COUNT_MAX 
					|| nResNum == 0 || nResNum > RS_FEC_REDU_COUNT_MAX)
				{
					m_nResumeEvent = 0;
					return -5;
				}
				m_FECResumeType = pFECHeader->GroupParam;
				m_ResCurMatrix.wNumDataCnt = nDataNum;
				m_ResCurMatrix.wNumReduCnt = nResNum;
				m_nResumeSizeMax = 0;
				InitResumeMatrix();
			}
			
			nDataCount = m_ResCurMatrix.wNumDataCnt;
			nReduCount = m_ResCurMatrix.wNumReduCnt;
			nGroupIntraNo = pFECHeader->GroupIntraNo;
			nFECType = pFECHeader->subtype.FECType;
			dwPacketNo = pFECHeader->dwPacketNo;
			dwGroupNo = pFECHeader->dwGroupNo;

			m_nHeaderSize = GetRealTagSize(*pFECHeader);
			m_nResumeSizeMax = m_nResumeSizeMax < nDatalen - m_nHeaderSize? nDatalen - m_nHeaderSize: m_nResumeSizeMax;
			
		//	if (NULL != g_pTransCounterManager)
		//	{
		//		DWORD dwPackNum = (nDataCount + nReduCount)*dwGroupNo + nGroupIntraNo;
			//	TRACE("packet num = %d\n",dwPackNum);
		//		int nChannel = m_nChannel > 0? m_nChannel - 1: 0;
		//		g_pTransCounterManager->StatisticsVideoLostStatNet(nChannel, dwPackNum);
		//	}
		}

		if (FEC_Data == nFECType) //视频带FEC视频包
		{
			if (m_FrameNoCallBack != NULL)
			{
				m_FrameNoCallBack(dwPacketNo, m_dwResumeUser);
			}

			//组号是否为当前组号
			if (m_dwResGroupNo != dwGroupNo) 
			{
				SendResumePacket(m_dwResGroupNo);
				m_dwResGroupNo = dwGroupNo;
			}
			
			if (nGroupIntraNo < 0 || nGroupIntraNo >= nDataCount) 
			{
				m_nResumeEvent = 0;
				return -5;
			}
			
			//判读是否丢包
			if (m_dwResPacketNo != dwPacketNo) 
			{
				m_bLostPacket = TRUE;
			}
			
			if (m_RecvStatus[nGroupIntraNo] == 0x00) //处理重包
			{
				if (NULL == m_pFECResData[nGroupIntraNo])
				{
					m_pFECResData[nGroupIntraNo] = new char[FEC_PACKET_TOTAL_LEN_MAX];
				}
				memset(m_pFECResData[nGroupIntraNo], 0x00, FEC_PACKET_TOTAL_LEN_MAX);
				memcpy(m_pFECResData[nGroupIntraNo], (char *)pData, nDatalen);
				
				m_RecvStatus[nGroupIntraNo] = 0x01;//已收到,未发送
				
				//如果未丢包,去掉FEC头直接发送数据包
				if (!m_bLostPacket)
				{
					//	cs<<"packetnum = "<<pFECHeader->dwPacketNo<<endl;
					m_ResumeCallBack(pData + nOffset, nDatalen - nOffset, m_dwResumeUser, m_nChannel, m_dwResumeFlag1, m_dwResumeFlag2);
					m_RecvStatus[nGroupIntraNo] = 0x03;//已发送
					
					//	TRACE("No Packet lost frameno = %d, type = %d, GroupParam = %d, GroupIntraNo = %d, datalen = %d\n", pFECHeader->dwPacketNo,
					//		pFECHeader->type, pFECHeader->GroupParam, pFECHeader->dwGroupNo, pFECHeader->nCodeSize);
				}
				
				m_nResGroupIntraNo++;
				m_dwResPacketNo = dwPacketNo + 1;
			}
			else
			{
				TRACE("CVC3RSFECEx.ResumePacket Data repeat packet, channel = %d, nGroupIntraNo = %d\n", 
						m_nChannel, nGroupIntraNo);
			}
		}
		else if (FEC_Redu == nFECType) //视频FEC冗余包
		{
			//未丢包,不需要进行丢包恢复
		//	if (!m_bLostPacket)
		//	{
		//		SetEvent(m_hResumeEvent);
		//		return 0;
		//	}

			//收到全部的数据包
			if (GetPacketNum() == nDataCount) 
			{
		//		SendResumePacket(m_dwResGroupNo);
				m_nResumeEvent = 0;
				return 0;
			}
			
			int nResumeNo = nGroupIntraNo - nDataCount;
			if (nResumeNo < 0 || nResumeNo >= nReduCount) 
			{
				m_nResumeEvent = 0;
				return -6;
			}
			
			if (m_RecvStatus[nGroupIntraNo] == 0x00)
			{
				if (NULL == m_pFECResPacket[nResumeNo]) 
				{
					m_pFECResPacket[nResumeNo] = new char[FEC_PACKET_TOTAL_LEN_MAX];
				}
			//	memset(m_pFECResPacket[nResumeNo], 0x00, FEC_PACKET_TOTAL_LEN_MAX);
				memcpy(m_pFECResPacket[nResumeNo], (char *)pData, nDatalen);
				
				m_RecvStatus[nGroupIntraNo] = 0x01;
				m_nResGroupIntraNo++;
				
				//收到一组的最后一个包,进行数据恢复
				if (nGroupIntraNo == nDataCount + nReduCount - 1) 
				{
					SendResumePacket(m_dwResGroupNo);
				}
			}
			else
			{
				TRACE("CVC3RSFECEx.ResumePacket Redu repeat packet, channel = %d, nGroupIntraNo = %d\n", 
						m_nChannel, nGroupIntraNo);
			}
		}
		else
		{
			m_nResumeEvent = 0;
			return -7;
		}
	}
	catch (...) 
	{
		TRACE0("CVC3RSFECEx ResumePacket catch error!\n");
	}
	
	m_nResumeEvent = 0;
	return 0;
}


void CVC3RSFECEx::ResumeReset()
{
	m_nHeaderSize = 0;
	m_nResPacketType = 0;
	m_dwRecveTick = 0;
	m_dwResPacketNo = 0;
	m_dwResGroupNo = 0;
	m_dwResFinGopNo = 0xffffffff;
	m_nResGroupIntraNo = 0;
	m_bLostPacket = FALSE;
	m_nResumeSizeMax = 0;
	m_FECResumeType = 0;
	m_ResCurMatrix.Reset();

	memset(m_RecvStatus, 0x00, sizeof(m_RecvStatus));
}

int CVC3RSFECEx::SendResumePacket(DWORD dwGroupNo)
{
	if (/*dwGroupNo == m_dwResFinGopNo || */NULL == m_ResumeCallBack) 
	{
		return 0;
	}
	
//	int nRet = ResumeData();
// 	if (nRet < 0)
// 	{
// 		return nRet;
// 	}

	int nOffset = 0;
	
	for (int i = 0; i < m_ResCurMatrix.wNumDataCnt; i++) 
	{
		if (NULL != m_pFECResData[i]) 
		{
			if (VC3FEC_Type_Normal == m_nResPacketType)
			{
				pVC3RSFECHeader pFECHeader = (pVC3RSFECHeader)m_pFECResData[i];
				nOffset = GetRealPosition(*pFECHeader);
				
				//数据状态为已收到,未发送 发送数据
				if (m_RecvStatus[i] == 0x01 && m_dwResGroupNo == pFECHeader->dwGroupNo) 
				{
					m_ResumeCallBack(m_pFECResData[i] + nOffset, pFECHeader->nCodeSize, m_dwResumeUser, m_nChannel, m_dwResumeFlag1, m_dwResumeFlag2);
					//	TRACE("SendResumePacket frameno = %d, type = %d, GroupParam = %d, GroupIntraNo = %d, datalen = %d\n", pFECHeader->dwPacketNo,
					//		pFECHeader->type, pFECHeader->GroupParam, pFECHeader->dwGroupNo, pFECHeader->nCodeSize);
				}	
			}
			else
			{
				pVC3RSFECHeaderEx pFECHeader = (pVC3RSFECHeaderEx)m_pFECResData[i];
				nOffset = GetRealPosition(*pFECHeader);
				
				//数据状态为已收到,未发送 发送数据
				if (m_RecvStatus[i] == 0x01 && m_dwResGroupNo == pFECHeader->dwGroupNo) 
				{
					m_ResumeCallBack(m_pFECResData[i] + nOffset, pFECHeader->nCodeSize, m_dwResumeUser, m_nChannel, m_dwResumeFlag1, m_dwResumeFlag2);
				//	TRACE("SendResumePacket frameno = %d, type = %d, GroupParam = %d, GroupIntraNo = %d, datalen = %d\n", pFECHeader->dwPacketNo,
				//	pFECHeader->type, pFECHeader->GroupParam, pFECHeader->dwGroupNo, pFECHeader->nCodeSize);
				}	
			}
		}
	}
	
	m_nResGroupIntraNo = 0;
	m_bLostPacket = FALSE;
	m_dwResGroupNo++;
	memset(m_RecvStatus, 0, sizeof(m_RecvStatus));
	m_dwResFinGopNo = dwGroupNo;
	m_nResumeSizeMax = 0;
	
// 	for (i = 0; i < FEC_REDU_COUNT_MAX; i++) 
// 	{
// 		if (NULL != m_pFECResPacket[i]) 
// 		{
// 			memset(m_pFECResPacket[i], 0x00, FEC_PACKET_TOTAL_LEN_MAX);
// 		}
// 	}
	
	return 0;
}

int CVC3RSFECEx::ResumeData()
{
	if (m_ResCurMatrix.wNumDataCnt == 0 || m_ResCurMatrix.wNumDataCnt > RS_FEC_DATA_COUNT_MAX
		|| m_ResCurMatrix.wNumReduCnt == 0 || m_ResCurMatrix.wNumReduCnt > RS_FEC_REDU_COUNT_MAX)
	{
		return -1;
	}

	int nDataCount = m_ResCurMatrix.wNumDataCnt;//一组中数据包的个数
	int nReduCount = m_ResCurMatrix.wNumReduCnt;//一组中冗余包的个数
//	int nOffset = 0; //构造FEC包端 FEC头结构大小 FECPacketHeader与FECPayload之和
//	int ntagOffset = 0;//构造FEC包端 FECPacketHeader结构大小
	pVC3RSFECHeader pFECHeader = NULL;
	pVC3RSFECHeader pFECResumeHeader = NULL;
	pVC3RSFECHeaderEx pFECHeaderEx = NULL;
	pVC3RSFECHeaderEx pFECResumeHeaderEx = NULL;
	pVC3FECPayload pFECPayloadHead = NULL;

	char *pFECResPacket[RS_FEC_REDU_COUNT_MAX] = {NULL};	//记录冗余包缓冲区
	char *pFECResData[RS_FEC_DATA_COUNT_MAX] = {NULL};		//记录数据包缓冲区
	
	int nErasuresCount = 0;
	int nErasures[RS_FEC_REDU_COUNT_MAX+RS_FEC_DATA_COUNT_MAX+1] = {0};

	DWORD dwPacketNum = 0;

    int i = 0;
	for (i = 0; i < nDataCount; i++)
	{
		//未收到数据
		if (m_RecvStatus[i] == 0)
		{
			nErasures[nErasuresCount] = i;
			nErasuresCount++;
		}
		if (NULL == m_pFECResData[i])
		{
			m_pFECResData[i] = new char[FEC_PACKET_TOTAL_LEN_MAX];
			memset(m_pFECResData[i], 0x00, FEC_PACKET_TOTAL_LEN_MAX);
		}
		pFECResData[i] = m_pFECResData[i] + m_nHeaderSize;
	}
	if (nErasuresCount == 0)
	{
		return 0;
	}

	for (i = 0; i < nReduCount; i++)
	{
		//已经收到数据
		if (m_RecvStatus[nDataCount + i] > 0)
		{
			if (VC3FEC_Type_Normal == m_nResPacketType)
			{
				if (NULL == pFECResumeHeader)
				{
					pFECResumeHeader = (pVC3RSFECHeader)m_pFECResPacket[i];
					m_nHeaderSize = GetRealTagSize(*pFECResumeHeader);
					dwPacketNum = pFECResumeHeader->dwPacketNo - (nDataCount - 1);
				}
			}
			else
			{
				if (NULL == pFECResumeHeaderEx)
				{
					pFECResumeHeaderEx = (pVC3RSFECHeaderEx)m_pFECResPacket[i];
					m_nHeaderSize = GetRealTagSize(*pFECResumeHeaderEx);
					dwPacketNum = pFECResumeHeaderEx->dwPacketNo - (nDataCount - 1);
				}
			}
			pFECResPacket[i] = m_pFECResPacket[i] + m_nHeaderSize;
		}
		else //未收到数据
		{
			pFECResPacket[i] = NULL;
			nErasures[nErasuresCount] = i + nDataCount;
			nErasuresCount++;
		}
	}
	nErasures[nErasuresCount] = -1;
	
	//丢包数小于等于冗余包数则能恢复所有数据
	//丢包数大于冗余包数则不能恢复所有数据
	if (nErasuresCount > nReduCount)
	{
		return -2;
	}

	if (m_nHeaderSize <= 0 || m_nHeaderSize > FEC_PACKET_HEAD_MAX)
	{
		return -3;
	}
	m_nResumeSizeMax = (m_nResumeSizeMax + 3) & ~3;

	int nRet = jerasure_matrix_decode__(nDataCount, nReduCount, W_COUNT, (int*)m_ResumeRSMatrix, 0, 
										nErasures, pFECResData, pFECResPacket, m_nResumeSizeMax) ;
	if (0 != nRet)
	{
		return nRet;
	}

	for (i = 0; i < nDataCount; i++)
	{
		//恢复未收到数据的数据头
		if (m_RecvStatus[i] == 0x00)
		{
			if (VC3FEC_Type_Normal == m_nResPacketType)
			{
				pFECHeader = (pVC3RSFECHeader)m_pFECResData[i];
				memcpy(pFECHeader, pFECResumeHeader, m_nHeaderSize);
				pFECHeader->GroupIntraNo = i;
				pFECHeader->dwPacketNo = (WORD)dwPacketNum + i;
				
				pFECPayloadHead = (pVC3FECPayload)pFECResData[i];
				pFECHeader->nCodeSize = pFECPayloadHead->dwPayloadlen;
			}
			else
			{
				pFECHeaderEx = (pVC3RSFECHeaderEx)m_pFECResData[i];
				memcpy(pFECHeaderEx, pFECResumeHeaderEx, m_nHeaderSize);
				pFECHeaderEx->GroupIntraNo = i;
				pFECHeaderEx->dwPacketNo = dwPacketNum + i;
				
				pFECPayloadHead = (pVC3FECPayload)pFECResData[i];
				pFECHeaderEx->nCodeSize = pFECPayloadHead->dwPayloadlen;
			}
			if (pFECPayloadHead->dwPayloadlen > 0 && pFECPayloadHead->dwPayloadlen < FEC_PACKET_MAX)
			{
				m_RecvStatus[i] = 0x01;
			}
		}
	}
	
	return 0;
}

int CVC3RSFECEx::GetPacketNum() const 
{
	int ret = 0;
	
	for (int i = 0; i < m_ResCurMatrix.wNumDataCnt; i++) 
	{
		if (m_RecvStatus[i] > 0) 
		{
			ret++;
		}
	}
	
	return ret;
}

void CVC3RSFECEx::InitResumeMatrix()
{
	if (m_ResCurMatrix.wNumDataCnt == 0 || m_ResCurMatrix.wNumDataCnt > RS_FEC_DATA_COUNT_MAX
		|| m_ResCurMatrix.wNumReduCnt == 0 || m_ResCurMatrix.wNumReduCnt > RS_FEC_REDU_COUNT_MAX)
	{
		return;
	}
		
	int *pMatrix = &m_ResumeRSMatrix[0][0] ;
	for ( int i = 0; i < m_ResCurMatrix.wNumReduCnt; i++) 
	{
		for (int j = 0; j < m_ResCurMatrix.wNumDataCnt; j++) 
		{
			pMatrix[i*m_ResCurMatrix.wNumDataCnt+j] = galois_single_divide(1, i ^ (m_ResCurMatrix.wNumReduCnt + j), W_COUNT);
		}
	}
}

int CVC3RSFECEx::GetRealPosition(const VC3RSFECHeader &PacketHeader)
{
	return (char *)&PacketHeader.CodeOffset - (char *)&PacketHeader + sizeof(PacketHeader.CodeOffset) + PacketHeader.CodeOffset;
}

int CVC3RSFECEx::GetRealTagSize(const VC3RSFECHeader &PacketHeader)
{
	return (char *)&PacketHeader.tagOffset - (char *)&PacketHeader + sizeof(PacketHeader.tagOffset) + PacketHeader.tagOffset;
}

int CVC3RSFECEx::GetCodeOffset(const VC3RSFECHeader &PacketHeader)
{
	return sizeof(PacketHeader.tagOffset) + sizeof(PacketHeader.GroupIntraNo);
}

int CVC3RSFECEx::GetRealPosition(const VC3RSFECHeaderEx &PacketHeader)
{
	return (char *)&PacketHeader.CodeOffset - (char *)&PacketHeader + sizeof(PacketHeader.CodeOffset) + PacketHeader.CodeOffset;
}

int CVC3RSFECEx::GetRealTagSize(const VC3RSFECHeaderEx &PacketHeader)
{
	return (char *)&PacketHeader.tagOffset - (char *)&PacketHeader + sizeof(PacketHeader.tagOffset) + PacketHeader.tagOffset;
}

int CVC3RSFECEx::GetCodeOffset(const VC3RSFECHeaderEx &PacketHeader)
{
	return sizeof(PacketHeader.tagOffset) + sizeof(PacketHeader.GroupIntraNo);
}