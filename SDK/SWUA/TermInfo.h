#ifndef __TERMINFO__H__
#define __TERMINFO__H__
//#include "ISWApi.h"
#include "StreamBuffer.h"
#include "DataSet.h"
#include "MVSProtocol.h"
#include "ISWCall.h"
#include "net.h"
//#include "IMediaDef.h"

namespace SWApi{



/*
	struct TermInfoList
	{
		int dataFlag;//0=no,1=hasdata
		varstr HostName;
		varstr szIPAddress
		varstr szMulticastAddress
		varstr szP2PAddressAudio
		varstr szP2PAddressVideo
		select<
			Version<=1:
			{
				varstr szMessage
				varstr szAudioStatus
				varstr szVideoStatus
				varstr szChairmanName
			}
			Version>1:
			{
				varstr userString1;
				varstr userString2;
				varstr saMessage;
				varstr OtherInfoStringEx; //SZOTHERINFOEX_SSCANF_FMT
			}
		/>
		
		int termType;
		varstr OtherInfoString;//SZOTHERINFO_SSCANF_FMT

	}[n];

*/

#define SZOTHERINFO_SSCANF_FMT \
	"%s " /*0 单位名称*/ \
	"%d " /*1 视频分割器型号*/ \
	"%d " /*2 镜头个数*/ \
	"%d " /*3 当前镜头号*/ \
	"%s " /*4 镜头0名称*/ \
	"%d " /*5 镜头0型号*/ \
	"%s " /*6 镜头1名称*/ \
	"%d " /*7 镜头1型号*/ \
	"%s " /*8 镜头2名称*/ \
	"%d " /*9 镜头2型号*/ \
	"%s " /*10镜头3名称*/ \
	"%d " /*11 镜头3型号*/ \
	"%s " /*12镜头4名称*/ \
	"%d " /*13 镜头4型号*/ \
	"%s " /*14镜头5名称*/ \
	"%d " /*15 镜头5型号*/ \
	"%s " /*16镜头6名称*/ \
	"%d " /*17 镜头6型号*/ \
	"%s " /*18镜头7名称*/ \
	"%d " /*19 镜头7型号*/ \
	"%s " /*20镜头8名称*/ \
	"%d " /*21 镜头8型号*/ \
	"%s " /*22镜头9名称*/ \
	"%d " /*23 镜头9型号*/ \
	"%s " /*24镜头10名称*/ \
	"%d " /*25 镜头10型号*/ \
	"%s " /*26镜头11名称*/ \
	"%d " /*27 镜头11型号*/ \
	"%s " /*28镜头12名称*/ \
	"%d " /*29 镜头12型号*/ \
	"%s " /*30镜头13名称*/ \
	"%d " /*31 镜头13型号*/ \
	"%s " /*32镜头14名称*/ \
	"%d " /*33 镜头14型号*/ \
	"%s " /*34镜头15名称*/ \
	"%d " /*35 镜头15型号*/ \
	"%d " /*36终端级别*/ \
	"%d " /*37编码器类型*/ \
	"%d " /*38是否主席*/ \
	"%s " /*39会议信息*/ \
	"%d " /*40是否夸局*/ \
	"%d " /*41夸局权限*/ \
	"%d " /*42显示顺序号*/ \
	"%lf " /*43 X坐标*/ \
	"%lf " /*44 Y坐标*/ \
	"%d " /*45 是否静态发送者*/ \
	"%s " /*46 终端别名*/ \
	"%d " /*47 终端子类型*/ \
	"%s " /*48 电话号码*/ \
	"%d " /*49 镜头0地址*/ \
	"%d " /*50 镜头1地址*/ \
	"%d " /*51 镜头2地址*/ \
	"%d " /*52 镜头3地址*/ \
	"%s " /*53 终端序号*/ \
	"%s " /*54 终端长名*/ \
	"%d " /*55 终端组合状态*/ \
	"%d " /*56 终端数值型ID*/ \


#define SZOTHERINFOEX_SSCANF_FMT \
	"%*s" /*0 保留(固定为"-")*/ \
	"%s" /*1 指挥/会议的主席/指挥者终端名*/ \
	"%s" /*2 MCU完整的长名(点分隔的多级MCU长名称，反应级联结构)*/ \
	"%d" /*3 底层版本号 */ \
	"%d" /*4 应用层版本号 */ \
	"%s" /*5 终端绑定信息 */ \
	"%s" /*6 挂靠单位*/ \





#define REFRESH_TIMERLEN 20000
#define TALK_ACTIVE_TIMERLEN 6000
#define ATTEND_ACTIVE_TIMERLEN 20000
#define RECSERVER_ACTIVE_TIMERLEN 10000
#define ELAPS_TIMERLEN 1000

//state timer length
#define WAIT_TALKCALL_RESP_TIMERLEN 10000
#define WAIT_TALKSTOP_RESP_TIMERLEN	3000
#define WAIT_TALKREPORT_RESP_TIMERLEN 10000

#define WAIT_CMDCALL_RESP_TIMERLEN 16000
#define WAIT_CMDJOIN_CALL_RESP_TIMERLEN 15000
#define WAIT_CMDSTOP_RESP_TIMERLEN 3000
#define	WAIT_CMDLEAVE_CALL_RESP_TIMERLEN  15000
//#define WAIT_CMDEXIT_RESP_TIMERLEN 15000
#define WAIT_CMDHAND_CALL_RESP_TIMERLEN 30000

#define WAIT_CONCALL_RESP_TIMERLEN 16000
#define WAIT_CONJOIN_CALL_RESP_TIMERLEN 15000
#define WAIT_CONSTOP_RESP_TIMERLEN 3000
#define	WAIT_CONLEAVE_CALL_RESP_TIMERLEN  15000
//#define WAIT_CONEXIT_RESP_TIMERLEN 15000
#define WAIT_CONHAND_CALL_RESP_TIMERLEN 30000

#define WAIT_CALLRECSERVER_RESP_TIMERLEN 5000
#define WAIT_CALLVSRC_RESP_TIMERLEN 3000

enum
{
	TOL_TALKCALL,
	TOL_CONCALL,
	TOL_HAND,
	TOL_LEAVE,
	TOL_REPORT,
	TOL_JOIN,

