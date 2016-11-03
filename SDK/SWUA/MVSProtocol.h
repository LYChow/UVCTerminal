#ifndef __MVSPROCOTOL__H__
#define __MVSPROCOTOL__H__
#include "StreamBuffer.h"


#pragma pack(push,1)
namespace SWApi{

	//////////////////////////////////////////////////////////////////////////
#define CLIENT_NODE_VER 240
#define PortLogin 263
#define PortMessage 261

#define PortVideo	7100
#define PortAudio 7101
#define PortDataChannel 7102



#define CONFTERM 0	//指挥会议终端类型
#define CTRLTERM 2	//指挥会议终端类型
#define DISPTERM 1	//显示终端类型
#define FRONTERM 3	//监控前端类型
#define LOOKTERM 4	//浏览终端类型
#define TVWALLTERM 5	//电视墙服务器类型
#define MULTIDISPTERM 6	//多分屏解码器类型
#define VIDEOMULTIPLEXER 7	//多画面合成器类型
#define STREAMADAPTER 26 //码流适配器（SAG）类型
#define CLIENTPROXY 27	//设备代理服务器类型
#define DATASERVER 28	//数据服务器(报警服务器)类型
#define VIDEOSERVER 29	//视频服务器类型
#define RECORDSERVER 30	//录象服务器类型
#define CONSOLETERM 255	//控制台终端 


	//////////////////////////////////////////////////////////////////////////
	//Mvs Message mask
#define  DEVICECHECK			0x0
#define  CALLSETUPCMD			0x1
#define  CONCREATESTATE			0x2 //wait response by server.
#define  CONBREAK				0x2 //CALLREJECTCMD
#define  CALLCONNECTCMD			0x4
#define  CONSHOWADMITSTATE		0x8
#define  CONCREATECMD			0x8
#define  CONPOINTADMITSTATE		0x10
#define  CONREJECTCMD			0x10
#define  CONJOINADMITSTATE		0x20
#define  CONINVITECMD			0x20
#define  CONJOINCMD				0x40
#define  CONEXITCMD				0x80
#define  CONAPPLYCMD			0x100
#define  CONSPEECHCMD			0x200
#define  CONSHOWCMD				0x400
#define  CONPOINTCMD			0x800
#define  CONSPEECHEXITCMD		0x1000
#define  CONSPEECHCLOSECMD		0x2000
#define  CONSPEECHADMITCMD		0x4000
#define  CONSPEECHREJECTCMD		0x8000
#define  CONSHOWCLOSECMD		0x10000
#define  YUNTAICONTROLCMD		0x20000
#define  CONSHUTDOWNCMD			0x40000
#define  CONREGCMD				0x80000
#define  CONGETCMD				0x100000
#define  CONREPORTCMD			0x200000
#define  CONBANDWIDTHCMD		0x400000 //reserved. Used in Mpeg2.
#define  CONTROLINVITECMD		0x800000 //邀请监控终端加入监控系统
#define  DISPLAYSWITCHCMD		0x1000000 //切换显示终端的显示
#define  SETVSTRANSMITRELATION	0x1000000	//设置VS的转发关系
#define  CONZIMUCMD				0x2000000 // 发布字模消息到每一个终端
#define  CONMULTIDISPLAYRXCMD	0x0000000 // 指定一个终端为多屏接收终端. 状态为: 'r'
#define  SETSENDERFILTER		0x4000000 // 设置解码器端的发送者过滤地址
#define  CONMULTIDISPLAYPOINTCMD 0x8000000 // 指定若干终端为多屏点播终端. 状态为: '0','1','2','3'...
#define  CONJOINGROUP			0x10000000	//加入一个组播组
#define  CONEXITGROUP			0x20000000	//退出一个组播组
#define  SENDCOMMAND			0x40000000	//向终端发送命令
#define  INSERTKEYFRAME			0x80000000	//编码终端发送一个I帧
#define SYNCSYSTEMTIME		 0x400000	//同步系统时钟

