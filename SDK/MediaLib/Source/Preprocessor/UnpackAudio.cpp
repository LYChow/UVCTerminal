// UnpackAudio.cpp: implementation of the CUnpackAudio class.
//
//////////////////////////////////////////////////////////////////////
#include "UnpackAudio.h"
#include "VC3FEC.h"
#include "VC3RSFEC.h"
#include "SwBaseWin.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUnpackAudio::CUnpackAudio(int nChannel)
:m_nChannel(nChannel),
m_pFECRecive(NULL),
m_pFrameCallBack(NULL),
RcvBufDat(NULL),
OutBufDat(NULL),
RcvBufHdr(NULL),
RcvBufRepack(NULL),
RcvBufUnJoin(NULL),
m_dwUser(0),
m_wNewFrameNo(0),
m_wPreviousFrameNo(0)
{
	RcvBufHdr = new char[32];
//	RcvBufDat = new char[AUDIOBUFFERSIZEMAX];
//	OutBufDat = new char[AUDIOBUFFERSIZEMAX];
//	RcvBufRepack = new char[AUDIOBUFFERSIZEMAX];

	memset(RcvBufHdr, 0, 32);

	m_pRcvBufHdrEx = NULL;
	m_pRcvBufDatEx = NULL;
	m_pOutBufDatEx = NULL;
	m_pAudioBuf = NULL;
	m_nFlag = 0xABCDE987;
	m_nFlag1 = 0x12345678;
	m_dwRemoteIP = 0;
}

CUnpackAudio::~CUnpackAudio()
{
	try
	{
		SAFE_DELETE(m_pFECRecive);
		SAFE_DELETE_ARRAY(RcvBufDat);
		SAFE_DELETE_ARRAY(OutBufDat);
		SAFE_DELETE_ARRAY(RcvBufHdr);
		SAFE_DELETE_ARRAY(RcvBufRepack);
		SAFE_DELETE_ARRAY(RcvBufUnJoin);
		
		SAFE_DELETE_ARRAY(m_pRcvBufHdrEx);
		SAFE_DELETE_ARRAY(m_pRcvBufDatEx);
		SAFE_DELETE_ARRAY(m_pOutBufDatEx);
		SAFE_DELETE_ARRAY(m_pAudioBuf);
	}
	catch (...)
	{
	//	TRACE("CUnpackAudio catch error!\n");
	}
}

//FEC数据回调函数
int __stdcall CUnpackAudio::FECReceiveCallBack(const char* pRecvData, int nRecvDatalen, LONG dwUser, int nChannel, DWORD flag1, DWORD flag2)
{
	CUnpackAudio* pThis = (CUnpackAudio*)dwUser;

	if (pThis != NULL)
	{
		return pThis->PreDealRawAudioData((char*)pRecvData, nRecvDatalen, flag1, TRUE);
	}

	return -1;
}

//设置帧数据回调输出函数
int CUnpackAudio::SetFrameDataCallBack(LPFRAMEDATACALLBACK pFrameDataCallBack, LONG dwUser)
{
	m_dwUser = dwUser;
	m_pFrameCallBack = pFrameDataCallBack;
	return 0;
}

//输入数据
int CUnpackAudio::InputData(char* pData, int nLength, DWORD RemoteIP)
{
	if (NULL == pData || nLength <= 0)
	{
		return -1;
	}

	DWORD remote = RemoteIP;
	//立刻查看是否有VS转发协议头，如果有，则需要优先用协议头中源地址，并且跳过该头
	if(memcmp(pData, "VSTP", 4) == 0)
	{
		VSTP* p = (VSTP*)pData;
		remote = p->uSourceIP;
		pData += sizeof(VSTP);
		nLength -= sizeof(VSTP);
	}
	if (pData[0] != PREFIX_SINOWAVE)
	{
		return -2;
	}

	int nType = (pData[1] & 0x0F);
	if(nType != 0x02 && nType != 0x03 && nType != 0x06 && nType != 0x0F)
	{
		return -3;
	}
	
	if (m_dwRemoteIP != remote)
	{
		m_dwRemoteIP = remote;
	}

	int nRet =0;
	//新版本音频传输协议
	if (nType == 0x0F)
	{
		nRet = DealNewPacket(pData, nLength, remote);
	}
	else
	{
		//老版本音频传输协议
		nRet = DealOldPacket(pData, nLength, remote);
	}

	return nRet;
}