	TOL_MAX
};


struct TIMEOUTLEN
{
	int lenArr[TOL_MAX];
public:
	TIMEOUTLEN()
	{
		ZeroMemory(this,sizeof(*this));
		lenArr[TOL_TALKCALL]= WAIT_TALKCALL_RESP_TIMERLEN ;
		lenArr[TOL_CONCALL]=  WAIT_CONCALL_RESP_TIMERLEN;
		lenArr[TOL_HAND]= WAIT_CONHAND_CALL_RESP_TIMERLEN ;
		lenArr[TOL_LEAVE]= WAIT_CONLEAVE_CALL_RESP_TIMERLEN ;
		lenArr[TOL_REPORT]=  WAIT_TALKREPORT_RESP_TIMERLEN;
		lenArr[TOL_JOIN]=  WAIT_CONJOIN_CALL_RESP_TIMERLEN;
	}
	TIMEOUTLEN(int talk,int con,int hand,int leave,int report,int join)
	{
		lenArr[TOL_TALKCALL]=talk;
		lenArr[TOL_CONCALL]=con;
		lenArr[TOL_HAND]=hand;
		lenArr[TOL_LEAVE]=leave;
		lenArr[TOL_REPORT]=report;
		lenArr[TOL_JOIN]=join;

	}
	TIMEOUTLEN(const TIMEOUTLEN& tl)
	{
		memcpy(this,&tl,sizeof(tl));
	}
	int& operator[](int iTimeout){return lenArr[iTimeout];}
	TIMEOUTLEN& operator=(const TIMEOUTLEN& tl)
	{
		memcpy(this,&tl,sizeof(tl));
		return *this;
	}
};

enum TERM_SUBTYPE
{
	//指挥终端
	TERMSUBTYPE_PCUSER_BEGIN		= 0,
	TERMSUBTYPE_CMD					= TERMSUBTYPE_PCUSER_BEGIN,
	TERMSUBTYPE_CONF_USB,
	TERMSUBTYPE_CONF,
	TERMSUBTYPE_CONF_CAP,
	TERMSUBTYPE_CMD_CAP,
	TERMSUBTYPE_H323GATEWAY,
	TERMSUBTYPE_SIPPHONE,

	//监控前端
	TERMSUBTYPE_MEDIASOURCE_BEGIN	= 10,
	TERMSUBTYPE_VGASOURCE			= TERMSUBTYPE_MEDIASOURCE_BEGIN,
	TERMSUBTYPE_VIDEOSOURCE,

