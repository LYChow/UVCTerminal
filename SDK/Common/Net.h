#ifndef __NET__H__
#define __NET__H__

#define _SOCKREF int
#ifdef __ENV_WIN
#include <afxsock.h>

struct FDSET:public fd_set
{
	FDSET()
	{
		Clear();
	}
	void Clear()
	{
		//ZeroMemory(this,sizeof(*this));
		memset(this,0xff,sizeof(*this));
		fd_count=0;
	}
	int Add(_SOCKREF fd)
	{
		if(fd==INVALID_SOCKET) return -1;
		int i=this->fd_count;
		this->fd_array[i]=fd;
		this->fd_count++;
		return i;
	}
	int Set(_SOCKREF fd)
	{
		if(fd==INVALID_SOCKET) return -1;
		int cnt=this->fd_count;
		int k=-1;
		for (int i=0;i<cnt-1;i++)
		{
			if(this->fd_array[i]==fd){k=i; break;}
		}
		if (k<0)
		{
			this->fd_array[cnt]=fd;
			k=cnt;
			this->fd_count++;
		}

		return k;

	}
	void Remove(_SOCKREF fd)
	{
		int cnt=this->fd_count;
		int k=-1;
		for (int i=0;i<cnt-1;i++)
		{
			if(this->fd_array[i]==fd){k=i; break;}
		}
		if (k>=0)
		{
			if(cnt-k-1) memmove(&this->fd_array[k],&this->fd_array[k+1],(cnt-k-1)*sizeof(fd));
			this->fd_count--;
		}
	}

};


#else

#include <sys/socket.h>
#import <arpa/inet.h>
#import <fcntl.h>
#import <ifaddrs.h>
#import <netdb.h>
#import <netinet/in.h>
#import <net/if.h>
#import <sys/socket.h>
#import <sys/types.h>
#import <sys/ioctl.h>
#import <sys/poll.h>
#import <sys/uio.h>
#import <sys/un.h>
#import <unistd.h>
#import <string.h>
#define closesocket close


#define INVALID_SOCKET -1
struct FDSET:public fd_set
{
	FDSET()
	{
		Clear();
	}
	void Clear()
	{
		//ZeroMemory(this,sizeof(*this));
		memset(this,0,sizeof(*this));
	}
	int Add(_SOCKREF fd)
	{
		if(fd==INVALID_SOCKET) return -1;
		FD_SET(fd,this);
		return fd;
	}
	int Set(_SOCKREF fd)
	{
		return Add(fd);

	}
	void Remove(_SOCKREF fd)
	{
		FD_CLR(fd,this);
	}

};


#endif //end env


LPCTSTR _INetAddrToStr(DWORD dwAddr);
DWORD _INetStrToAddr(LPCTSTR szAddr);

struct SocketIpAddr
{
	union
	{
		DWORD dwIpAddr;
		struct
		{
			BYTE b1;BYTE b2;BYTE b3;BYTE b4;
		};  
	};
	SocketIpAddr(const SocketIpAddr& addr)
	{
		dwIpAddr=addr.dwIpAddr;
	}
	SocketIpAddr(LPCTSTR szAddr=0)
	{
		dwIpAddr=_INetStrToAddr(szAddr);
	}
	SocketIpAddr(BYTE b1,BYTE b2,BYTE b3,BYTE b4)
	{
		FromBytes(b1,b2,b3,b4);
	}
	DWORD FromBytes(BYTE b1,BYTE b2,BYTE b3,BYTE b4)
	{
		this->b1=b1;this->b2=b2;this->b3=b3;this->b4=b4;
		return dwIpAddr;
	}
	DWORD FromStr(LPCTSTR szAddr)
	{
		dwIpAddr=_INetStrToAddr(szAddr);
		return dwIpAddr;
	} 
	LPCTSTR ToStr()
	{
		return _INetAddrToStr(dwIpAddr);
	}
	operator DWORD(){return dwIpAddr;}
	SocketIpAddr& operator=(DWORD dwAddr){dwIpAddr=dwAddr;return *this;}
	operator LPCTSTR(){return ToStr();}
	bool operator==(LPCTSTR szIpAddr)
	{
		return _INetStrToAddr(szIpAddr)==dwIpAddr;
	}
	bool operator!=(LPCTSTR szIpAddr)
	{
		return _INetStrToAddr(szIpAddr)!=dwIpAddr;
	}
	bool operator==(DWORD dwAddr)
	{
		return dwIpAddr==dwAddr;
	}
	UINT FromSocketGetLocalPort(int s);
	UINT FromSocketGetPeerPort(int s);
	sockaddr* MakeSockAddr(UINT port)
	{
		static sockaddr_in addr;
		addr.sin_family=AF_INET;
		addr.sin_port=htons(port);
		addr.sin_addr.s_addr=dwIpAddr;
		return (sockaddr*)&addr;
	}
};




#endif
