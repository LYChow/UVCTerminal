#include "__PCH.h"
#include "SendCommand.h"
#include "GenericCallProtocol.h"
#include "SWUA.h"
#include "ISWCall.h"

namespace SWApi{


#define ItoaDec(v) LPCTSTR(ConvStr("%u",v))	//(itoa(v,g_strBuf,10))

_BEGIN_IMPL_ENUMSTR(ESCallType)
"CallType_None",	
"CallType_VideoTalk",	
"CallType_VideoConfInvite",	
"CallType_VideoConfJoin",	

_END_IMPL_ENUMSTR

_BEGIN_IMPL_ENUMSTR(ESCallDir)
"CallDir_OutGoing",	
"CallDir_InComming",	
_END_IMPL_ENUMSTR

_BEGIN_IMPL_ENUMSTR(ESCallState)
"CallState_Idle",	
"CallState_Setup",	
"CallState_Alert",	
"CallState_Connect",	
"CallState_Release",	

_END_IMPL_ENUMSTR

_BEGIN_IMPL_ENUMSTR(ESCallMsg)
"CallMsg_Setup = 0",	
"CallMsg_Cancel",	
"CallMsg_Accept",	
"CallMsg_Reject",	
"CallMsg_Release",	
"CallMsg_ReleaseComplete",	
"CallMsg_Alive",	
"CallMsg_AliveAck",	
"CallMsg_Alert",	
_END_IMPL_ENUMSTR

_BEGIN_IMPL_ENUMSTR(ESCallResult)
"CallResult_Success",	
"CallResult_Canceled",	
"CallResult_Busy",	
"CallResult_Offline",	
"CallResult_Reject",	
"CallResult_Missed",	
"CallResult_LossConnection",	
"CallResult_ErrorKnown",	
_END_IMPL_ENUMSTR


void SWCall::OnTimerTimeout( ITimer* hTimer,long userParam )
{
	//int state=int(userParam);
//	TRACE("TIMER-CallTimeout:state=%d\n",userParam)
	TRACEERR("TIMER-CallTimeout:state=%s,timer=%s,alive=%d\n",ESCallState(state),ESCallState(userParam),pPeer->CallAlive);
	if (state!=int(userParam))
	{
		TRACEERR("ERR-CallTimeout:state=%d,timer=%ld\n",state,userParam);
	}
	SWCallCmd cmd(pUA);
	if(state!=CallState_Connect) StopMsgTimer();
	if(dir==CallDir_InComming)
	{
		switch(state)
		{
		case CallState_Idle:
			break;	
		case CallState_Setup:
		case CallState_Alert:
			EndCall(CallResult_Missed);
			pUA->m_pHandler->OnUACall(pUA,this,CallMsg_Cancel,result);
			break;	
		case CallState_Connect:
			if (pPeer->pActiveCall==this)
			{
				pPeer->CallAlive--;

			}
			if (pPeer->CallAlive<0)
			{
				result=CallResult_LossConnection;
				cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,CallMsg_ReleaseComplete,(dwCallId),pUA->GetGlobalName(),result);
				OnReleaseComplete(result);
			}
			break;	
		case CallState_Release:
			break;	
		}

	}
	else
	{
		switch(state)
		{
		case CallState_Idle:
			break;	
		case CallState_Setup:
			OnReject(CallResult_Offline);
			break;	
		case CallState_Alert:
			OnReject(CallResult_Missed);
			cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,CallMsg_Cancel,(dwCallId),pUA->GetGlobalName(),result);

			break;	
		case CallState_Connect:
			if (pPeer->pActiveCall==this)
			{
				pPeer->CallAlive--;

			}
			if (pPeer->CallAlive<0)
			{
				OnReleaseComplete(CallResult_LossConnection);
				cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,CallMsg_Release,(dwCallId),pUA->GetGlobalName(),result);
			}
			else
			{
				cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,CallMsg_Alive,(dwCallId),pUA->GetGlobalName(),pPeer->GetActiveTimeout(type));
			}
			break;	
		case CallState_Release:
			OnReleaseComplete(CallResult_Success);
			break;	
		}

	}
}

void SWCall::EndCall(int result)
{
	if(state!=CallState_Idle)
	{
		if(pPeer->pActiveCall==this) pPeer->pActiveCall=0;
		EndMsgTimer();
		state=CallState_Idle;
		result=result;
		tmEnd=OSGetSysTick();
	}
}


