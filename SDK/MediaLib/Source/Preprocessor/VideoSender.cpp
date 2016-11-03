// VideoSender.cpp: implementation of the CVideoSender class.
//
//////////////////////////////////////////////////////////////////////
#include "VideoSender.h"
#include "CaptionDef.h"
#include "h264define.h"
#include "MyPrivateTypeDefine.h"

char* PrefixVSTPHeaderIfNeed( char* pBuf, int &nLen, unsigned int dwLocalIP);
int CalcChecksumValueByInt(const char* buf, int len);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVideoSender::CVideoSender()
:m_bUseSmooth(FALSE),
m_bUseJitter(FALSE),
m_bUseFEC(FALSE),
m_nFECType(0),
//m_pSmoothSend(NULL),
m_pFECSend(NULL),
m_pJitter(NULL),
m_pFrameBuf(NULL),
m_pSmoothSndBuf(NULL),
m_lpVideoDataCB(NULL),
m_dwUser(0),
m_dwPacknum(0)
{
	m_nFrameBufLen = VIDEOBUFFERSIZEINIT;
	m_nStreamType = 0;
	m_pFECBuf = NULL;
	m_pJitterBuf = NULL;
	m_nLastFECType = 0;
	m_pVSTPBuffer = NULL;
	m_dwCodecFOURCC = 0;
	memset(m_szHostName, 0x00, sizeof(m_szHostName));
}

CVideoSender::~CVideoSender()
{
//	SAFE_DELETE(m_pSmoothSend);
	SAFE_DELETE(m_pFECSend);
	SAFE_DELETE(m_pJitter);
	SAFE_DELETE_ARRAY(m_pFrameBuf);
	SAFE_DELETE_ARRAY(m_pSmoothSndBuf);
	SAFE_DELETE_ARRAY(m_pFECBuf);
	SAFE_DELETE_ARRAY(m_pJitterBuf);
	SAFE_DELETE_ARRAY(m_pVSTPBuffer);
	galois_release();
}

//设置应用策略
void CVideoSender::Init(unsigned int dwLocalIP, DWORD dwMask, int nFECType, const char *szHostName, DWORD CodecFOURCC, LPVS_NETDATACALLBACK lpVideoDataCB, LONG dwUser)
{
	m_nFECType = -1;
	m_nLastFECType = -1;
	if (dwMask & SWVideoSendMask_Smooth)
	{
		m_bUseSmooth = TRUE;
	}
	if (dwMask & SWVideoSendMask_FEC)
	{
		m_bUseFEC = TRUE;
		m_nFECType = nFECType;
		m_nLastFECType = nFECType;
	}
	if (dwMask & SWVideoSendMask_Jitter)
	{
		m_bUseJitter = TRUE;
	}
	m_dwLocalIP = dwLocalIP;
	m_dwCodecFOURCC = CodecFOURCC;
	strncpy(m_szHostName, szHostName, sizeof(m_szHostName) - 1);
	m_dwUser = dwUser;
	m_lpVideoDataCB = lpVideoDataCB;
}

int CVideoSender::InputData( const char* pData, int nLen, int nWidth, int nHeight, DWORD FrameNo, int nFrameType, int nFrameRate, int nBandWidth, DWORD dwTimestamp )
{
	//有效性检查
	if (NULL == pData || nLen <=0) 
		return -1;

	VIDEOFRAMEHEADER FrameHeader;
	memset(&FrameHeader, 0x00, sizeof(FrameHeader));
	FrameHeader.prefix = PREFIX_SINOWAVE;
	FrameHeader.type = 0x07;
	FrameHeader.flagFOURCC = m_dwCodecFOURCC;
	FrameHeader.WidthHeight = (LONG)(((WORD)(nHeight)) | ((DWORD)((WORD)(nWidth))) << 16);
	FrameHeader.FrameNo = FrameNo;
	FrameHeader.FrameRate = (char)nFrameRate;
	FrameHeader.dwFlags = (nFrameType==1) ? 0x10 : 0;
	FrameHeader.CodeSize = nLen;
	FrameHeader.CodeOffset = GETOFFSETOFVDOHDRV2(FrameHeader);
	memcpy(FrameHeader.HostName, m_szHostName, 15);
	memcpy(FrameHeader.HostNameEx, m_szHostName, 31);
	FrameHeader.timestamp.VideoTimeStamp.dwLowPart = dwTimestamp;
	FrameHeader.timestamp.VideoTimeStamp.wBandWidth = nBandWidth;

	return InputData(pData, nLen, &FrameHeader);
}

