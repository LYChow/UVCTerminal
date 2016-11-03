#ifndef __INETLIB__H__
#define __INETLIB__H__

#include "IDataObject.h"
#include "Net.h"
 

enum SocketObserverFlag
{
	SocketObserverRead =1,
	SocketObserverWrite = 2,
	SocketObserverCreate = 4,
	SocketObserverThread =8,
};
struct ISocket;

#define ISocketObserver IDataSockObserver
//typedef IDataSockObserver ISocketObserver;
/*
struct ISocketObserver:IDataSockObserver
{
	
};
*/

struct ISocket:IDataSock
{
	virtual void RegisterObserver(struct ISocketObserver* pObserver,long userParam,WORD dwFlags=SocketObserverRead|SocketObserverWrite){};
	virtual bool OpenSock(SocketIpAddr peerIp,UINT peerPort,SocketIpAddr localIp=0,UINT localPort=0){return 0;};
	virtual void CloseSock(){};
	virtual void Release(){};
	virtual SocketIpAddr GetLocalIp(){return "";};
	virtual SocketIpAddr GetPeerIp(){return "";};
	virtual UINT GetLocalPort(){return 0;};
	virtual UINT GetPeerPort(){return 0;};

	virtual int GetSocketType(){return 0;}
	virtual int GetSocketHandle(){return -1;}
	//only udp
	virtual bool SetPeerAddr(SocketIpAddr peerIp,UINT peerPort){return 0;}

};



ISocket* NetLib_CreateSocketObject(int type=0);//,ISocketObserver* pObserver=0,long userParam=0);



#endif