void SWCall::OnSetup(DWORD CallId,long lParam)
{
	if(state!=CallState_Idle) return;
	pPeer->pActiveCall=this;
	SWCallCmd cmd(pUA);
	dwCallId=CallId;
	dir=CallDir_InComming;
	cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,CallMsg_Alert,(0),pUA->GetGlobalName());
	state=CallState_Alert;
	StartMsgTimer(state,pPeer->GetCallTimeout(type));
	pUA->m_pHandler->OnUACall(pUA,this,CallMsg_Setup,lParam);

}
void SWCall::OnCancel()
{
	if (state!=CallState_Setup&&state!=CallState_Alert)
	{
		return ;
	}
	EndCall(CallResult_Canceled);
	pUA->m_pHandler->OnUACall(pUA,this,CallMsg_Cancel,0);

}

void SWCall::OnAlert()
{
	if (state==CallState_Setup)
	{
		state=CallState_Alert;
		StartMsgTimer(state,pPeer->GetCallTimeout(type));
		pUA->m_pHandler->OnUACall(pUA,this,CallMsg_Alert,0);
	}
	
}
void SWCall::OnAccept()
{
	StopMsgTimer();
	if (state==CallState_Alert || state==CallState_Setup)
	{
		state=CallState_Connect;
		StartMsgTimer(state,pPeer->GetActiveTimeout(type));
		pPeer->CallAlive=3;
		SWCallCmd cmd(pUA);
		cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,CallMsg_Alive,(dwCallId),pUA->GetGlobalName(),pPeer->GetActiveTimeout(type));
		pUA->m_pHandler->OnUACall(pUA,this,CallMsg_Accept,0);
	}
	
}
void SWCall::OnReject(int cause)
{
	EndCall(result);
	pUA->m_pHandler->OnUACall(pUA,this,CallMsg_Reject,cause);
}
void SWCall::OnRelease()
{
	if (pPeer->callProctol!=NETPROTOCOL_GENERAL && state!=CallState_Connect)
	{
		OnCancel();
		return;
	}
	if (state==CallState_Connect)
	{
		result=CallResult_Success;
	}
	pUA->m_pHandler->OnUACall(pUA,this,CallMsg_Release,0);
	state=CallState_Idle;
	SWCallCmd cmd(pUA);
	cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,CallMsg_ReleaseComplete,(dwCallId),pUA->GetGlobalName(),result);
	tmEnd=OSGetSysTick();
	EndMsgTimer();

}
void SWCall::OnReleaseComplete(int ret)
{
	if(state!=CallState_Connect && state!=CallState_Release)
	{
		return;
	}
	EndCall(ret);
	pUA->m_pHandler->OnUACall(pUA,this,CallMsg_ReleaseComplete,ret);
}

int SWCall::Setup(long lParam)
{
	if (!this)
	{
		return -1;
	}
	
	if(state!=CallState_Idle || pPeer->pActiveCall)
	{
		return CallResult_Busy;
	}
	dir=CallDir_OutGoing;
	state=CallState_Setup;
	tmStart=OSGetSysTick();
	pPeer->pActiveCall=this;
	pUA->UpdateTerminalInfo(pPeer->Name());
	StartMsgTimer(state,5);
	SWCallCmd cmd(pUA);
	cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,CallMsg_Setup,(dwCallId),pUA->GetGlobalName());
	return 0;

}

int SWCall::Cancel()
{
	if (!this)
	{
		return -1;
	}
	if (state!=CallState_Setup&&state!=CallState_Alert)
	{
		return -1;
	}
	EndCall(CallResult_Canceled);
	SWCallCmd cmd(pUA);
	cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,CallMsg_Cancel,(dwCallId),pUA->GetGlobalName());
	return 0;
}

int SWCall::Accept()
{
	if (this==0)
	{
		return -1;
	}
	if (state!=CallState_Setup&&state!=CallState_Alert && dir!=CallDir_InComming)
	{
		return -1;
	}
	pPeer->CallAlive=3;
	state=CallState_Connect;
	StartMsgTimer(state,pPeer->GetActiveTimeout(type));
	SWCallCmd cmd(pUA);
	cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,CallMsg_Accept,(dwCallId),pUA->GetGlobalName());
//	cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,CallMsg_Alive,ConvStr(dwCallId),pUA->GetGlobalName());
	return 0;
}
int SWCall::Reject(int cause,LPCTSTR szCause)
{
	if (this==0)
	{
		return -1;
	}
	if (state!=CallState_Setup&&state!=CallState_Alert && dir!=CallDir_InComming)
	{
		return -1;
	}
	EndCall(CallResult_Reject);
	
	SWCallCmd cmd(pUA);
	cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,CallMsg_Reject,(dwCallId),pUA->GetGlobalName(),cause,szCause);
	return 0;
}