int CUnpackAudio::DealOldPacket( char* pData, int nLength, DWORD RemoteIP )
{
	if (pData[0] != PREFIX_SINOWAVE)
	{
		return -1;
	}
    char *pFrame = pData;
	if ((pData[1]&0x0F) == 0x06)//老版本FEC包
	{
		int nFrameLen = 0;
		if (NULL == m_pAudioBuf)
		{
			m_pAudioBuf = new char[10240];
			memset(m_pAudioBuf, 0x00, 10240);
		}
		pFrame = UnpackToFrame(m_pAudioBuf + 256, pData, nLength, nFrameLen);
		if (NULL == pFrame)
		{
			return 0;
		}
	}
	return DealFrame(pFrame, nLength, RemoteIP);
}

int CUnpackAudio::DealNewPacket( char* pData, int nLength, DWORD RemoteIP )
{
	if (pData == NULL || nLength <= 0)
		return -1;
	
	VC3COMMONHEADER *header = (VC3COMMONHEADER*)pData;
	//去通用头（已经去掉VSTP）
	if (header->version == 0x01 && header->majortype == VC3MT_AUDIO)
	{
		pData += sizeof(VC3COMMONHEADER);
		nLength  -= sizeof(VC3COMMONHEADER);
	}

	//防抖动处理
	if (pData[0] == ADOST_JITTER)
	{
		return 0;
	}
	else
	{
		return PreDealFECPacket(pData, nLength, RemoteIP);
	}
}

int CUnpackAudio::DealFrame( char *pData, int nLength, DWORD addr_remote )
{
	AUDIOFRAMEHEADER &Header = *(AUDIOFRAMEHEADER *)pData;
	if (Header.prefix != PREFIX_SINOWAVE || NULL == m_pFrameCallBack)
	{
		return -1;
	}

	if (!(((Header.type & 0x0F) == 0x02) || ((Header.type & 0x0F) == 0x03)))
	{
		return -2;
	}

	int sp=GETCODERELATIVEPOS(Header);	
	int nCodeSize = Header.CodeSize;
	char *pEncodeData = pData + sp;
	if (Header.NextSegmentFlag == 0)
	{
		if(Header.wFormatTag == CODEC_OPUS)
		{
			nCodeSize = *(WORD *)pEncodeData;
			pEncodeData += 2;
		}
		m_pFrameCallBack(pEncodeData, nCodeSize, Header.wFormatTag, Header.FrameNo, m_dwRemoteIP, m_nChannel, m_dwUser);
	}
	else
	{
		char *pNextSegment = pData + sp + Header.CodeSize;
		do 
		{
			AUDIOFRAMEHEADER Header2 = *(AUDIOFRAMEHEADER *)pNextSegment;
			if (0 == Header2.CodeSize)
			{
				//主席混音
				m_pFrameCallBack(pEncodeData, nCodeSize, Header.wFormatTag, Header.FrameNo, m_dwRemoteIP, m_nChannel, m_dwUser);
				break;
			}
			if( Header2.CodeSize != Header.CodeSize && Header2.wFormatTag != CODEC_OPUS )
			{
				break;
			}
			sp=GETCODERELATIVEPOS(Header2);
			nCodeSize = Header2.CodeSize;
			pEncodeData = pNextSegment + sp;
			if(Header2.wFormatTag == CODEC_OPUS)
			{
				nCodeSize = *(WORD *)pEncodeData;
				pEncodeData += 2;
			}

			m_pFrameCallBack(pEncodeData, nCodeSize, Header2.wFormatTag, Header2.FrameNo, Header2.uSrcIP, m_nChannel, m_dwUser);
			if (Header2.NextSegmentFlag == 0)
			{
				break;
			}

			pNextSegment += sp + Header.CodeSize;
		} while (1);
	}

	return 0;
}

