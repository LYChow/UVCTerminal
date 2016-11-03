#ifndef __SWUA__H__
#define __SWUA__H__



//----包含接口列表

//#include "IMediaLib.h"	//1.媒体设备,采集播放
#include "NetLib.h"	//3.网络
#include "ISWUA.h"		//4.用户接口
#include "Timer.h"
#include "MVSProtocol.h"
#include "TermInfo.h"
#include "ISWCall.h"


namespace SWApi{


_DECL_ENUMSTR(ESLoginRet);


enum 
{
	SocketControl = 0,
	SocketVideo,
	SocketAudio,
	SocketDataChannel,
};
struct SWUA;
struct PTZControl:public IPTZControl
{
	PTZControl(SWUA* pUa)
	{
		_pUa=pUa;
		_pTerm=0;
	}
	int SendControl(LPCTSTR szTerminal, DWORD dwControlType,int nDeviceNo, int nAction, int nMode, char* szExtend=NULL);

	virtual void SetControlTerminal(ITerminal* pTerm){_pTerm=pTerm;}
	virtual ITerminal* GetControlTerminal(){return _pTerm;}
	virtual int Move(int dir)
	{
		return 0;
	}
	virtual int Move3D(int pan,int tile,int zoom)
	{
		return 0;
	}
	virtual int SavePreset(int iPreset)
	{
		return 0;
	}
	virtual int LoadPreset(int iPreset)
	{
		return 0;
	}
	//analog:0=大小,1=光圈,3=聚集
	virtual int AdjustAnalog(int iAnalog,int step)
	{
		return 0;
	}
	//switch:0=灯光,1=雨刷,2=电源
	virtual int SetSwitch(int iSwitch,bool bOpen)
	{
		return 0;
	}
	virtual int SendControlCode(const char* pCode,int size)
	{
		return 0;
	}
//	virtual int SetDeviceIndex(int i);
//	virtual int GetDeviceIndex();

protected:
	SWUA* _pUa;
	ITerminal* _pTerm;

};

struct SWUA:public ISWUA,public ISocketObserver,public ITimerObserver
{
	ISWUAEventHandler* m_pHandler;
	PTZControl m_PTZControl;
	int m_state;
	int m_linkCheck;
	SocketIpAddr m_mvsIp;
	SocketIpAddr m_localIp;

	string m_strName;
	string m_strIDName;
	string m_strPass;
	string m_strMvsName;
	string m_strTermFilter;

	NODEINFO m_nodeInfo;
	HOSTINFO m_hostInfo;


	IAsyncSocket* m_pSockControl;//控制:PortLogin=263,PortMessage=261



	MVSRecvBuffer m_recvBuffer;
	MVSSendBuffer m_sendBuffer;
	
	SecondTimer* m_hTimerMsg;
	int m_curMsgState;

	int m_bNeedRet;

	TermInfoMap m_terminfoMap;
	TermInfoList m_termList;
	VC3Terminal* m_pTermSelf;
	SWCallList m_callList;

	SWNatHole m_NatVideo;
	SWNatHole m_NatAudio;

	
 
//	TCPConnection m_mvsMsgSocket;
public:
	SWUA(ISWUAEventHandler* pHandler=0);
	~SWUA();
	virtual void Release();
public:
	virtual int Login(LPCTSTR szServer,LPCTSTR szUsername,LPCTSTR szPassword);
	virtual int CancelLogin(){return 0;};
	virtual int Logout();
public:
	//terminal
	virtual ITerminal* AddTerminal(LPCTSTR szTermName,LPCTSTR szFriendlyName="");
	virtual ITerminal* GetSelf(){return m_pTermSelf;};
	virtual ITerminal* GetTerminalByName(LPCTSTR szTermName);
	virtual int GetTerminalCount();
	virtual ITerminal* GetTerminal(int i);
	virtual int UpdateTerminalInfo(int nCount,ITerminal* pTerms[]=0);
	int UpdateTerminalInfo(LPCTSTR szTermName);

	//call
	virtual ISWCall* CreateCall(ITerminal* pTerm,int type);
	//获取所有呼叫,包括正则进行中的和完成的呼叫
	virtual int GetCallCount();
	virtual ISWCall* GetCall(int i);
	//设置最大的呼叫历史保存条数(包括正则进行的),超过最大条数自动删除最早的呼叫
	virtual int SetMaxCallCount(int nMax=200);

	//other command
	virtual int SendShortMessage(ITerminal* pTerm,LPCTSTR szMessage,LPCTSTR szInfo="");
	virtual int BroadShortMessage(int nCount,ITerminal* pTerms[],LPCTSTR szMessage,LPCTSTR szInfo="");