int SWCall::ReleaseCall(int ret)
{
	if (this==0)
	{
		return -1;
	}
	if (state!=CallState_Connect)
	{
		return Cancel();
	}
	int msg;
	if (dir==CallDir_InComming)
	{
		msg=CallMsg_ReleaseComplete;
		EndCall(CallResult_Success);
	}
	else
	{
		msg=CallMsg_Release;
		state=CallState_Release;
		StartMsgTimer(state,3);
	}
	SWCallCmd cmd(pUA);
	cmd.MakeAndSend(pPeer->Name(),pPeer->callProctol,type,msg,(dwCallId),pUA->GetGlobalName(),ret,"");

	return 0;
}

FindCallById& findCallById(DWORD CallId,int Dir=-1){static FindCallById f(0,0);f.dwCallId=CallId;f.dir=Dir;return f;}
FindCallByInfo& findCallByInfo(ITerminal* pTerm,int Type=-1,int State=-1,int Dir=-1)
{
	static FindCallByInfo f(0);
	f.type=Type;f.dir=Dir;f.state=State;f.pterm=pTerm;
	return f;
}


bool SWCall::OnCallMessage( SWUA* pUA,CmdSendCommand* pCmd )
{
	CallMessag CallMsg;
	SWCallCmd cmd(pUA);
	if (!CallMsg.ParseMessage(pCmd))
	{
		return 0;
	}
	TRACEINFO("CALL-RECV MSG:msg=%s\n",ESCallMsg(CallMsg.msg));

	SWCall* pCall=0;
	int i=0;
	SWCallList* pCallList=pUA->GetCallList();
	VC3Terminal* pTerm=(VC3Terminal*)pUA->GetTerminalByName(CallMsg.szFrom);
	if (!pTerm)
	{
		return 1;
	}
	switch(CallMsg.msg)
	{
	case CallMsg_Setup:
		pTerm->callProctol=(WORD)CallMsg.proct;
		pCall=SWCall::CreateCall(pUA,pTerm,CallMsg.type);
		if (pCall->state==CallState_Idle && pTerm->pActiveCall==0)
		{
			pCall->OnSetup(CallMsg.dwCallId,CallMsg.lParam);
		}
		else
		{
			cmd.MakeAndSend(pTerm->Name(),pTerm->callProctol,CallMsg.type,CallMsg_Reject,(CallMsg.dwCallId),pUA->GetGlobalName(),CallResult_Busy);

		}
		break;	
	case CallMsg_Cancel:
	case CallMsg_Release:
		if (CallMsg.proct==NETPROTOCOL_GENERAL)
		{
			i=pCallList->FindCall(&findCallById(CallMsg.dwCallId,CallDir_InComming));
		}
		else
		{
			i=pCallList->FindCall(&findCallByInfo(pTerm,CallMsg.type,-1,CallDir_InComming));
		}
		if (i>=0)
		{
			switch(CallMsg.msg)
			{
			case CallMsg_Cancel:
				pCallList->GetCall(i)->OnCancel();break;
			case CallMsg_Release:
				pCallList->GetCall(i)->OnRelease();break;
//			case CallMsg_Alive:
//				pCallList->GetCall(i)->OnAlive(CallMsg.lParam);break;
			}
		}
		break;	
	case CallMsg_Alert:
	case CallMsg_Accept:
	case CallMsg_Reject:
	case CallMsg_ReleaseComplete:
		if (CallMsg.proct==NETPROTOCOL_GENERAL)
		{
			i=pCallList->FindCall(&findCallById(CallMsg.dwCallId,CallDir_OutGoing));
		}
		else
		{
			i=pCallList->FindCall(&findCallByInfo(pTerm,CallMsg.type,-1,CallDir_OutGoing));
		}
//		i=pCallList->FindCall(&FindCallById(CallMsg.dwCallId,CallDir_OutGoing));
		if (i>=0)
		{
			switch(CallMsg.msg)
			{
			case CallMsg_Alert:
				pCallList->GetCall(i)->OnAlert();break;
			case CallMsg_Accept:
				pCallList->GetCall(i)->OnAccept();break;
			case CallMsg_Reject:
				pCallList->GetCall(i)->OnReject(CallMsg.lParam);break;
			case CallMsg_ReleaseComplete:
				pCallList->GetCall(i)->OnReleaseComplete(CallMsg.lParam);break;
			}

		}

		break;
	case CallMsg_Alive:
			cmd.MakeAndSend(pTerm->Name(),CallMsg.proct,CallMsg.type,CallMsg_AliveAck,(CallMsg.dwCallId),pUA->GetGlobalName(),(int)CallMsg.lParam,CallMsg.szInfo);
	case CallMsg_AliveAck:
//		i=pCallList->FindCall(&FindCallByInfo(pTerm,-1,CallState_Connect,CallDir_OutGoing));
//		if(i>=0) pCallList->GetCall(i)->OnAliveAck(CallMsg.lParam);break;
		pTerm->CallAlive=3;
		break;


	}
	return 1;
}

