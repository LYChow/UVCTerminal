#include "__PCH.h"
#include "debug.h"
#include "NetLib.h"
#include "Thread.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef __ENV_WIN
struct WinSockLoad
{
	WinSockLoad()
	{
		AfxSocketInit(0);
	}
	~WinSockLoad()
	{
		AfxSocketTerm();
	}
};

static WinSockLoad _SockLoader;

#endif
struct SockEventChecker:public MsgDispatcher
{
	MsgDispatcher* pEventHandler;
	SockEventChecker(int fd=-1,MsgDispatcher* pHandler=0)
	{
		this->fd=fd;pEventHandler=pHandler;bChecking=0;
		rwFlag=0;
		IMsgQueue* p=IMsgQueue::Create();
		AttachToQue(p);
	}
	~SockEventChecker()
	{
		DetachQue()->Release();
	}
	int fd;
	int rwFlag;
	bool bChecking;
	bool CanWrite()
	{
		return (rwFlag&2)!=0;
	}
	bool CanRead()
	{
		return (rwFlag&1)!=0;
	}
	void WaitReadEvent()
	{
		rwFlag&=~1;
		if(bChecking==0)
		{
			PostMsg(1,0);
		}
	}
	void WaitWriteEvent()
	{
		rwFlag&=~2;
		if(bChecking==0)
		{
			PostMsg(1,0);
		}
	}

	int StartCheck(int fd,int evFlag,MsgDispatcher* pHandler=0)
	{
		this->fd=fd;pEventHandler=pHandler;
		rwFlag=(~evFlag)&3;
		PostMsg(1,0);
		return 1;
	}
		
	void StopCheck()
	{
		rwFlag=-1;
		while (bChecking)
		{
			Sleep(100);
		}
		fd=-1;
		TRACE("EvCheck Stoped\n");
	}
	int OnMsgSourceMessage(DWORD dwSourceId,UINT msg,void* pMsg)
	{
		if (msg==1 )
		{
			if(fd!=-1)
			{
				bChecking=1;
				DoCheckSockEvent();
				bChecking=0;

			}
		}
		return 0;	
	}
	void DoCheckSockEvent()
	{
		SocketIpAddr addr;
		UINT port=addr.FromSocketGetLocalPort(fd);
		TRACETIMER("DoCheckEvent:tid=%d,fd=%d,%s:%d\n",OSGetCurrentThreadId(),fd,addr.ToStr(),port);
		timeval tv={1,0};
		int rw;
		while(fd!=-1&& rwFlag>=0)
		{
		//	rwFlag&=3;
			rw=~rwFlag;
			rw&=3;
			if(rw==0)
			{
		//		TRACESOCKET("DoCheckEvent-noev:fd=%d,ret=%d,\n",fd,0);
				return;
			}
			FDSET rs,ws;
			fd_set *pr=0;
			fd_set *pw=0;
			rs.Clear();
			ws.Clear();
			if(rw&1)
			{
				rs.Set(fd);
				pr=&rs;
			}
			if(rw&2)
			{
				ws.Set(fd);
				pw=&ws;
			}
			TRACETIMER("--Enter select,fd=%d\n",fd);
			int ret=select(fd+1,pr,pw,NULL,&tv);
			TRACETIMER("--Leave select,fd=%d\n",fd);
			if(fd==7)
			{
				TRACETIMER("===========================event=%d\n",ret);
			}
			if (ret>0)
			{
				if(FD_ISSET(fd,&rs))
				{
					rwFlag|=1;
					NotifyHandler(1,1);

				}
				if(FD_ISSET(fd,&ws))
				{
					rwFlag|=2;
					NotifyHandler(2,1);
				}
			}
			else if (ret<0)
			{
				//	TRACE("select error,fd=%d,err=%d\n",fd,WSAGetLastError());
				NotifyHandler(rw,ret);
				TRACETIMER("DoCheckEvent-err:fd=%d,ret=%d,\n",fd,ret);
				return;
			}
			//TRACESOCKET("DoCheckEvent-timeout:fd=%d,ret=%d,\n",fd,ret);
		}
		TRACETIMER("DoCheckEvent-End:fd=%d,ret=%d,\n",fd,0);
		
	}
	void NotifyHandler(int ev,int err)
	{
		if (pEventHandler)
		{
			if (pEventHandler->pQue==this->pQue)
			{
				pEventHandler->OnMsgSourceMessage(pEventHandler->dwSourceId,ev,(void*)(long)err);
			}
			else
			{
				pEventHandler->PostMsg(ev,(void*)(long)err);
			}
		}

	}

};