	//{数据通道专用
#define CONPOINTCMDDATA 0x80000	//原CONREGCMD
#define CONSHOWCMDDATA 0x100000	//原CONGETCMD
#define CONSHOWCLOSECMDDATA 0x200000	//原CONREPORTCMD
	//数据通道专用}

	//Max number is   4Bytes  0x100 000 000   =   4,294,967,296
	//Max number is   4Bytes  0x8 000 000   =   134,217,728  

#define NO_FLAGS_SET         0  /* Used with recv()/send()          */
#define MAX_PENDING_CONNECTS 4  /* The backlog allowed for listen() */

	//add by fxd
#define TERM_PACKSIZE 4096
#define TERM_PACKSIZE_EX		4*1024*1024
#define PACKDATAOFFSET 6
#define PACKDATAOFFSET_EX	PACKDATAOFFSET+2
	//end fxd

#define STRING_RX			"R接收"
#define STRING_rx			"r多屏"
#define STRING_V_BROADCAST	"M组播"
#define STRING_V_POINT		"P单播"

	//For MCU
	//											chairman
	//debug status bit	      bit Key init conn crea inConf √×
#define CSB_CALLSETUP		1//7	√	√	√		√		√		  
#define CSB_SPEECHHAND		2//3	× 	×	√		√		×		
#define CSB_VEDIOHAND		4//0	× 	×	√		√		×		
#define CSB_SERVERADMIT	8//Enter	× 	√	√		√		√		

#define CSB_SERVERREJECT 16//Del	× 	√	√		√		√		
#define CSB_SPEECHCALLBACK 32//		× 	×	√		√		×		
#define CSB_CONFERENCEQUIT 64//		× 	×	√		√		×		
#define CSB_YUNTAI		  128//		× 	×	√		√		×		

	//status value definition。（状态名称及含义在《ASP_ACA使用说明.txt》）
#define CSM_INITIAL			0x1//0x301	//状态0 ：可呼叫态。
#define CSM_CALLCONNECT		0x19//0xc0b	//状态1 ：呼叫已连接态。
#define CSM_CONCHAIRMAN		0xff	//状态2：是会议主席。
	//#define CS_CONJOINOR		0xff	//状态3：是与会者。

	//For terminal
	//											chairman
	//debug status bit	      bit Key init conn crea inConf spchAdmit continue exception √×
#define CSB_CONCREATE		1//4 	×	√	×		×		×		
#define CSB_CONINVITE		2//5 	×	×	√		×		×		
#define CSB_CONJOIN			4//6	× 	√	×		×		×		
#define CSB_SPEECHAPPLY		8//1	× 	×	√×	√		×		

#define CSB_SPEECHRETURN	16//2	× 	×	√×	√		
#define CSB_CONFERENCEEXIT  32//	× 	×	√		√		×		
	//#define CSB_YUNTAI			64//4 	×	√	√		√			

	//status value definition。（状态名称及含义在《ASP_ACA使用说明.txt》）
#define CST_INITIAL			0x00//0x301	//状态0 ：可呼叫态。
#define CST_CALLCONNECT		0x45//0xc0b	//状态1 ：呼叫已连接态。
#define CST_CONFERENCECREATE	0x7A	//状态2：会议已创建态。
#define CST_CONFERENCEIN		0x78	//状态2：会议已加入态。