//FEC预处理
int CUnpackAudio::PreDealFECPacket(char* pPack, int nLen, DWORD addr_remote)
{
	char subtype = pPack[0];
	
	//FEC处理
	if (subtype == ADOST_FEC)
	{
		//FEC相关处理代码
		pVC3FECPacketHeader pFECHeader = (pVC3FECPacketHeader)pPack;
		if (m_nFECVersion != pFECHeader->subtype.version)
		{
			SAFE_DELETE(m_pFECRecive);
		}
		if (NULL == m_pFECRecive) 
		{
			if (SWFECVersion_Nor == pFECHeader->subtype.version)
			{
				m_pFECRecive = new CVC3FEC();
			}
			else
			{
				m_pFECRecive = new CVC3RSFEC();
			}

		//	TRACE("CUnpackAudio Channel = %d, FECType = %d\n", m_nChannel, pFECHeader->subtype.version);
			
			m_pFECRecive->InitResume(FECReceiveCallBack, NULL, (LONG)this);
			m_pFECRecive->SetChannel(m_nChannel);
			m_nFECVersion = pFECHeader->subtype.version;
		}

 		m_pFECRecive->ResumePacket(pPack, nLen, addr_remote, 0);
	}
	else
	{
		PreDealRawAudioData(pPack, nLen, addr_remote);
	}
	return 0;	
}

//处理音频数据
int CUnpackAudio::PreDealRawAudioData(char* pPack, int nLen, DWORD addr_remote, BOOL bUseFEC)
{
	char* pRawData = pPack;
	int   nRawLen  = nLen;

	//检查是否还有VS转发协议头
	if( memcmp(pRawData,"VSTP",4) == 0 )
	{
//		VSTP* p = (VSTP*)pRawData;
		pRawData += sizeof(VSTP);
		nRawLen	 -= sizeof(VSTP);
	}

	char* RcvBuf = pRawData;

	char subtype = RcvBuf[0];

	//如果是网络拆包后数据，则需组包
	if (subtype == ADOST_REPACK || subtype == ADOST_REPACK_EX)
	{
		if (NULL == m_pRcvBufHdrEx)
		{
			m_pRcvBufHdrEx = new char[32];
			memset(m_pRcvBufHdrEx, 0x00, 32);
		}
		if (NULL == m_pRcvBufDatEx)
		{
			m_pRcvBufDatEx = new char[AUDIOBUFFERSIZEMAX];
			memset(m_pRcvBufDatEx, 0x00, AUDIOBUFFERSIZEMAX);
		}
		if (NULL == m_pOutBufDatEx)
		{
			m_pOutBufDatEx = new char[AUDIOBUFFERSIZEMAX];
			memset(m_pOutBufDatEx, 0x00, AUDIOBUFFERSIZEMAX);
		}
		//偏移256字节，防止写覆盖
		char* pFrameBuffer = m_pRcvBufDatEx + 256;
		char* pOuterBuffer = m_pOutBufDatEx + 256;
		//组包
		int nTotalLen = 0;
		RcvBuf = UnpackToFrameEx(pFrameBuffer, pOuterBuffer, m_pRcvBufHdrEx, pRawData, nRawLen, nTotalLen);

		//未获得一帧数据
		if ( RcvBuf == NULL || nTotalLen <= 0)
		{	
			return -1;
		}
		nLen = nTotalLen;
	}

	//获取帧类型
	subtype = RcvBuf[0];
	//如果是音频拆包数据
	if (subtype == ADOST_FRAME_REPACK)
	{
		int nOutLen = 0;
		if (NULL == RcvBufRepack)
		{
			RcvBufRepack = new char[AUDIOBUFFERSIZEMAX];
		}
		char* pOuterBuffer = RcvBufRepack + 256;
		RepackFrameHeader(RcvBuf, nLen, pOuterBuffer, nOutLen);

		if (nOutLen <= 0)
			return -1;

		RcvBuf = pOuterBuffer;
		nLen = nOutLen;
	}
	
	//获取帧类型
	subtype = RcvBuf[0];
	//如果是帧数据，则通过回调函数推送到外部
	if (subtype == ADOST_FRAME_NORMAL)
	{
		VC3AUDIOFRAMEHEADER* phdr = (VC3AUDIOFRAMEHEADER*)RcvBuf;
		int nBlocks = phdr->blocks;
		
		if (nBlocks > 1)
		{
			int nOutLen = 0;
			if (NULL == RcvBufUnJoin)
			{
				RcvBufUnJoin = new char[AUDIOBUFFERSIZEMAX];
			}
			char* pOuterBuffer = RcvBufUnJoin + 256;
			
			UnJoinMultiBlocksToFrame(RcvBuf, nLen, pOuterBuffer, nOutLen);

			if (nOutLen <= 0)
				return -1;

			RcvBuf = pOuterBuffer;
			nLen = nOutLen;
		}

		if (NULL != m_pFrameCallBack && nBlocks > 0)
		{
			int nOffset = 0;
			int nUnJoinPackSize = nLen/nBlocks;

			for (int i=0; i<nBlocks; i++)
			{
				m_pFrameCallBack(RcvBuf + nOffset + sizeof(VC3AUDIOFRAMEHEADER), nUnJoinPackSize - sizeof(VC3AUDIOFRAMEHEADER), phdr->formattag, phdr->FrameNo, addr_remote, m_nChannel, m_dwUser);
				nOffset += nUnJoinPackSize;
			}
		}
	}
	else
	{
	//	TRACE("subtype = %d\n", subtype);
	}

	return 0;	
}

