// GenericCallProtocol.cpp: implementation of the GenericCallProtocol class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "__PCH.h"
#include "GenericCallProtocol.h"


namespace SWApi{
#define SIZEOFARRAY(arr) (sizeof(arr)/sizeof(arr[0]))
//////////////////////////////////////////////////////////////////////
#define SZMSGCALL "Call"
#define SZMSGCANCEL "Cancel"
#define SZMSGACCEPT "Accept"
#define SZMSGREJECT "Reject"
#define SZMSGSTOP "Stop"
#define SZMSGEXIT "Exit"
#define SZMSGSTATUS "Status"
#define SZMSGSTATUSACK "SatausAck"

static const char* g_szMsgNames[]={
	SZMSGCALL,// "Call"
	SZMSGCANCEL,// "Cancel"
	SZMSGACCEPT,// "Accept"
	SZMSGREJECT,// "Reject"
	SZMSGSTOP,// "Stop"
	SZMSGEXIT,// "Exit"
	SZMSGSTATUS,// "Status"
	SZMSGSTATUSACK,// "SatausAck"

};
#define MSGNAME_COUNT (sizeof(g_szMsgNames)/sizeof(g_szMsgNames[0]))

#define SZKEYMSGNAME "MSGName"
#define SZKEYCALLID "CallID"
#define SZKEYSENDER "Sender"
#define SZKEYPURPOSE "Purpose"
#define SZKEYAPPLICATION "Application"
#define SZKEYCAUSE "Cause"
#define SZKEYRESULT "Result"
#define SZKEYSTATUS "Status"
#define SZKEYKEEPALIVE "KeepAlive"
#define SZKEYINFOTYPE "InfoType"
#define SZKEYINFO "Info"

static const char* g_szKeys[]={
	SZKEYMSGNAME,
	SZKEYCALLID,
	SZKEYSENDER,
	SZKEYPURPOSE,
	SZKEYAPPLICATION,
	SZKEYCAUSE,
	SZKEYRESULT,
	SZKEYSTATUS,
	SZKEYKEEPALIVE,
	SZKEYINFOTYPE,
	SZKEYINFO,
	
};

#define SZCREATE "Create"
#define SZJOIN "Join"
#define SZREQUEST "Request"


static const char* g_szPurposes[]={SZCREATE,SZJOIN,SZREQUEST};

#define PURPOSE_COUNT (sizeof(g_szPurposes)/sizeof(g_szPurposes[0]))


#define  SZIDLE "Idle"
#define  SZSURVEIL "Surveil"
#define  SZTALK "Talk"
#define  SZCOMMAND "Command"
#define  SZCONFERENCE "Conference"

static const char* g_szApplication[]=
{
//	SZIDLE,// "Idle"
	SZSURVEIL,// "Surveil"
	SZTALK,// "Talk"
	SZCOMMAND,// "Command"
	SZCONFERENCE,// "Conference"

};

#define APPLICATION_COUNT (sizeof(g_szApplication)/sizeof(g_szApplication[0]))


static const char* g_szStatus[]={
	SZIDLE,SZSURVEIL,SZTALK,SZCOMMAND,SZCONFERENCE,
};

#define STATUS_COUNT (sizeof(g_szStatus)/sizeof(g_szStatus[0]))


#define SZREQTYPECREATE "Create"
#define SZREQTYPEHAND	"Hand"
#define SZREQTYPELEAVE	"Leave"

static const char* g_szReqTypes[]={SZREQTYPECREATE,SZREQTYPEHAND,SZREQTYPELEAVE};

#define REQTYPE_COUNT SIZEOFARRAY(g_szReqTypes)



#define SEPI "\r\n"
#define SEPV "="
#define SEPVCH '='

#define ISNUMBER(str) (!str[0] || (str[0]>='0' && str[0]<='9'))

#define IFNUMRET(str) if(ISNUMBER(str)) return atoi(str);
//////////////////////////////////////////////////////////////////////////
LPCTSTR EnumMsgNameToStr(int iMsgName)
{
	if (iMsgName<0 || iMsgName>=MSGNAME_COUNT)
	{
		return "";
	}
	return g_szMsgNames[iMsgName];
}

int StrMsgNameToEnum(LPCTSTR szMsgName)
{
	IFNUMRET(szMsgName);
	for (int i=0;i<MSGNAME_COUNT;i++)
	{
		if (strcmp(szMsgName,g_szMsgNames[i])==0)
		{
			return i;
		}
	}
	return -1;
}

LPCTSTR EnumPurposToStr(int iPurpose)
{
	if (iPurpose<0 || iPurpose>=PURPOSE_COUNT)
	{
		return "";
	}
	return g_szPurposes[iPurpose];
}

int StrPurposeToEnum(LPCTSTR szPurpose)
{
	IFNUMRET(szPurpose);
	for (int i=0;i<PURPOSE_COUNT;i++)
	{
		if (strcmp(szPurpose,g_szPurposes[i])==0)
		{
			return i;
		}
	}
	return -1;
}

LPCTSTR EnumApplicationToStr(int iApp)
{
	if (iApp<0 || iApp>=APPLICATION_COUNT)
	{
		return "";
	}
	return g_szApplication[iApp];
}

int StrApplicationToEnum(LPCTSTR szApp)
{
	IFNUMRET(szApp);
	for (int i=0;i<APPLICATION_COUNT;i++)
	{
		if (strcmp(szApp,g_szApplication[i])==0)
		{
			return i;
		}
	}
	return -1;
}

LPCTSTR EnumStatusToStr(int iStatus)
{
	if (iStatus<0 || iStatus>=STATUS_COUNT)
	{
		return "";
	}
	return g_szStatus[iStatus];
	
}

int StrStatusToEnum(LPCTSTR szStatus)
{
	IFNUMRET(szStatus);
	for (int i=0;i<STATUS_COUNT;i++)
	{
		if (strcmp(szStatus,g_szStatus[i])==0)
		{
			return i;
		}
	}
	return -1;
}
LPCTSTR EnumReqTypeToStr(int iType)
{
	if (iType<0 || iType>=REQTYPE_COUNT)
	{
		return "";
	}
	return g_szReqTypes[iType];
	
}

int StrReqTypeToEnum(LPCTSTR szReqType)
{
	IFNUMRET(szReqType);
	for (int i=0;i<REQTYPE_COUNT;i++)
	{
		if (strcmp(szReqType,g_szReqTypes[i])==0)
		{
			return i;
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////

#if 0
extern int FormatKeyStr(LPCTSTR szKeyEsc,LPCTSTR szValueEsc,char* szBuf,int size,...);
#else
int FormatKeyStr(LPCTSTR szKeyEsc,LPCTSTR szValueEsc,char* szBuf,int size,...)
{
	va_list va;
	va_start(va,size);
	char* p=szBuf;
	int l1=strlen(szKeyEsc);
	int l2=strlen(szValueEsc);
	if (!l1|| !l2)
	{
		ASSERT(0);
		return 0;
	}
	LPCTSTR arg=va_arg(va,LPCTSTR);
	do
	{
		if ((p-szBuf+l1+l2+strlen(arg))>=(UINT)size)
		{
			ASSERT(0);
			break;
		}
		p+=sprintf(p,"%s%s",arg,szKeyEsc);
		arg=va_arg(va,LPCTSTR);
		if ((p-szBuf+l1+l2+strlen(arg))>=(UINT)size)
		{
			ASSERT(0);
			break;
		}
		if(arg==0) arg="";
		p+=sprintf(p,"%s%s",arg,szValueEsc);
		arg=va_arg(va,LPCTSTR);
	}while(arg);
	va_end(va);
	return p-szBuf;
}

#endif
//////////////////////////////////////////////////////////////////////////
GenericCallMsgParser::GenericCallMsgParser(LPCTSTR szMessage,int* pRet)
{
	int ret=Parse(szMessage);
	if (pRet)
	{
		*pRet=ret;
	}
}

GenericCallMsgParser::~GenericCallMsgParser()
{
}

int GenericCallMsgParser::Parse(LPCTSTR szMessage)
{
	ZeroMemory(m_Buf,GENERALCALL_MSG_MAXSIZE);
	int i=0;
	for (i=0;i<GC_KEYIDX_COUNT;i++)
	{
		m_szValues[i]="";
	}
	if (!szMessage)
	{
		return 0;
	}
	strncpy(m_Buf,szMessage,GENERALCALL_MSG_MAXSIZE-1);

	int nKey=0;

	char* pLine=m_Buf;
//	char* pNextLine=NULL;
	while (pLine && *pLine)	
	{
		char* pNextLine=strstr(pLine,SEPI);
		//找到换行位,换行位置零,调整指针到下一行开始
		if (pNextLine)
		{
			*pNextLine=0;
			pNextLine+=2;
		}
		char* pName=pLine;
		char* pValue=strstr(pLine,SEPV);
		//行中无分界符,消息错误
		if (pValue==NULL)
		{
			return 0;
		}
		//调整指针到正确的'值'起始位置
		*pValue=0;
		pValue++;

		//设置key的value值
		for (i=0;i<GC_KEYIDX_COUNT;i++)
		{
			if (strcmp(pName,g_szKeys[i])==0)
			{
				nKey++;
				m_szValues[i]=pValue;
			}
		}

		pLine=pNextLine;
	}
	
	return nKey;
}
int GenericCallMsgParser::MSGName()
{
	return StrMsgNameToEnum(m_szValues[GC_KEYIDX_MSGNAME]);

}

DWORD GenericCallMsgParser::CallID()
{
	return atoi(m_szValues[GC_KEYIDX_CALLID]);
}

LPCTSTR GenericCallMsgParser::Sender()
{
	return m_szValues[GC_KEYIDX_SENDER];
}

int GenericCallMsgParser::Purpose()
{
	return StrPurposeToEnum(m_szValues[GC_KEYIDX_PURPOSE]);
}

int GenericCallMsgParser::Application()
{
	return StrApplicationToEnum(m_szValues[GC_KEYIDX_APPLICATION]);
}

int GenericCallMsgParser::Cause()
{
	return atoi(m_szValues[GC_KEYIDX_CAUSE]);
}

int GenericCallMsgParser::Result()
{
	return atoi(m_szValues[GC_KEYIDX_RESULT]);
}

int GenericCallMsgParser::Status()
{
	return StrStatusToEnum(m_szValues[GC_KEYIDX_STATUS]);
}

int GenericCallMsgParser::KeepAlive()
{
	return atoi(m_szValues[GC_KEYIDX_KEEPALIVE]);
}
 
int GenericCallMsgParser::InfoType()
{
	return StrReqTypeToEnum(m_szValues[GC_KEYIDX_INFOTYPE]);
}
LPCTSTR GenericCallMsgParser::szInfo()
{
	return m_szValues[GC_KEYIDX_INFO];
}
 
//////////////////////////////////////////////////////////////////////////

GenericCallMsgGenerator::GenericCallMsgGenerator()
{
	ZeroMemory(m_Buf,GENERALCALL_MSG_MAXSIZE);
}

GenericCallMsgGenerator::~GenericCallMsgGenerator()
{
}


#define BEGNIN_FORMAT FormatKeyStr(SEPV,SEPI,m_Buf,GENERALCALL_MSG_MAXSIZE,

#define END_FORMAT ,0);
  
LPCTSTR GenericCallMsgGenerator::FormatCallMsg(LPCTSTR szCallID,LPCTSTR szSender,LPCTSTR szPurpose,LPCTSTR szApplication,LPCTSTR szInfoType,LPCTSTR szInfo)
{
	if (szInfo==0) szInfo="";
	BEGNIN_FORMAT
		SZKEYMSGNAME,SZMSGCALL,
		SZKEYCALLID,szCallID,
		SZKEYSENDER,szSender,
		SZKEYPURPOSE,szPurpose,
		SZKEYAPPLICATION,szApplication,
		SZKEYINFOTYPE,szInfoType,
		SZKEYINFO,szInfo
	END_FORMAT

	return m_Buf;
}

LPCTSTR GenericCallMsgGenerator::FormatCancelMsg(LPCTSTR szCallID,LPCTSTR szSender)
{
	BEGNIN_FORMAT
		SZKEYMSGNAME,SZMSGCANCEL,
		SZKEYCALLID,szCallID,
		SZKEYSENDER,szSender
	END_FORMAT
	return m_Buf;
}

LPCTSTR GenericCallMsgGenerator::FormatAcceptMsg(LPCTSTR szCallID,LPCTSTR szSender)
{
	BEGNIN_FORMAT
		SZKEYMSGNAME,SZMSGACCEPT,
		SZKEYCALLID,szCallID,
		SZKEYSENDER,szSender
	END_FORMAT
	return m_Buf;
}

LPCTSTR GenericCallMsgGenerator::FormatRejectMsg(LPCTSTR szCallID,LPCTSTR szSender,LPCTSTR szCause)
{
	BEGNIN_FORMAT
		SZKEYMSGNAME,SZMSGREJECT,
		SZKEYCALLID,szCallID,
		SZKEYSENDER,szSender,
		SZKEYCAUSE,szCause
	END_FORMAT
	return m_Buf;
}

LPCTSTR GenericCallMsgGenerator::FormatStopMsg(LPCTSTR szCallID,LPCTSTR szSender)
{
	BEGNIN_FORMAT
		SZKEYMSGNAME,SZMSGSTOP,
		SZKEYCALLID,szCallID,
		SZKEYSENDER,szSender
	END_FORMAT
	return m_Buf;
}

LPCTSTR GenericCallMsgGenerator::FormatExitMsg(LPCTSTR szCallID,LPCTSTR szSender,LPCTSTR szResult)
{
	BEGNIN_FORMAT
		SZKEYMSGNAME,SZMSGEXIT,
		SZKEYCALLID,szCallID,
		SZKEYSENDER,szSender,
		SZKEYRESULT,szResult
	END_FORMAT
	return m_Buf;
}

LPCTSTR GenericCallMsgGenerator::FormatStatusCheckMsg(LPCTSTR szSender,LPCTSTR szStatus,LPCTSTR szKeepAlive)
{
	BEGNIN_FORMAT
		SZKEYMSGNAME,SZMSGSTATUS,
		SZKEYSENDER,szSender,
		SZKEYSTATUS,szStatus,
		SZKEYKEEPALIVE,szKeepAlive
	END_FORMAT
	return m_Buf;
}

LPCTSTR GenericCallMsgGenerator::FormatStatusCheckAckMsg(LPCTSTR szSender,LPCTSTR szStatus,LPCTSTR szKeepAlive)
{
	BEGNIN_FORMAT
		SZKEYMSGNAME,SZMSGSTATUSACK,
		SZKEYSENDER,szSender,
		SZKEYSTATUS,szStatus,
		SZKEYKEEPALIVE,szKeepAlive
	END_FORMAT
	return m_Buf;
}

};//end namespace SWApi
