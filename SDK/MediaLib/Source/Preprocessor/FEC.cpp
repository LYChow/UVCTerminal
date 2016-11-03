// FEC.cpp: implementation of the CFEC class.
//
//////////////////////////////////////////////////////////////////////

#include "FEC.h"
#include "MyPrivateTypeDefine.h"
#include "SwBaseWin.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//数据恢复矩阵 数据包与生成冗余包之间的关系
FECMatrix CFEC::m_TriBudMatStu[FECMatrix_MAX] = 
{
	{
        2, 1,
        {
            {1, 1},
        }
    },
    
    {
        4, 3,
        {
            {1, 0, 1, 1},
            {1, 1, 0, 1},
            {1, 1, 1, 0},
        }
    },
    
    {
        4, 4,
        {
            {1, 1, 1, 1},
            {1, 1, 1, 1},
            {1, 0, 0, 1},
            {0, 1, 0, 1},
        }
    },

    {
        6, 4,
        {
            {1, 1, 1, 1, 1, 1},
            {1, 0, 1, 1, 1, 0},
            {0, 0, 1, 1, 0, 1},
            {1, 1, 0, 1, 0, 0},
        }

    },
  
   {
        9, 3,
        {
            {1, 0, 0, 1, 0, 0, 1, 0, 0},
            {0, 1, 0, 0, 1, 0, 0, 1, 0},
            {0, 0, 1, 0, 0, 1, 0, 0, 1},
        }
    },

    {
        9, 4,
        {
            {1, 0, 1, 1, 1, 1, 0, 0, 0},
            {1, 1, 0, 0, 1, 0, 1, 0, 0},
            {1, 1, 0, 1, 0, 0, 0, 1, 0},
            {1, 1, 1, 0, 0, 0, 0, 0, 1},
        }
    },
    
    {
        10, 4,
        {
            {0, 1, 1, 0, 0, 1, 1, 1, 1, 1},
            {1, 0, 1, 0, 1, 1, 0, 0, 1, 1},
            {0, 1, 1, 1, 1, 0, 0, 1, 0, 1},
            {1, 0, 1, 1, 1, 1, 1, 0, 1, 0},
        }
    },

    {
        10, 5,
        {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 0, 1, 1, 0, 1, 1, 1, 1, 1},
            {1, 1, 0, 0, 0, 0, 1, 1, 0, 1},
            {1, 0, 0, 0, 1, 1, 1, 0, 1, 0},
            {0, 0, 1, 0, 0, 1, 1, 0, 0, 1},
        }
    },

    {
        15, 5,
        {
            {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1},
            {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0},
            {0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1},
            {0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1},
            {1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1},
        }
    },

    {
        20, 5,
        {
            {0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1},
            {1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1},
            {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0},
            {0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0},
            {1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1},
        }
    },
    
    {
        24, 6,
        {
            {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
            {1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
            {1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0},
            {1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
            {1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0},
            {1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0},
        }
    },

    {
        29, 6,
        {
            {0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1},
            {0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0},
            {1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0},
            {0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0},
            {1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1},
        }
    },

    {
        34, 6,
        {
            {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0},
            {1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1},
            {1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1},
            {0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1},
            {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0},
            {0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1},
        }
    }
};

CFEC::CFEC()
{
	m_nChannel = 0;
	
	m_nBuidPacketType = 0;
	m_dwSendTick = 0;
	m_dwBuidPacketNo = 0;
	m_dwBuidGroupNo = 0;
	m_nBuidGroupIntraNo = 0;
	m_dwBuildUser = 0;
	m_nBuildEvent = 0;
	m_FECBuildType = FECMatrix_6_4;
	m_BuildCallBack = NULL;
	m_dwBuildFlag = 0;
	m_pBuidCurMatrix = NULL;
	m_pBuidCurMatrix = &m_TriBudMatStu[FECMatrix_6_4];
    int i = 0;
	for (i = 0; i < FEC_REDU_COUNT_MAX; i++)
	{
		m_pFECBuidPacket[i] = NULL;
	}
	m_pFECBuidData = NULL;

	m_nResPacketType = 0;
	m_dwRecveTick = 0;
	m_dwResPacketNo = 0;
	m_dwResGroupNo = 0;
	m_dwResFinGopNo = 0xffffffff;
	m_nResGroupIntraNo = 0;
	m_dwResumeUser = 0;
	m_bLostPacket = FALSE;
	m_nResumeEvent = 0;
	m_dwResumeFlag1 = 0;
	m_dwResumeFlag2 = 0;
	m_FECResumeType = FECMatrix_6_4;
	m_ResumeCallBack = NULL;
	m_FrameNoCallBack = NULL;
	m_pResCurMatrix = NULL;
	for (i = 0; i < FEC_REDU_COUNT_MAX; i++) 
	{
		m_pFECResPacket[i] = NULL;
	}
	for (i = 0; i < FEC_DATA_COUNT_MAX; i++) 
	{
		m_pFECResData[i] = NULL;
	}
	memset(m_RecvStatus, 0x00, sizeof(m_RecvStatus));
}

CFEC::~CFEC()
{
	ReleaseBuild();
	ReleaseResume();
}

//int CFEC::InitBuildFEC(FECPacketType nType,FECMatrixIndex FECType, pPacketCallBack BuildCallBack, DWORD dwUser)
int CFEC::InitBuildFEC(int nType, int FECType, pPacketCallBack BuildCallBack, LONG dwUser, VC3FECPacketType nPacketType)
{
	if (NULL == BuildCallBack)
	{
		return -1;
	}
	if (FECType < 0 || FECType >= FECMatrix_MAX) 
	{
		return -2;
	}
// 	if (nType < FEC_Type_Video || nType > FEC_Type_MAX)
// 	{
// 		return -3;
// 	}

	m_nBuidPacketType = nPacketType;
	m_FECBuildType = (FECMatrixIndex)FECType;
	m_BuildCallBack = BuildCallBack;
	m_dwBuildUser = dwUser;

	m_pBuidCurMatrix = &m_TriBudMatStu[FECType];

	m_dwSendTick = 0;
	m_dwBuidPacketNo = 0;
	m_dwBuidGroupNo = 0;
	m_nBuidGroupIntraNo = 0;
	m_dwBuildFlag = 0;

	return 0;
}

int CFEC::BuildPacket(const char *pData, int nDatalen, DWORD flag)
{
	if (NULL == pData || nDatalen < 0 || nDatalen > FEC_PACKET_MAX) 
	{
		return -1;
	}
	if (NULL == m_BuildCallBack) 
	{
		return -2;
	}
	if (NULL == m_pBuidCurMatrix) 
	{
		return -3;
	}
	
	const char *pReduSrc = NULL;
	char *pReduDst = NULL;
	pFECPacketHeader pFECHeader = NULL;
	pFECPayload pFECPayloadHead = NULL;

	m_nBuildEvent = 1;
	
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

		if (NULL == m_pFECBuidData) 
		{
			m_pFECBuidData = new char[FEC_PACKET_TOTAL_LEN_MAX];
		}
		pFECHeader = (pFECPacketHeader)m_pFECBuidData;
		
		pFECHeader->prefix = PREFIX_SINOWAVE;
		pFECHeader->type = 0x09;
		pFECHeader->subtype.version = 0x1;
		pFECHeader->subtype.FECType = FEC_Data;
		pFECHeader->subtype.PacketType = m_nBuidPacketType;
		pFECHeader->GroupParam = (char)m_FECBuildType;
		pFECHeader->GroupIntraNo = m_nBuidGroupIntraNo;
		pFECHeader->dwGroupNo = m_dwBuidGroupNo;
		pFECHeader->dwPacketNo = m_dwBuidPacketNo;
		pFECHeader->nCodeSize = nDatalen;
		pFECHeader->CodeOffset = GetCodeOffset(*pFECHeader) + sizeof(FECPayload);
		pFECHeader->tagOffset = GetCodeOffset(*pFECHeader) - sizeof(pFECHeader->tagOffset);

		pFECPayloadHead = (pFECPayload)(m_pFECBuidData + sizeof(FECPacketHeader));
		pFECPayloadHead->dwPayloadlen = nDatalen;
		
		memcpy(m_pFECBuidData + sizeof(FECPacketHeader) + sizeof(FECPayload), pData, nDatalen);

		if (m_nBuidGroupIntraNo < m_pBuidCurMatrix->wNumDataCnt) 
		{
			for (int i = 0; i < m_pBuidCurMatrix->wNumReduCnt; i++) 
			{
				//生成冗余包需要此数据包
				if (m_pBuidCurMatrix->wValue[i][m_nBuidGroupIntraNo]) 
				{
					if (NULL == m_pFECBuidPacket[i])
					{
						m_pFECBuidPacket[i] = new char[FEC_PACKET_TOTAL_LEN_MAX];
					}
					
					//第一次使用前清空
					if (IsFECFirstPacket(m_nBuidGroupIntraNo, i, *m_pBuidCurMatrix)) 
					{
						memset(m_pFECBuidPacket[i], 0, FEC_PACKET_TOTAL_LEN_MAX);
					}
					
					pFECHeader = (pFECPacketHeader)m_pFECBuidPacket[i];		
					if (pFECHeader->nCodeSize < nDatalen) 
					{
						pFECHeader->nCodeSize = nDatalen;
					}
					
//					pFECPayloadHead = (pFECPayload)(m_pFECBuidPacket[i] + sizeof(FECPacketHeader));
//					pFECPayloadHead->dwPayloadlen = 0;

					pReduSrc = m_pFECBuidData + sizeof(FECPacketHeader);
					pReduDst = m_pFECBuidPacket[i] + sizeof(FECPacketHeader);
					
					//进行异或 生成冗余数据,FECPayload也放到数据中参与计算,以便恢复时得知真实数据长度
					for (int j = 0; j < nDatalen + (int)sizeof(FECPayload); j++) 
					{
						pReduDst[j] ^= pReduSrc[j];
					}
				}
			}
		}
		
		m_BuildCallBack(m_pFECBuidData, sizeof(FECPacketHeader) + sizeof(FECPayload) + nDatalen, m_dwBuildUser);
		
		m_nBuidGroupIntraNo++;
			
		//已生成所有的冗余包,发送		
		if (m_nBuidGroupIntraNo == m_pBuidCurMatrix->wNumDataCnt) 
		{
			for (int i = 0; i < m_pBuidCurMatrix->wNumReduCnt; i++)
			{
				pFECHeader = (pFECPacketHeader)m_pFECBuidPacket[i];
				
				pFECHeader->prefix = PREFIX_SINOWAVE;
				pFECHeader->type = 0x09;
				pFECHeader->subtype.version = 0x1;
				pFECHeader->subtype.FECType = FEC_Redu;
				pFECHeader->subtype.PacketType = m_nBuidPacketType;
				pFECHeader->GroupParam = (char)m_FECBuildType;
				pFECHeader->GroupIntraNo = m_nBuidGroupIntraNo;
				pFECHeader->dwGroupNo = m_dwBuidGroupNo;
				pFECHeader->dwPacketNo = m_dwBuidPacketNo;
				pFECHeader->CodeOffset = GetCodeOffset(*pFECHeader) + sizeof(FECPayload);
				pFECHeader->tagOffset = GetCodeOffset(*pFECHeader) - sizeof(pFECHeader->tagOffset);

				m_BuildCallBack(m_pFECBuidPacket[i], sizeof(FECPacketHeader) + sizeof(FECPayload) + pFECHeader->nCodeSize, m_dwBuildUser);
				
				m_nBuidGroupIntraNo++;
			}
			m_nBuidGroupIntraNo = 0;
			m_dwBuidGroupNo++;
		}
		
		m_dwBuidPacketNo++;
		
	}
	catch (...) 
	{
	//	TRACE0("BuildPacket catch error!\n");
	}
	
	m_nBuildEvent = 0;

	return 0;
}

int CFEC::ReleaseBuild()
{	
	try
	{
        int i = 0;
		//防止BuildPacket与ReleaseBuild不在一个线程调用引起冲突
		for (i = 0; i < 100; i++)
		{
			if (m_nBuildEvent != 0)
			{
				Sleep(1);
			}
			else
			{
				break;
			}
		}

		m_BuildCallBack = NULL;
		
		for (i = 0; i < FEC_REDU_COUNT_MAX; i++) 
		{
			SafeDeleteArray(m_pFECBuidPacket[i]);
		}
		
		SafeDeleteArray(m_pFECBuidData);
	}
	catch (...) 
	{
	//	TRACE0("FEC ReleaseBuild catch error!\n");
	}

	return 0;
}

int CFEC::BuildReset()
{
	m_nBuidPacketType = 0;
	m_dwSendTick = 0;
	m_dwBuidPacketNo = 0;
	m_dwBuidGroupNo = 0;
	m_nBuidGroupIntraNo = 0;

	return 0;
}

int CFEC::InitResume(pResumeCallBack ResumeCallBack,  pFrameNoCallBack FrameNoCallBack, LONG dwUser)
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
	m_bLostPacket = FALSE;
	m_dwResumeFlag1 = 0;
	m_dwResumeFlag2 = 0;
	m_pResCurMatrix = NULL;
	memset(m_RecvStatus, 0x00, sizeof(m_RecvStatus));

	return 0;
}