//发送数据
int CVideoSender::InputData(const char* pData, int nLen, VIDEOFRAMEHEADER* pHdr)
{
	//有效性检查
	if (NULL == pData || nLen <=0 || NULL == pHdr) 
		return -1;

	int ret = PreDealingH264(pData, nLen, pHdr);

	return ret;
}

int CVideoSender::PreDealingH264(const char* pData, int nLen, VIDEOFRAMEHEADER* pHdr)
{
	const char *pCodeSrc = pData;
	pPacket264 pPacket = NULL;
	
	if (pHdr->timestamp.VideoTimeStamp.bOptimizeNalu)
	{
		AddInfoToFrameX264(pData, nLen, pHdr);
		m_dwPacknum++;
	}
	else if (CODEC_H264 != pHdr->flagFOURCC) 
	{
		AddInfoToFrame(pData, nLen, pHdr);
		m_dwPacknum++;
	}
	else
	{
		DWORD dwNAL = 0x01000000;
		if (0 == memcmp(pData, &dwNAL, sizeof(dwNAL))) 
		{
			AddInfoToFrame(pData, nLen, pHdr);
			m_dwPacknum++;
		}
		else
		{
			pPacket = (pPacket264)pData;
			int nNalnum = pPacket->dwPacketTotalNum;
			if (nNalnum <= 0) 
			{
				return -2;
			}
			BOOL bKeyFrame = FALSE;
			pPacket264 pPackettem = (pPacket264)(pData + (nNalnum - 1)*sizeof(Packet264));
			if (pPackettem->dwNalutype >= 5 && pPackettem->dwNalutype <= 8)
			{
				bKeyFrame = 16;
			}
			else
			{
				bKeyFrame = 0;
			}
			nNalnum = pPackettem->dwPacketTotalNum;
			
			for (int i = 0; i < nNalnum; i++) 
			{	
				VIDEOFRAMEHEADER SendHeader(*pHdr);
				pPacket = (pPacket264)pCodeSrc;
				AddInfoToFrame(pCodeSrc, pPacket->nPacketSize + sizeof(Packet264) - PAYLOAD_264_MAX, &SendHeader);
				m_dwPacknum++;
				pCodeSrc += sizeof(Packet264);
			}
		}
	}

	return 0;
}

int CVideoSender::AddInfoToFrame(const char* pData, int nLen, VIDEOFRAMEHEADER* pHdr)
{
	//有效性检查
	if (pData == NULL || nLen <= 0 || pHdr == NULL)
		return -1;

	int status = 0;
	int nFrameSize = nLen;
	int sp=GETCODERELATIVEPOS((*pHdr));
	int nStreamType = (pHdr->type&0x0F);
	BOOL bKeyFrameFlag = ( pHdr->dwFlags > 10 );
	DWORD dwFrameNo = 0;

	if (nLen + 512 > m_nFrameBufLen)
	{
		SAFE_DELETE_ARRAY(m_pFrameBuf);
		m_nFrameBufLen = ((nLen + 512 + VIDEOBUFFERSIZEINIT - 1)/VIDEOBUFFERSIZEINIT)*VIDEOBUFFERSIZEINIT;
	}

	if (NULL == m_pFrameBuf)
	{
		m_pFrameBuf = new char[m_nFrameBufLen];
	}

	if (m_nStreamType == 0)
	{
		if (pHdr->FrameNo == 0)
		{
		//	pHdr->FrameNo = m_dwPacknum;
		}
		else //透传广播转移数据
		{
		//	pHdr->FrameNo = m_dwPacknum + pHdr->FrameNo;
		}
		pHdr->CodeSize = nLen;
		pHdr->ChecksumLowByte = (BYTE)CalcChecksumValueByInt(pData, nLen);
		dwFrameNo = pHdr->FrameNo;
	}
	else
	{
		pVideoFrameHeaderNew pFrameHeader = (pVideoFrameHeaderNew)pHdr;
		pFrameHeader->FrameNo = m_dwPacknum;
		pFrameHeader->CodeSize = nLen;
		pFrameHeader->ChecksumLowByte = (BYTE)CalcChecksumValueByInt(pData, nLen);
		bKeyFrameFlag = pFrameHeader->dwFlag&VideoFrameHeaderNewFlag_FrameType;
		dwFrameNo = m_dwPacknum;
	}

	//预留视频数据包头信息
	char* pFrameBuf = &m_pFrameBuf[sizeof(VSTP)+sizeof(REPACKHEADEREX)+sp+256];

	//拷贝数据到帧缓冲区
	memcpy((void *)(pFrameBuf+sp), (char *)pData, nFrameSize);
	memcpy((void *)pFrameBuf, (void *)pHdr, sp);

	//滚动字幕处理
//	HandleFrameCaptionScroll(HIWORD(pHdr->WidthHeight), LOWORD(pHdr->WidthHeight));
	
	char szFrameCaption[4] = {0};
	FRAMECAPTIONHEADER FrameCaptionHeader;
	//字幕信息加到帧尾部
	memcpy((void *)&pFrameBuf[sp+nFrameSize],&FrameCaptionHeader,sizeof(FRAMECAPTIONHEADER));	//字幕头信息
	int CapLen = min(strlen(szFrameCaption),FRAMECAPTIONLENGTHMAX);
	memcpy((void *)&pFrameBuf[sp+nFrameSize+sizeof(FRAMECAPTIONHEADER)], szFrameCaption, CapLen);	//字幕信息附加到结尾
	pFrameBuf[sp+nFrameSize+sizeof(FRAMECAPTIONHEADER)+CapLen] = 0;
	nFrameSize+=sp+sizeof(FRAMECAPTIONHEADER)+(CapLen+1);

	try
	{
		status = RepackSendTo(pFrameBuf,nFrameSize,0, (BYTE)dwFrameNo, nStreamType);		
	}
	catch (...) 
	{
		printf("Send Video data catch Exception Send\n");
	}	

	return 0;
}
 