	virtual int SendCommand(ITerminal* pTerm,DWORD dwCommandCode,int lParam,LPCTSTR szParam="");
	virtual IPTZControl* GetPTZControl(ITerminal* pTerm){ m_PTZControl.SetControlTerminal(pTerm);return &m_PTZControl;};

	//
	virtual DWORD GetNetMediaId()
	{
		return m_localIp;
	}
	virtual int SendTermVideo(ITerminal* pTermTo,int iChannel=0);;
	virtual int SendTermAudio(ITerminal* pTermTo,int iChannel=0);
	int OpenTermMedia(ITerminal* pTerm,int type,int channel=0);
	int CloseTermMedia(ITerminal* pTerm,int type,int channel=0);
	int OpenMediaByStreamId(DWORD sid,int type,int channel=0);
	int CloseMediaByStreamId(DWORD sid,int type,int channel=0);
	


public:
	virtual int SendMsgUpdateTerminalInfo(LPCTSTR szTermFilter="");
	virtual int SendCommand(LPCTSTR szTerminal,DWORD dwCommand,int lParam,LPCTSTR szParam,char* szOutBuffer=NULL);
	virtual int CreateAVTransmission(int type,LPCTSTR szTerminalSrc,LPCTSTR szTerminalDst);
	virtual int DestroyAVTransmission(int type,LPCTSTR szTerminalSrc,LPCTSTR szTerminalDst);


public:
	virtual void OnTimerTimeout(ITimer* hTimer,long name);

	//ISocketObserver
public:
	virtual void OnSourceCanRead(int err,IDataSource* pSocket,long userParam);
	virtual void OnSinkCanWrite(int err,IDataSink* pSocket,long userParam);
	void OnSockClose(int err,struct IDataSock* pSocket,long userParam);


public:
	void OnLogin(int ret)
	{
		if(m_pHandler){m_pHandler->OnUALogin(this,ret);}
	}
	LPCTSTR GetGlobalName(VC3Terminal* pTerm=0)
	{
		static char szGlobalName[64];
		static string strIdName;
		if(pTerm==0) return m_strIDName.c_str();
		int i=pTerm->strName.find('.');
		if (i>=0)
		{
			return pTerm->Name();
		}
		sprintf(szGlobalName,"%s.%s",m_strMvsName.c_str(),pTerm->Name());
		return szGlobalName;
	}
	LPCTSTR GetRealName(LPCTSTR szTermName)
	{
		if(!szTermName || *szTermName==0) return 0;
		LPCTSTR p=strchr(szTermName,'.');
		if (p && strncmp(szTermName,m_strIDName.c_str(),m_strMvsName.size()+1)==0)
		{
			return p+1;
		}
		return szTermName;

	}
	SWCallList* GetCallList(){return &m_callList;}
	int GetTermLocalOrder(ITerminal* pTerm);
	VC3Terminal* GetTermByLocalOrder(int iOrder);
	VC3Terminal* GetTermByStreamId(long sid);
	//comunication
	void StartMsgTimer(int name,int timeout)
	{
		m_curMsgState=name;
		TRACETIMER("TIMER-Start:name=%s,timeout=%d\n",ESMvsMsg(name),timeout);
		if(timeout==0) return;
		if(!m_hTimerMsg)
		{
			m_hTimerMsg=new SecondTimer(this, (long)name);
			m_hTimerMsg->Start(timeout);
		}
		else
		{
			m_hTimerMsg->Start(timeout,name);
		}
	}
	void StopMsgTimer()
	{
		TRACETIMER("TIMER-Stop:name=%s\n",ESMvsMsg(m_curMsgState));
		m_hTimerMsg->Stop();
	}
	int PTZControl(LPCTSTR szTerminal, DWORD dwControlType,int nDeviceNo, int nAction, int nMode, char* szExtend=NULL);
	int OnRecvMVSMsg(int msg,LPCTSTR szMsg,int size);

	int SendMVSMsg(int type,LPCTSTR szMsg,int size,int timeout=0);
	void DoMVSMsgSend();

	int OnUpdateTermInfoList(int nTerm,LPCTSTR szTermInfoList,int size);
	//0=video,1=audio
	void SetNatHoleSocket(IAsyncSocket* pSocket,int type)
	{
		if(type==0) m_NatVideo.SetHoleSocket(pSocket);
		else if(type==1) m_NatAudio.SetHoleSocket(pSocket);
	}


protected:
	void ReleaseTermInfoMap();
	void OnReconnectMvsSuccess();


};

};//end namespace SWApi


#endif