//组音频帧(并且在拼帧的)
char* CUnpackAudio::UnpackToFrame(char *RecvPackData, char* OutPackData, char* RecvPackHeader, char* pPack, int nPackSize)
{
	VC3REPACKHEADER* pph = (VC3REPACKHEADER*)pPack;	//接收数据包头
	VC3REPACKHEADER* pRecvHeaderBuf = (VC3REPACKHEADER*)RecvPackHeader;	//缓存区包头
	
	BYTE* ppd = (BYTE*)(pPack + sizeof(VC3REPACKHEADER));			//接收到的实际数据
	char* pRecvDataBuf = RecvPackData + sizeof(VC3REPACKHEADER);	//第一个包数据在缓存区的开始位置
	
	//防止溢出
	if ( sizeof(VC3REPACKHEADER) + pph->offset + nPackSize > AUDIOBUFFERSIZEMAX )
	{
		return NULL;
	}
	
	BOOL bHasData = FALSE;

	//此处借用packcounter记录接收到小包数量
	if (pph->offset == 0)
	{
		//到新的一帧
		if (pRecvHeaderBuf->packcounter > 0)
		{
			bHasData = TRUE;
			memcpy(OutPackData, pRecvDataBuf, pRecvHeaderBuf->offset);
		}

		//拷贝帧头
		memcpy( pRecvHeaderBuf, pph, sizeof(VC3REPACKHEADER) );
		pRecvHeaderBuf->offset = 0;
		pRecvHeaderBuf->packcounter = 0;
		
		//拷贝第一帧数据
		memcpy( pRecvDataBuf, ppd, nPackSize-sizeof(VC3REPACKHEADER) );
		pRecvHeaderBuf->packcounter++;
		pRecvHeaderBuf->offset += nPackSize-sizeof(VC3REPACKHEADER);
	}
	else
	{
		//拷贝后续数据
		memcpy( pRecvDataBuf+pRecvHeaderBuf->offset, ppd, nPackSize-sizeof(VC3REPACKHEADER) );
		pRecvHeaderBuf->packcounter++;
		pRecvHeaderBuf->offset += nPackSize-sizeof(VC3REPACKHEADER);
	}

	if (bHasData)
		return OutPackData;
	else
		return NULL;
}