int CVideoSender::RepackSendTo(const char * buf, int len, int flags, BYTE frameno, int nStreamType)
{
	int nPackSizeMax = MAXUDPPACKSIZE-34;
//	VIDEOFRAMEHEADER* pvhdr = (VIDEOFRAMEHEADER*)buf;	
//	BOOL bKeyFrameFlag = ( pvhdr->dwFlags > 10 );
	int framelen = len;

	int nPackCount = (len / nPackSizeMax) + ((len%nPackSizeMax==0) ? 0 : 1);	//拆包后包的个数

	//拆包发送
	framelen = len;
	int nByteSend = 0;
	for( int i=0; i<nPackCount; i++ )
	{
		//设置包头
		DWORD offset = i * nPackSizeMax;

		//第一通道视频码流，兼容使用拆包头结构为：REPACKHEADER
		if( nStreamType == 0x01 )
		{
			REPACKHEADER* pph = (REPACKHEADER*)(buf + offset - sizeof(REPACKHEADER));
			pph->prefix = PREFIX_SINOWAVE;
			pph->type = nStreamType;
			pph->FrameNoLowByte = frameno;
			pph->offset = (WORD)offset;
			pph->packcounter = nPackCount;
			
			//重新计算包大小
			int packsize = nPackSizeMax;
			if( framelen < packsize ) packsize = framelen;
			DWORD sendlen = packsize+sizeof(REPACKHEADER);
			
			//检查是否需要追加VS转发协议头，如果需要，则追加
			char* pbufsend = (char *)pph;//PrefixVSTPHeaderIfNeed( (char*)pph, &sendlen, ((sockaddr_in*)to)->sin_addr.s_addr );

			int nSendLen = 0;
			//编码后直接发送出去
		//	nSendLen = PreDealingSmoothSend(s, pbufsend, sendlen, flags, to, tolen, bKeyFrameFlag, udest, packsize);
			nSendLen = PreDealingFEC(pbufsend, sendlen, flags);

			nByteSend += nSendLen;
			framelen -= packsize;
		}
		//第一通道大视频数据及第二、第三通道视频数据使用扩展拆包头结构为：REPACKHEADEREX
		else if( nStreamType == 0x07 || nStreamType == 0x0B || nStreamType == 0x0C )
		{
			REPACKHEADEREX* pph = (REPACKHEADEREX*)(buf + offset - sizeof(REPACKHEADEREX));
			pph->prefix = PREFIX_SINOWAVE;
			pph->type = nStreamType;
			pph->FrameNoLowByte = frameno;
			pph->offset = offset;
			pph->packcounter = nPackCount;

			//重新计算包大小
			int packsize = nPackSizeMax;
			if( framelen < packsize ) packsize = framelen;
			DWORD sendlen = packsize+sizeof(REPACKHEADEREX);
			
			//检查是否需要追加VS转发协议头，如果需要，则追加
			char* pbufsend = (char *)pph;//PrefixVSTPHeaderIfNeed( (char*)pph, &sendlen, ((sockaddr_in*)to)->sin_addr.s_addr );

			int nSendLen = 0;
		//	nSendLen = PreDealingSmoothSend(s, pbufsend, sendlen, flags, to, tolen, bKeyFrameFlag, udest, packsize);
			nSendLen = PreDealingFEC(pbufsend, sendlen, flags);

			nByteSend += nSendLen;
			framelen -= packsize;
		}
		else if ((nStreamType&0x0F) == 0x0F) //子码流
		{
			pVC3VideoRepackHeader pVideoRepackHeader = (pVC3VideoRepackHeader)(buf + offset - sizeof(VC3VideoRepackHeader));
			pVideoRepackHeader->prefix = PREFIX_SINOWAVE;
			pVideoRepackHeader->fixtype = 0x0F;
			pVideoRepackHeader->version = 0x01;
			pVideoRepackHeader->majortype = VC3MT_VIDEO;
			pVideoRepackHeader->VideoType = m_nStreamType;
			pVideoRepackHeader->PacketType = VideoSubType_Repack;
			pVideoRepackHeader->offset = offset;
			pVideoRepackHeader->packcounter = nPackCount;
			pVideoRepackHeader->FrameNoLowByte = frameno;

			//重新计算包大小
			int packsize = nPackSizeMax;
			if( framelen < packsize ) 
				packsize = framelen;
			DWORD sendlen = packsize+sizeof(VC3VideoRepackHeader);
			
			//检查是否需要追加VS转发协议头，如果需要，则追加
			char* pbufsend = (char *)pVideoRepackHeader;//PrefixVSTPHeaderIfNeed( (char*)pVideoRepackHeader, &sendlen, ((sockaddr_in*)to)->sin_addr.s_addr );
			
			int nSendLen = 0;
			nSendLen = PreDealingFEC(pbufsend, sendlen, flags);
		
			nByteSend += nSendLen;
			framelen -= packsize;
		}
	}
	return nByteSend;
}

