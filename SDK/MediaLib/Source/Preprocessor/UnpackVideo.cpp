// Unpack_SVIDEO.cpp: implementation of the CUnpackVideo class.
//
//////////////////////////////////////////////////////////////////////
#include "UnpackVideo.h"
#include "CaptionDef.h"

const int JitterPacketNum = 10;
extern int CalcChecksumValueByInt( const char* buf, int len );
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnpackVideo::CUnpackVideo(int nChannel)
:m_pJitter(NULL),
m_pFECRecive(NULL),
m_pFrameCallBack(NULL),
RcvBufHdr(NULL),
RcvBufDat(NULL),
m_dwOldFrameNo(0),
m_dwRemoteIP(0),
m_nFECVersion(0),
m_nChannel(nChannel)
{
	m_pX264FrameBuf = NULL;
	m_pUnpackVideoX264 = NULL;
	m_nMatrixType = -1;
	m_nRcvBufDatLen = VIDEOBUFFERSIZEINIT;

	RcvBufHdr = new char[32];
	memset(RcvBufHdr, 0x00, 32);
	
	RcvBufTmp = new char[2048];
	memset(RcvBufTmp, 0x00, 2048);

	m_dwUser = 0;
	RcvBufDat = NULL;// new char[VIDEOBUFFERSIZEMAX];
	m_pOptimizex264Buf = NULL;
	memset(&UnpackStatus, 0, sizeof(UnpackStatus));
}

CUnpackVideo::~CUnpackVideo()
{
	SAFE_DELETE_ARRAY(RcvBufDat);
	SAFE_DELETE_ARRAY(RcvBufHdr);
	SAFE_DELETE_ARRAY(RcvBufTmp);
	SAFE_DELETE_ARRAY(m_pOptimizex264Buf);
	SAFE_DELETE(m_pJitter);
	SAFE_DELETE(m_pFECRecive);
	SAFE_DELETE(m_pUnpackVideoX264);
	SAFE_DELETE_ARRAY(m_pX264FrameBuf);
}


//设置帧数据回调输出函数
int CUnpackVideo::SetFrameDataCallBack(LPFRAMEDATACALLBACK pFrameDataCallBack, LONG dwUser)
{
	m_pFrameCallBack = pFrameDataCallBack;
	m_dwUser = dwUser;
	return 0;
}

//抖动数据回调函数
int __stdcall CUnpackVideo::JitterCallBack(const char* pSendData, int nSendDatalen, LONG dwUser, DWORD dwflag)
{
	CUnpackVideo* pThis = (CUnpackVideo*)dwUser;

	if (pThis != NULL)
	{
		return pThis->PreDealFECPacket((char*)pSendData, nSendDatalen, dwflag);
	}

	return -1;
}

//FEC数据回调函数
int __stdcall CUnpackVideo::FECReceiveCallBack(const char* pRecvData, int nRecvDatalen, LONG dwUser, int nChannel, DWORD flag1, DWORD flag2)
{
	CUnpackVideo* pThis = (CUnpackVideo*)dwUser;

	if (pThis != NULL)
	{
		return pThis->PreDealRawVideoData((char*)pRecvData, nRecvDatalen, flag1, TRUE);
	}

	return -1;
}

//输入数据
int CUnpackVideo::InputData(char* pData, int nLength, DWORD addr_remote)
{
	if (NULL == pData || nLength <= 0)
	{
		return -1;
	}

	DWORD remote = addr_remote;
	//立刻查看是否有VS转发协议头，如果有，则需要优先用协议头中源地址，并且跳过该头
	if(memcmp(pData,"VSTP",4) == 0)
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
	if (nType != 0x01 && nType != 0x07 && nType != 0x09 && nType != 0x0A && nType != 0x0F)
	{
		return -3;
	}

	if (m_dwRemoteIP != remote)
	{
		m_dwRemoteIP = remote;
	}

	//0x0A - Jitter结构
	if ((pData[1] & 0x0F) == 0x0A)
	{
		if (NULL == m_pJitter) 
		{
			m_pJitter = new CJitterRecover;
			m_pJitter->InitRecover(JitterCallBack, (LONG)this, JitterPacketNum, SWJitterRecverType_Syn);
		}
		
		m_pJitter->DealJitterDataEx(pData, nLength, remote);
		
		return 0;
	}

	return PreDealFECPacket(pData, nLength, remote);
}