int CFEC::ReleaseResume()
{
	try
	{
        int i = 0;

		//防止BuildPacket与ReleaseBuild不在一个线程调用引起冲突
		for (i = 0; i < 100; i++)
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
		
		for (i = 0; i < FEC_REDU_COUNT_MAX; i++) 
		{
			SafeDeleteArray(m_pFECResPacket[i]);
		}
		for (i = 0; i < FEC_DATA_COUNT_MAX; i++) 
		{
			SafeDeleteArray(m_pFECResData[i]);
		}
	}
	catch (...) 
	{
	//	TRACE0("CFEC ReleaseResume catch error!\n");
	}
	return 0;
}

int CFEC::SendResumePacket(DWORD dwGroupNo)
{
	if (dwGroupNo == m_dwResFinGopNo || NULL == m_ResumeCallBack) 
	{
		return 0;
	}

	ResumeData();

	pFECPacketHeader pFECHeader = NULL;
	int nOffset = 0;

    int i = 0;
	for (i = 0; i < m_pResCurMatrix->wNumDataCnt; i++)
	{
		if (NULL != m_pFECResData[i]) 
		{
			pFECHeader = (pFECPacketHeader)m_pFECResData[i];
			nOffset = GetRealPosition(*pFECHeader);
			
			//数据状态为已收到,未发送 发送数据
			if (m_RecvStatus[i] == 0x01 && m_dwResGroupNo == pFECHeader->dwGroupNo) 
			{
				m_ResumeCallBack(m_pFECResData[i] + nOffset, pFECHeader->nCodeSize, m_dwResumeUser, m_nChannel, m_dwResumeFlag1, m_dwResumeFlag2);
			//	TRACE("SendResumePacket frameno = %d, type = %d, GroupParam = %d, GroupIntraNo = %d, datalen = %d\n", pFECHeader->dwPacketNo,
			//		pFECHeader->type, pFECHeader->GroupParam, pFECHeader->dwGroupNo, pFECHeader->nCodeSize);
			}
		}
	}

	m_nResGroupIntraNo = 0;
	m_bLostPacket = FALSE;
	m_dwResGroupNo++;
	memset(m_RecvStatus, 0, sizeof(m_RecvStatus));
	m_dwResFinGopNo = dwGroupNo;

	for (i = 0; i < FEC_REDU_COUNT_MAX; i++) 
	{
		if (NULL != m_pFECResPacket[i]) 
		{
			memset(m_pFECResPacket[i], 0x00, FEC_PACKET_TOTAL_LEN_MAX);
		}
	}

	return 0;
}