	//One Word configure item
#define ConfigureItemBase   0x0001
#define ConfigureMultiVideo 0x0002
#define ConfigureScanPoint  0x0004
#define ConfigureScanBroad  0x0008
#define ConfigureWhiteBoard 0x0010


#define SZHOSTINFO \
	"%s" /*0 主机名*/ \
	"%s" /*1 IP地址*/ \
	"%s" /*2 多播地址*/ \
	"%s" /*3 P2P音频地址*/ \
	"%s" /*4 P2P视频地址*/ \
	"%s" /*5 消息*/ \
	"%s" /*6 音频状态*/ \
	"%s" /*7 视频状态*/ \
	"%s" /*8 主席名字*/ \
	"%s" /*9 终端类型*/ 

//#define VC3TERM_SIG ((DWORD)('T'<<24|'E'<<16|'R'<<8|'M'))

//MVS发送到UA的命令ID
enum MVS_CALL_FUNCTIONID
{
	FID_SWRmtControlYT,
	FID_SWRmtControlCamera,
	FID_SWRmtControlSwitch,
	FID_SWCreateAVTransmission,
	FID_SWDestroyAVTransmission,
	FID_SWCreateAVP2PChannel,
	FID_SWDestroyAVP2PChannel,
	FID_SWAllocMulticastGroup,
	FID_SWFreeMulticastGroup,
	FID_SWCreateAVMulticast,
	FID_SWDestroyAVMulticast,
	FID_SWJoinAVMulticastGroup,
	FID_SWExitAVMulticastGroup,
	FID_SWRmtIsSystenAvilable,
	FID_SWRmtIsPartAvilable,
	FID_SWRmtIsPartEnabled,
	FID_SWRmtGetWorkMode,
	FID_SWRmtGetRelatedTerminal,
	FID_SWGetAllTerminalInfo,
	FID_SWSendCommand,
	FID_SWGetAVReferenceCount,
	FID_SWRmtSetVideoFrameRate,
	FID_SWRmtSetBandWidth,
	FID_SWGetAllTerminalInfoOnceForAll = 23,
	FID_SWRmtPTZControlDirect		= 24,
};
//云台相关动作
//Camara
#define PTZ_ZOOM						1	/* 代表变焦 */
#define PTZ_FOCUS						2	/* 代表对焦 */
#define PTZ_IRIS						3	/* 代表光圈 */
#define	PTZ_LIGHT						4   /* 代表光源选择逆光或正常光 ( 被光补偿？？)*/ 
#define PTZ_POWER						5   /* 代表电源开关控制 */

//YT
#define PTZ_YUTAIRESET					6   /* 代表云台复位 */
#define PTZ_PAN							7	/* 代表水平方向转动 */
#define PTZ_TILT						8	/* 代表垂直方向转动 */
#define PTZ_PAN_AUTO					9	/* 代表自动扫描模式 */
#define PTZ_FRONT						10  /* 代表主正前位 */
#define PTZ_MICVOLUME					11  /* 代表麦克风音量控制 */
#define PTZ_SYSRESET					12	/* 代表系统复位 */
#define PTZ_CAMERAPWR					13  /* 代表摄象机开关控制 */
#define PTZ_LIGHTPWR					14  /* 代表灯光开关控制 */
#define PTZ_WIPERPWR					15  /* 代表雨刷开关控制 */
#define PTZ_AUXPWR						16  /* 代表AUX开关控制 */
//零照度是否打开？？？
//新增加两个动作-hxf
#define PTZ_HEATER						17  /* 代表加热器开关 */  

//Switch
#define PTZ_SWITCH_OFFSET				18	/* 代表镜头切换的偏移*/
#define PTZ_FAN							18	/* 代表风扇开关 */
//切换动作
#define PTZ_CAMERA						19  /* 代表镜头切换FULL SCREEN */
#define PTZ_FREEZE						20	/* 代表图象冻结FREEZE */
#define PTZ_SET							21	/* 代表设置SET */
#define PTZ_BZ							22	/* 代表鸣叫BZ */
#define PTZ_ALARM						23	/* 代表警报ALARM */
#define PTZ_LOCK						24	/* 代表锁定LOCK */
#define PTZ_DIV							25	/* 代表画面分割“田” */
#define PTZ_OSD							26	/* 代表OSD */
#define PTZ_SEQ							27  /* 代表SEQ */
#define PTZ_RECORD						28  /* 代表录象 */
//预置点与巡航相关 
#define PTZ_SET_PRESET					57//29	/* 设置预置点 */ 
#define PTZ_CLE_PRESET					58//30	/* 清除预置点 */ 
#define PTZ_GOTO_PRESET					59//31	/* 转到预置点 */ 
//新增加四个动作-hxf ×××
#define PTZ_SET_CRUISE_SPEED			32	/* 设置巡航速度 */
#define PTZ_SET_CRUISE_POINT_DELAYTIME  33	/* 设置巡航点停顿时间*/ 
#define PTZ_SEQ_ADD_PREPOINT			34	/* 将预置点加入巡航序列 */
#define PTZ_SEQ_DEL_PREPOINT			35	/* 将预置点从巡航序列中删除 */
#define PTZ_RUN_CURISE					36	/* 开始巡航 */
#define PTZ_STOP_CURISE					37	/* 停止巡航 */
//记录航线功能，暂不实现
#define PTZ_3DMOVE						201 /* mode=(pan)|(tilt<<8)|(zoom<<16)|(0<<24)*/

//控制命令模式，在云台相关动作中 nAction 的最高两位31~32表示
#define PTZ_MODE_DEFAULT				0x00000000	/* 表示缺省模式 */
#define PTZ_MODE_STEP					0x01000000	/* 表示步进模式 */
#define PTZ_MODE_CONTINUE				0x10000000	/* 表示连续模式 */
#define PTZ_MODE_UNDEFAULT				0x11000000	/* 表示与缺省相反模式 */


#define PTZ_MAX_CMD						200 /* 云台控制参数最大值 */
const int SW_VC3_PTZ_YUNTAI				= 104;		//控制云台
const int SW_VC3_PTZ_SWITCH				= 105;		//镜头切换
const int SW_VC3_PTZ_CRUISE				= 106;		//控制
const int SW_VC3_PTZ_DIRECT				= 107;		//透传

#include "NodeInfo.h"


enum MVSMSGTYPE_E
{
//	MVSMSG_CMD,
//	MVSMSG_RESP,
	MVSMSG_INVALID = -1,
	MVSMSG_NONE =0,
	MVSMSG_CONNECT_LOGIN,
	MVSMSG_LOGIN,	//
	MVSMSG_CONNECT_REGNODE,
	MVSMSG_RECONNECT_REGNODE,
	MVSMSG_REGNODE, //CALLSETUP
	MVSMSG_LINKCHECK, //LINKCHECK
	MVSMSG_CMDRET,
	MVSMSG_TERMINFOLIST,//
	MVSMSG_SENDCOMMAND,//SENDCOMMAND
	MVSMSG_CREATEAV,
	MVSMSG_DESTROYAV,
	MVSMSG_PTZCTL,//
	MVSMSG_JOINGROUP,
	MVSMSG_EXITGROUP,
	MVSMSG_SETSENDERFILTER,
	MVSMSG_INSERTKEYFRAME,
	MVSMSG_ADMITSPEECH,
	MVSMSG_CLOSESPEECH,
	MVSMSG_VIDEOGROUPBROAD,//CONSHOWCMD
	MVSMSG_VIDEOOPEN,
	MVSMSG_VIDEOCLOSE,
	MVSMSG_DATAGROUPBROAD,
	MVSMSG_DATAOPEN,
	MVSMSG_DATACLOSE,
	MVSMSG_REBOOT,
	MVSMSG_REGNODE_ERROR, //CONBREAK
	MVSMSG_JOINCMD,
	MVSMSG_CALLCONNECTCMD,