//FEC预处理
int CUnpackVideo::PreDealFECPacket( char* pPack, int nLen, DWORD addr_remote)
{
	//ZDS FEC结构(音视频通用)
	if ((pPack[1] & 0x0F) == 0x09)
	{
		pVC3FECPacketHeaderEx pFECHeader = (pVC3FECPacketHeaderEx)pPack;
		if (m_nFECVersion != pFECHeader->subtype.version)
		{
			SAFE_DELETE(m_pFECRecive);
		}
		if (NULL == m_pFECRecive) 
		{
			if (SWFECVersion_Nor == pFECHeader->subtype.version)
			{
				m_pFECRecive = new CFEC();
			}
			else if (SWFECVersion_RS == pFECHeader->subtype.version)
			{
				m_pFECRecive = new CVC3RSFEC();
			}
			else
			{
				m_pFECRecive = new CVC3RSFECEx();
			}
			
		//	TRACE("CUnpackVideo Channel = %d, FECType = %d\n", m_nChannel, pFECHeader->subtype.version);

			m_pFECRecive->InitResume(FECReceiveCallBack, NULL, (LONG)this);
			m_pFECRecive->SetChannel(m_nChannel);
			m_nFECVersion = pFECHeader->subtype.version;
		}
		m_nMatrixType = pFECHeader->GroupParam;
		if (m_nFECVersion == SWFECVersion_RS)
		{
			m_nMatrixType += FECMatrix_RS_Base;
		}

		DWORD flag = 0;//(nThreadID<<4)&0x000000f0;
		
		m_pFECRecive->ResumePacket(pPack, nLen, addr_remote, flag);

		return 0;
	}
	m_nMatrixType = -1;

	return PreDealRawVideoData(pPack, nLen, addr_remote, FALSE);
}

int CUnpackVideo::PreDealRawVideoData(char* pPack, int nLen, DWORD addr_remote, BOOL bUseFEC)
{
	if (NULL == pPack || nLen <=0)
	{
		return 0;
	}
	//预留128个字节以防止内存写覆盖(以后可以在Jitter、FEC等输出数据头上预留一定长度内存)
	memcpy(RcvBufTmp+128, pPack, nLen);

	char* pRawData = RcvBufTmp+128;
	int   nRawLen  = nLen;

	//检查是否还有VS转发协议头(做FEC时，加了两次VSTP头)
	if( memcmp(pRawData,"VSTP",4) == 0 )
	{
//		VSTP* p = (VSTP*)pRawData;
		pRawData += sizeof(VSTP);
		nRawLen	 -= sizeof(VSTP);
	}

	if (NULL == RcvBufDat)
	{
		RcvBufDat = new char[m_nRcvBufDatLen];
	}

	int nFrameLen = 0;
	//组包
	char* RcvBuf = PreUnpackToFrame(RcvBufHdr, pRawData, nRawLen, nFrameLen);

	//未获得一帧数据
	if ( RcvBuf == NULL )
		return -1;

	return CallBackFrame(RcvBuf, nFrameLen, addr_remote);
}