int CFEC::ResumeData()
{
	BOOL bResumeFinish = FALSE;//恢复数据完成
	int nDataCount = m_pResCurMatrix->wNumDataCnt;//一组中数据包的个数
	int nReduCount = m_pResCurMatrix->wNumReduCnt;//一组中冗余包的个数
	int nLostPktCnt = 0;//丢包个数
	int nResumePos = 0; //丢包位置
	int i = 0;//循环控制变量
	int j = 0;//循环控制变量
	int k = 0;//循环控制变量
	int nFillLen = 0;//恢复数据长度
	int nOffset = 0; //构造FEC包端 FEC头结构大小 FECPacketHeader与FECPayload之和
	int ntagOffset = 0;//构造FEC包端 FECPacketHeader结构大小
	char *pResumeSrc = NULL;//恢复数据源地址
	char *pResumeDest = NULL;//恢复数据目标地址
	pFECPacketHeader pFECHeader = NULL;
	pFECPacketHeader pFECResumeHeader = NULL;
	pFECPayload pFECPayloadHead = NULL;

	while (!bResumeFinish) 
	{
		bResumeFinish = TRUE;

		for (i = 0; i < nReduCount; i++) 
		{
			//是否收到该冗余包
			if (0 == m_RecvStatus[nDataCount + i] || NULL == m_pFECResPacket[i]) 
			{
				continue;
			}

			nLostPktCnt = 0;
			//判断与该冗余包参与运算的数据包的丢失个数，并且记录丢包的位置
			for (j = 0; j < nDataCount; j++) 
			{
				if (0x00 == m_RecvStatus[j] && 1 == m_pResCurMatrix->wValue[i][j]) 
				{
					nResumePos = j;
					nLostPktCnt++;
				}
			}

			//只有丢一个包时才恢复
			if (1 == nLostPktCnt) 
			{
				pFECResumeHeader = (pFECPacketHeader)m_pFECResPacket[i];
				nOffset = GetRealPosition(*pFECResumeHeader);
				ntagOffset = GetRealTagSize(*pFECResumeHeader);
				if (nOffset < ntagOffset || nOffset - nOffset > FEC_PACKET_HEAD_MAX) 
				{
				//	TRACE3("CFEC ResumeData error, Recvpacket = %d, nOffset = %d, ntagOffset = %d\n", nDataCount + i, nOffset, ntagOffset);
					m_RecvStatus[nDataCount + i] = 0;
					continue;	
				}

				pResumeDest = m_pFECResPacket[i] + ntagOffset;

				if (pFECResumeHeader->nCodeSize < 0 || pFECResumeHeader->nCodeSize > FEC_PACKET_MAX) 
				{
				//	TRACE2("CFEC ResumeData error, Recvpacket = %d, nCodeSize = %d\n", nDataCount + i, pFECResumeHeader->nCodeSize);
					m_RecvStatus[nDataCount + i] = 0;
					continue;
				}

				for (j = 0; j < nDataCount; j++)
				{
					if (0x00 == m_RecvStatus[j] || 0 == m_pResCurMatrix->wValue[i][j]
						|| NULL == m_pFECResData[j]) 
					{
						continue;
					}
					
					pFECHeader = (pFECPacketHeader)m_pFECResData[j];

					// 将数据包缓存填充成4字节对齐，不够的补0
                    //nFillLen = (pFECHeader->nCodeSize + 3) & 0xFFFFFFFC;
					//恢复数据长度为: 有效长度+发送端FECPayload结构大小
                    nFillLen = pFECHeader->nCodeSize + nOffset - ntagOffset;
					if (nFillLen < 0 || nFillLen > FEC_PACKET_TOTAL_LEN_MAX) 
					{
						continue;
					}

					pResumeSrc = m_pFECResData[j] + ntagOffset;
					
					//进行异或操作，恢复丢失的包
                    for (k = 0; k < nFillLen; k++)
                    {
                        pResumeDest[k] ^= pResumeSrc[k];
                    }
				}

				if (NULL == m_pFECResData[nResumePos]) 
				{
					m_pFECResData[nResumePos] = new char[FEC_PACKET_TOTAL_LEN_MAX];
				}
			
				pFECPayloadHead = (pFECPayload)(pResumeDest);

				pFECHeader = (pFECPacketHeader)m_pFECResData[nResumePos];	
				if (pFECPayloadHead->dwPayloadlen < 0 || pFECPayloadHead->dwPayloadlen > FEC_PACKET_MAX) 
				{
				//	TRACE1("ResumeData FECPayloadHead dataerror dwPayloadlen = %d!\n", pFECPayloadHead->dwPayloadlen);
					m_RecvStatus[nDataCount + i] = 0;
					continue;
				}
				
				//填充FECPacketHeader信息
				pFECHeader->prefix = PREFIX_SINOWAVE;
				pFECHeader->type = 0x09;
				pFECHeader->GroupParam = (char)m_FECResumeType;
				pFECHeader->GroupIntraNo = nResumePos;
				pFECHeader->dwGroupNo = m_dwResGroupNo;
				pFECHeader->dwPacketNo = pFECResumeHeader->dwPacketNo + nResumePos + 1 - nDataCount;
				pFECHeader->CodeOffset = pFECResumeHeader->CodeOffset;
				pFECHeader->tagOffset = pFECResumeHeader->tagOffset;
				pFECHeader->nCodeSize = pFECPayloadHead->dwPayloadlen;

				//拷贝恢复出来的FECPayload头结构和数据
				memcpy(m_pFECResData[nResumePos] + ntagOffset, pResumeDest, pFECHeader->nCodeSize + nOffset - ntagOffset);
				
				m_RecvStatus[nResumePos] = 1;
				
				//设置丢包循环控制变量,重新从头查找是否有可以恢复的包
				bResumeFinish = FALSE;
			}
		}
	}
	
	return 0;
}