int CVideoSender::RepackSendToEx(const char *p264Buf, const char * buf, int len, int flags, BYTE frameno, int nStreamType)
{
	int nPackSizeMax = MAXUDPPACKSIZE-34;
	pPacket264 p264Pack = (pPacket264)p264Buf;
	int nNalnum = p264Pack->dwPacketTotalNum;
	if (nNalnum <= 0) 
	{
		return -1;
	}
	
	VIDEOFRAMEHEADER* pvhdr = (VIDEOFRAMEHEADER*)buf;
	
	BOOL bKeyFrameFlag = ( pvhdr->dwFlags > 10 );
	if (m_nStreamType != 0)
	{
		pVideoFrameHeaderNew pVideoFrameHeader = (pVideoFrameHeaderNew)buf;
		bKeyFrameFlag = pVideoFrameHeader->dwFlag&VideoFrameHeaderNewFlag_FrameType;
	}
	int framelen = len;
	int nPackCount = 0;	//拆包后包的个数
	int *nNalLenIndex = new int[nNalnum + 1];
	memset(nNalLenIndex, 0x00, sizeof(int)*(nNalnum + 1));

	int nLen = GETCODERELATIVEPOS((*pvhdr));
	for (int nNul = 0; nNul < nNalnum; nNul++)
	{
		p264Pack = (pPacket264)(p264Buf + nNul*sizeof(Packet264));
		nLen += p264Pack->nPacketSize;
		if (nLen >= nPackSizeMax)
		{
			nNalLenIndex[nPackCount] = nLen - p264Pack->nPacketSize;;
			nPackCount++;
			nLen = p264Pack->nPacketSize;
			if (nNul == nNalnum - 1)
			{
				nNalLenIndex[nPackCount] = nLen;
				nPackCount++;
				break;
			}
		}
		if (nNul == nNalnum - 1)
		{
			nNalLenIndex[nPackCount] = nLen;
			nPackCount++;
			break;
		}
	}
	
	//拆包发送
	framelen = len;
	int nByteSend = 0;
	DWORD offset = 0;
	for( int i=0; i<nPackCount; i++ )
	{
		//设置包头
	//	DWORD offset = i * nPackSizeMax;		
		//第一通道视频码流，兼容使用拆包头结构为：REPACKHEADER
		if( nStreamType == 0x01 )
		{
			REPACKHEADER* pph = (REPACKHEADER*)(buf + offset - sizeof(REPACKHEADER));
			pph->prefix = PREFIX_SINOWAVE;
			pph->type = nStreamType;
			pph->FrameNoLowByte = frameno;
			pph->offset = (WORD)offset;
			pph->packcounter = nPackCount;
			
			//重新计算包大小
			int packsize = nNalLenIndex[i];
			if (i == (nPackCount - 1))
			{
				packsize = len - offset;
			}
			if(framelen < packsize)
			{
				packsize = framelen;
			}
			offset += packsize;
			DWORD sendlen = packsize+sizeof(REPACKHEADER);
			
			//检查是否需要追加VS转发协议头，如果需要，则追加
			char* pbufsend = (char*)pph;//PrefixVSTPHeaderIfNeed( (char*)pph, &sendlen, ((sockaddr_in*)to)->sin_addr.s_addr );
			
			int nSendLen = 0;
			//编码后直接发送出去
			//	nSendLen = PreDealingSmoothSend(s, pbufsend, sendlen, flags, to, tolen, bKeyFrameFlag, udest, packsize);
			nSendLen = PreDealingFEC(pbufsend, sendlen, flags);
			
			nByteSend += nSendLen;
			framelen -= packsize;
		}
		//第一通道大视频数据及第二、第三通道视频数据使用扩展拆包头结构为：REPACKHEADEREX
		else if( nStreamType == 0x07 || nStreamType == 0x0B || nStreamType == 0x0C )
		{
			REPACKHEADEREX* pph = (REPACKHEADEREX*)(buf + offset - sizeof(REPACKHEADEREX));
			pph->prefix = PREFIX_SINOWAVE;
			pph->type = nStreamType;
			pph->FrameNoLowByte = frameno;
			pph->offset = offset;
			pph->packcounter = nPackCount;
			
			//重新计算包大小
			int packsize = nNalLenIndex[i];
			if (i == (nPackCount - 1))
			{
				packsize = len - offset;
			}
			if(framelen < packsize)
			{
				packsize = framelen;
			}
			offset += packsize;
			DWORD sendlen = packsize+sizeof(REPACKHEADEREX);
			
			//检查是否需要追加VS转发协议头，如果需要，则追加
			char* pbufsend = (char*)pph;//PrefixVSTPHeaderIfNeed( (char*)pph, &sendlen, ((sockaddr_in*)to)->sin_addr.s_addr );
			
			int nSendLen = 0;
			//	nSendLen = PreDealingSmoothSend(s, pbufsend, sendlen, flags, to, tolen, bKeyFrameFlag, udest, packsize);
			nSendLen = PreDealingFEC(pbufsend, sendlen, flags);
	
			nByteSend += nSendLen;
			framelen -= packsize;
		}
		else if ((nStreamType&0x0F) == 0x0F) //子码流
		{
			pVC3VideoRepackHeader pVideoRepackHeader = (pVC3VideoRepackHeader)(buf + offset - sizeof(VC3VideoRepackHeader));
			pVideoRepackHeader->prefix = PREFIX_SINOWAVE;
			pVideoRepackHeader->fixtype = 0x0F;
			pVideoRepackHeader->version = 0x01;
			pVideoRepackHeader->majortype = VC3MT_VIDEO;
			pVideoRepackHeader->VideoType = m_nStreamType;
			pVideoRepackHeader->PacketType = VideoSubType_Repack;
			pVideoRepackHeader->offset = offset;
			pVideoRepackHeader->packcounter = nPackCount;
			pVideoRepackHeader->FrameNoLowByte = frameno;
			
			//重新计算包大小
			int packsize = nNalLenIndex[i];
			if (i == (nPackCount - 1))
			{
				packsize = len - offset;
			}
			if(framelen < packsize)
			{
				packsize = framelen;
			}
			offset += packsize;
			DWORD sendlen = packsize+sizeof(VC3VideoRepackHeader);
			
			//检查是否需要追加VS转发协议头，如果需要，则追加
			char* pbufsend = (char *)pVideoRepackHeader;//PrefixVSTPHeaderIfNeed( (char*)pVideoRepackHeader, &sendlen, ((sockaddr_in*)to)->sin_addr.s_addr );
			
			int nSendLen = 0;
			//	nSendLen = PreDealingSmoothSend(s, pbufsend, sendlen, flags, to, tolen, bKeyFrameFlag, udest, packsize);
			nSendLen = PreDealingFEC(pbufsend, sendlen, flags);
			
			nByteSend += nSendLen;
			framelen -= packsize;
		}
	}
	SAFE_DELETE_ARRAY(nNalLenIndex);
	return nByteSend;
}