SWCall* SWCall::CreateCall( SWUA* pUA,VC3Terminal* pTerm,int type)
{
	SWCall* pCall=pUA->GetCallList()->AddCall(pUA);
	if(pCall->state==CallState_Idle)
	{
		pCall->type=type;pCall->pPeer=pTerm;
	}
	return pCall;
}



//template<>
bool SWCallCmd::MakeCallMessage(CallMessag& callMsg)
{
	return MakeCallMessage(callMsg.szTo,callMsg.proct,callMsg.type,callMsg.msg,callMsg.dwCallId,callMsg.szFrom,callMsg.lParam,callMsg.szInfo);
}
bool MakeOldMessage(SWCallCmd* pCmd,LPCTSTR szTo,int proct,int type,int msg,DWORD dwCallId,LPCTSTR szFrom,int lParam,LPCTSTR szInfo)
{
	switch(type)
	{
	case CallType_VideoTalk:
		pCmd->lParam=dwCallId;
		strcpy(pCmd->szParam,szFrom);
		switch (msg)
		{
		case CallMsg_Setup:
			pCmd->dwCommandCode=CMD_INTERTALK_START;
			break;	
		case CallMsg_Alert:
		//	pCmd->dwCommandCode=CMD_INTERTALK_START;
			return 0;
			break;	
		case CallMsg_Accept:
			pCmd->dwCommandCode=CMD_INTERTALK_ACCEPT_START;
			break;	
		case CallMsg_Reject:
			pCmd->dwCommandCode=CMD_INTERTALK_REJECT_START;
			break;	
		case CallMsg_Cancel:
		case CallMsg_Release:
			pCmd->dwCommandCode=CMD_INTERTALK_STOP;
			break;	
		case CallMsg_ReleaseComplete:
			pCmd->dwCommandCode=CMD_INTERTALK_BACKSTOP;
			break;	
		case CallMsg_Alive:
			pCmd->dwCommandCode=CMD_INTERTALK_PULSE;
			break; 
		case CallMsg_AliveAck:
			pCmd->dwCommandCode=CMD_INTERTALK_PULSE_RESPONSE;
			break;
		}
		break;
	default:
		break;
	}

	return 1;
}
//template<>
bool SWCallCmd::MakeCallMessage(LPCTSTR szTo,int proct,int type,int msg,DWORD dwCallId,LPCTSTR szFrom,int lParam,LPCTSTR szInfo)
{
	strcpy(this->szTermName,szTo);
	if (proct!=CMD_GENERAL_CALLMSG)
	{
		return MakeOldMessage(this,szTo,proct,type,msg,dwCallId,szFrom,lParam,szInfo);
	}
	this->dwCommandCode=CMD_GENERAL_CALLMSG;
	this->lParam=0;
	GenericCallMsgGenerator GCMsgGen;
	switch(type)
	{
	case CallType_VideoTalk:
		switch (msg)
		{
		case CallMsg_Setup:
			GCMsgGen.FormatCallMsg(ConvStr(dwCallId),szFrom,EnumPurposToStr(PURPOSEIDX_CREATE),EnumApplicationToStr(APPLICATIONIDX_TALK),"0","");
			break;	
		case CallMsg_Cancel:
			GCMsgGen.FormatCancelMsg(ConvStr(dwCallId),szFrom);
			break;	
		case CallMsg_Alert:
			return 0;
			break;	
		case CallMsg_Accept:
			GCMsgGen.FormatAcceptMsg(ConvStr(dwCallId),szFrom);
			break;	
		case CallMsg_Reject:
			GCMsgGen.FormatRejectMsg(ConvStr(dwCallId),szFrom,szInfo);
			break;	
		case CallMsg_Release:
			GCMsgGen.FormatStopMsg(ConvStr(dwCallId),szFrom);
			break;
		case CallMsg_ReleaseComplete:
			GCMsgGen.FormatExitMsg(ConvStr(dwCallId),szFrom,szInfo);
			break;
		case CallMsg_Alive:
			GCMsgGen.FormatStatusCheckMsg(szFrom,EnumStatusToStr(STATUSIDX_TALK),ItoaDec(lParam));
			break;
		case CallMsg_AliveAck:
			GCMsgGen.FormatStatusCheckAckMsg(szFrom,EnumStatusToStr(STATUSIDX_TALK),ItoaDec(lParam));
			break;
		default:
			return 0;
		}
		break;
	default:
		break;
	}
	strcpy(szParam,GCMsgGen);

	return 1;
}
//template<>
int SWCallCmd::Send()
{
	return pUA->SendCommand(szTermName,dwCommandCode,lParam,szParam);
}



