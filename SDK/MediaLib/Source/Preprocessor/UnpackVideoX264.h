// UnpackVideoX264.h: interface for the CUnpackVideoX264 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNPACKVIDEOX264_H__03172DFB_F3E6_4211_8F49_C7D8A410C247__INCLUDED_)
#define AFX_UNPACKVIDEOX264_H__03172DFB_F3E6_4211_8F49_C7D8A410C247__INCLUDED_

#include "h264define.h"

class CUnpackVideoX264  
{
public:
	int UnpackToFrame(char *pPack, int nPackSize, char *pFrameBuf, int &nFrameType, DWORD &dwFrameNo );
public:
	CUnpackVideoX264();
	virtual ~CUnpackVideoX264();
private:
	int m_nFrameLen;
	DWORD m_dwFrameNum;
	DWORD m_dwCount;
	DWORD m_dwPacketTotalNum;
	DWORD m_dwPacketNum;
};

#endif // !defined(AFX_UNPACKVIDEOX264_H__03172DFB_F3E6_4211_8F49_C7D8A410C247__INCLUDED_)