int CVideoSender::PreDealingSmoothSend(const char *pbufsend, int sendlen, int flags)
{
//	if (!m_bUseSmooth)
	{
		if (m_lpVideoDataCB != NULL)
		{
			if (NULL == m_pVSTPBuffer)
			{
				m_pVSTPBuffer = new char[MAX_SMOOTH_SEND_LEN];
				memset(m_pVSTPBuffer, 0x00, MAX_SMOOTH_SEND_LEN);
			}
			memcpy(m_pVSTPBuffer + 128, pbufsend, sendlen);

			char *pSendBuf = PrefixVSTPHeaderIfNeed((char *)(m_pVSTPBuffer + 128), sendlen, m_dwLocalIP);
			return m_lpVideoDataCB(pSendBuf, sendlen, m_dwUser);
		}
		else
		{
			return 0;
		}
	}
	/*
	else
	{
		if (NULL == m_pSmoothSndBuf)
		{
			m_pSmoothSndBuf = new char[MAX_SMOOTH_SEND_LEN];
		}

		if (NULL == m_pSmoothSend) 
		{
			int nDelayTime = SMOOTH_DELAYTIME_MIX;
			m_pSmoothSend = new CSmoothSendNew(nBandWidth, nDelayTime, SmoothPacketCallBack, (DWORD)this);
		}
		
		if (sendlen + sizeof(SendSocketInfo) > MAX_SMOOTH_SEND_LEN) 
		{
			return packsize;
		}
		
		pSendSocketInfo pHeader = (pSendSocketInfo)m_pSmoothSndBuf;
		pHeader->dwsock = s;
		pHeader->nflags = flags;
		memcpy(pHeader->SendtoAddr, to, sizeof(*to));
		pHeader->ntolen = tolen;
		pHeader->bKeyFrameFlag = bKeyFrameFlag;
		pHeader->dest = udest;
		
		memcpy(m_pSmoothSndBuf + sizeof(SendSocketInfo), pbufsend, sendlen);
		m_pSmoothSend->PutDataIn(m_pSmoothSndBuf, sendlen + sizeof(SendSocketInfo), sendlen + sizeof(SendSocketInfo), (DWORD)m_pSmoothSend);

		return packsize;
	}*/
}

