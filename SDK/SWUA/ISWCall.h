#ifndef __ICALL__H__
#define __ICALL__H__
#include "DataSet.h"
#include "StreamBuffer.h"
#include "ISWUA.h"
#include "MVSProtocol.h"
#include "Timer.h"


namespace SWApi{


_DECL_ENUMSTR(ESCallType);
_DECL_ENUMSTR(ESCallDir);
_DECL_ENUMSTR(ESCallState);
_DECL_ENUMSTR(ESCallMsg);
_DECL_ENUMSTR(ESCallResult);

struct SWUA;
struct VC3Terminal;
struct SWCall:public ISWCall,public ITimerObserver
{
	String strInfo;
	struct SWUA* pUA;
	DWORD dwCallId;
	int protocol;
	int type;
	int dir;
	int state;
	int result;
	VC3Terminal* pPeer;
	SecondTimer* msgTimer;
	time_t tmStart;
	time_t tmEnd;
public:
	SWCall()
	{
		ZeroThisFrom(dwCallId);
	}
	~SWCall()
	{
		EndMsgTimer();
	}
	void Reset()
	{
		ZeroThisFrom(type);
	}
	void StartMsgTimer(int name,int timeout)
	{
		TRACE("CALL-TIMERStart:name=%s,timeout=%d\n",ESCallState(name),timeout);
		if(timeout==0) return;
		if(!msgTimer)
		{
			msgTimer=new SecondTimer(this, (long)name);
			msgTimer->Start(timeout);
		}
		else
		{
			msgTimer->Start(timeout,name);
		}
	}
	void StopMsgTimer()
	{
		if(state) TRACE("CALL-TIMERStop:name=%s\n",ESCallState(state));
		//delete msgTimer;
		msgTimer->Stop();
	}
	void EndMsgTimer()
	{
		if(state) TRACE("CALL-TIMEREnd:name=%s\n",ESCallState(state));
		if(msgTimer)
		{
			delete msgTimer;
		}
		msgTimer=0;
	}

public:
	virtual int GetType(){return type;};
	virtual int GetDirection(){return dir;}
	virtual int GetState(){return state;}
	virtual struct ITerminal* GetPeer(){return (ITerminal*)pPeer;}
	virtual int GetCallResult(){return result;};
	virtual LPCTSTR GetCallInfo(){return strInfo.c_str();}
	virtual time_t GetStartTime(){return tmStart;};
	virtual int GetDuration(){return (int)(tmEnd-tmStart);};
	virtual void Release(){};

public:
	virtual void OnTimerTimeout(ITimer* hTimer,long userParam);
	void EndCall(int result=0);
	void OnSetup(DWORD dwCallId,long lParam);
	void OnCancel();
	void OnAlert();
	void OnAccept();
//	void OnAlive(int timeout);
//	void OnAliveAck(int timeout);
	void OnReject(int cause);
	void OnRelease();
	void OnReleaseComplete(int ret=0);

	virtual int Setup(long lParam=0);
	virtual int Cancel();
	virtual int Accept();
	virtual int Reject(int cause,LPCTSTR szCause="");
	virtual int ReleaseCall(int ret=0);

	static SWCall* CreateCall(SWUA* pUA,VC3Terminal* pTerm,int type);
	static bool OnCallMessage(SWUA* pUA,CmdSendCommand* pCmd);
	

};

struct ISWFindCall
{
	virtual bool Find(SWCall* pCall)=0;
};

struct FindCallById:public ISWFindCall
{
	DWORD dwCallId;
	int dir;
	FindCallById(DWORD CallId,int Dir=-1)
	{
		dwCallId=CallId;dir=Dir;
	}
	virtual bool Find(SWCall* pCall)
	{
		return (pCall->dwCallId==dwCallId && ( dir==-1 || dir==pCall->dir));		
	}
};

struct FindCallByInfo:public ISWFindCall
{
	int type;
	int dir;
	int state;
	ITerminal* pterm;
	FindCallByInfo(ITerminal* pTerm,int Type=-1,int State=-1,int Dir=-1)
	{
		type=Type;dir=Dir;state=State;pterm=pTerm;
	}
	virtual bool Find(SWCall* pCall)
	{
		return ((pterm==NULL || pterm==(ITerminal*)pCall->pPeer)
			&&(type==-1 || type==pCall->type)
			&&(dir=-1 || dir==pCall->dir)
			&&(state=-1 || state==pCall->state)
			);		
	}
};



struct SWCallList:public BaseArray<SWCall>
{
	int iFirst;
	int cnt;
public:
	SWCallList(int size=200)
	{
		iFirst=0;
		cnt=0;
		SetSize(size);
	}
	void Reset()
	{
		for (int i=0;i<m_nMaxSize;i++)
		{
			GetAt(i).Reset();
			iFirst=0;
			cnt=0;
		}
		
	}
	int GetCallCount(){return cnt;}

