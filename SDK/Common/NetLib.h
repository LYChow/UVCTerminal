#ifndef __NETLIB__H__
#define __NETLIB__H__
#include "Net.h"
#include "INetLib.h"

struct IAsyncSocket:public ISocket
{
	IAsyncSocket(int type=0){ ZeroThisFrom(_pObserver);_sockType=type;};
	virtual ~IAsyncSocket(){};
	virtual void RegisterObserver(struct ISocketObserver* pObserver,long userParam,WORD dwFlags=SocketObserverRead|SocketObserverWrite)
	{
		_observerFlag=dwFlags;
		_pObserver=pObserver;
		this->_userParam=userParam;
		
     cout<<"debug_RegisterObserver"<<endl;
		
	}
	virtual SocketIpAddr GetLocalIp(){return _localIp;}
	virtual SocketIpAddr GetPeerIp(){return _PeerIp;};
	virtual UINT GetLocalPort(){return _localPort;}
	virtual UINT GetPeerPort(){return _PeerPort;};
	virtual void Release(){delete this;}

	virtual int GetSocketType(){return _sockType;}
	//only udp
	virtual bool SetPeerAddr(SocketIpAddr peerIp,UINT peerPort)
	{
		if(_sockType==SOCK_DGRAM)
		{
			_PeerIp=peerIp;
			_PeerPort=peerPort;
			return 1;
		}
		return 0;
	}
	virtual long GetSockHandle(){return 0;}


public:
	ISocketObserver* _pObserver;
	long _userParam;
	SocketIpAddr _localIp;
	SocketIpAddr _PeerIp;
	UINT _localPort;
	UINT _PeerPort;
	WORD _sockType;
	WORD _observerFlag;

};

#endif