struct AsyncSocket:public IAsyncSocket,public MsgDispatcher
{
	SockEventChecker _EvChecker;
	int _sock;
	int _state;
	AsyncSocket(int type):IAsyncSocket(type)
	{
		_sock=-1;
		_state=0;
		
	};
	~AsyncSocket()
	{
		CloseSock();
	};
	virtual long GetSockHandle(){return _sock;}


	virtual bool OpenSock(SocketIpAddr peerIp,UINT peerPort,SocketIpAddr localIp=0,UINT localPort=0)
	{
		_PeerPort=peerPort;
		_PeerIp=peerIp;
		_localIp=localIp;
		_localPort=localPort;
		int fd=socket(PF_INET,_sockType,0);


		int on=1;
		int ret=setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(const char*)&on,sizeof(on));
		TRACESOCKET("setopt,sock=%d,ret=%d\n",fd,ret);
#ifndef __ENV_WIN
		setsockopt(fd,SOL_SOCKET,SO_NOSIGPIPE,&on,sizeof(on));
		int flags=fcntl(fd,F_GETFL,0);
		flags|=O_NONBLOCK;
		fcntl(fd,F_SETFL,flags);

#else
		unsigned long ul=1;
		ret=ioctlsocket(fd,FIONBIO,&ul);
		TRACESOCKET("ioctl,sock=%d,ret=%d\n",fd,ret);

#endif
		sockaddr_in addr;
		addr.sin_family=AF_INET;
		addr.sin_port=htons(localPort);
		addr.sin_addr.s_addr=inet_addr(localIp);
		ret=::bind(fd,(struct sockaddr*)&addr,sizeof(struct sockaddr));
		int len=sizeof(addr);
		ret=getsockname(fd,(struct sockaddr*)&addr,(socklen_t*)&len);


		TRACESOCKET("bind,sock=%d,addr=%s:%d,ret=%d\n",fd,localIp.ToStr(),localPort,ret);

