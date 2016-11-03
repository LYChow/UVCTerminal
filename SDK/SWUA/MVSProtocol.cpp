#include "__PCH.h"
#include "NetLib.h"	//3.网络
#include "MVSProtocol.h"
namespace SWApi{

_NodeStatus::_NodeStatus()
{
	memset(this,0,sizeof(*this));
	track = -1; TermLevel = 99; 
	NodeInfoEx.dwVersion=240;
	BitrateInfo.BitrateVideoEnc=2048; 
	BitrateInfo.BitrateVideoDec=768;
	NodeInfoEx2.dwAppVersion=APPVERSION();

	NodeInfoEx.TermSubType = 0;//g_nTermSubType;
	NodeInfoEx.dwProductID = 0;//GetProductSerialNumber();
	NodeInfoEx.NetCondition =0;// GetNetCondition();
	NodeInfoEx.bSupportSystemScan = 0;//1;
	NodeInfoEx.bSupportInactiveOnlineState = 0;//g_nSupportInactiveOnlineState&0x00000001 > 0? true: false;
	NodeInfoEx.bInactiveOnlineInitially = 0;//(g_nSupportInactiveOnlineState&0x00010000) > 0? true: false;
	NodeInfoEx.bSupportActivedByCmd301 = 0;//g_bSupportActivedByCmd301? true: false;
	NodeInfoEx.bExcludeWhenSenderClearAV = 0;//g_bExcludeWhenSenderClearAV? true: false;
	NodeInfoEx.bSupportNewClearAVMode = 0;//g_bSupportNewClearAVMode? true: false;


}


/*
LPCTSTR ESMvsCmd(UINT cmd)
{
	const char* szMVSCmd[]= 
	{
		"DEVICECHECK",			//0x00000000
		"CALLSETUPCMD",			//0x00000001
		"CONBREAK",				//0x00000002
		"CONSPEECHADMITSTATE",	//0x00000004
		"CONSHOWADMITSTATE",	//0x00000008
		"CONPOINTADMITSTATE",	//0x00000010	"CONREJECTCMD",			//0x00000010
		"CONINVITECMD",			//0x00000020
		"CONJOINCMD",			//0x00000040
		"CONEXITCMD",			//0x00000080
		"CONAPPLYCMD",			//0x00000100
		"CONSPEECHCMD",			//0x00000200
		"CONSHOWCMD",			//0x00000400
		"CONPOINTCMD",			//0x00000800
		"CONSPEECHEXITCMD",		//0x00001000
		"CONSPEECHCLOSECMD",	//0x00002000
		"CONSPEECHADMITCMD",	//0x00004000	
		"CONSPEECHREJECTCMD",	//0x00008000	
		"CONSHOWCLOSECMD",		//0x00010000	
		"YUNTAICONTROLCMD",		//0x00020000	
		"CONSHUTDOWNCMD",		//0x00040000	
		"CONREGCMD",			//0x00080000
		"CONGETCMD",			//0x00100000
		"CONREPORTCMD",			//0x00200000
		"CONBANDWIDTHCMD",		//0x00400000
		"CONTROLINVITECMD",		//0x00800000
		"DISPLAYSWITCHCMD",		//0x01000000
		"CONZIMUCMD",			//0x02000000
		"SETSENDERFILTER",		//0x04000000
		"CONMULTIDISPLAYPOINTCMD",//0x08000000
		"CONJOINGROUP",			//0x10000000
		"CONEXITGROUP",			//0x20000000
		"SENDCOMMAND",			//0x40000000
		"INSERTKEYFRAME",		//0x80000000
	};
	int i=0;
	if (cmd)
	{
		i=1;
		while ((cmd&1)==0)
		{
			i++;
			cmd>>=1;
		}
	}
	return szMVSCmd[i];
};

LPCTSTR ESMvsMsg(int msg)
{
	const char* szMvsMsg[]={"MVSMSG_NONE","MVSMSG_LINKCHECK","MVSMSG_RESP","MVSMSG_CMD"};
	if(i<0 || i>MVSMSG_CNT) return "MVSMSG_Unknown";
	return szMvsMsg[i];
}
}
*/

_BEGIN_IMPL_ENUMSTR(ESMvsMsg)
	"MVSMSG_NONE",	
	"MVSMSG_CONNECT_LOGIN",
	"MVSMSG_LOGIN",	
	"MVSMSG_CONNECT_REGNODE",
	"MVSMSG_CONNECT_REGNODE_ERR",
	"MVSMSG_REGNODE",	
	"MVSMSG_LINKCHECK",
	"MVSMSG_CMDRET",
	"MVSMSG_TERMINFOLIST",	
	"MVSMSG_SENDCOMMAND",	
	"MVSMSG_CREATEAV",	
	"MVSMSG_DESTROYAV",	
	"MVSMSG_PTZCTL",
	"MVSMSG_JOINGROUP",	
	"MVSMSG_EXITGROUP",	
	"MVSMSG_SETSENDERFILTER",	
	"MVSMSG_INSERTKEYFRAME",	
	"MVSMSG_ADMITSPEECH",	
	"MVSMSG_CLOSESPEECH",	
	//CONSHOWCMD
	"MVSMSG_VIDEOGROUPBROAD",
	"MVSMSG_VIDEOOPEN",	
	"MVSMSG_VIDEOCLOSE",	
	"MVSMSG_DATAGROUPBROAD",	
	"MVSMSG_DATAOPEN",	
	"MVSMSG_DATACLOSE",	
	"MVSMSG_REBOOT",	
	//CONBREAK
	"MVSMSG_REGNODE_ERROR",
	"MVSMSG_CALLCONNECTCMD",
	"MVSMSG_JOINCMD",
_END_IMPL_ENUMSTR



const char* memstr( const char* p,int size,const char* str )
{
	int len=(int)strlen(str);
	const char* q=p+size-len;
	const char* t=p;
	while (t<q)
	{
		if (memcmp(t,str,len)==0)
		{
			return t;
		}
		t++;
	}
	return 0;
}

int MVS_ConfuseString(char* str)
{
	int len=(int)strlen(str);
	for (int i=0;i<len;i++)
	{
		str[i]=158-str[i]+(i%2);
	}
	return len;
}

int MVS_MakeLoginMessage(char* szBuf,LPCTSTR szUserName,LPCTSTR szPass)
{
	char* p=szBuf;
	p+=sprintf(p,"logingetinfo");
	p+=sprintf(p,"HST%s@",szUserName);
	char* pwd=p;
	p+=sprintf(p,"%s",szPass);
	MVS_ConfuseString(pwd);

	return (int)(p-szBuf+1);
}

bool MVS_ParseHostInfo( const char * szFileData,PHOSTINFO pHostInfo)
{
	const char *szHost20 = strchr(szFileData,'/');
	if(szHost20==0) return 0;
	ZeroMemory(pHostInfo,sizeof(HOSTINFO));


	//如果第一行是注释行,则去掉第一行注释
	//	if( szFileData[0] == '/' )
	if( szHost20[0] == '/' )
	{
		szHost20 = strchr(szHost20,']');
		if( szHost20 != NULL )
		{	//去掉回车换行符号 /r/n 
			szHost20 = strchr( szHost20, '\n' );
		}
	}
	if (szHost20==NULL)
	{
		return 0;
	}
	//没有注释则直接读取
	sscanf(szHost20, "%s %s %s %s %s %d %d %d %d %d %d",pHostInfo->szHostName,pHostInfo->szIpAddr,pHostInfo->szMultiAddr,pHostInfo->szMvsAddr,
		pHostInfo->szDeviceName,&pHostInfo->nTermType,&pHostInfo->nTermLevel,&pHostInfo->dwExFlag,&pHostInfo->dwPTZMode,&pHostInfo->nPackSize,&pHostInfo->nCodecType);
	szHost20=strchr(szHost20,'/');	
	szHost20=strchr(szHost20,'\n');
	if(szHost20==NULL) return 1;


	szHost20++;
	sscanf(szHost20,"%s",pHostInfo->szOwer);
	szHost20=strchr(szHost20,'/');	
	szHost20=strchr(szHost20,'\n');
	if(szHost20==NULL) return 1;
	szHost20++;
	sscanf(szHost20,"%d",&pHostInfo->nSysType);
	return 1;
}

int MVS_AsignNodeInfo( NODEINFO* pNodeInfo,HOSTINFO* pHostInfo)
{
	strcpy((char*)pNodeInfo->HostName,pHostInfo->szHostName);
/*
	if (pHostInfo->szIpAddr[0]==0 || strcmp(pHostInfo->szIpAddr,"0.0.0.0")==0)
	{
		//	dest_sin.sin_addr.S_un.S_addr=htonl(dest_sin.sin_addr.S_un.S_addr);
		strcpy(pNodeInfo->IPAddress,inet_ntoa(dest_sin.sin_addr));
		strcpy(pHostInfo->szIpAddr,inet_ntoa(dest_sin.sin_addr));
	}
	else
*/
	{
		strcpy(pNodeInfo->IPAddress,pHostInfo->szIpAddr);
		strcpy(pNodeInfo->PointAddress,pHostInfo->szIpAddr);
		strcpy(pNodeInfo->PointAddressAudio,pHostInfo->szIpAddr);
		strcpy(pNodeInfo->PointAddressDataStream,pHostInfo->szIpAddr);
	}
	strcpy(pNodeInfo->MultiAddress,pHostInfo->szMultiAddr);
	strcpy(pNodeInfo->MultiAddressEx,pHostInfo->szMultiAddr);
	pNodeInfo->TermType=pHostInfo->nTermType;
	pNodeInfo->TermLevel=pHostInfo->nTermLevel;
	pNodeInfo->Chairman=-1;
	pNodeInfo->IsInConf=0;
	sprintf(pNodeInfo->Audio,"----");
	sprintf(pNodeInfo->Video,"----");
	pNodeInfo->test_zero=0;
	sprintf(pNodeInfo->Status,"0已开机");
	sprintf(pNodeInfo->Message,"已与MCU连接");
	pNodeInfo->NodeInfoEx.dwProductID=0;//MVS_GetProductSerialNumber();
	pNodeInfo->NodeInfoEx.dwVersion=CLIENT_NODE_VER;

	return 0;
}

DWORD MVS_GetProductSerialNumber()
{
	return 0;
}

#define SZLOGINGETINFO "logingetinfoHST"

int MvsCmdToMsgName(UINT cmd)
{
	int msg=MVSMSG_CNT;
	if (cmd&CALLSETUPCMD)
	{
		msg=MVSMSG_REGNODE;
	}
	else if (cmd&SENDCOMMAND)
	{
		msg=MVSMSG_SENDCOMMAND;
	}
	else if (cmd&CONJOINGROUP)
	{
		msg=MVSMSG_JOINGROUP;
	}
	else if (cmd&CONEXITGROUP)
	{
		msg=MVSMSG_EXITGROUP;
	}
	else if (cmd&SETSENDERFILTER)
	{
		msg=MVSMSG_SETSENDERFILTER;
	}
	else if (cmd&INSERTKEYFRAME)
	{
		msg=MVSMSG_INSERTKEYFRAME;
	}
	else if (cmd&CONSPEECHADMITCMD)
	{
		msg=MVSMSG_ADMITSPEECH;
	}
	else if (cmd&CONSPEECHCLOSECMD)
	{
		msg=MVSMSG_CLOSESPEECH;
	}
	else if (cmd&CONSHOWCMD)
	{
		msg=MVSMSG_VIDEOGROUPBROAD;
	}
	else if (cmd&CONPOINTCMD)
	{
		msg=MVSMSG_VIDEOOPEN;
	}
	else if (cmd&CONSHOWCLOSECMD)
	{
		msg=MVSMSG_VIDEOCLOSE;
	}
	else if (cmd&CONSHOWCMDDATA)
	{
		msg=MVSMSG_DATAGROUPBROAD;
	}
	else if (cmd&CONPOINTCMDDATA)
	{
		msg=MVSMSG_DATAOPEN;
	}
	else if (cmd&CONSHOWCLOSECMDDATA)
	{
		msg=MVSMSG_DATACLOSE;
	}
	else if (cmd&YUNTAICONTROLCMD)
	{
		msg=MVSMSG_PTZCTL;
	}
	else if (cmd&CONSHUTDOWNCMD)
	{
		msg=MVSMSG_REBOOT;
	}
	else if (cmd&CONBREAK)
	{
		msg=MVSMSG_REGNODE_ERROR;
	}
	else if (cmd&CALLCONNECTCMD)
	{
		msg=MVSMSG_CALLCONNECTCMD;
	}
	else if (cmd&CONJOINCMD)
	{
		msg=MVSMSG_JOINCMD;
	}
	else
	{
		TRACEERR("\nERR---------cmd to msg err,cmd=%p\n\n",cmd);
	}
	return msg;
}

int MVSRecvBuffer::GetMvsMessage(int* pSize)
{
	int msgSize=GetHeadSize();
	if (msgSize==0)
	{
		*pSize=0;
		return 0;
	}
	
	char* pMsg=_pHead;
	if(expectMsg==MVSMSG_NONE)
	{
		expectSize=-1;
		if (msgSize>=sizeof(SZMSGLINKCHECK) && strcmp(pMsg,SZMSGLINKCHECK)==0)
		{
			expectMsg=MVSMSG_LINKCHECK;
			expectSize=SIZE_LINKCHECK;
		}
		else if(msgSize>=sizeof(MVSCmdMsgHdr) && *LPWORD(pMsg)==0xFEFE)
		{
			MVSCmdMsgHdr* p=(MVSCmdMsgHdr*)pMsg;
			if(p->bMsgRet)
			{
				expectMsg=MVSMSG_CMDRET;
			}
			else
			{
				expectMsg=MVSMSG_TERMINFOLIST;
			}
			expectSize=p->GetPackSize();
		}
		else if(msgSize>=sizeof(SZLOGINGETINFO) && strncmp(pMsg,SZLOGINGETINFO,sizeof(SZLOGINGETINFO)-1)==0)
		{
			expectMsg=MVSMSG_LOGIN;
			expectSize=-1;
		}
		else if(msgSize>22)
		{
			int cmd=0,cmd1=0;
			char ch;
			int n=sscanf(pMsg,"%d %c %d",&cmd,&ch,&cmd1);
			if(n==3 && cmd==cmd1 && ch=='|')
			{
				expectMsg=MvsCmdToMsgName(cmd);
				
				expectSize=SIZE_SENDCMD;
				if (expectMsg==MVSMSG_SENDCOMMAND)
				{
					expectSize=-1;
				}
				
			}
			else
			{
				expectMsg=MVSMSG_INVALID;
				expectSize=-1;
			}
		}
		
	}//if(expectMsg==MVSMSG_NONE)
	if (expectMsg)
	{
		if(expectSize<0)
		{
			if (expectMsg==MVSMSG_SENDCOMMAND)
			{
				LPCTSTR p=(LPCTSTR)memchr(pMsg,0,msgSize);
				if (p)
				{
					int realSize=int(p-pMsg)+1;
					if(realSize>SIZE_SENDCMDEX) expectSize=SIZE_SENDCMDEX1;
					else if(realSize>SIZE_SENDCMD) expectSize=SIZE_SENDCMDEX;
					else expectSize=SIZE_SENDCMD;
				}
			}
			else if (expectMsg==MVSMSG_INVALID)
			{
				if(msgSize>SIZE_LINKCHECK)
				{
					LPCTSTR p=memstr(pMsg,msgSize,SZMSGLINKCHECK);
					if (p)
					{
						expectSize=int(p-pMsg)+SIZE_LINKCHECK;
					}
				}

			}
			else //if (expectMsg==MVSMSG_LOGIN)
			{
				LPCTSTR p=(LPCTSTR)memchr(pMsg,0,msgSize);
				if (p)
				{
					expectSize=(int)(p-pMsg)+1;
				}
			}
			
			
		}
		if(expectSize>0 && msgSize>=expectSize)
		{
			*pSize=expectSize;
			return expectMsg;
		}
	}
	
	return 0;
}


SWNatHole::SWNatHole(const char* szHostName)
{
	ZeroMemory(this,sizeof(*this));
	memset(&_HolePacket,'N',32);
	SetHostName(szHostName);
}
void SWNatHole::SetHostName(const char* szHostName)
{
	if (szHostName)
	{
		strncpy(_HolePacket.szHostName,szHostName,32);
	}
}
void SWNatHole::SetHoleSocket(struct IAsyncSocket* pSocket)
{
	_pHoleSocket=pSocket;
	if (pSocket)
	{
		_HolePacket.uInnerIPAddr=(DWORD)pSocket->GetLocalIp();
		_HolePacket.wInnerPort=htons(pSocket->GetLocalPort());
	}
}
void SWNatHole::BindIp(const char* szIpAddr)
{
	SocketIpAddr addr(szIpAddr);
	_HolePacket.uInnerIPAddr=addr;

}

int SWNatHole::MakeHoleToHost(const char* szAddr,UINT port)
{
	int ret=-1;
	if (_pHoleSocket)
	{
		SocketIpAddr addr(szAddr);
		_pHoleSocket->SetPeerAddr(addr,port);
		ret=_pHoleSocket->WriteData((const char*)&_HolePacket,sizeof(_HolePacket));
		addr=(DWORD)_HolePacket.uInnerIPAddr;
		TRACETIMER("SendNATPacket:desp=%s,local=%s:%d,peer=%s:%d\n",(char*)_HolePacket.Prefix,addr.ToStr(),ntohs(_HolePacket.wInnerPort),szAddr,port);
	}
	return ret;
}




};//end namespace SWApi