int CUnpackVideo::CallBackFrame( char *RcvBuf, int nFrameLen, DWORD addr_remote, BOOL bLostPacket )
{
	VIDEOFRAMEHEADER &header = *(VIDEOFRAMEHEADER *)RcvBuf;
//	CopyMemory((LPVOID)&header, (LPVOID)RcvBuf, sizeof(header));
	int sp=GETCODERELATIVEPOS(header);
	int CodeSize = header.CodeSize;
	if (CodeSize <= 0 || sp <= 0 || header.prefix != PREFIX_SINOWAVE)
	{
		return -1;
	}

	//效验数据的有效性
	if (!bLostPacket && header.ChecksumLowByte != (BYTE)CalcChecksumValueByInt(RcvBuf+sp,CodeSize))
	{
	//	TRACE("Stream checksum error, skip frame CodeSize=%d\n", CodeSize);
		return -1;
	}

	if (m_dwOldFrameNo+1 != header.FrameNo)
	{
	//	TRACE ("[CUnpackVideo]: %x lost from %d to %d \n", addr_remote, m_dwOldFrameNo, header.FrameNo);
	}

	m_dwOldFrameNo = header.FrameNo;

	//输出视频帧数据
	if (m_pFrameCallBack != NULL)
	{
		int nHeader2Len = 0;
		//提取字幕信息
		FRAMECAPTIONHEADER *pFrameCaptionHeader = (FRAMECAPTIONHEADER*)&RcvBuf[sp+CodeSize];
		char *szCaption = (char*)pFrameCaptionHeader + sizeof(FRAMECAPTIONHEADER);
	//	szCaption[FRAMECAPTIONLENGTHMAX] = 0;

		int nCapSize = min(strlen(szCaption), FRAMECAPTIONLENGTHMAX);
		int nOutSize = CodeSize+sp+sizeof(FRAMECAPTIONHEADER)+nCapSize+1;

		if (pFrameCaptionHeader->HeaderEx.Reserved == 0x01) //有扩展结构
		{
			pFrameCaptionHeaderEx2 pHeaderEx2 = (pFrameCaptionHeaderEx2)&RcvBuf[nOutSize];
			nOutSize += pHeaderEx2->wSize;
			nHeader2Len = pHeaderEx2->wSize;
		}

		if (nOutSize != nFrameLen)
		{
		//	TRACE("CUnpackVideo::PreDealRawVideoData nOutSize = %d, nFrameLen = %d, addr_remote = %d\n", nOutSize, nFrameLen, addr_remote);
		}
		char *pFrame = RcvBuf + sp;
		int nFrameLength = header.CodeSize;

		BOOL bUseX264 = FALSE;
		if (header.flagFOURCC == CODEC_H264 && (SWH264NALU_HEADER != *(DWORD *)(RcvBuf+sp)))
		{
			bUseX264 = TRUE;
		}
		if (bUseX264)
		{
			if (1)
			{
				//拼成整帧
				if (NULL == m_pUnpackVideoX264)
				{
					m_pUnpackVideoX264 = new CUnpackVideoX264();
				}
				if (NULL == m_pX264FrameBuf)
				{
					m_pX264FrameBuf = new char[256*1024];
				}
				int nFrameType = 0;
				DWORD dwFrameNo = 0;
				int nX264FrameLen = m_pUnpackVideoX264->UnpackToFrame(RcvBuf+sp, header.CodeSize, m_pX264FrameBuf + sp, nFrameType, dwFrameNo);
				if (nX264FrameLen <= 0)
				{
					return 0;
				}
				VIDEOFRAMEHEADER &FrameHeadr = *(VIDEOFRAMEHEADER *)m_pX264FrameBuf;
				FrameHeadr = header;
				FrameHeadr.FrameNo = dwFrameNo;
				char *pCopyPositon = m_pX264FrameBuf + sp + nX264FrameLen;
				int nCopyLen = sizeof(FRAMECAPTIONHEADER) + nCapSize + 1 + nHeader2Len;
				memcpy(pCopyPositon, RcvBuf + sp + header.CodeSize, nCopyLen);
//				int nOutSize = nX264FrameLen + sp + nCopyLen;

				pFrame = m_pX264FrameBuf + sp;
				nFrameLength = nX264FrameLen;
			}
			else
			{
				//nalu
				pFrame += Packet264::GetHeadSize();
				nFrameLength -= Packet264::GetHeadSize();
			}
		}
		m_pFrameCallBack(pFrame, nFrameLength, &header, pFrameCaptionHeader, szCaption, addr_remote, m_dwUser);
	}

	return 0;
}

//组包预处理函数(把REPACKHEADER头转换成REPACKHEADEREX头结构，从而使UnpackToFrame()只处理REPACKHEADEREX这种情况)
char* CUnpackVideo::PreUnpackToFrame(char* RecvPackHeader, char* pPack, int nPackSize, int &nFrameLen)
{
	nFrameLen = 0;

	int nPackType = REPACK_HEADER;
	char packettype = pPack[1];
	if ((packettype & 0x0F) == 0x01) 
	{
		nPackType = REPACK_HEADER;
	}
	else if ((packettype & 0x0F) == 0x07) 
	{
		nPackType = REPACK_HEADER_X;
	}
	else
	{
		return NULL;
	}
	
	if( nPackType == REPACK_HEADER_X )
	{
		return UnpackToFrame( RecvPackHeader, pPack, nPackSize, nFrameLen );
	}
	
	//此处在pPack起始地址之前块内存，在循环缓冲区已经预留出来
	if( nPackType == REPACK_HEADER )
	{
		int nDiffLen = sizeof(REPACKHEADEREX) - sizeof(REPACKHEADER); //表示两个结构长度之差
		REPACKHEADER ppht;
		REPACKHEADER* pph = (REPACKHEADER*)pPack;
		ppht = *pph;
		REPACKHEADEREX* pphx = (REPACKHEADEREX*)(pPack-nDiffLen);
		pphx->prefix = ppht.prefix;
		pphx->type = ppht.type;
		pphx->offset = ppht.offset;
		pphx->packcounter = ppht.packcounter;
		pphx->FrameNoLowByte = ppht.FrameNoLowByte;
		
		return UnpackToFrame( RecvPackHeader, (char*)pphx, nPackSize+nDiffLen, nFrameLen );
	}

	return NULL;
}