	TERMSUBTYPE_VMP					= 20,

};

enum TermInfoBaseProp
{
	//update low
	TermInfo_Name					=0x00000001,
	TermInfo_NameAliase				=0x00000002,
	TermInfo_PhoneNumber			=0x00000004,
	TermInfo_Domain					=0x00000008,
	TermInfo_Department				=0x00000010,
	TermInfo_IpAddr					=0x00000020,
	TermInfo_IpAddrMulti			=0x00000040,
	TermInfo_IpAddrVideo			=0x00000080,
	TermInfo_IpAddrAudio			=0x00000100,
	TermInfo_Chairman				=0x00000200,
	TermInfo_Message				=0x00000400,
	TermInfo_ConfInfo				=0x00000800,//会议信息
	TermInfo_BandInfo				=0x00001000,
	TermInfo_BandOwner				=0x00002000,
	TermInfo_UserData1				=0x00004000,
	TermInfo_UserData2				=0x00008000,
	TermInfo_NetVersion				=0x00010000,
	TermInfo_AppVersion				=0x00020000,
	TermInfo_TermType				=0x00040000,
	TermInfo_TermSubType			=0x00080000,
	TermInfo_TermLevel				=0x00100000,
	TermInfo_Grant					=0x00200000,
	TermInfo_DispOrder				=0x00400000,
	TermInfo_Order					=0x00800000,
	TermInfo_GlobalFlag				=0x01000000,
	TermInfo_GlobalId				=0x02000000,
	TermInfo_AuxDevInfo				=0x04000000,
	TermInfo_GeoX					=0x08000000,
	TermInfo_GeoY					=0x10000000,
	TermInfo_IsChairman				=0x20000000,
	TermInfo_IsOuter				=0x40000000,//是否夸局
	TermInfo_IsStaticSender			=0x80000000,

	//update hight

	TermInfo_UpdateFlag			=0x80000000,

};

#define BITSTATE_ACTIVE						0x01
#define BITSTATE_INACTIVEONLINE				0x04
#define BITSTATE_VIRTUALTERM_STATICBIND		0x08
#define BITSTATE_CANACTIVEDBYCMD301			0x10

#define GRANT_CANCOMMAND		   		1		//允许指挥
#define GRANT_CANWATCH					2		//允许监控
#define GRANT_CANCONFIG					4		//允许配置
#define GRANT_CANSCANDEVICE				8		//允许巡检
#define GRANT_CANSHORTMSG				16		//允许短信
#define GRANT_CANBACKREPORT				32		//允许上报
#define GRANT_CANPTZCTL					64
#define GRANT_CANPTZCFG					128

struct VC3AUXDEVICE
{
	DWORD dwSize;
	char PCName[32];//远程机器名
	BYTE CodecType;	//编解码器型号：0 - 内置；1-n其它型号
	BYTE VideoDivType;	//视频分割器型号
	BYTE DEVNum;		//设备（镜头）个数
	BYTE CurDevNo;		//当前选择设备（镜头）号(从0开始,=0xff表示初始化切换器为分割画面，初始化后置为0)，用于云台地址选择与切换器选择的捆绑

	char DevName[16][32];		//设备名字
	BYTE YTFlag[16];	//云台配置标志:=1有；=0无
	BYTE YTAddr[16];	//云台地址
};
//::operator const char*(String& str){return str.c_str();}



struct VC3Terminal:public ITerminal
{
//base prop
	String strName;
	String strNameAliase;
	String strNameFriendly;
	String strPhoneNumber;
	String strDomain;
	String strDepartment;
	String strChairman;
	String strMessage;
	String strConfInfo;//会议信息
	String strBandInfo;
	String strBandOwner;
	String strUserData1;
	String strUserData2;

	DWORD dwNetVersion;
	SocketIpAddr IpAddr;
	SocketIpAddr IpAddrMulti;
	SocketIpAddr IpAddrVideo;
	SocketIpAddr IpAddrAudio;
	APPVERSION dwAppVersion;
	int nTermType;
	int nTermSubType;
	int nTermLevel;
	DWORD dwGrant;
	int lDispOrder;
	int Order;

	DWORD dwGlobalFlag;
	DWORD dwGlobalId;

	VC3AUXDEVICE* pAuxDevInfo;
	double geoX;
	double geoY;

	bool IsChairman;
	bool IsOuter;//是否夸局
	bool IsStaticSender;
	bool bOnline;

	//extend prop
	DWORD dwStatus;
	VC3Terminal* pChairman;
	void* pUserData;
	TIMEOUTLEN* pTimeout;
	SWCall* pActiveCall;
	short CallAlive;
	WORD callProctol;
	WORD callProctolVer;
	WORD localOrder;
	long lUserData;
	//mask
	union
	{
		DWORD dwMediaState;
		struct
		{
			DWORD VideoRecv:1;
			DWORD AudioRecv:1;
			DWORD VidwoSend:1;
			DWORD AudioSend:1;
		}MediaState;
	};