int CFEC::ResumePacket(const char *pData, int nDatalen, DWORD flag1, DWORD flag2)
{
	if(NULL == pData)
	{
		return -1;
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

		//	TRACE0("FEC Resume reset\n");
		}
		
		if (m_dwRecveTick > 0 && GetTickCount() - m_dwRecveTick > FEC_TIMEOUT) 
		{
			ResumeReset();	
		}
		m_dwRecveTick = GetTickCount();

		pFECPacketHeader pFECHeader = (pFECPacketHeader)pData;

		int nOffset = GetRealPosition(*pFECHeader);
		if (nDatalen < nOffset || nDatalen > nOffset + FEC_PACKET_MAX) 
		{
			m_nResumeEvent = 0;
			return -2;
		}
		if (NULL == m_ResumeCallBack) 
		{
			m_nResumeEvent = 0;
			return -3;
		}
		
		if (nOffset < 0 || nOffset > FEC_PACKET_HEAD_MAX) 
		{
			m_nResumeEvent = 0;
			return -4;
		}
		
		if (pFECHeader->type != 0x09) 
		{
			m_nResumeEvent = 0;
			return -4;
		}

		if(pFECHeader->subtype.version != 0x1)
		{
			if (FEC_Data == pFECHeader->subtype.FECType)
			{
				m_ResumeCallBack(pData + nOffset, nDatalen - nOffset, m_dwResumeUser, m_nChannel, m_dwResumeFlag1, m_dwResumeFlag2);
				m_nResumeEvent = 0;
				return 0;
			}
			else
			{
				m_nResumeEvent = 0;
				return -5;
			}
		}
		//TRACE1("FEC Resume packetno = %d\n", pFECHeader->dwPacketNo);
		//cs<<"packetnum = "<<pFECHeader->dwPacketNo<<" m_bLostPacket = "<<m_bLostPacket<<endl;

		if (NULL == m_pResCurMatrix) 
		{
			m_FECResumeType = (FECMatrixIndex)pFECHeader->GroupParam;
			m_pResCurMatrix = &m_TriBudMatStu[m_FECResumeType];
		}
		else if (m_FECResumeType != pFECHeader->GroupParam)
		{
			SendResumePacket(m_dwResGroupNo);
			m_FECResumeType = (FECMatrixIndex)pFECHeader->GroupParam;
			m_pResCurMatrix = &m_TriBudMatStu[m_FECResumeType];
		}

		if (pFECHeader->subtype.PacketType != m_nResPacketType) 
		{
			SendResumePacket(m_dwResGroupNo);
			m_nResPacketType = pFECHeader->subtype.PacketType;
		}
		
		int nDataCount = m_pResCurMatrix->wNumDataCnt;
		int nReduCount = m_pResCurMatrix->wNumReduCnt;
		int nGroupIntraNo = pFECHeader->GroupIntraNo;

		if (FEC_Data == pFECHeader->subtype.FECType) //视频带FEC视频包
		{
			if (m_FrameNoCallBack != NULL)
			{
				m_FrameNoCallBack(pFECHeader->dwPacketNo, m_dwResumeUser);
			}

			//组号是否为当前组号
			if (m_dwResGroupNo != pFECHeader->dwGroupNo) 
			{
				SendResumePacket(m_dwResGroupNo);
				m_dwResGroupNo = pFECHeader->dwGroupNo;
			}
			
			if (nGroupIntraNo < 0 || nGroupIntraNo >= nDataCount) 
			{
				m_nResumeEvent = 0;
				return -5;
			}
			
			//判读是否丢包
			if (m_dwResPacketNo != pFECHeader->dwPacketNo) 
			{
				m_bLostPacket = TRUE;
			}
			
			if (NULL == m_pFECResData[nGroupIntraNo])
			{
				m_pFECResData[nGroupIntraNo] = new char[FEC_PACKET_TOTAL_LEN_MAX];
			}
			memcpy(m_pFECResData[nGroupIntraNo], pData, nDatalen);
			
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
			m_dwResPacketNo = pFECHeader->dwPacketNo + 1;
		}
		else if (FEC_Redu == pFECHeader->subtype.FECType) //视频FEC冗余包
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
			
			if (NULL == m_pFECResPacket[nResumeNo]) 
			{
				m_pFECResPacket[nResumeNo] = new char[FEC_PACKET_TOTAL_LEN_MAX];
			}
			memcpy(m_pFECResPacket[nResumeNo], pData, nDatalen);
			
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
			m_nResumeEvent = 0;
			return -7;
		}
	}
	catch (...) 
	{
	//	TRACE0("CFEC ResumePacket catch error!\n");
	}
	
	m_nResumeEvent = 0;

	return 0;
}