/*
long __stdcall CVideoSender::SmoothPacketCallBack (char* pSendData, int nSendDatalen, LONG dwUser)
{
	if (NULL == pSendData || nSendDatalen <= sizeof(SendSocketInfo)) 
	{
		return -1;
	}

	CVideoSender& This = *(CVideoSender*)dwUser;

	if (This.m_lpVideoDataCB == NULL)
	{
		return -1;
	}

	pSendSocketInfo pHeader = (pSendSocketInfo)pSendData;

	if (This.m_lpVideoDataCB != NULL)
	{
		return This.m_lpVideoDataCB(pHeader->dwsock, pSendData + sizeof(SendSocketInfo), nSendDatalen - sizeof(SendSocketInfo), pHeader->nflags, 
								(const struct sockaddr FAR *)pHeader->SendtoAddr, pHeader->ntolen, pHeader->bKeyFrameFlag, pHeader->dest, nSendDatalen, This.m_dwUser);
	}

	return 0;
}*/

int CVideoSender::PreDealingFEC(const char *pbufsend, int sendlen, int flags)
{
	if (m_nFECType == -1 && m_bUseFEC)
	{
		m_bUseFEC = FALSE;
	}
	if (!m_bUseFEC)
	{
		return PreDealingJitter(pbufsend, sendlen, flags);
	}
		
	if (m_nLastFECType != m_nFECType)
	{
		SAFE_DELETE(m_pFECSend);
	}
	if (NULL == m_pFECSend) 
	{
		int nFECType = m_nFECType;
		if (m_nFECType >= FECMatrix_MAX)
		{	
			if (m_nFECType < FECMatrix_RS_Base)
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
			m_pFECSend = new CFEC();
		}

		printf("CVideoSender FECType = %d\n", m_nFECType);

		m_pFECSend->InitBuildFEC(FEC_PACKET_PREFIX, nFECType, FECSendCallBack, (LONG)this, VC3FEC_Type_Special);

		m_nLastFECType = m_nFECType;
	}
	
	m_pFECSend->BuildPacket(pbufsend, sendlen, flags);
	
	return sendlen;
}