char * CUnpackAudio::UnpackToFrame( char* RecvBuf, char* pPack, int nPackSize,  int &nOutSize )
{
	AUDIOFRAMEHEADER header;
	memcpy((LPVOID)&header,(LPVOID)pPack,sizeof(header));
	
	//附加音频数据包小于等于4个（老版本且小于4路的音频数据）
	if( (header.type & 0x0F) == 0x02 || (header.type & 0x0F) == 0x03)
	{
		nOutSize = nPackSize;
		return pPack;
	}
	
	//如果是FEC数据包
	if( (header.type & 0x0F) == 0x07 || (header.type & 0x0F) == 0x08 )
	{
		return NULL;
	}
	
	//跳过额外增加的包头(FECGROUPHEADER/AUDIOFRAMEHEADER)
	pPack += sizeof(FECGROUPHEADER);
	nPackSize -= sizeof(FECGROUPHEADER);
	
	//准备解包
	REPACKHEADEREX* pph = (REPACKHEADEREX*)pPack;	//接收数据包头
	REPACKHEADEREX* pRecvHeaderBuf = (REPACKHEADEREX*)RecvBuf;	//缓存区包头
	
	//防止错误数据进入
	if( pph->type == 0x11 || pph->type == 0x21 || pph->type == 0x07 || pph->type == 0x08 )
	{
		return NULL;
	}
	
	BYTE* ppd = (BYTE*)(pPack + sizeof(REPACKHEADEREX));	//接收到的实际数据
	char* pRecvDataBuf = RecvBuf + sizeof(REPACKHEADEREX);	//第一个包数据在缓存区的开始位置
	
	//防止溢出
	if( sizeof(REPACKHEADEREX) + pph->offset + nPackSize > AUDIOBUFFERSIZEMAX )
		return NULL;
	
	//解包
	if( pph->offset == 0 )
	{
		if( pph->type == 0x11 || pph->type == 0x21 )
		{
			return NULL;
		}
		
		m_nFrameLen = 0;
		if( pRecvHeaderBuf->packcounter != 0 )
		{
		}
		
		//如果是帧的第一包,则拷贝包头
		memcpy( pRecvHeaderBuf, pph, sizeof(REPACKHEADEREX) );
		
		//拷贝数据到适当位置
		memcpy( pRecvDataBuf+pph->offset, ppd, nPackSize-sizeof(REPACKHEADEREX) );
		
		m_nFrameLen += nPackSize-sizeof(REPACKHEADEREX);
		//减少未完成包计数器
		pRecvHeaderBuf->packcounter --;
		
	}
	else
	{
		//如果帧的头包没到而后续的包先到了（码流仍然连续是的），则暂时用后续的包头代替头包包头
		if( pph->FrameNoLowByte == (BYTE)(pRecvHeaderBuf->FrameNoLowByte+1) )
		{
			memcpy( pRecvHeaderBuf, pph, sizeof(REPACKHEADEREX) );
		}
		
		//只有同一帧的数据包才可以拼接
		if( pph->FrameNoLowByte == pRecvHeaderBuf->FrameNoLowByte )
		{
			//拷贝数据到适当位置
			memcpy( pRecvDataBuf+pph->offset, ppd, nPackSize-sizeof(REPACKHEADEREX) );
			
			m_nFrameLen += nPackSize-sizeof(REPACKHEADEREX);
			//减少未完成包计数器
			pRecvHeaderBuf->packcounter --;
			
		}
		else
		{
			return NULL;
		}
	}
	
	//判断是否合成完毕
	if( pRecvHeaderBuf->packcounter == 0 )
	{
		nOutSize = m_nFrameLen;
		return pRecvDataBuf;
	}
	else
		return NULL;
}