	MVSMSG_CNT
};

_DECL_ENUMSTR(ESMvsMsg);



/*

LPCTSTR ESMvsMsg(int msg);
LPCTSTR ESMvsCmd(UINT cmd);

*/

#define SZMSGLINKCHECK "  0 0 -LinkCheck..."
const char szLinkCheckSend[128]="-LinkCheck...";


#define SIZE_LINKCHECK		64
#define SIZE_MVSRESPHDR		12	
#define SIZE_CSTR			-1  
#define SIZE_SENDCMD		96
#define SIZE_SENDCMDEX		(96+768)
#define SIZE_SENDCMDEX1		4096

enum SWVC3_APPTYPE_E
{
	SWVC3_APPTYPE_UNKNOWN	= 0,
	SWVC3_APPTYPE_SWVC3API,
	SWVC3_APPTYPE_SWVC3API_IOS,

};


union APPVERSION
{
	struct  
	{
		WORD appVer;
		BYTE apiVer;
		BYTE type;
	};
	DWORD dwValue;
	APPVERSION(BYTE Type=SWVC3_APPTYPE_SWVC3API_IOS,BYTE ApiVer=0, WORD AppVer=0)
	{
		type=Type;apiVer=ApiVer;appVer=AppVer;
	}
	APPVERSION(DWORD ver){dwValue=ver;}

