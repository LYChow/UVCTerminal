#include "__PCH.h"
#include "SWUA.h"
#include "SendCommand.h"
//#include "MediaLibImpl.h"

typedef unsigned int UINT,*LPINT,DWORD,*LPDWORD;

namespace SWApi{

_BEGIN_IMPL_ENUMSTRFROM(ESLoginRet,LoginConnectFail) 
	"LoginConnectFail",	
	"LoginGetRegInfoFail",	
	"LoginGetRegInfoLoginedElsewhere",
	"LoginRegNodeFail",
	"LoginSucess",	
	"LoginConnecting",	
	"LoginGetRegInfo",	
	"LoginRegNode",
_END_IMPL_ENUMSTRFROM;

//using namespace MediaLib;


struct TestHandler:public ISWUAEventHandler
{
	virtual void OnUALogin(ISWUA* pua,int ret)
	{
		TRACEINFO("INFO-TestHandler:recv OnLogin,ua=%p,ret=%s\n",pua,ESLoginRet(ret));
	};
	virtual void OnUACall(int ret,ISWCall* pCall,ISWUA* pua=0){};

};

int PTZControl::SendControl(LPCTSTR szTerminal, DWORD dwControlType,int nDeviceNo, int nAction, int nMode, char* szExtend)
{
	if(_pTerm)	return _pUa->PTZControl(_pTerm->Name(),dwControlType,nDeviceNo,nAction,nMode,szExtend);
	return -1;
}

//////////////////////////////////////////////////////////////////////////
//SWUA
#pragma warning(disable:4355)

SWUA::SWUA( ISWUAEventHandler* pHandler/*=0*/ )
:m_PTZControl(this)
{
	static TestHandler __Handler;
	m_pHandler=pHandler;
	if (!pHandler)
	{
		m_pHandler=&__Handler;
	}
	
	m_state=UA_Idle;
	m_pSockControl=0;
	m_hTimerMsg=0;
	m_curMsgState=0;
	m_linkCheck=0;


	m_callList.SetSize(200);
	
}

SWUA::~SWUA()
{
	Logout();

};

void SWUA::Release()
{
    //	ReleaseUA(this);
	delete this;
}


int SWUA::Logout()
{
	TRACEINFO("INFO-UA Logout\n");
	if(m_state!=UA_Idle)
	{
		m_state=UA_Logout;
		if (m_pHandler) {
			m_pHandler->OnUAStateChange(this, m_state);
		}
		StopMsgTimer();
		m_pSockControl->CloseSock();
		m_pSockControl->Release();
		m_pSockControl=0;
		m_sendBuffer.Reset();
		m_recvBuffer.Reset();
		m_state=UA_Idle;
		if (m_pHandler) {
			m_pHandler->OnUAStateChange(this, m_state);
		}
		if(m_hTimerMsg)
		{
			delete m_hTimerMsg;
			m_hTimerMsg=0;
		}
		m_terminfoMap.clear();
		m_termList.RemoveAll();
		m_callList.Reset();
		m_strName="";
		m_mvsIp=0;

	}
	return 0;

}
int SWUA::GetTermLocalOrder(ITerminal* pTerm)
{
	VC3Terminal* p=(VC3Terminal*)pTerm;
	return p->localOrder;
/*
	VC3Terminal* pBase=m_termList.GetData();
	int i=(int)(p-pBase);
	if(i<0 || i>=m_termList.GetSize()) i=-1;
	return i;
*/
}
VC3Terminal* SWUA::GetTermByLocalOrder(int iOrder)
{
	return m_termList[iOrder];
}

VC3Terminal* SWUA::GetTermByStreamId(long sid)
{
	for (int i=0;i<m_termList.GetSize();i++)
	{
		if (m_termList[i]->GetNetId()==(DWORD)sid)
		{
			return m_termList[i];
		}
	}
	return 0;	
}


ITerminal* SWUA::AddTerminal(LPCTSTR szTermName,LPCTSTR szFriendlyName)
{
	m_strTermFilter=szTermName;
	VC3Terminal* pTerm=0;
	ITerminal** ppTerms=0;
	if(szTermName && *szTermName)
	{
		szTermName=GetRealName(szTermName);
		pTerm=m_terminfoMap[szTermName];
		if(pTerm==NULL)
		{
			pTerm=new VC3Terminal();
			pTerm->localOrder=m_termList.Add(pTerm);
			pTerm->strName=szTermName;
			pTerm->strNameFriendly=szFriendlyName;
			m_terminfoMap[szTermName]=pTerm;
			UpdateTerminalInfo(szTermName);
		}
		ppTerms=(ITerminal**)(&pTerm);
	}
	UpdateTerminalInfo(1,ppTerms);
	return pTerm;
};
ITerminal* SWUA::GetTerminalByName(LPCTSTR szTermName)
{
	if (szTermName==0 || *szTermName==0)
	{
		return 0;
	}
	return AddTerminal(szTermName,"");
};
int SWUA::GetTerminalCount()
{
	return m_termList.GetSize();
};
ITerminal* SWUA::GetTerminal(int i)
{
	return m_termList[i];
};

int SWUA::UpdateTerminalInfo(LPCTSTR szTermName)
{
	char szFilter[96]={0};
	if(szTermName && *szTermName)
	{
		sprintf(szFilter,"hostname=\"%s\"",szTermName);
	}
	return SendMsgUpdateTerminalInfo(szFilter);

}


int SWUA::UpdateTerminalInfo(int nCount,ITerminal* pTerms[])
{
	LPCTSTR szTermName=0;
	if(nCount==1)
	{
		szTermName=pTerms[0]->Name();		
	}
	char szFilter[96]={0};
	if(szTermName && *szTermName)
	{
		sprintf(szFilter,"hostname=\"%s\"",szTermName);
	}
	return SendMsgUpdateTerminalInfo(szFilter);
}


ISWCall* SWUA::CreateCall( ITerminal* pTerm,int type)
{
	if (pTerm==m_pTermSelf)
	{
		return 0;
	}
	
	return SWCall::CreateCall(this,(VC3Terminal*)pTerm,type);
}



int SWUA::GetCallCount()
{
	return m_callList.GetCallCount();
};
ISWCall* SWUA::GetCall(int i)
{
	return m_callList.GetCall(i);
};
int SWUA::SetMaxCallCount(int nMax)
{
	m_callList.SetSize(nMax);
	return nMax;
};
int SWUA::SendShortMessage(ITerminal* pTerm,LPCTSTR szMessage,LPCTSTR szInfo)
{
	char szParam[1024]={0};
	sprintf(szParam,"%s\r\n%s",GetGlobalName(),szMessage);
	return SendCommand(pTerm->Name(),100,0,szParam);
};
int SWUA::BroadShortMessage(int nCount,ITerminal* pTerms[],LPCTSTR szMessage,LPCTSTR szInfo)
{
	return 0;
};
int SWUA::SendCommand(ITerminal* pTerm,DWORD dwCommandCode,int lParam,LPCTSTR szParam)
{
	return SendCommand(pTerm->Name(),dwCommandCode,lParam,szParam);
};


int SWUA::Login( LPCTSTR szServer,LPCTSTR szUsername,LPCTSTR szPassword )
{
	if(m_mvsIp==szServer && m_strName==szUsername)//
	{
		return m_state;
	}
	if (m_state)
	{
		Logout();
	}
    
	m_state=UA_Login;
	if (m_pHandler) {
		m_pHandler->OnUAStateChange(this, m_state);
	}
	m_mvsIp=szServer;
	m_strName=szUsername;
	m_strPass=szPassword;
	m_pSockControl=(IAsyncSocket*)NetLib_CreateSocketObject(1);//tcp

    

	m_pSockControl->RegisterObserver(this,SocketControl);
	OnLogin(LoginConnecting);
	//连接到登录端口,等待连接消息->OnConnect
	if(!m_pSockControl->OpenSock(m_mvsIp,PortLogin))
	{
        
		OnLogin(LoginConnectFail);
		return 1;
	}
	int fd=m_pSockControl->GetSockHandle();
	StartBackgourndNetwork(fd);
	//定时器等待
	StartMsgTimer(MVSMSG_CONNECT_LOGIN,10);
 
	return 0;

}

int SWUA::SendMVSMsg(int msg,LPCTSTR szMsg,int size,int timeout)
{
	if(msg!=MVSMSG_LINKCHECK) TRACETIMER("INFO-SendMVSMsg :msg=%s,size=%d\n",ESMvsMsg(msg),size);
	m_sendBuffer.PushVal(msg);
	m_sendBuffer.PushVal(size);
	m_sendBuffer.PushData(szMsg, size);
	DoMVSMsgSend();
	if(timeout)
	{
		StartMsgTimer(msg,timeout);
	}
	return  0;
}
void SWUA::DoMVSMsgSend()
{
/*
	if(!m_sendBuffer.canSend)
	{
		SMHDR& phdr=m_sendBuffer.GetHead<SMHDR>();
		TRACE("DoMVSMsgSend while net unaviliable,curmsg=%s\n",ESMvsMsg(phdr.type));
		return;	
	}
*/
	
	while(m_sendBuffer.GetHeadSize() && m_sendBuffer.canSend)
	{
		SMHDR& phdr=m_sendBuffer.GetHead<SMHDR>();
		int need=phdr.size-m_sendBuffer.sendSize;
		char* p=phdr.szMsg+m_sendBuffer.sendSize;
		int err=m_pSockControl->WriteData(p,need);
//////////////////////////////////////////////////////////////////////////
//TRACE sendmsg info
		if(phdr.type!=MVSMSG_LINKCHECK)
		{
			TRACEMSG("INFO-DoMVSMsgSend(%d/%d):msttype=%s -- ",err,need,ESMvsMsg(phdr.type));
			if (phdr.type==MVSMSG_SENDCOMMAND)
			{
				MVSCmdMsgHdr* pcm=(MVSCmdMsgHdr*)(phdr.szMsg);
				StreamBuffer rb(pcm->GetPacket()+4,pcm->GetPackSize()-4);
				LPCTSTR szTerm=rb.PopString();int cmd=*rb.PopVal<int>();int lp=*rb.PopVal<int>();
				TRACEMSG(" to %s,cmd=%s,lp=%d,szParam=%s\n",szTerm,GetVC3CmdName(cmd),lp,rb.PopString());
			}
			else
			{
				TRACEMSG("\n");
			}
		}
		
		if(err<need)
		{
			if(err<0)
			{
				TRACEERR("ERR-SendMsg:msgtype=%s ret -1\n",ESMvsMsg(phdr.type));
				err=0;
			}
			m_sendBuffer.canSend=0;
			m_sendBuffer.sendSize+=err;
			break;
		}
		//	if(err==need) 
		{
			m_sendBuffer.sendSize=0;
			if (phdr.type==MVSMSG_TERMINFOLIST)
			{
				m_sendBuffer.canSend=0;
			}
			else
			{
				m_sendBuffer.canSend=1;
			}
			m_sendBuffer.RemoveHead(phdr.size+sizeof(SMHDR));
		//	m_sendBuffer.canSend=1;
		//	continue;
		}
			
		
	};
	
}


//收到消息
void SWUA::OnSourceCanRead(int err,IDataSource* pSocket,long userParam)
{
	TRACEINFO("INFO-OnBytesAvailable:err=%d,pSock=%p,param=%ld\n",err,pSocket,userParam);
	if (err!=0)
	{
		if(err<0) OnSockClose(err+1,m_pSockControl,userParam);
		return;
	}
	//接收数据
	m_recvBuffer.PushSource(pSocket);
	int msg;
	int msgSize;
	while ((msg=m_recvBuffer.GetMvsMessage(&msgSize))!=0)
	{
		//如果消息处理需要重连,就不再处理后续消息
		OnRecvMVSMsg(msg,m_recvBuffer.GetHeadPtr(),msgSize);
		m_recvBuffer.RemoveHead(msgSize);
	}

	return ;
}
void SWUA::OnSinkCanWrite(int err,IDataSink* pSocket,long userParam)
{
	TRACEINFO("INFO-OnSinkCanWrite=%d,pSock=%p,param=%ld\n",err,pSocket,userParam);
	if (err!=0)
	{
		if(err<0) OnSockClose(err+1,m_pSockControl,userParam);
		return;
	}
	m_sendBuffer.canSend=1;
	switch(m_curMsgState)
	{
		case MVSMSG_CONNECT_LOGIN:
		{
			StopMsgTimer();
			OnLogin(LoginGetRegInfo);

			//发送登录消息,等待回应
			char szMsg[256];
			int len=MVS_MakeLoginMessage(szMsg,m_strName.c_str(),m_strPass.c_str());
			SendMVSMsg(MVSMSG_LOGIN, szMsg, len,10);
		}
		break;
		case MVSMSG_CONNECT_REGNODE:
		{
			//发送注册节点消息,等待回应
			SendMVSMsg(MVSMSG_REGNODE, LPCTSTR(&m_nodeInfo), sizeof(NODEINFO),5);
		}
		break;
		default:
		{
			DoMVSMsgSend();
		}
		break;
	}
		
}

int SWUA::OnUpdateTermInfoList(int nTerm,LPCTSTR szTermInfoList,int size)
{
	TRACEINFO("BEGIN UPDATE_TERMLIST:cnt=%d\n",nTerm);
	if(nTerm<=0) return nTerm;
	TermInfoItr it;
	VC3Terminal* pTerm=0;
	TerminalInfo tp(szTermInfoList,size);
	if (nTerm>1)
	{
		int x=0;
	}
	while(tp.ParseNext())
	{
		TRACEINFO("\t termname=%s,type=%d\n",tp.szHostName,tp.nTermType);
		switch (tp.nTermType)
		{
		case CONFTERM:
		case CTRLTERM:
		case FRONTERM:
		case VIDEOMULTIPLEXER:
		case RECORDSERVER:
			break;
		default:
			continue;
		
		}
		pTerm=0;
		it=m_terminfoMap.find(tp.szHostName);
		if (it==m_terminfoMap.end())
		{
			//if(m_strTermFilter=="")
			{
				pTerm=new VC3Terminal();
				pTerm->localOrder=m_termList.Add(pTerm);
				pTerm->strName=tp.szHostName;
				m_terminfoMap[tp.szHostName]=pTerm;
			}
		}
		else
		{
			pTerm=it->second;
		}
		if (strcmp(tp.szHostName,"ZYJ1")==0)
		{
			int x=0;
		}
		if(pTerm==0) continue;
		if(pTerm->strName==m_strName) m_pTermSelf=pTerm;

		DWORD dwUpdateHigh=0;
		DWORD dwUpdateLow=tp.UpdateTerminal(pTerm,dwUpdateHigh);
		if (pTerm->strName=="")
		{
			int x=0;
		}
		DWORD dwFlag=TermInfoFlagToProp(dwUpdateLow,dwUpdateHigh);
		pTerm->pChairman=0;
		if(tp.szChairman[0]) pTerm->pChairman=m_terminfoMap[tp.szChairman];
		
		if (dwFlag&BitMask(TermPropType))
		{
			if(pTerm->nTermType==CONFTERM || pTerm->nTermType==CTRLTERM)
			{
				BitClear(dwFlag,TermPropType);
				BitSet(dwFlag,TermPropState);
				BitClear(pTerm->dwStatus,TermStateInConf);
				if(pTerm->nTermType==CONFTERM)
				{
					pTerm->nTermType=CTRLTERM;
					BitSet(pTerm->dwStatus,TermStateInConf);
				}
			}
		}
		BitSet(pTerm->dwStatus,TermStateOnLine);
		if(!IsBitSet(pTerm->dwStatus,TermStateOnLine)) BitSet(dwFlag,TermPropState);
//////////////////////////////////////////////////////////////////////////
//term state update
			if ((pTerm->dwAppVersion.type==SWVC3_APPTYPE_SWVC3API && pTerm->dwAppVersion.apiVer>=1)
				||pTerm->dwAppVersion.type==SWVC3_APPTYPE_SWVC3API_IOS
				||(pTerm->dwGlobalFlag&BITSTATE_CANACTIVEDBYCMD301))
			{
				pTerm->callProctol=NETPROTOCOL_GENERAL;
			}
			else
			{
				pTerm->callProctol=NETPROTOCOL_PC;
			}
			if (pTerm->pTimeout==0)
			{
				if (pTerm->dwAppVersion.type>=SWVC3_APPTYPE_SWVC3API || pTerm->nTermSubType==TERMSUBTYPE_SIPPHONE)
				{
					pTerm->pTimeout=GetTimeoutConfig(62000,62000,30000,62000,62000,62000);
				}
			}
			//使用获取终端信息检测在线，表示alert
			if (pTerm->pActiveCall && pTerm->pActiveCall->state==CallState_Setup)
			{
				pTerm->pActiveCall->OnAlert();
			}



//////////////////////////////////////////////////////////////////////////
		
		m_pHandler->OnUATerminalInfoUpdate(this,pTerm,dwFlag);

		TRACEINFO("TERMINFO-UpdateInfo:exist=%d,term=%s,type=%d,subtype=%d,ip=%s,isChair=%d,chair=%s,conf=%s\n",
			it!=m_terminfoMap.end(),tp.szHostName,tp.nTermType,tp.nSubType,tp.IpAddr.ToStr(),tp.IsChairman,tp.szChairman,tp.szConfInfo);

	}

	return nTerm;
}


int SWUA::OnRecvMVSMsg(int msg,LPCTSTR szMsg,int size)
{
	static long tic=OSGetSysTick();
	//if(msg!=MVSMSG_LINKCHECK)
	TRACEMSG("RECV-- %s,size=%d,msginterval=%d\n",ESMvsMsg(msg),size,OSGetSysTick()-tic);
	tic=OSGetSysTick();
	m_linkCheck=3;
	switch(msg)
	{
	case MVSMSG_LOGIN:
		{
			TRACEMSG("INFO-Recv Host20:%s\n",szMsg);
			StopMsgTimer();
			if(m_curMsgState==MVSMSG_LOGIN && MVS_ParseHostInfo(szMsg,&m_hostInfo))
			{
				m_localIp=m_pSockControl->GetLocalIp();
				//if (m_hostInfo.szIpAddr[0]==0 || strcmp(m_hostInfo.szIpAddr,"0.0.0.0")==0)
				{
					strcpy(m_hostInfo.szIpAddr,m_localIp.ToStr());
				}
				MVS_AsignNodeInfo(&m_nodeInfo,&m_hostInfo);
				m_sendBuffer.Reset();
				m_recvBuffer.Reset();
				m_pSockControl->CloseSock();
				OnLogin(LoginRegNode);
				m_pSockControl->OpenSock(m_mvsIp,PortMessage,m_localIp);
				StartMsgTimer(MVSMSG_CONNECT_REGNODE,10);
				m_NatAudio.SetHostName(m_hostInfo.szHostName);
				m_NatVideo.SetHostName(m_hostInfo.szHostName);
				m_NatAudio.BindIp(m_localIp.ToStr());
				m_NatVideo.BindIp(m_localIp.ToStr());
				return 1;
			}
			Logout();
			OnLogin(LoginGetRegInfoFail);
			return 1;
		}
		break;
		//CALLSETUP
	case MVSMSG_REGNODE:
		{
			StopMsgTimer();
			CmdRegNode Cmd;ZeroVar(Cmd);
			Cmd.Parse(szMsg);
			m_strMvsName=Cmd.szMvsName;
			m_strIDName=m_strMvsName+"."+m_strName;
			strcpy(m_nodeInfo.PointAddress,Cmd.szMvsIp);

			m_nodeInfo.track=Cmd.track;
			NODEINFO* pNode=&m_nodeInfo;
			sprintf(pNode->Status,"SpeechAdmit");
			sprintf(pNode->Message,"发言已获批准");
			strcpy(pNode->PointAddress,pNode->IPAddress);
			pNode->timer=3;
			if (m_state==UA_Login)
			{
				m_pTermSelf=(VC3Terminal*)AddTerminal(m_strName.c_str());
				OnLogin(LoginSucess);
			}
			else if(m_state==UA_ReConnect)
			{
				OnReconnectMvsSuccess();
			}
			m_state=UA_Working;
			StartMsgTimer(MVSMSG_LINKCHECK,3);
			TRACEMSG("INFO- register node sucess,mvsname=%s,mvsip=%s,centerip=%s,cap=%p,mvsver=%d,!\n",
				Cmd.szMvsName,Cmd.szMvsIp,Cmd.szCenterIp,Cmd.dwMvsCapability,Cmd.dwMvsVersion);

			if (m_pHandler) {
				m_pHandler->OnUAStateChange(this, m_state);
			}
		}
		break;
		//CONBREAK
	case MVSMSG_REGNODE_ERROR:
		{
			StopMsgTimer();
			CmdRegNodeErr Cmd;ZeroVar(Cmd);
			Cmd.Parse(szMsg);
			Logout();
			OnLogin(LoginRegNodeFail);
			TRACEMSG("INFO- register node err=[%d]%s!\n",Cmd.Reason,Cmd.szReason);
		}
		break;
		//SENDCOMMAND
	case MVSMSG_SENDCOMMAND:
		{
			CmdSendCommand Cmd;ZeroVar(Cmd);
			Cmd.Parse(szMsg);
			TRACEMSG("RECV SENDCOMMAND:cmd=%s,lParam=%d,szParam=%s\n",GetVC3CmdName(Cmd.nCommandCode),Cmd.lParam,Cmd.szParam);
			do 
			{
				if (Cmd.nCommandCode==CMD_INSERTKEYFRAME)
				{
                    //					SWApi::InsertVideoKeyframe();
					break;
				}
				SWCall::OnCallMessage(this,&Cmd);
			} while (0);
			

		}
		break;
		//LINKCHECK
	case MVSMSG_LINKCHECK:
		{
		//	TRACEINFO("INFO- Recv LINKCHECK:tic=%d\n",GetSysTick());
	//		m_linkCheck=3;
		}
		break;
		//
	case MVSMSG_TERMINFOLIST:
		{
			MVSCmdMsgHdr* p=(MVSCmdMsgHdr*)szMsg;
			TRACEMSG("INFO-Recv TERMINFOLIST:cnt=%d,infosize=%d\n",p->ret,size);
			OnUpdateTermInfoList(p->ret,p->GetPacket(),size);
			m_sendBuffer.canSend=1;
			DoMVSMsgSend();

		}
		break;
	case MVSMSG_CMDRET:
		{
			MVSCmdMsgHdr* p=(MVSCmdMsgHdr*)szMsg;
			TRACEMSG("INFO-Recv CMDRET:cmdret=%d,%s\n",p->ret,p->GetPacket());
		}
		break;	
	case MVSMSG_CREATEAV:
		break;	
	case MVSMSG_DESTROYAV:
		break;	
		//
	case MVSMSG_PTZCTL:
		break;
		//CONJOINGROUP,加入组播组
	case MVSMSG_JOINGROUP:
		break;
		//CONEXITGROUP,退出组播组
	case MVSMSG_EXITGROUP:
		break;
		//SETSENDERFILTER,设置媒体ID:MVS通知终端准备接收媒体流{}
	case MVSMSG_SETSENDERFILTER:
		{
			CmdSetSenderFilter filter={0};
			filter.Parse(szMsg);
			TRACEMSG("OnMVSMsg_SETSENDERFILTER:av=%d,mid=%s,term=%s,flag=%d,param=%d\n",filter.av,filter.szId,filter.szHostName,filter.flag,filter.lParam);
			VC3Terminal* pTerm=(VC3Terminal*)GetTerminalByName(filter.szHostName);
			if((DWORD)pTerm->IpAddr==0)
			{
				pTerm->IpAddr=atoi(filter.szId);
			}
			m_pHandler->OnUAMessage(this, UAMsg_TerminalMediaOpen, filter.av, (long)(ITerminal*)pTerm);
		}
		break;	
	case MVSMSG_INSERTKEYFRAME:
		{
			CmdOther Cmd={0};
			Cmd.Parse(szMsg);
			TRACEMSG("OnMVSMsg_INSERTKEYFRAME:MultiIp=%s,Ip=%s,AudioIp=%s,DSIp=%s,Track=%c\n",
					 Cmd.szMultiAddr,Cmd.szAddr,Cmd.szAddrAudio,Cmd.szAddrData,Cmd.Track[0]);
            //			SWApi::InsertVideoKeyframe();
			m_pHandler->OnUAMessage(this, UAMsg_NetVideoNeedKeyFrame, 0, (long)Cmd.szAddr);
		}
		break;
		//CONSPEECHADMITCMD,打开音频
	case MVSMSG_ADMITSPEECH:
		{
			CmdOther Cmd={0};
			Cmd.Parse(szMsg);
			TRACEMSG("OnMVSMsg_ADMITSPEECH:MultiIp=%s,Ip=%s,AudioIp=%s,DSIp=%s,Track=%c\n",
				Cmd.szMultiAddr,Cmd.szAddr,Cmd.szAddrAudio,Cmd.szAddrData,Cmd.Track[0]);
			char* szAddr=Cmd.szAddrAudio;
			if(Cmd.Track[0]>='0' && Cmd.Track[0]<='9') szAddr=Cmd.szMultiAddr;
			m_pHandler->OnUAMessage(this, UAMsg_NetOpenMedia, 2, (long)szAddr);
		}
		break;
		//CONSPEECHCLOSECMD,关闭音频
	case MVSMSG_CLOSESPEECH:
		{
			CmdOther Cmd={0};
			Cmd.Parse(szMsg);
			TRACEMSG("OnMVSMsg_CLOSESPEECH:MultiIp=%s,Ip=%s,AudioIp=%s,DSIp=%s,Track=%c\n",
				Cmd.szMultiAddr,Cmd.szAddr,Cmd.szAddrAudio,Cmd.szAddrData,Cmd.Track[0]);
			m_pHandler->OnUAMessage(this, UAMsg_NetCloseMedia, 2, (long)Cmd.szAddr);
		}

		break;	
		//CONSHOWCMD,视频VS组播打开
	case MVSMSG_VIDEOGROUPBROAD:
		{
			CmdOther Cmd={0};
			Cmd.Parse(szMsg);
			TRACEMSG("OnMVSMsg_VIDEOGROUPBROAD:MultiIp=%s,Ip=%s,AudioIp=%s,DSIp=%s,Track=%c\n",
				Cmd.szMultiAddr,Cmd.szAddr,Cmd.szAddrAudio,Cmd.szAddrData,Cmd.Track[0]);
			m_pHandler->OnUAMessage(this, UAMsg_NetOpenMedia, 1, (long)Cmd.szMultiAddr);
		}
		break;
		//CMDPOINTCMD,视频单播打开
	case MVSMSG_VIDEOOPEN:
		{
			CmdOther Cmd={0};
			Cmd.Parse(szMsg);
			TRACEMSG("OnMVSMsg_VIDEOOPEN:MultiIp=%s,Ip=%s,AudioIp=%s,DSIp=%s,Track=%c\n",
				Cmd.szMultiAddr,Cmd.szAddr,Cmd.szAddrAudio,Cmd.szAddrData,Cmd.Track[0]);
			m_pHandler->OnUAMessage(this, UAMsg_NetOpenMedia, 1, (long)Cmd.szAddr);
		}
		break;
		//CMDSHOWCLOSECMD,视频VS组播/单播关闭
	case MVSMSG_VIDEOCLOSE:
		{
			CmdOther Cmd={0};
			Cmd.Parse(szMsg);
			TRACEMSG("OnMVSMsg_VIDEOCLOSE:MultiIp=%s,Ip=%s,AudioIp=%s,DSIp=%s,Track=%c\n",
				Cmd.szMultiAddr,Cmd.szAddr,Cmd.szAddrAudio,Cmd.szAddrData,Cmd.Track[0]);
			m_pHandler->OnUAMessage(this, UAMsg_NetCloseMedia, 1, (long)Cmd.szAddr);
		}
		break;	
	case MVSMSG_DATAGROUPBROAD:
		break;	
	case MVSMSG_DATAOPEN:
		break;	
	case MVSMSG_DATACLOSE:
		break;	
	case MVSMSG_REBOOT:
		break;	
	case MVSMSG_INVALID:
		break;	
	case MVSMSG_NONE:
		break;	
		//
	case MVSMSG_CONNECT_REGNODE:
		break;	
	case MVSMSG_CONNECT_LOGIN:
		break;	
	case MVSMSG_RECONNECT_REGNODE:
		break;	

	}
	return 0;
}

void SWUA::OnTimerTimeout(ITimer* hTimer,long name)
{
	TRACETIMER("TIMER-timeout:name=%s,linkcnt=%d\n",ESMvsMsg(name),m_linkCheck);
	if (name!=MVSMSG_LINKCHECK)
	{
		TRACETIMER("TIMER-timeout:name=%s,linkcnt=%d\n",ESMvsMsg(name),m_linkCheck);
		StopMsgTimer();
	}

	switch(name)
	{
	case MVSMSG_LINKCHECK:
		{
			m_linkCheck--;
			if (m_linkCheck<0)
			{
				StopMsgTimer();
				m_pSockControl->CloseSock();
				m_sendBuffer.Reset();
				m_recvBuffer.Reset();
				//m_pSockControl->OpenSock(m_mvsIp,PortMessage,m_localIp);
				StartMsgTimer(MVSMSG_RECONNECT_REGNODE,3);
				m_state=UA_ReConnect;
				if (m_pHandler) {
					m_pHandler->OnUAStateChange(this, m_state);
				}
			}
			else
			{
				SMHDR* phdr=&m_sendBuffer.GetHead<SMHDR>();
				SMHDR* pEnd=(SMHDR*)m_sendBuffer.GetTailPtr();
				while(phdr<pEnd)
				{
					if(phdr->type==MVSMSG_LINKCHECK) return ;
					phdr=(SMHDR*)(LPBYTE(phdr+1)+phdr->size);
				}

				TRACETIMER("TIMER-send link check timeout\n");
				SendMVSMsg(name,szLinkCheckSend , sizeof(szLinkCheckSend),0);

				m_NatVideo.MakeHoleToHost(m_hostInfo.szMultiAddr,7101);
				m_NatAudio.MakeHoleToHost(m_hostInfo.szMultiAddr,7101);

			}

		}
		break;
	case MVSMSG_LOGIN:
		{
			Logout();
			OnLogin(LoginGetRegInfoFail);
		}
		break;

	case MVSMSG_REGNODE:
		if (m_state==UA_Login)
		{
			Logout();
			OnLogin(LoginRegNodeFail);
		}
		else
		{
			StopMsgTimer();
			m_pSockControl->CloseSock();
			m_pSockControl->OpenSock(m_mvsIp,PortMessage,m_localIp);
			StartMsgTimer(MVSMSG_RECONNECT_REGNODE,3);
			m_state=UA_ReConnect;
			if (m_pHandler) {
				m_pHandler->OnUAStateChange(this, m_state);
			}
		}

		break;
	case MVSMSG_CONNECT_LOGIN:
		Logout();
		OnLogin(LoginGetRegInfoFail);
		break;
	case MVSMSG_CONNECT_REGNODE:
		if (m_state==UA_Login)
		{
			Logout();
			OnLogin(LoginRegNodeFail);
		}
		else
		{
			StopMsgTimer();
			m_pSockControl->CloseSock();
//			m_pSockControl->OpenSock(m_mvsIp,PortMessage,m_localIp);
			StartMsgTimer(MVSMSG_RECONNECT_REGNODE,3);
			m_state=UA_ReConnect;
			if (m_pHandler) {
				m_pHandler->OnUAStateChange(this, m_state);
			}
		}

		break;
	case MVSMSG_RECONNECT_REGNODE:
		StopMsgTimer();
		m_pSockControl->CloseSock();
		m_sendBuffer.Reset();
		m_recvBuffer.Reset();
		m_pSockControl->OpenSock(m_mvsIp,PortMessage,m_localIp);
		StartMsgTimer(MVSMSG_CONNECT_REGNODE,5);
		m_state=UA_ReConnect;
			if (m_pHandler) {
				m_pHandler->OnUAStateChange(this, m_state);
			}
		break;

	}
}

void SWUA::OnSockClose(int err,IDataSock* pSocket,long userParam)
{
	TRACE("OnSockClse--err=%d\n",err);
	if (m_state==UA_Login)
	{
		if(err==0) return;
		Logout();
		int err=LoginConnectFail;
		switch (m_curMsgState)
		{
		case MVSMSG_CONNECT_LOGIN:err=LoginConnectFail;break;
		case MVSMSG_CONNECT_REGNODE:
		case MVSMSG_LOGIN:err=LoginGetRegInfoFail;break;
		case MVSMSG_REGNODE:err=LoginRegNodeFail;break;
		}
		OnLogin(err);
	}
	else if (m_state==UA_Working || m_state==UA_ReConnect)
	{
		m_pSockControl->CloseSock();
		m_sendBuffer.Reset();
		m_recvBuffer.Reset();
		StartMsgTimer(MVSMSG_RECONNECT_REGNODE,3);
		m_state=UA_ReConnect;
		if (m_pHandler) {
			m_pHandler->OnUAStateChange(this, m_state);
		}
	}
	
	
}

//////////////////////////////////////////////////////////////////////////
int SWUA::SendMsgUpdateTerminalInfo(LPCTSTR szTermFilter)
{
	m_strTermFilter=szTermFilter;
	m_sendBuffer.PushVal(MVSMSG_TERMINFOLIST);
	int& size=m_sendBuffer.GetPushBuffer<int>();
	WORD& wSig=m_sendBuffer.GetPushBuffer<WORD>();
	wSig=0xfefe;
	m_sendBuffer.PushVal(1);//ret
	m_sendBuffer.PushVal(WORD(0));
	DWORD& packsize=m_sendBuffer.GetPushBuffer<DWORD>();
	m_sendBuffer.PushVal(FID_SWGetAllTerminalInfoOnceForAll);

	char szFilter[128];
	char* p=szFilter+sprintf(szFilter,"<version=\"2\"");
	if (szFilter && *szTermFilter)
	{
		p+=sprintf(p," %s",szTermFilter);
	}
	p+=sprintf(p,">");

	m_sendBuffer.PushString(szFilter);
	size=DWORD(m_sendBuffer.GetTailPtr()-(LPCTSTR)&wSig);

	TRACEMSG("UpdateTermInfoList:filter=%s\n",szFilter);
	packsize=size-8;
	DoMVSMsgSend();

	return 0;
}



int SWUA::SendCommand(LPCTSTR szTerminal,DWORD dwCommand,int lParam,LPCTSTR szParam,char* szOutBuffer)
{
//	TRACEINFO("---SENDCOMMAND:to=%s,cmd=%d,lp=%d,szParam=%s\n",szTerminal,dwCommand,lParam,szParam);
	m_sendBuffer.PushVal(MVSMSG_SENDCOMMAND);
	int& size=m_sendBuffer.GetPushBuffer<int>();
	WORD& wSig=m_sendBuffer.GetPushBuffer<WORD>();
	wSig=0xfefe;
	m_sendBuffer.PushVal(0);//ret
	WORD& packsize=m_sendBuffer.GetPushBuffer<WORD>();
	m_sendBuffer.PushVal(FID_SWSendCommand);
	m_sendBuffer.PushString(szTerminal);
	m_sendBuffer.PushVal(dwCommand);
	m_sendBuffer.PushVal(lParam);
	m_sendBuffer.PushString(szParam);
	size=DWORD(m_sendBuffer.GetTailPtr()-(LPCTSTR)&wSig);
	packsize=size-6;
	DoMVSMsgSend();
	return 0;	
}
int SWUA::CreateAVTransmission(int type,LPCTSTR szTerminalSrc,LPCTSTR szTerminalDst)
{
	m_sendBuffer.PushVal(MVSMSG_CREATEAV);
	int& size=m_sendBuffer.GetPushBuffer<int>();
	WORD& wSig=m_sendBuffer.GetPushBuffer<WORD>();
	wSig=0xfefe;
	m_sendBuffer.PushVal(0);//ret
	WORD& packsize=m_sendBuffer.GetPushBuffer<WORD>();
	m_sendBuffer.PushVal(FID_SWCreateAVTransmission);
	m_sendBuffer.PushVal(type);
	m_sendBuffer.PushString(szTerminalSrc);
	m_sendBuffer.PushString(szTerminalDst);
	size=DWORD(m_sendBuffer.GetTailPtr()-(LPCTSTR)&wSig);
	packsize=size-6;
	DoMVSMsgSend();
	return 0;
}
int SWUA::DestroyAVTransmission(int type,LPCTSTR szTerminalSrc,LPCTSTR szTerminalDst)
{
	m_sendBuffer.PushVal(MVSMSG_DESTROYAV);
	int& size=m_sendBuffer.GetPushBuffer<int>();
	WORD& wSig=m_sendBuffer.GetPushBuffer<WORD>();
	wSig=0xfefe;
	m_sendBuffer.PushVal(0);//ret
	WORD& packsize=m_sendBuffer.GetPushBuffer<WORD>();
	m_sendBuffer.PushVal(FID_SWDestroyAVTransmission);
	m_sendBuffer.PushVal(type);
	m_sendBuffer.PushString(szTerminalSrc);
	m_sendBuffer.PushString(szTerminalDst);
	size=DWORD(m_sendBuffer.GetTailPtr()-(LPCTSTR)&wSig);
	packsize=size-6;
	DoMVSMsgSend();
	return 0;	

}
int SWUA::PTZControl(LPCTSTR szTerminal, DWORD dwControlType,int nDeviceNo, int nAction, int nMode, char* szExtend)
{
	m_sendBuffer.PushVal(MVSMSG_PTZCTL);
	int& size=m_sendBuffer.GetPushBuffer<int>();
	WORD& wSig=m_sendBuffer.GetPushBuffer<WORD>();
	wSig=0xfefe;
	m_sendBuffer.PushVal(0);//ret
	WORD& packsize=m_sendBuffer.GetPushBuffer<WORD>();
	int fid=FID_SWRmtControlYT;
	if (dwControlType==SW_VC3_PTZ_YUNTAI)
	{
		nAction&=0x3fffffff;
		if(nAction<PTZ_YUTAIRESET)
		{
			fid=FID_SWRmtControlCamera;
		}
		else if (nAction<PTZ_MAX_CMD)
		{
			nAction-=PTZ_YUTAIRESET;
		}
	}
	else if(dwControlType==SW_VC3_PTZ_SWITCH)
	{
		fid=FID_SWRmtControlSwitch;
		nAction-=PTZ_SWITCH_OFFSET;
	}
	else if (dwControlType==SW_VC3_PTZ_DIRECT)
	{
		fid=FID_SWRmtPTZControlDirect;
	}
	
	m_sendBuffer.PushString(szTerminal);
	if (dwControlType!=SW_VC3_PTZ_DIRECT)
	{
		if(dwControlType!=SW_VC3_PTZ_SWITCH)
		{
			m_sendBuffer.PushVal(nDeviceNo);
		}
		m_sendBuffer.PushVal(nAction);
		m_sendBuffer.PushVal(nMode);
	}
	else
	{
		m_sendBuffer.PushData(szExtend+4,*LPINT(szExtend));
	}
	size=DWORD(m_sendBuffer.GetTailPtr()-(LPCTSTR)&wSig);
	packsize=size-6;
	DoMVSMsgSend();
	return 0;	

}


int SWUA::SendTermVideo( ITerminal* pTermTo,int iChannel/*=0*/ )
{
    //	CreateAVTransmission(MediaTypeVideo,GetGlobalName(),pTermTo->Name());
	return 0;
}

int SWUA::SendTermAudio(ITerminal* pTermTo,int iChannel)
{
	return 0;
};

int SWUA::OpenTermMedia(ITerminal* pTerm,int type,int channel)
{
	if (!pTerm) {
		TRACEERR("OpenTermMedia:err,term is null\n");
		return 1;
	}
	VC3Terminal* pt=ConvertToDerivedPtr<VC3Terminal>(pTerm);
    if(type&1)
	{
		CreateAVTransmission(1,pTerm->Name(),GetGlobalName());
		pt->MediaState.VideoRecv=1;

	}
	if(type&2)
	{
		CreateAVTransmission(2,pTerm->Name(),GetGlobalName());
		pt->MediaState.AudioRecv=1;

	}
	return 0;

}
int SWUA::CloseTermMedia(ITerminal* pTerm,int type,int channel)
{
	if (!pTerm) {
		TRACEERR("CloseTermMedia:err,term is null\n");
		return 1;
	}
	VC3Terminal* pt=ConvertToDerivedPtr<VC3Terminal>(pTerm);
	ASSERT(pTerm);
	if(type&1)
	{
		DestroyAVTransmission(1,pTerm->Name(),GetGlobalName());
		pt->MediaState.VideoRecv=0;
	}
	if(type&2)
	{
		DestroyAVTransmission(2,pTerm->Name(),GetGlobalName());
		pt->MediaState.AudioRecv=0;
	}
	return 0;

}
int SWUA::OpenMediaByStreamId(DWORD sid,int type,int channel)
{
	VC3Terminal* pTerm=GetTermByStreamId(sid);
	if (!pTerm) {
		TRACEERR("OpenMediaByStreamId,term is null\n");
		return 1;
	}
	ASSERT(pTerm);
	TRACEMEDIA("OpenMedia:term=%s,type=%d,sid=%p\n",pTerm->Name(),type,sid);
	return OpenTermMedia(pTerm,type,channel);	
}
int SWUA::CloseMediaByStreamId(DWORD sid,int type,int channel)
{
	VC3Terminal* pTerm=GetTermByStreamId(sid);
	if (!pTerm) {
		TRACEERR("CloseMediaByStreamId,term is null\n");
		return 1;
	}
	ASSERT(pTerm);
	TRACEMEDIA("Media:term=%s,type=%d,sid=%p\n",pTerm->Name(),type,sid);
	return CloseTermMedia(pTerm,type,channel);	
}

void SWUA::OnReconnectMvsSuccess()
{
	//recreate media stream
	for (int i=0;i<m_termList.GetSize();i++)
	{
		VC3Terminal& term=*m_termList[i];
		if(term.MediaState.VideoRecv)
		{
			TRACEMEDIA("OnReconnectMvsSuccess:ReOpen Video ,term=%s\n",term.Name());
			OpenTermMedia(&term, 1);
		}
		if (term.MediaState.AudioRecv) {
			TRACEMEDIA("OnReconnectMvsSuccess:ReOpen Audio ,term=%s\n",term.Name());
			OpenTermMedia(&term, 2);
		}
	}

}



};//end namespace SWApi