	VC3Terminal();
	~VC3Terminal();
protected:
	virtual void Release(){};
public:

	virtual LPCTSTR Name()
	{
		return strName.c_str();
	};
	virtual LPCTSTR Aliase()
	{
		if(strNameAliase!="") return strNameAliase.c_str();
		return strName.c_str();
	};
	virtual LPCTSTR FriendlyName()
	{
		return strNameFriendly.c_str();
	};
	virtual void SetFriendlyName(LPCTSTR szName)
	{
		strNameFriendly=szName;
	}

	virtual LPCTSTR Domain()
	{
		return strDomain.c_str();
	};

	virtual LPCTSTR Department()
	{
		return strDepartment.c_str();
	};

	virtual LPCTSTR IPAddress()
	{
		return IpAddr;
	};
	virtual LPCTSTR PhoneNumber()
	{
		return strPhoneNumber.c_str();
	}
	virtual ITerminal* Chairman()
	{
		return pChairman;
	};
	virtual LPCTSTR ConfInfo()
	{
		return strConfInfo.c_str();
	}
	virtual LPCTSTR BandInfo()
	{
		return strBandInfo.c_str();
	}
	virtual int DispOrder()
	{
		return lDispOrder;
	}

	virtual DWORD Type()
	{
		return MAKEDWORD(nTermType,nTermSubType);
	};

	virtual DWORD Level()
	{
		return nTermLevel;
	};

	virtual DWORD Grant()
	{
		return dwGrant;
	};

	virtual DWORD State()
	{
		return dwStatus;
	};

	virtual DWORD GetNetId()
	{
		return IpAddr;
	}

	virtual void* UserPtr()
	{
		return pUserData;
	};

	virtual void SetUserPtr(void* pUserPtr)
	{
		pUserData=pUserPtr;
	};
	//
	int GetCallTimeout(int type)
	{
		TIMEOUTLEN timeout;
		TIMEOUTLEN* pto=pTimeout;
		int to=60;
		if (!pTimeout)
		{
			pto=&timeout;
		}
		switch(type)
		{
		case CallType_VideoTalk:
			to=(*pto)[TOL_TALKCALL]/1000;
			break;
		}
		return to;
		
	}
	int GetActiveTimeout(int type)
	{
		int to=20;
		switch(type)
		{
		case CallType_VideoTalk:
			to=20;
			break;
		}

		return to;

	}
	virtual long SetUserData(long lUserData){return this->lUserData=lUserData;}
	virtual long GetUserData(){return lUserData;}




};

typedef VC3Terminal* LPTerminal;

typedef map<String,VC3Terminal*> TermInfoMap;
typedef TermInfoMap::iterator TermInfoItr;

typedef AutoReleaseArray<VC3Terminal> TermInfoList;
struct TerminalInfo:public StreamBuffer
{
	char szHostName[64];
/*
	char szIpAddr[16];
	char szMultiIpAddr[16];
	char szAudioIpAddr[16];
	char szVideoIpAddr[16];
*/
	SocketIpAddr IpAddr;
	SocketIpAddr IpAddrMulti;
	SocketIpAddr IpAddrVideo;
	SocketIpAddr IpAddrAudio;

	char szUserData1[64];
	char szUserData2[64];
	char szMessage[64];

	//otherinfoex
	char szChairman[64];
	char szDomain[128];
	DWORD dwVersion;
	DWORD dwAppVersion;
	char szBandInfo[128];
	char szBandOwner[128];

	//
	int nTermType;
	//otherinfo
	char szDepartment[128];
	VC3AUXDEVICE auxDev;
	int nTermLevel;
	int IsChairman;
	char szConfInfo[128];
	int IsOuter;
	DWORD dwGrant;
	long lDispOrder;
	double geoX;
	double geoY;
	int IsStaticSender;
	char szAliase[128];
	int nSubType;
	char szPhoneNumber[64];


	char szOrder[32];
	char szLongName[128];
	DWORD dwGlobalFlag;
	DWORD dwGlobalId;
	TerminalInfo(LPCTSTR szTermInfoList,int size):StreamBuffer(szTermInfoList,size){}
	bool ParseNext();
	DWORD UpdateTerminal(VC3Terminal* pTerm,DWORD& dwUpdateHigh);

};

DWORD TermInfoFlagToProp(DWORD dwLow,DWORD dwHigh);

TIMEOUTLEN* GetTimeoutConfig(int talk,int con,int hand,int leave,int report,int join);


};//end namespace SWApi

#endif