	operator DWORD()
	{
		return dwValue;
	}
};


typedef struct tagHOSTINFO
{
	DWORD dwSize;
	char szHostName[32];
	char szIpAddr[32];
	char szMultiAddr[32];
	char szMvsAddr[32];
	char szDeviceName[64];
	int nTermType;
	int nTermLevel;
	DWORD dwExFlag;//bool DualHead
	DWORD dwPTZMode;//
	int nPackSize;
	int nCodecType;
	char szOwer[64];
	int nSysType;

}HOSTINFO,*PHOSTINFO;

typedef struct  
{
	DWORD dwCommand;
	int lParam;
	int nSize;
	char* szParam;
}RMT_SENDCOMMAND,*PRMT_SENDCOMMAND;


//////////////////////////////////////////////////////////////////////////

#define RECVBUFSIZE (4*1024*1024)
#define SENDBUFSIZE (512*1024)



struct MVSLinkCheck
{
	char szLink[SIZE_LINKCHECK];
	MVSLinkCheck()
	{
		ZeroMemory(this,sizeof(*this));
		strcpy(szLink,SZMSGLINKCHECK);
	}
	static int Check(LPCTSTR szMsg,int size)
	{
		return (size>=SIZE_LINKCHECK && strcmp(szMsg,SZMSGLINKCHECK)==0)*SIZE_LINKCHECK;
	}
};

struct MVSCmdMsgHdr
{
	WORD wSig;//0xfefe
	int ret;//send for bNeetResp

	union
	{
		struct  //
		{
			WORD size;
			char szParam[0];
		};
		struct	//
		{
			WORD bMsgRet;//=0
			DWORD sizeEx;
			char szParamEx[0];
		};

	};
	int GetPackSize()
	{
		return size==0?sizeEx+8:size+6;
	}

	char* GetPacket()
	{
		return bMsgRet?szParam:szParamEx;
	}

	static int Check(LPCTSTR szMsg,int size)
	{
		 if(size>=SIZE_MVSRESPHDR&&(*LPWORD(szMsg))==0xfefe)
		 {
			 MVSCmdMsgHdr* pResp=(MVSCmdMsgHdr*)szMsg;
			 return pResp->GetPackSize();
		 }
		 return 0;
	}
};


struct CmdRegNode
{
	int cmd;
	char szMvsName[64];
	char szMvsIp[32];
	int track;
	char szCenterIp[32];
	DWORD dwMvsCapability;
	DWORD dwMvsVersion;
	int Parse(LPCTSTR szMsg)
	{
		//"%9u	|	%9u -1 %s %s 0 %s %u %d",
		int n=sscanf(szMsg,"%d %*s %d %*d %s %s %d %s %u %d",
			&cmd,&cmd,szMvsName,szMvsIp,&track,szCenterIp,&dwMvsCapability,&dwMvsVersion
			);
		return n;
	}
};

struct CmdSetSenderFilter
{
	int cmd;
	int lParam;
	int av;
	char szId[32];
//	DWORD dwId;
	char szHostName[64];
	int flag;