//组帧
char* CUnpackVideo::UnpackToFrame(char* RecvPackHeader, char* pPack, int nPackSize, int &nFrameLen)
{
	//准备解包
	UNPACKSTATUS* pUnpackStatus = &UnpackStatus;	//解包状态
	REPACKHEADEREX* pph = (REPACKHEADEREX*)pPack;	//接收数据包头
	REPACKHEADEREX* pRecvHeaderBuf = (REPACKHEADEREX*)RecvPackHeader;	//缓存区包头
	//防止溢出
	if ( sizeof(REPACKHEADEREX) + pph->offset + nPackSize > VIDEOBUFFERSIZEMAX )
	{
		return NULL;
	}

	if (sizeof(REPACKHEADEREX) + pph->offset + nPackSize > m_nRcvBufDatLen)
	{
		int nVideoBufLen = m_nRcvBufDatLen;
		m_nRcvBufDatLen = ((sizeof(REPACKHEADEREX) + pph->offset + nPackSize + VIDEOBUFFERSIZEINIT - 1)/VIDEOBUFFERSIZEINIT)*VIDEOBUFFERSIZEINIT;
	//	TRACE("CUnpackVideo m_nRcvBufDatLen = %d, nVideoBufLen = %d\n", m_nRcvBufDatLen, nVideoBufLen);
		char *pTemp = new char[m_nRcvBufDatLen];
		if (NULL == pTemp)
		{
			m_nRcvBufDatLen = nVideoBufLen;
			return NULL;
		}
		memcpy(pTemp, RcvBufDat, nVideoBufLen);
		SAFE_DELETE_ARRAY(RcvBufDat);
		RcvBufDat = pTemp;
		SAFE_DELETE_ARRAY(m_pOptimizex264Buf);
	}

	//偏移256字节，防止写覆盖
	char* RecvPackData = RcvBufDat + 256;

	BYTE* ppd = (BYTE*)(pPack + sizeof(REPACKHEADEREX));	//接收到的实际数据
	char* pRecvDataBuf = RecvPackData + sizeof(REPACKHEADEREX);	//第一个包数据在缓存区的开始位置

	//丢包
	if (pRecvHeaderBuf->packcounter != 0 && pph->FrameNoLowByte != pRecvHeaderBuf->FrameNoLowByte)
	{
		if (pUnpackStatus->IsFirstPacketRecved())
		{
			VIDEOFRAMEHEADER &header = *(VIDEOFRAMEHEADER *)pRecvDataBuf;
			int sp = GETCODERELATIVEPOS(header);
			//优化版本小包264
			if (header.timestamp.VideoTimeStamp.bOptimizeNalu && (header.prefix == PREFIX_SINOWAVE) 
				&& sp >=86 && header.flagFOURCC == CODEC_H264)
			{
				if (NULL == m_pOptimizex264Buf)
				{
					m_pOptimizex264Buf = new char[m_nRcvBufDatLen];
				}
				int nFrameLen = 0;
				char *pCallBackBuf = m_pOptimizex264Buf + 256 + sizeof(REPACKHEADEREX);
				DWORD dwMaxOffset = 0;
				int nMaxOffsetCount = pUnpackStatus->FindMaxOffset(dwMaxOffset);
				for (int j = 0; j < pUnpackStatus->count; j++)
				{
					if (j != nMaxOffsetCount)
					{
						if (pUnpackStatus->status[j].offset != 0)
						{
							if (SWH264NALU_HEADER != *(DWORD *)(pRecvDataBuf + pUnpackStatus->status[j].offset))
							{
							//	TRACE("UnpackToFrame error, IP = %d, header = %d\n", m_dwRemoteIP, *(DWORD *)(pRecvDataBuf + pUnpackStatus->status[j].offset));
								continue;
							}
						}
						memcpy(pCallBackBuf + nFrameLen, pRecvDataBuf + pUnpackStatus->status[j].offset, pUnpackStatus->status[j].len);
						nFrameLen += pUnpackStatus->status[j].len;
					}
				}

				memcpy(pCallBackBuf + nFrameLen, pRecvDataBuf + dwMaxOffset, pUnpackStatus->status[nMaxOffsetCount].len);
				nFrameLen += pUnpackStatus->status[nMaxOffsetCount].len;
				int nCodecSize = nFrameLen - sp;
			//	if (SWH264NALU_HEADER == *(DWORD *)(pRecvDataBuf + dwMaxOffset))
				if (dwMaxOffset + pUnpackStatus->status[nMaxOffsetCount].len > header.CodeSize + sp)
				{
				//	nCodecSize -= pUnpackStatus->status[nMaxOffsetCount].len;
					nCodecSize -= (dwMaxOffset + pUnpackStatus->status[nMaxOffsetCount].len - (header.CodeSize + sp));
				}
				else
				{	
					//没有收到字幕
					memset(pCallBackBuf + nFrameLen, 0x00, 7);
					nFrameLen += 7;	
				}
				if (nCodecSize > 0 && nCodecSize <= header.CodeSize)
				{
					VIDEOFRAMEHEADER &header = *(VIDEOFRAMEHEADER *)pCallBackBuf;
					header.CodeSize = nCodecSize;
					CallBackFrame(pCallBackBuf, nFrameLen, m_dwRemoteIP, TRUE);
				}
			}
		}
		else
		{
		//	TRACE("CUnpackVideo::UnpackToFrame FirstPacket not Recv m_dwRemoteIP = %d\n", m_dwRemoteIP);
		}
	}

	//解包
	if ( pph->offset == 0 )
	{
		//收到的包为FEC纠错包（(pph->type & 0xF0) == 0x10表示该包为FEC纠错包）
		if ( (pph->type & 0xF0) == 0x10 )
		{
			return NULL;
		}

		//如果是帧的第一包,则拷贝包头
		memcpy( pRecvHeaderBuf, pph, sizeof(REPACKHEADEREX) );
		
		//拷贝数据到适当位置
		memcpy( pRecvDataBuf+pph->offset, ppd, nPackSize-sizeof(REPACKHEADEREX) );

		//设置解包状态表
		memset( pUnpackStatus, 0, sizeof(UNPACKSTATUS) );
		pUnpackStatus->status[pUnpackStatus->count].offset = pph->offset;
		pUnpackStatus->status[pUnpackStatus->count].len = nPackSize-sizeof(REPACKHEADEREX);
		pUnpackStatus->count++;

		//减少未完成包计数器
		pRecvHeaderBuf->packcounter --;
	}
	else
	{
		//如果帧的头包没到而后续的包先到了（码流仍然连续是的），则暂时用后续的包头代替头包包头
		if ( pph->FrameNoLowByte != pRecvHeaderBuf->FrameNoLowByte )
		{
			memcpy( pRecvHeaderBuf, pph, sizeof(REPACKHEADEREX) );
			//设置解包状态表
			memset( pUnpackStatus, 0, sizeof(UNPACKSTATUS) );
		}

		//检查是否是重复数据
		BOOL bHaveRcved = FALSE;
		for (int i = 0; i < pUnpackStatus->count; i++)
		{
			if (pph->offset == pUnpackStatus->status[i].offset)
			{
				bHaveRcved = TRUE;
			}
		}

		if (!bHaveRcved)
		{
			//拷贝数据到适当位置
			memcpy( pRecvDataBuf+pph->offset, ppd, nPackSize-sizeof(REPACKHEADEREX) );
			
			//设置解包状态表
			if( pUnpackStatus->count < sizeof(pUnpackStatus->status)/sizeof(pUnpackStatus->status[0]) )
			{
				pUnpackStatus->status[pUnpackStatus->count].offset = pph->offset;
				pUnpackStatus->status[pUnpackStatus->count].len = nPackSize-sizeof(REPACKHEADEREX);
				pUnpackStatus->count ++;
			}
			else
			{
			//	TRACE("CUnpackVideo::UnpackToFrame pUnpackStatus->count = %d\n", pUnpackStatus->count);
				//设置解包状态表
				memset( pUnpackStatus, 0, sizeof(UNPACKSTATUS) );
				return NULL;
			}
			
			//减少未完成包计数器
			pRecvHeaderBuf->packcounter --;
		}
		else
		{
			return NULL;
		}
	}
	//判断是否合成完毕
	if ( pRecvHeaderBuf->packcounter == 0 )
	{
		for (int i = 0; i < pUnpackStatus->count; i++)
		{
			nFrameLen += pUnpackStatus->status[i].len;
		}

		return pRecvDataBuf;
	}
	else
		return NULL;
}