		//struct sockaddr_in saddr;
		addr.sin_family=AF_INET;
		addr.sin_port=htons(peerPort);
		addr.sin_addr.s_addr=inet_addr(peerIp);
		int err=0;
		if(_sockType==SOCK_STREAM)
		{
			err=connect(fd,(struct sockaddr*)&addr,sizeof(struct sockaddr));
		}
		TRACESOCKET("connect to %s:%d ",_PeerIp.ToStr(),_PeerPort);
		TRACESOCKET("from %s:%d atsock=%d,ret=%d\n",localIp.ToStr(),localPort,fd,err);
		//SocketIpAddr ipaddr;
		this->_localPort=this->_localIp.FromSocketGetLocalPort(fd);
		TRACESOCKET("OpenSock::----local ip=%s:%d\n",this->_localIp.ToStr(),_localPort);
		if(_observerFlag&SocketObserverThread)
		{
			AttachToQue(_EvChecker.pQue);
		}
		else
		{
			AttachToQue(IMsgQueue::GetMain());
		}
		_sock=fd;
		_EvChecker.StartCheck(fd,_observerFlag&3,this);
		_state=0;
		return 1;
	}
	virtual void CloseSock()
	{
		if(_sock==-1) return;
		_EvChecker.StopCheck();
		int ret=closesocket(_sock);
		_sock=-1;
		_state=0;
		TRACESOCKET("--CloseSocket,ret=%d\n",ret);

	};

	virtual bool SetPeerAddr(SocketIpAddr peerIp,UINT peerPort)
	{
		if(_sockType==SOCK_DGRAM)
		{
			_PeerIp=peerIp;
			_PeerPort=peerPort;
/*
			if(_sock>-1)
			{
				sockaddr_in addr;
				addr.sin_family=AF_INET;
				addr.sin_port=htons(peerPort);
				addr.sin_addr.s_addr=inet_addr(peerIp.ToStr());
				int err=connect(_sock,(struct sockaddr*)&addr,sizeof(struct sockaddr));
				TRACESOCKET("AsyncSocket-SetPeerAddr:peer=%s:%d,err=%d(%d)\n",peerIp.ToStr(),peerPort,err,OSGetLastError());
			}
*/
			return 1;
		}
		return 0;
	}

	virtual int WriteData(const char* buf,int size)
	{

		int ret=0;
		if(_EvChecker.CanWrite())
		{
			if(_sockType==SOCK_DGRAM)
			{
				ret=sendto(_sock,buf,size,0,_PeerIp.MakeSockAddr(_PeerPort),sizeof(sockaddr_in));
				//SocketIpAddr IpAddr("192.168.2.205");
				//*LPDWORD(buf+4)=(DWORD)IpAddr;
			}
			else
			{

				ret=send(_sock,buf,size,0);
			}
			int err=OSGetLastError();
			TRACESOCKET("AsyncSocket-WriteData[%s]:Peer=%s:%d,size=%d,ret=%d,err=%d\n",_sockType==SOCK_DGRAM?"UDP":"TCP",_PeerIp.ToStr(),_PeerPort,size,ret,err);
	//		if(_sockType==SOCK_DGRAM) usleep(30);

		}
		if(ret<size)
		{
			this->_EvChecker.WaitWriteEvent();	
		}
		return ret;
	}
	virtual int ReadData(char* buf,int size)
	{
		
		int ret=recv(_sock,buf,size,0);
		TRACESOCKET("AsyncSocket_ReadData(fd=%d--%s:%d):read=%d,ret=%d,err=%d\n",_sock,_localIp.ToStr(),_localPort,size,ret,errno);
		if(ret<size)
		{
			this->_EvChecker.WaitReadEvent();
		}
		if (_localPort==7100) {
			int x=0;
		}
		else if(_localPort==7101)
		{
			int x=0;
		}
		return ret;
	};
	int OnMsgSourceMessage(DWORD dwSourceId,UINT msg,void* pMsg)
	{
		int err=(int)(long)pMsg;
		TRACESOCKET("OnSockEvent type=%d(fd=%d):ev=%p,err=%d\n",_sockType,_sock,msg,err);
		SocketIpAddr ipaddr;
		UINT port=ipaddr.FromSocketGetLocalPort(_sock);
		TRACESOCKET("OnMsgSourceMessage::----local ip=%s:port=%d\n",ipaddr.ToStr(),port);
		if(pMsg==0)
		{
			return 0;
		}
		//can read
		if (msg&1)
		{

			if(_pObserver)
			{
				_pObserver->OnSourceCanRead(err>0?0:err,this,_userParam);
			}
			else
			{
				//如果观察者未把数据读完,把数据清空
				static char tmpBuf[1600];
				while(ReadData(tmpBuf,1600)>0);

			}

		}
		//can write
		if(msg&2)
		{
			if (_state==0)
			{
				this->_localPort=this->_localIp.FromSocketGetLocalPort(_sock);

				//			TRACESOCKET("Connect to %s:%d from %s:%d sucess\n",&StrBuf32(_PeerIp),_PeerPort,&StrBuf32(_localIp),this->_localPort);
				if(_pObserver) _pObserver->OnSinkCanWrite(err,this,_userParam);
				_state=1;
				if(_pObserver) _pObserver->OnSinkCanWrite(0,this,_userParam);
			}
			else
			{
				if(_pObserver) _pObserver->OnSinkCanWrite(err>0?0:err,this,_userParam);
			}
			
		}
		return 0;
		
	}

	
};


ISocket* NetLib_CreateSocketObject(int type)
{
	return new AsyncSocket(type);
}