	int Parse(LPCTSTR szMsg)
	{
	//	char szId[32]={0};
		int n=sscanf(szMsg,"%d %*s %d %d %s %s %d",&cmd,&cmd,&av,szId,szHostName,&flag);
		//dwId=_INetStrToAddr(szId);
		return n;
	}
};

struct CmdSendCommand
{
	char szTerminal[64];
	int cmd;
	int nCommandCode;
	int lParam;
	int nSize;
	const char* szParam;
	int Parse(LPCTSTR szMsg)
	{
		szTerminal[0]=0;
		int n=sscanf(szMsg,"%d %*s %d %*d %d %d ",&cmd,&cmd,&nCommandCode,&lParam);
		LPCTSTR p=strchr(szMsg,'|')+1;
		for (int i=0;i<4;i++)
		{
			p=strchr(p,' ');
			p++;
		}
		szParam=p;
		nSize=strlen(szParam)+1;
		return n;
	}
};

struct CmdOther
{
	int cmd;
	char szMultiAddr[32];
	char szAddr[32];
	char Track[4];
	char szAddrAudio[32];
	char szAddrData[32];
	int Parse(LPCTSTR szMsg)
	{
		int n=sscanf(szMsg,"%d %*s %d %*d %s %s %c %s %s",&cmd,&cmd,szMultiAddr,szAddr,Track,szAddrAudio,szAddrData);
		return n;
	}

};

struct CmdGroupBroad
{
	int cmd;
	char szMultiAddr[32];
	int lParam;
//	int flag;
//	char szExt[32];
	int Parse(LPCTSTR szMsg)
	{
		int n=sscanf(szMsg,"%d %*s %d %*d %s %d",&cmd,&cmd,szMultiAddr,&lParam);
		return n;
	}

};


struct CmdRegNodeErr
{
	int cmd;
	int Reason;
	char szReason[128];
	int Parse(LPCTSTR szMsg)
	{
		int n=sscanf(szMsg,"%d %*s %d %d %s",&cmd,&cmd,&Reason,szReason);
		return n;
	}

};


struct MVSRecvBuffer:public StreamBuffer
{
	int expectMsg;
	int expectSize;
public:
	MVSRecvBuffer():StreamBuffer(RECVBUFSIZE)
	{
		expectMsg=0;expectSize=0;
	};
	void RemoveHead(int size)
	{
		StreamBuffer::RemoveHead(size);
		expectMsg=0;
		expectSize=0;
	}

	void Reset()
	{
		StreamBuffer::Reset();
		expectMsg=0;expectSize=0;
	}
	int GetMvsMessage(int* pSize);
};


struct SMHDR //SENDMSGHDR
{
	int type;
	int size;
	char szMsg[0];
};


struct MVSSendBuffer:public StreamBuffer
{
	bool canSend;
	int sendSize;
public:
	MVSSendBuffer():StreamBuffer(SENDBUFSIZE)
	{
		sendSize=0;
		canSend=0;
	};
	void Reset()
	{
		StreamBuffer::Reset();
		sendSize=0;
		canSend=0;
	}

};

//穿越NAT打洞包格式信息
struct NATHOLEPACKET
{
	BYTE Prefix[32];
	char szHostName[32];
	DWORD uInnerIPAddr;
	WORD wInnerPort;
	BYTE Reserved[2];
};

struct SWNatHole
{
	NATHOLEPACKET _HolePacket;
	struct IAsyncSocket* _pHoleSocket;
	SWNatHole(const char* szHostName=0);
	void SetHostName(const char* szHostName);
	void SetHoleSocket(struct IAsyncSocket* pSocket);
	void BindIp(const char* szIpAddr);
	int MakeHoleToHost(const char* szAddr,UINT port);
};

const char* memstr(const char* p,int size,const char* str);

int MVS_ConfuseString(char* str);
int MVS_MakeLoginMessage(char* szBuf,LPCTSTR szUserName,LPCTSTR szPass);
bool MVS_ParseHostInfo( const char * szFileData,PHOSTINFO pHostInfo);
int MVS_AsignNodeInfo(NODEINFO* pNodeInfo,HOSTINFO* pHostInfo);
DWORD MVS_GetProductSerialNumber();




};//end namespace SWApi

#pragma pack(pop)

#endif