int CFEC::ResumeReset()
{
	m_nResPacketType = 0;
	m_dwRecveTick = 0;
	m_dwResPacketNo = 0;
	m_dwResGroupNo = 0;
	m_dwResFinGopNo = 0xffffffff;
	m_nResGroupIntraNo = 0;
	m_bLostPacket = FALSE;
	memset(m_RecvStatus, 0x00, sizeof(m_RecvStatus));

	return 0;
}

BOOL CFEC::IsFECFirstPacket(int nGroupIntraNo, int nFECPacketNo, const FECMatrix &CurMatrix) const
{
	for (int i = 0; i < CurMatrix.wNumDataCnt; i++) 
	{
		if (CurMatrix.wValue[nFECPacketNo][i]) 
		{
			if (i == nGroupIntraNo) 
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}			
	}
	return FALSE;
}

int CFEC::GetPacketNum() const 
{
	int ret = 0;

	for (int i = 0; i < m_pResCurMatrix->wNumDataCnt; i++) 
	{
		if (m_RecvStatus[i] > 0) 
		{
			ret++;
		}
	}

	return ret;
}

int CFEC::GetRealPosition(const FECPacketHeader &PacketHeader)
{
	return (char *)&PacketHeader.CodeOffset - (char *)&PacketHeader + sizeof(PacketHeader.CodeOffset) + PacketHeader.CodeOffset;
}

inline int CFEC::GetRealTagSize(const FECPacketHeader &PacketHeader)
{
	return (char *)&PacketHeader.tagOffset - (char *)&PacketHeader + sizeof(PacketHeader.tagOffset) + PacketHeader.tagOffset;
}

inline int CFEC::GetCodeOffset(const FECPacketHeader &PacketHeader)
{
	return sizeof(PacketHeader.tagOffset) + sizeof(PacketHeader.Reserve);
}