bool ParseOldMessage(CmdSendCommand* pCmd,CallMessag& callMsg)
{
	callMsg.proct=NETPROTOCOL_PC;
	callMsg.type=CallType_VideoTalk;
	callMsg.szFrom=pCmd->szParam;
	callMsg.dwCallId=pCmd->lParam;
	callMsg.lParam=pCmd->lParam;
	//sprintf(callMsg.szCallId,"%d",pCmd->lParam);
	char* p=strchr((char*)pCmd->szParam,',');
	if(p)
	{
		*p=0;
	}
	int ret=1;
	switch(pCmd->nCommandCode)
	{
	case CMD_INTERTALK_START:
		callMsg.msg=CallMsg_Setup;
		break;	
	case CMD_INTERTALK_ACCEPT_START:
		callMsg.msg=CallMsg_Accept;
		break;	
	case CMD_INTERTALK_REJECT_START:
		callMsg.msg=CallMsg_Reject;
		break;	
	case CMD_INTERTALK_STOP:
		callMsg.msg=CallMsg_Release;
		break;	
	case CMD_INTERTALK_BACKSTOP:
		callMsg.msg=CallMsg_ReleaseComplete;
		break;	
	case CMD_INTERTALK_PULSE:
		callMsg.msg=CallMsg_Alive;
		break;	
	case CMD_INTERTALK_PULSE_RESPONSE:
		callMsg.msg=CallMsg_AliveAck;
		break;	
	case CMD_INTERTALK_REPORT:
	case CMD_INTERTALK_REJECT_REPORT:
	case CMD_INTERTALK_CANCEL_REPORT:
	default:
		callMsg.msg=-1;
		ret=0;
		break;

	}
	return ret;
}
bool CallMessag::ParseMessage(CmdSendCommand* pCmd)
{
	ZeroVar(*this);
	msg=-1;
	if (pCmd->nCommandCode!=CMD_GENERAL_CALLMSG)
	{
		return ParseOldMessage(pCmd,*this);
	}
	int ret=0;
	static GenericCallMsgParser msgParser;
	ret=msgParser.Parse(pCmd->szParam);
	proct=NETPROTOCOL_GENERAL;
	switch(msgParser.Application())
	{
	case APPLICATIONIDX_TALK:
		type=CallType_VideoTalk;
		break;
	default:
//		return 1;
		break;
	}
	msg=msgParser.MSGName();
	dwCallId=atoi(msgParser.m_szValues[GC_KEYIDX_CALLID]);
	szFrom=msgParser.m_szValues[GC_KEYIDX_SENDER];
	switch(msg)
	{
	case CallMsg_Setup:
		szInfo=msgParser.m_szValues[GC_KEYIDX_INFO];
		break;	
	case CallMsg_Cancel:
		szInfo=msgParser.m_szValues[GC_KEYIDX_CAUSE];
		break;	
	case CallMsg_Alert:
		break;	
	case CallMsg_Accept:
		break;	
	case CallMsg_Reject:
		szInfo=msgParser.m_szValues[GC_KEYIDX_CAUSE];
		break;	
	case CallMsg_Release:
		break;
	case CallMsg_ReleaseComplete:
		szInfo=msgParser.m_szValues[GC_KEYIDX_RESULT];
		break;
	case CallMsg_Alive:
	case CallMsg_AliveAck:
		switch(msgParser.Status())
		{
		case STATUSIDX_TALK:type=CallType_VideoTalk;break;
		}
		lParam=msgParser.KeepAlive();
		break;
	default:
		msg=-1;
		break;

	}
	return 1;
}


};//end namespace SWApi