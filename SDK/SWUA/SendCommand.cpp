#include "__PCH.h"
#include "SendCommand.h"
#include "TermInfo.h"
namespace SWApi{

const char *g_szLocCmdName[]={
	"CMD_ENABLEMIXLOCALAUDIOWITHRECEIVED",
	"CMD_SETMULTIDISPSRCADDR",
	"CMD_ENABLESENDVIDEORECEIVED",
	"CMD_ENABLERECORDMIXWITHLOCALAUDIO",
	"CMD_SNAPSHOTPICTURE",
	"CMD_CREATE_DIRECTP2PAV",
	"CMD_DESTROY_DIRECTP2PAV",
	"CMD_GETSTATUS_DIRECTP2PAV",
	"CMD_SETALKBACK_PEERHOSTINFO",
	"CMD_CLEARTALKBACK_PEERHOSTINFO",
	"CMD_DISABLESPEECHOFVIDEOCHANNEL",
	"CMD_GETSTATUS_VIDEOCHANNEL",
	"CMD_GET_SUBTERMTYPE",
	"CMD_STARTRECORD",
	"CMD_STOPRECORD",
	"CMD_GET_RECORDSTATE",
	"CMD_SETSINGLEDISPSRCADDR",
	"CMD_SELECTVIDEOSOURCEINPUT",
	"CMD_GET_TERMTYPE",
	"CMD_SETDISPLAYCOUNT",
	"CMD_GETDISPLAYCOUNT",
	"CMD_HOTKEYTYPE",
	"CMD_DELAYQUITPROCESS",
	""

};
//1001-1004
const char *g_szMcuCmdName[]={
	"CMD_CLEARALLSENDFROMME",
	"CMD_SETTERMTYPEONMCU",
	"CMD_SETTERMLEVELONMCU",
	"CMD_REGISTERASCOMMANDER",
	""

};
//103-127
const  char *g_szCmdCmdName[]={
	"CMD_CMDMODE_CALL",//103
	"CMD_CMDMODE_ACCEPT_CALL",//104
	"CMD_CMDMODE_REJECT_CALL",//105
	"CMD_CMDMODE_LEAVE",//107
	"CMD_CMDMODE_ACCEPT_LEAVE",//108,
	"CMD_CMDMODE_REJECT_LEAVE",
	"CMD_CMDMODE_STOP",
	"CMD_CMDMODE_EXIT",
	"CMD_CMDMODE_PULSE",
	"CMD_CMDMODE_HAND",
	"CMD_CMDMODE_ACCEPT_HAND",
	"CMD_CMDMODE_REJECT_HAND",
	"CMD_CMDMODE_PULSE_RESPONSE",
	"CMD_CMDMODE_ASKJOINCOMMAND",
	"CMD_CMDMODE_REJECT_ASK",
	"CMD_CMDMODE_CANCEL_ASK_JOIN",
	"CMD_CMDMODE_STOP_ASK_JOIN",
	"CMD_CMDMODE_CANCEL_HAND",	
	"CMD_INTERTALK_START",
	"CMD_INTERTALK_ACCEPT_START",
	"CMD_INTERTALK_REJECT_START",
	"CMD_INTERTALK_STOP",
	"CMD_INTERTALK_BACKSTOP",
	"CMD_INTERTALK_PULSE",
	"CMD_INTERTALK_PULSE_RESPONSE",
	"CMD_INTERTALK_REPORT",
	"CMD_INTERTALK_REJECT_REPORT",
	"CMD_INTERTALK_CANCEL_REPORT",//130
};

//203-219
const  char *g_szConCmdName[]={
	"CMD_CONMODE_CALL",//203
	"CMD_CONMODE_ACCEPT_CALL",
	"CMD_CONMODE_REJECT_CALL",
	"CMD_CONMODE_LEAVE",
	"CMD_CONMODE_ACCEPT_LEAVE",
	"CMD_CONMODE_REJECT_LEAVE",
	"CMD_CONMODE_STOP",
	"CMD_CONMODE_EXIT",
	"CMD_CONMODE_PULSE",
	"CMD_CONMODE_HAND",
	"CMD_CONMODE_ACCEPT_HAND",
	"CMD_CONMODE_REJECT_HAND",
	"CMD_CONMODE_PULSE_RESPONSE",
	"CMD_CONMODE_ASKJOINCONFERENCE",
	"CMD_CONMODE_REJECT_ASK",
	"CMD_CONMODE_CANCEL_ASK_JOIN",
	"CMD_CONMODE_STOP_ASK_JOIN",
	"CMD_CMDMODE_CANCEL_HAND",//220

};
//143-155
const char *g_szSpeechCmdName[]={
	"CMD_LOUDSPEECH_CALL",//143
	"CMD_LOUDSPEECH_ACCEPT_CALL",	
	"CMD_LOUDSPEECH_REJECT_CALL",
	"",	
	"",	
	"",	
	"CMD_LOUDSPEECH_STOP",
	"CMD_LOUDSPEECH_EXIT",
	"CMD_LOUDSPEECH_PULSE",
	"",	
	"",	
	"",	
	"CMD_LOUDSPEECH_PULSE_RESPONSE",//155
	""

};

const  char* g_szRecordCmdName[]={
	"CMD_RECORDSERVER_CALL",
	"CMD_RECORDSERVER_BROAD",
	"CMD_RECORDSERVER_STOP",
	"CMD_RECORDSERVER_ACCEPT_CALL",
	"CMD_RECORDSERVER_REJECT_CALL",
	"CMD_RECORDSERVER_PULSE",
	"CMD_RECORDSERVER_PULSE_RESPONSE",
	"CMD_RECORDSERVER_EXIT",
	"",
};

const char* g_szInfoCmdName[]={
	"CMD_ATTENDLIST",
	"CMD_GENERAL_CALLMSG",
	"CMD_DATASTREAMSWITCH",
	"CMD_DATACONFERENCESWITCH",
	"CMD_REPORT_CAPBILITY",
	"CMD_CLEARECHO",
	"",
};

const char* g_szLocalCmdName[]=
{
	"SW_VC3_CMD_LOCALHOST_ENABLEMIXLOCALAUDIOWITHRECEIVED",	
	"SW_VC3_CMD_LOCALHOST_SETMULTIDISPSRCADDR",	
	"SW_VC3_CMD_LOCALHOST_ENABLESENDVIDEORECEIVED",	
	"SW_VC3_CMD_LOCALHOST_ENABLERECORDMIXWITHLOCALAUDIO",	
	"SW_VC3_CMD_LOCALHOST_SNAPSHOTPICTURE",	
	"SW_VC3_CMD_LOCALHOST_CREATE_DIRECTP2PAV",	
	"SW_VC3_CMD_LOCALHOST_DESTROY_DIRECTP2PAV",	
	"SW_VC3_CMD_LOCALHOST_GETSTATUS_DIRECTP2PAV",	
	"SW_VC3_CMD_LOCALHOST_SETALKBACK_PEERHOSTINFO",	
	"SW_VC3_CMD_LOCALHOST_CLEARTALKBACK_PEERHOSTINFO",	
	"SW_VC3_CMD_LOCALHOST_DISABLESPEECHOFVIDEOCHANNEL",	
	"SW_VC3_CMD_LOCALHOST_GETSTATUS_VIDEOCHANNEL",	
	"SW_VC3_CMD_LOCALHOST_GET_SUBTERMTYPE",	
	"SW_VC3_CMD_LOCALHOST_STARTRECORD",	
	"SW_VC3_CMD_LOCALHOST_STOPRECORD",	
	"SW_VC3_CMD_LOCALHOST_GET_RECORDSTATE",	
	"SW_VC3_CMD_LOCALHOST_SETSINGLEDISPSRCADDR",	
	"SW_VC3_CMD_LOCALHOST_SELECTVIDEOSOURCEINPUT",	
	"SW_VC3_CMD_LOCALHOST_GET_TERMTYPE",	
	"SW_VC3_CMD_LOCALHOST_SETDISPLAYCOUNT",	
	"SW_VC3_CMD_LOCALHOST_GETDISPLAYCOUNT",	
	"SW_VC3_CMD_LOCALHOST_HOTKEYTYPE",	
	"SW_VC3_CMD_LOCALHOST_DELAYQUITPROCESS",	
	"SW_VC3_CMD_LOCALHOST_REMOVEMULTIDISPSRCADDR",	
	"SW_VC3_CMD_LOCALHOST_REMOVETALKBACK_PEERHOSTINFO",	
	"SW_VC3_CMD_LOCALHOST_GETCURRENTINDEX_BYPOSITION",	
	"SW_VC3_CMD_LOCALHOST_SETLOCALCAPTION",	
	"SW_VC3_CMD_LOCALHOST_SET_MAPVIDEOWND",	
	"SW_VC3_CMD_LOCALHOST_SHOW_VIDEOWND",	
	"SW_VC3_CMD_LOCALHOST_SET_EXTERNVIDEO_MODE",	
	"SW_VC3_CMD_LOCALHOST_SET_VIDEOCAPTURECOUNT",	
	"SW_VC3_CMD_LOCALHOST_SET_VIDEOCAPTUREDEVINFO",	
	"SW_VC3_CMD_LOCALHOST_RESET_AUDIO_DEVICE",	
	"SW_VC3_CMD_LOCALHOST_DISABLE_FUNCTION",	
	"SW_VC3_CMD_LOCALHOST_SET_EXTERNVIDEOWND",	
	"SW_VC3_CMD_LOCALHOST_SET_EXTERNDSIPSRCADDR",	
	"SW_VC3_CMD_LOCALHOST_SET_UPER_AUDIODIVIDE",	
	"SW_VC3_CMD_LOCALHOST_SET_DOWN_AUDIODIVIDE",	
	"SW_VC3_CMD_LOCALHOST_SET_TITLEPOSITION",	
	"SW_VC3_CMD_LOCALHOST_ORIGINAL_SIZE_DISPLAY",	
	"SW_VC3_CMD_LOCALHOST_START_ORIGINAL_SIZE_DISPLAY",	
	"SW_VC3_CMD_LOCALHOST_STOP_ORIGINAL_SIZE_DISPLAY",	
	"SW_VC3_CMD_LOCALHOST_SET_UPER_AUDIOVOLUME",	
	"SW_VC3_CMD_LOCALHOST_SET_SMALLVIDEOWND",	
	"SW_VC3_CMD_LOCALHOST_SET_VIDEO_DISPLAY_MODE",	
	"SW_VC3_CMD_LOCALHOST_GET_MAX_COMMAND",	
	"SW_VC3_CMD_LOCALHOST_SET_AUDIO_SUPPORTAEC",	
	"SW_VC3_CMD_LOCALHOST_SET_AUDIO_USEAEC",	
	"SW_VC3_CMD_LOCALHOST_GET_AUDIO_USEAEC",	
	"SW_VC3_CMD_LOCALHOST_SET_AUDIO_USENOISE",	
	"SW_VC3_CMD_LOCALHOST_GET_AUDIO_USENOISE",	
	"SW_VC3_CMD_LOCALHOST_SET_AUDIO_WAVEVOLUME",	
	"SW_VC3_CMD_LOCALHOST_GET_AUDIO_WAVEVOLUME",	
	"SW_VC3_CMD_LOCALHOST_SET_AUDIO_EQALIZER",	
	"SW_VC3_CMD_LOCALHOST_GET_AUDIO_EQALIZER",	
	"SW_VC3_CMD_LOCALHOST_SET_VIDEO_KEEPSSCALE",	
	"SW_VC3_CMD_LOCALHOST_GET_VIDEO_KEEPSSCALE",	
	"SW_VC3_CMD_LOCALHOST_SET_AUDIO_DEVICE_DETECT",	
	"SW_VC3_CMD_LOCALHOST_GET_AUDIO_DETECT_DATASTATUS",	
	"SW_VC3_CMD_LOCALHOST_GET_AUDIO_DEVICEID",	
	"SW_VC3_CMD_LOCALHOST_GET_CURRENT_AUDIO_DEVICEID",	
	"SW_VC3_CMD_LOCALHOST_SET_AUDIO_DEVICEID",	
	"SW_VC3_CMD_LOCALHOST_SET_QOS_PARAMETER",	
	"SW_VC3_CMD_LOCALHOST_GET_AVFLUX_BYDSPCHANNEL",	
	"SW_VC3_CMD_LOCALHOST_GET_VIDEO_RESOLUTION_COUNT",	
	"SW_VC3_CMD_LOCALHOST_GET_VIDEO_RESOLUTION",	
	"SW_VC3_CMD_LOCALHOST_SET_VIDEO_RESOLUTION",	
	"SW_VC3_CMD_LOCALHOST_SET_FULLSCREEN",	
	"SW_VC3_CMD_LOCALHOST_SET_MULTMODE",	
	"SW_VC3_CMD_LOCALHOST_SET_INDEPENDENCEWND",	
	"SW_VC3_CMD_LOCALHOST_GET_TITLEFONTTYPE",	
	"SW_VC3_CMD_LOCALHOST_SET_TITLEFONTTYPE",	
	"SW_VC3_CMD_LOCALHOST_SET_ENABLEBACKGROUND",	
	"SW_VC3_CMD_LOCALHOST_SET_VERTICALPOSITION",	
	"SW_VC3_CMD_LOCALHOST_SET_BACKGROUNDCOLOR",	
	"SW_VC3_CMD_LOCALHOST_SET_CAPTIONSIZE",	
	"SW_VC3_CMD_LOCALHOST_SET_MULTIDISP_TERMINFO",	
	"SW_VC3_CMD_LOCALHOST_SET_VDOENCODE_CHANINFO",	
	"SW_VC3_CMD_LOCALHOST_SET_USER_PRIVATE_DATA",	
	"SW_VC3_CMD_LOCALHOST_SET_ADOMIX_EXCLUDE_TERMLIST",	
	"SW_VC3_CMD_LOCALHOST_GET_ENCODESTATUS",	
	"SW_VC3_CMD_LOCALHOST_GET_SENDBANDWIDTH",	
	"SW_VC3_CMD_LOCALHOST_GET_RECVBANDWIDTH",	
	"SW_VC3_CMD_LOCALHOST_GET_LOSTPACKETPERCENT",	
	"SW_VC3_CMD_LOCALHOST_SET_INACTIVESTATUS",	
	"SW_VC3_CMD_LOCALHOST_SET_APPVERSION",
	"SW_VC3_CMD_LOCALHOST_SET_SUPPORTACTIVED",	
	"SW_VC3_CMD_LOCALHOST_SET_SHOWDIAGNOSTIC",	
	"SW_VC3_CMD_LOCALHOST_GET_VIDEO_DISPLAY_MODE",	
	"SW_VC3_CMD_LOCALHOST_GET_MVSCAPABILITY",	
	"SW_VC3_CMD_LOCALHOST_SET_MVSMSGCALLBACK",	

};
#define CMDLOCAL_FIRST 1101
#define CMDLOCAL_LAST (sizeof(g_szLocalCmdName)/sizeof(char*)+CMDLOCAL_FIRST-1)


LPCTSTR GetVC3CmdName(int cmd)
{
	static char buf[16];
	if(cmd>=103 && cmd<=130) return g_szCmdCmdName[cmd-103];
	if(cmd>=203 && cmd<=220) return g_szConCmdName[cmd-203];
	if(cmd>=143 && cmd<=155) return g_szSpeechCmdName[cmd-143];
	if(cmd>=300 && cmd<=305) return g_szInfoCmdName[cmd-300];
	if (cmd>=501 && cmd<=508) return g_szRecordCmdName[cmd-501];
	if (cmd>=1001 && cmd<=1004) return g_szMcuCmdName[cmd-1001];
	if (cmd>=CMDLOCAL_FIRST && cmd<=CMDLOCAL_LAST) return g_szLocalCmdName[cmd-CMDLOCAL_FIRST];
	switch (cmd)
	{
	case 600:return "CMD_CONFSTATUS";
	case 610:return "CMD_DATACONFMSG";
	case 611:return "CMD_TERMCONFIG";
	case 810:return "CMD_SIPPHONEMSG";
	case 815:return "CMD_EXTENDPANEL_IND";
	case 816:return "CMD_EXTENDPANEL_RESP";
	case 821:return "CMD_INSERTKEYFRAME";
	case 902:return "CMD_FORMATED_REPORT";
	case 903:return "CMD_FORMATED_REPORT_RESP";
	}
	sprintf(buf,"unknown[%d]",cmd);
	return buf;
}


int FormatConfInfoNotifyMsg(String& csMsg,int op,LPCTSTR szOldName,LPCTSTR szNewName,time_t start,time_t duration)//,LPCTSTR szTimeSchedule,LPCTSTR szDuration)
{

/*
	//会议信息:发送者,操作{Add,Del,Rename},主席,会议名称[新名称|原名称],预约时间,
	csMsg.Format("%s,%d,%s,%s%s%s,%s,%d",
		g_pNet->GetLocalTerm()->GetGlobalIdName(),
		op,
		g_pNet->GetLocalTerm()->GetGlobalIdName(),
		szNewName,
		(op==2)?"|":"",
		(op==2)?szOldName:"",
		TimeToStr(start),
		duration
		);
*/
	return 0;
	
}

int ParseConfInfoMsg( LPCTSTR szMsg,String& csChair,int& op,String& csOldName,String& csNewName,time_t& start,time_t duration )
{

/*
	CStringArray caInfo;
	int n=::SplitString(szMsg,caInfo,',');
	if (n>=6)
	{
		csChair=caInfo[2];
		op=atoi(caInfo[1]);
		int f=caInfo[3].Find('|');
		if (f<0)
		{
			csOldName="";
			csNewName=caInfo[3];
		}
		else
		{
			csNewName=caInfo[3].Left(f);
			csOldName=caInfo[3].Mid(f+1);
		}
		start=StrToTime(caInfo[4]);
		duration=atoi(caInfo[5]);
	}
*/
	return 0;
}


MSG_TERMOPENAV::MSG_TERMOPENAV(bool bOpen, int type,struct TERMINAL* pTerm )
{
	//csMsg.Format("%s,%d,%d,%s,%s",g_pNet->GetLocalTerm()->GetGlobalIdName(),bOpen,type,pTerm->GetGlobalIdName(),pTerm->csIP);
}

TERMOPENAV_MSG::TERMOPENAV_MSG( LPCTSTR szMsg )
{
/*
	bOpen=0;
	type=-1;
	pTerm=0;
	pSendTerm=0;
	char szTerm[64];
	char szSender[64];
	char szIp[32];
	if(sscanf(szMsg,"%[^,],%d,%d,%[^,],%[^,]",szSender,&bOpen,&type,szTerm,szIp)>4)
	{
		pSendTerm=(LPTERMINAL)g_pNet->GetTermID(szSender);
		pTerm=(LPTERMINAL)g_pNet->GetTermID(szTerm);
		if (pTerm->csIP=="")
		{
			pTerm->csIP=szIp;
		}
	}
*/
}


MSG_TERMOPENAV_RET::MSG_TERMOPENAV_RET( int ret,int type,struct TERMINAL* pTerm )
{
	//csMsg.Format("%s,%d,%d,%s,%s",g_pNet->GetLocalTerm()->GetGlobalIdName(),ret,type,pTerm->GetGlobalIdName(),g_pNet->GetLocalTerm()->GetGlobalIdName());
}

TERMOPENAV_MSG_RET::TERMOPENAV_MSG_RET( LPCTSTR szMsg )
{
/*
	ret=-1;
	type=-1;
	pTerm=0;
	pSendTerm=0;
	char szTerm[64];
	char szSender[32];
	if(sscanf(szMsg,"%[^,],%d,%d,%[^,],%[^,]",szSender,&ret,&type,szTerm)>=4)
	{
		pTerm=(LPTERMINAL)g_pNet->GetTermID(szTerm);
		pSendTerm=(LPTERMINAL)g_pNet->GetTermID(szSender);
	}
*/
}

int FormatTermConfigMsg(char* szMsg,LPCTSTR szFrom,DWORD dwVersion,TIMEOUTLEN* tmout,LPCTSTR szIpAddr,LPCTSTR szOwner,int level,DWORD dwCapbility)
{
	char* p=szMsg;
	p+=sprintf(p,"%s|Version=%d",szFrom,dwVersion);
	if(dwVersion==0)
	{
		p+=sprintf(p,";timeout=62000,62000,30000,62000,62000,62000");
	}
	if(*szIpAddr) p+=sprintf(p,";IPAddr=%s",szIpAddr);
	if(*szOwner)
	{
		p+=sprintf(p,";OwnerName=%s",szOwner);
	}
	p+=sprintf(p,";Level=%d",level);
	if(dwCapbility!=DWORD(-1)) p+=sprintf(p,";Capability=%d",dwCapbility);
	return int(p-szMsg)+1;

}
int ParseTermConfigMsg(LPCTSTR szMsg,char* szFrom,DWORD& dwVersion,TIMEOUTLEN* tmout,char* szIpAddr,char* szOwner,int& level,DWORD& dwCapbility)
{
	*szFrom=0;
	dwVersion=0;
	ZeroMemory(tmout,sizeof(TIMEOUTLEN));
	*szIpAddr=0;
	*szOwner=0;
	level=-1;
	dwCapbility=-1;

	char* caTmp[16]={0};

	int n=SplitStr((char*)szMsg,caTmp,16,';');
	int i=0;
	for (i=0;i<n;i++)
	{
		char* szKey=caTmp[i];
		char* szVal=strchr(szKey,'=');
		if(szKey==NULL) continue;
		*szVal=0;
		szVal++;
		String csKey=szKey;
		if (csKey=="Version")
		{
			dwVersion=atoi(szVal);	
		}
		else if (csKey=="timeout")
		{
			sscanf(szVal,"%d,%d,%d,%d,%d,%d",
				&tmout->lenArr[0],
				&tmout->lenArr[1],
				&tmout->lenArr[2],
				&tmout->lenArr[3],
				&tmout->lenArr[4],
				&tmout->lenArr[5]
			);

		}
		else if (csKey=="IPAddr")
		{
			strcpy(szIpAddr,szVal);
		}
		else if (csKey=="OwnerName")
		{
			strcpy(szOwner,szVal);
		}
		else if (csKey=="Level")
		{
			level=atoi(szVal);
		}
		else if (csKey=="Capability")
		{
			dwCapbility=atoi(szVal);
		}
	}

	return 0;
}


};//end namespace SWApi;