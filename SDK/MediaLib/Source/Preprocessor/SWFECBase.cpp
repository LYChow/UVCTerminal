// SWFECBase.cpp: implementation of the CSWFECBase class.
//
//////////////////////////////////////////////////////////////////////

#include "SWFECBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSWFECBase::CSWFECBase()
{
	m_nChannel = 0;

	m_nChannel = 0;
	
	m_nEFCDataType = 0;
	m_nPacketType = 0;
	m_dwSendTick = 0;
	m_dwBuidPacketNo = 0;
	m_dwBuidGroupNo = 0;
	m_nBuidGroupIntraNo = 0;
	m_dwBuildUser = 0;
	m_nBuidEvent = 0;
	m_BuildCallBack = NULL;
	m_dwBuildFlag = 0;
	
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
	m_ResumeCallBack = NULL;
	m_FrameNoCallBack = NULL;
}

CSWFECBase::~CSWFECBase()
{

}

int CSWFECBase::GetRealPosition(const VC3FECPacketHeader &PacketHeader)
{
	return (char *)&PacketHeader.CodeOffset - (char *)&PacketHeader + sizeof(PacketHeader.CodeOffset) + PacketHeader.CodeOffset;
}

int CSWFECBase::GetRealTagSize(const VC3FECPacketHeader &PacketHeader)
{
	return (char *)&PacketHeader.tagOffset - (char *)&PacketHeader + sizeof(PacketHeader.tagOffset) + PacketHeader.tagOffset;
}

int CSWFECBase::GetCodeOffset(const VC3FECPacketHeader &PacketHeader)
{
	return sizeof(PacketHeader.tagOffset);
}

int CSWFECBase::GetRealPosition(const VC3FECPacketHeaderEx &PacketHeader)
{
	return (char *)&PacketHeader.CodeOffset - (char *)&PacketHeader + sizeof(PacketHeader.CodeOffset) + PacketHeader.CodeOffset;
}

int CSWFECBase::GetRealTagSize(const VC3FECPacketHeaderEx &PacketHeader)
{
	return (char *)&PacketHeader.tagOffset - (char *)&PacketHeader + sizeof(PacketHeader.tagOffset) + PacketHeader.tagOffset;
}

int CSWFECBase::GetCodeOffset(const VC3FECPacketHeaderEx &PacketHeader)
{
	return sizeof(PacketHeader.tagOffset);
}