//去掉音频拆包帧头
int CUnpackAudio::RepackFrameHeader(char* pData, int nLen, char*pOutData, int &nOutLen)
{
	//构造音频包头
	BYTE* pNextSegment = (BYTE*)pData;
	VC3AUDIOFRAMEHEADER* phdr = (VC3AUDIOFRAMEHEADER*)pNextSegment;
	BYTE *pEnd = pNextSegment + nLen;

	//原始音频数据长度
	DWORD dwSegmentSize = phdr->CodeSize;
	
	int offset_write = 0;
	int nSegmentIdx=0;
	
//	while( phdr->CodeSize == dwSegmentSize )
	while(1)
	{
		if (phdr->CodeSize != dwSegmentSize && phdr->formattag != CODEC_OPUS)
		{
			break;
		}

		//段中音频数据位置
		int sp = GETCODERELATIVEPOS((*phdr));
		dwSegmentSize = phdr->CodeSize;

		if (pNextSegment >= pEnd || pNextSegment < (BYTE *)pData
			|| sp <= 0 || dwSegmentSize > AUDIOBUFFERSIZEMAX
			|| phdr->CodeOffset < 0 || phdr->subtype < 0)
		{
		//	TRACE("SP = %d, dwSegmentSize = %d\n", sp, dwSegmentSize);
			break;
		}

		char subtype = pNextSegment[0];
		//第一个segment使用音频帧头替换（使用音频帧头代替音频拆包包头）
		if (nSegmentIdx == 0 && subtype == ADOST_FRAME_REPACK)
		{
			VC3AUDIOREPACKHEADER* prph = (VC3AUDIOREPACKHEADER*)pNextSegment;
			VC3AUDIOFRAMEHEADER* pafh = (VC3AUDIOFRAMEHEADER*)(pOutData+offset_write);

			memcpy(pOutData+offset_write, pNextSegment, sizeof(VC3AUDIOCOMMON));
			pafh->subtype = ADOST_FRAME_NORMAL;
			pafh->FrameNo = prph->FrameNo;
			pafh->CodeOffset = sizeof(pafh->FrameNo);
	//		TRACE("Repack FrameNo = %d, audio FrameNo = %d\n", prph->FrameNo, pafh->FrameNo);

			//计算输出缓冲区位置
			offset_write += sizeof(VC3AUDIOFRAMEHEADER);
		}
		//后续的音频拆包包头（使用音频叠加头代替音频拆包包头）
		else if (nSegmentIdx != 0 && subtype == ADOST_FRAME_REPACK)
		{
			VC3AUDIOREPACKHEADER* prph = (VC3AUDIOREPACKHEADER*)pNextSegment;
			VC3AUDIOAPPENDFRAMEHEADER* paph = (VC3AUDIOAPPENDFRAMEHEADER*)(pOutData+offset_write);
			
			memcpy(pOutData+offset_write, pNextSegment, sizeof(VC3AUDIOCOMMON));
			paph->subtype = ADOST_FRAME_APPEND;
			paph->uSrcIP = prph->uSrcIP;
			paph->CodeOffset = sizeof(paph->uSrcIP);

			//计算输出缓冲区位置
			offset_write += sizeof(VC3AUDIOAPPENDFRAMEHEADER);
		}
		else
		{
			memcpy(pOutData+offset_write, pNextSegment, sizeof(VC3AUDIOAPPENDFRAMEHEADER));

			//计算输出缓冲区位置
			offset_write += sizeof(VC3AUDIOAPPENDFRAMEHEADER);
		}

		//拷贝帧内片断
		memcpy(pOutData+offset_write, pNextSegment+sp, dwSegmentSize);
		offset_write += dwSegmentSize;
		
		//判断是否该停止
		if( phdr->nextsegment == 0 )	//NextSegmentFlag=0表示没有后续的段
			break;
		
		nSegmentIdx++;
		
		//准备下一个段
		pNextSegment += sp + phdr->CodeSize;
		phdr = (VC3AUDIOFRAMEHEADER*)pNextSegment;
	}

	nOutLen = offset_write;
	return 0;
}

