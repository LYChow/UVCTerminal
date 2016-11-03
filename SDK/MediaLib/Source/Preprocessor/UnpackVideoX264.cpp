// UnpackVideoX264.cpp: implementation of the CUnpackVideoX264 class.
//
//////////////////////////////////////////////////////////////////////

#include "UnpackVideoX264.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnpackVideoX264::CUnpackVideoX264()
{
	m_nFrameLen = 0;
	m_dwFrameNum = 0;
	m_dwCount = 0;
	m_dwPacketTotalNum = 0;
	m_dwPacketNum = 0;
}

CUnpackVideoX264::~CUnpackVideoX264()
{

}

int CUnpackVideoX264::UnpackToFrame( char *pPack, int nPackSize, char *pFrameBuf, int &nFrameType, DWORD &dwFrameNo )
{
	if (NULL == pPack || nPackSize <= Packet264::GetHeadSize() || pFrameBuf == NULL)
	{
		return 0;
	}
	
	pPacket264 ptemPacket264 = (pPacket264)pPack;

	//有丢包
	if (m_dwFrameNum != ptemPacket264->dwFrameNum) 
	{
	//	TRACE("C264Decoder dwFrameNum = %d,ptemPacket264->dwFrameNum =%d,nCount =%d,ptemPacket264->Packettotal = %d\n", m_dwFrameNum, ptemPacket264->dwFrameNum, m_dwCount, ptemPacket264->dwPacketTotalNum);
		if (m_dwCount != 0)
		{
	//		TRACE("CUnpackVideoX264::UnpackToFrame Lost packet dwFrameNum = %d,ptemPacket264->dwFrameNum =%d,nCount =%d,ptemPacket264->Packettotal = %d\n", 
	//				m_dwFrameNum, ptemPacket264->dwFrameNum, m_dwCount, ptemPacket264->dwPacketTotalNum);
		}
		m_dwCount = 0;
		m_nFrameLen = 0;
		m_dwFrameNum = ptemPacket264->dwFrameNum;
	}

	if (m_dwCount != ptemPacket264->dwPacketNum)
	{
		m_dwCount = 0;
		m_nFrameLen = 0;
		return 0;
	}

	if (ptemPacket264->nPacketSize < 0 || m_nFrameLen + ptemPacket264->nPacketSize > H264_DECODER_BUFLEN) 
	{
		m_dwCount = 0;
		m_nFrameLen = 0;
		return 0;
	}

	m_dwPacketTotalNum = ptemPacket264->dwPacketTotalNum;
	memcpy(pFrameBuf + m_nFrameLen, ptemPacket264->Payload, ptemPacket264->nPacketSize);
	m_nFrameLen += ptemPacket264->nPacketSize;
	m_dwCount++;

	if (m_dwCount == ptemPacket264->dwPacketTotalNum) 
	{
		int nLen = m_nFrameLen;
		m_dwCount = 0;
		m_nFrameLen = 0;
		dwFrameNo = m_dwFrameNum;
		nFrameType = H264_TYPE_PVOP;
		if (ptemPacket264->dwNalutype >= 5)
		{
			nFrameType = H264_TYPE_IVOP;
		}

		return nLen;
	}
	else
	{
		return 0;
	}
}