	SWCall* AddCall(SWUA* pUA)
	{
		static DWORD dwLastCallId=1;
		int iCall=(iFirst+m_nSize-1)%m_nSize;
		SWCall* pCall=&m_pData[iCall];
		if (pCall->state==CallState_Idle)
		{
			pCall->Reset();
			pCall->pUA=pUA;
			pCall->dwCallId=dwLastCallId++;
			if(dwLastCallId==0) dwLastCallId=1;
	//		return 0;
		}
		iFirst=iCall;
		if (cnt<m_nSize)
		{
			cnt++;
		}
		return pCall;
	}

	SWCall* GetCall(int i)
	{
		return &m_pData[(iFirst+i)%m_nSize];
	}

	SWCall* GetCallByPos(int iCall)
	{
		if(iCall<0 || iCall>=m_nSize) return 0;
		return &m_pData[iCall];
	}
	
	int FindCall(ISWFindCall* pf,int from=0)
	{
		if(from<0 || from>=cnt) from=0;

		int iCall=(iFirst+from)%m_nSize;
		int i=0;
		while(i<cnt)
		{
			SWCall* pCall=&m_pData[iCall];
			if (pf->Find(pCall))
			{
				return (iCall-iFirst+m_nSize)%m_nSize;
			}
			
			iCall=(iCall+1)%m_nSize;
			i++;
		}

		return -1;
		
	}


};




enum NETPROTOCOL_TYPE_E
{
	NETPROTOCOL_NONE,	//未使用本协议(老协议流程)
	NETPROTOCOL_GENERAL,//通用呼叫协议
	NETPROTOCOL_PC,//指挥终端呼叫协议
	NETPROTOCOL_VOIP,//VOIP呼叫协议

	NETPROTOCOL_COUNT
};

enum CallMsgTypeEx
{
	CallMsg_Alive = 6,
	CallMsg_AliveAck = 7,
};

struct CallMessag
{
	int proct;
	int type;
	int msg;
	DWORD dwCallId;
//	LPCTSTR szCallId;
	LPCTSTR szFrom;
	LPCTSTR szTo;
	LPCTSTR szInfo;
	int lParam;
	
	CallMessag(int proct=0,int type=0,int msg=0,DWORD CallId=0,LPCTSTR szFrom="",LPCTSTR szTo="",LPCTSTR szInfo="",int lParam=0)
	{
		proct=proct;msg=msg;dwCallId=CallId;szFrom=szFrom;szTo=szTo;szInfo=szInfo;lParam=lParam;
	}

	bool ParseMessage(CmdSendCommand* pCmd);
};

template<int _CMDBUFLEN=4096>
struct SWCmdBuffer
{
	SWUA* pUA;
	char szTermName[64];
	DWORD dwCommandCode;
	int lParam;
	char szParam[_CMDBUFLEN];
	SWCmdBuffer(SWUA* p)
	{
		ZeroMemory(this,sizeof(*this));

		pUA=p;
	}
};


struct SWCallCmd:public SWCmdBuffer<1024>
{
	SWCallCmd(SWUA* p):SWCmdBuffer(p){}
	bool MakeCallMessage(LPCTSTR szTo,int proct,int type,int msg,DWORD dwCallId,LPCTSTR szFrom,int lParam=0,LPCTSTR szInfo="");
	bool MakeCallMessage(CallMessag& callMsg);
	int MakeAndSend(LPCTSTR szTo,int proct,int type,int msg,DWORD dwCallId,LPCTSTR szFrom,int lParam=0,LPCTSTR szInfo="")
	{
		if(MakeCallMessage(szTo,proct,type,msg,dwCallId,szFrom,lParam,szInfo))
		{
			//	TRACE("CALL-SEND to [%s]:msg=%s,cmd=%d,lp=%d,szMsg=%s\n",szTo,ESCallMsg(msg),dwCommandCode,lParam,szParam);
			return Send();
		}
		return -1;
	}
	int Send();
	
};

};//end namespace SWApi

#endif