//分离拼接在一起的音频帧
int CUnpackAudio::UnJoinMultiBlocksToFrame(char* pData, int nLen, char* pOutData, int &nOutLen)
{
	//构造音频包头
	VC3AUDIOFRAMEHEADER* phdr = (VC3AUDIOFRAMEHEADER*)pData;

	int offset_data  = 0;
	int offset_write = 0;
	int nBlocks = phdr->blocks;
	int nBlockSize = phdr->CodeSize/nBlocks;

	int nLostCount=0;
	//不连续或到达最大值(65535)
	if (m_wPreviousFrameNo+1 != phdr->FrameNo)
	{
		nLostCount = phdr->FrameNo-m_wPreviousFrameNo;
	}
	m_wPreviousFrameNo = phdr->FrameNo;

	//丢帧后重新计算帧序号
	m_wNewFrameNo += nLostCount*nBlocks;

	for (int i=0; i<nBlocks; i++)
	{
		offset_data = i*nBlockSize;

		BYTE* pNextSegment = (BYTE*)pData;

		VC3AUDIOFRAMEHEADER* phdr2 = (VC3AUDIOFRAMEHEADER*)pNextSegment;

		int nSegmentIdx=0;
		while( phdr2->CodeSize == phdr->CodeSize )
		{	
			int sp = GETCODERELATIVEPOS((*phdr2));
			//拷贝帧头
			memcpy(pOutData+offset_write, pNextSegment, sp);
			//设置帧头信息
			VC3AUDIOFRAMEHEADER* pNewHdr = (VC3AUDIOFRAMEHEADER*)(pOutData+offset_write);
			pNewHdr->CodeSize = nBlockSize;
			pNewHdr->blocks = 1;

			//重新设置帧序号
			if (nSegmentIdx == 0)
			{
				pNewHdr->FrameNo = m_wNewFrameNo;
				m_wNewFrameNo++;
			}

			//计算缓冲区位置
			offset_write += sp;

			//拷贝帧内片断
			memcpy(pOutData+offset_write, pNextSegment+sp+offset_data, nBlockSize);
			offset_write += nBlockSize;
			
			//判断是否该停止
			if( phdr2->nextsegment == 0 )	//NextSegmentFlag=0表示没有后续的段
				break;
			
			nSegmentIdx++;
				
			//准备下一个段
			pNextSegment += sp + phdr2->CodeSize;
			phdr2 = (VC3AUDIOFRAMEHEADER*)pNextSegment;
		}
	}

	nOutLen = offset_write;

	return 0;
}

//组包函数
char* CUnpackAudio::UnpackToFrameEx(char *RecvPackData, char* OutPackData, char* RecvPackHeader, 
									char* pPack, int nPackSize, int &nTotalLen)
{
	nTotalLen = 0;
	VC3REPACKHEADER* pph = (VC3REPACKHEADER*)pPack;	//接收数据包头
	VC3REPACKHEADER* pRecvHeaderBuf = (VC3REPACKHEADER*)RecvPackHeader;	//缓存区包头
	
	BYTE* ppd = (BYTE*)(pPack + sizeof(VC3REPACKHEADER));			//接收到的实际数据
	char* pRecvDataBuf = RecvPackData + sizeof(VC3REPACKHEADER);	//第一个包数据在缓存区的开始位置
	
	//防止溢出
	if ( sizeof(VC3REPACKHEADER) + pph->offset + nPackSize > AUDIOBUFFERSIZEMAX )
	{
		return NULL;
	}

	if (pRecvHeaderBuf->FrameNoLowByte != pph->FrameNoLowByte)
	{
		//拷贝拆包头
		memcpy( pRecvHeaderBuf, pph, sizeof(VC3REPACKHEADER) );
		pRecvHeaderBuf->offset = 0;
	}

//	if (pph->offset == 0)
	{
		//拷贝数据
		memcpy( pRecvDataBuf + pRecvHeaderBuf->offset, ppd, nPackSize-sizeof(VC3REPACKHEADER) );
		pRecvHeaderBuf->packcounter--;
		pRecvHeaderBuf->offset += nPackSize-sizeof(VC3REPACKHEADER);
	}
// 	else
 //	{
 		//拷贝后续数据
 //		CopyMemory( pRecvDataBuf+pRecvHeaderBuf->offset, ppd, nPackSize-sizeof(VC3REPACKHEADER) );
 //		pRecvHeaderBuf->packcounter--;
 //		pRecvHeaderBuf->offset += nPackSize-sizeof(VC3REPACKHEADER);
 //	}
	
	if (pRecvHeaderBuf->packcounter == 0)
	{
		memcpy(OutPackData, pRecvDataBuf, pRecvHeaderBuf->offset);
		nTotalLen = pRecvHeaderBuf->offset;
		return OutPackData;
	}
	else
		return NULL;
}