int __stdcall CVideoSender::FECSendCallBack(const char* pSendData, int nSendDatalen, LONG dwUser)
{
	if (nSendDatalen < 0 || NULL == pSendData || 0 == dwUser)
	{
		return -1;
	}
	
	CVideoSender& This = *(CVideoSender*)dwUser;
	if (This.m_nStreamType != 0)
	{
		//子码流 需要加头以区分原来的码流
		if (NULL == This.m_pFECBuf)
		{
			This.m_pFECBuf = new char[2048];
			memset(This.m_pFECBuf, 0x00, 2048);
		}
		pVC3VideoCommonHeader pHeader = (pVC3VideoCommonHeader)This.m_pFECBuf;
		pHeader->prefix = PREFIX_SINOWAVE;
		pHeader->fixtype = 0x0F;
		pHeader->version = 0x01;
		pHeader->majortype = VC3MT_VIDEO;
		pHeader->VideoType = This.m_nStreamType;
		pHeader->PacketType = VideoSubType_FEC;
		memcpy(This.m_pFECBuf + sizeof(VC3VideoCommonHeader), (char *)pSendData, nSendDatalen);

		pSendData = This.m_pFECBuf;
		nSendDatalen += sizeof(VC3VideoCommonHeader);
	}

	This.PreDealingJitter(pSendData, nSendDatalen, 0);
	
	return 0;
}

//增加防抖动数据包
int CVideoSender::PreDealingJitter(const char *pbufsend, int sendlen, int flags)
{
	const char *pSendtmp = pbufsend;
	int nSendlentmp = sendlen;
	
	if (m_bUseJitter)
	{	
		try
		{
			if (NULL == m_pJitter) 
			{
				m_pJitter = new CJitterRecover();
			}
			pSendtmp = m_pJitter->AddHeader(pbufsend, sendlen, nSendlentmp);
		}
		catch (...)
		{
			printf("PreDealingJitter catch error!");
		}
		if (m_nStreamType != 0)
		{
			//子码流 需要加头以区分原来的码流
			if (NULL == m_pJitterBuf)
			{
				m_pJitterBuf = new char[2048];
				memset(m_pJitterBuf, 0x00, 2048);
			}
			pVC3VideoCommonHeader pHeader = (pVC3VideoCommonHeader)m_pJitterBuf;
			pHeader->prefix = PREFIX_SINOWAVE;
			pHeader->fixtype = 0x0F;
			pHeader->version = 0x01;
			pHeader->majortype = VC3MT_VIDEO;
			pHeader->VideoType = m_nStreamType;
			pHeader->PacketType = VideoSubType_Jitter;
			memcpy(m_pJitterBuf + sizeof(VC3VideoCommonHeader), (char *)pSendtmp, nSendlentmp);
	
			pSendtmp = m_pJitterBuf;
			nSendlentmp += sizeof(VC3VideoCommonHeader);
		}
	}
	
	PreDealingSmoothSend(pSendtmp, nSendlentmp, flags);

	return nSendlentmp;
}

