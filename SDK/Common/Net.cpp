#include "__PCH.h"
#include "Net.h"

LPCTSTR _INetAddrToStr(DWORD dwAddr)
{
	in_addr* p=(in_addr*)&dwAddr;
	return inet_ntoa(*p);
}
DWORD _INetStrToAddr(LPCTSTR szAddr)
{
	if(szAddr==0 || *szAddr==0) return 0;
	return (DWORD)inet_addr(szAddr);
}

UINT SocketIpAddr::FromSocketGetLocalPort(int s)
{
	sockaddr_in addr;
	ZeroVar(addr);
	UINT len=sizeof(addr);
	int ret=getsockname(s,(struct sockaddr*)&addr,(socklen_t*)&len);
	dwIpAddr=-1;
	if(ret)
	{
		return 0;
	}
	dwIpAddr=addr.sin_addr.s_addr;
	return ntohs(addr.sin_port);

}
UINT SocketIpAddr::FromSocketGetPeerPort(int s)
{
	sockaddr_in addr;
	ZeroVar(addr);
#ifndef __ENV_WIN
	UINT len=sizeof(addr);
#else
	int len=sizeof(addr);
#endif
	int ret=getpeername(s,(sockaddr*)&addr,&len);
	dwIpAddr=0;
	if(ret)
	{
		return 0;
	}
	dwIpAddr=addr.sin_addr.s_addr;
	return ntohs(addr.sin_port);

}