int CVideoSender::AddInfoToFrameX264(const char* pData, int nLen, VIDEOFRAMEHEADER* pHdr)
{
	//有效性检查
	if (pData == NULL || nLen <= 0 || pHdr == NULL)
		return -1;

	const char *pCodeSrc = pData;
	pPacket264 pPacket = (pPacket264)pData;
	int nNalnum = pPacket->dwPacketTotalNum;
	if (nNalnum <= 0) 
	{
		return -2;
	}
//	BOOL bKeyFrame = pHdr->dwFlags > 10? TRUE: FALSE;
	DWORD dwFrameNo = 0;
	char HeaderBuf[256] = {0};
	VIDEOFRAMEHEADER *pSendHeader = (VIDEOFRAMEHEADER *)HeaderBuf;
	*pSendHeader = *pHdr;
	VIDEOFRAMEHEADER &SendHeader = *pSendHeader;
	pVideoFrameHeaderNew pFrameHeaderNew = (pVideoFrameHeaderNew)HeaderBuf;
	VideoFrameHeaderNew &FrameHeaderNew = *pFrameHeaderNew;

//	int status = 0;
	int nFrameSize = 0;
	int sp=GETCODERELATIVEPOS((*pHdr));
	int nStreamType = (pHdr->type&0x0F);
	
	if (m_nStreamType == 0)
	{
		if (1)//SendHeader.FrameNo == 0)
		{
			SendHeader.FrameNo = m_dwPacknum;
		}
//		else //透传广播转移数据
//		{
//			SendHeader.FrameNo = m_dwPacknum + pHdr->FrameNo;
//		}
		dwFrameNo = SendHeader.FrameNo;
	}
	else
	{
		FrameHeaderNew = *(pVideoFrameHeaderNew)pHdr;
		FrameHeaderNew.FrameNo= m_dwPacknum;
		dwFrameNo = FrameHeaderNew.FrameNo;
	}

	if (nLen + 512 > m_nFrameBufLen)
	{
		SAFE_DELETE_ARRAY(m_pFrameBuf);
		m_nFrameBufLen = ((nLen + 512 + VIDEOBUFFERSIZEINIT - 1)/VIDEOBUFFERSIZEINIT)*VIDEOBUFFERSIZEINIT;
	}

	if (NULL == m_pFrameBuf)
	{
		m_pFrameBuf = new char[m_nFrameBufLen];
	}

	//预留视频数据包头信息
	char* pFrameBuf = &m_pFrameBuf[sizeof(VSTP)+sizeof(REPACKHEADEREX)+sp+256];

	//拷贝数据到帧缓冲区
	char *pFrameData = pFrameBuf + sp;
	for (int i = 0; i < nNalnum; i++)
	{
		pPacket = (pPacket264)pCodeSrc;
		memcpy((void *)pFrameData, (char *)pPacket->Payload, pPacket->nPacketSize);
		nFrameSize += pPacket->nPacketSize;
		pFrameData += pPacket->nPacketSize;
		pCodeSrc += sizeof(Packet264);
	}

	if (m_nStreamType == 0)
	{
		SendHeader.CodeSize = nFrameSize;
		SendHeader.ChecksumLowByte = (BYTE)CalcChecksumValueByInt(pFrameBuf + sp, nFrameSize);
		//拷贝帧头
		memcpy((void *)pFrameBuf, (void *)&SendHeader, sp);
	}
	else
	{
		FrameHeaderNew.CodeSize = nFrameSize;
		FrameHeaderNew.ChecksumLowByte = (BYTE)CalcChecksumValueByInt(pFrameBuf + sp, nFrameSize);
		memcpy((void *)pFrameBuf, (void *)&FrameHeaderNew, sp);
	}

	char szFrameCaption[4] = {0};
	FRAMECAPTIONHEADER FrameCaptionHeader;
	//字幕信息加到帧尾部
	memcpy((void *)&pFrameBuf[sp+nFrameSize],&FrameCaptionHeader,sizeof(FRAMECAPTIONHEADER));	//字幕头信息
	int CapLen = min(strlen(szFrameCaption),FRAMECAPTIONLENGTHMAX);
	memcpy((void *)&pFrameBuf[sp+nFrameSize+sizeof(FRAMECAPTIONHEADER)], szFrameCaption, CapLen);	//字幕信息附加到结尾
	pFrameBuf[sp+nFrameSize+sizeof(FRAMECAPTIONHEADER)+CapLen] = 0;
	nFrameSize+=sp+sizeof(FRAMECAPTIONHEADER)+(CapLen+1);

	try
	{
		RepackSendToEx(pData, pFrameBuf, nFrameSize, 0, (BYTE)dwFrameNo, nStreamType);				
	}
	catch (...) 
	{
		printf("CVideoSender::Repackx264Frame catch Exception!\n");
	}

	return 0;
}

char* PrefixVSTPHeaderIfNeed( char* pBuf, int &nLen, unsigned int dwLocalIP)
{
	if (NULL == pBuf || nLen <= 0 || nLen >= MAX_SMOOTH_SEND_LEN)
	{
		return NULL;
	}

	if (memcmp(pBuf, "VSTP", 4) != 0) 
	{
		VSTP *p = (VSTP *)(pBuf - sizeof(VSTP));
		memcpy( p->szFlag, "VSTP", 4 );
		p->uSourceIP = dwLocalIP;
		nLen += sizeof(VSTP);
		return (char *)p;
	}
	else
	{
		return pBuf;
	}
}

//按整数(int)计算校验和
int CalcChecksumValueByInt( const char* buf, int len )
{
	int sum = 0;
	int* p = (int*)buf;
	int* end = (int*)(buf+len);
	while( p < end )
		sum += *p++;
	return sum;
}