// GenericCallProtocol.h: interface for the GenericCallProtocol class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(__GENERICCALLPROTOCOL__H__)
#define __GENERICCALLPROTOCOL__H__
namespace SWApi{
/************************************************************************
//协议格式
	1.使用字符串协议
	2.参数使用<Key>=<Value>形式
	3.参数之间使用"\r\n"分隔


//呼叫相关命令(消息)
1 呼叫命令
名称: GENERALMSG_CALL
参数:
	<呼叫ID>
	<发送者>
	<呼叫目的>
	<呼叫用途>
	<呼叫信息类型>
	<呼叫信息>

2 取消命令
名称: GENERALMSG_CANCEL
参数:
	<呼叫ID>
	<发送者>


3 接受命令
名称: GENERALMSG_ACCEPT
参数:
	<呼叫ID>
	<发送者>

4 拒绝命令
名称: GENERALMSG_REJECT
参数:
	<呼叫ID>
	<发送者>
	<原因>

5 停止命令
名称: GENERALMSG_STOP
参数:
	<呼叫ID>
	<发送者>

6 退出命令
名称: GENERALMSG_EXIT
参数:
	<呼叫ID>
	<发送者>
	<结果>

ccccccccccccccccccccccccccccccc
//状态检测相关命令
**************************************************** 
1.状态检测请求
名称: GENERALMSG_STATUS_CHECK
参数:
	<发送者>
	<状态信息>
	<保留时间>

2.状态检测应答
名称: GENERALMSG_STATUS_CHECK_ACK
参数:
	<发送者>
	<状态信息>
	<保留时间>

//状态信息
1.对讲状态=(msgpeer=talkpeer)
2.会议状态=(msgpeer is one member of conference)
3.参会状态=(msgpeer is chairman)


//命令相关参数定义(不区分大小写)
0.命令码
	Key=MSGName
	Value=枚举值{Call,Cancel,Accept,Reject,Stop,Exit,Status,StatusAck}

1.呼叫ID
	Key=CallID
	Value=任意字符串
2.发送者
	Key=Sender
	Value=终端名称(发送到跨局终端要加局名)
3.呼叫目的
	Key=Purpose
	Value=枚举值{Create,Join,Request}
4.呼叫用途
	Key=Application
	Value=枚举值{Surveil,Talk,Command,Conference,HandUp}
5.原因
	Key=Cause
	Value=枚举值{{公共的原因定义枚举}-{应用相关枚举定义}}
7.结果
	Key=Result
	Value=枚举值{Success,{...}}

8.状态信息
	Key=Status
	Value=枚举值{Idle,Surveil,Talk,Command,Conference,HandUp}
9.保留时间
	Key=KeepAlive
	Value=秒数

10.呼叫信息类型
	Key=InfoType
	Value=枚举值(根据呼叫目的不同取不同的枚举值):
		1).呼叫目的为Create或Join时表示对端创建呼叫需要的附加信息类型
		2).呼叫目的为Request时表示请求的类型={Create,Hand,Leave}

11.呼叫信息
	Key=Info
	Value=呼叫中用到的参数信息

************************************************************************/
//#define CMD_GENERAL_CALLMSG		301
//SendCommand发送消息,参数定义
//	dwCommand=CMD_GENERAL_CALLMSG
//	lParam=0
//	sParam=<消息文本(包括消息名)>


//SendCommand参数lParam值定义
enum GENERAL_MESSAGE_E
{
	GENERALMSG_CALL,	//Call
	GENERALMSG_CANCEL,	//Cancel
	GENERALMSG_ACCEPT,	//Accept
	GENERALMSG_REJECT,	//Reject
	GENERALMSG_STOP,	//Stop
	GENERALMSG_EXIT,	//Exit

	GENERALMSG_STATUS_CHECK,	//Status
	GENERALMSG_STATUS_CHECKACK,	//StatusAck

	GENERALMSG_COUNT

}; 



#define GENERALCALL_MSG_MAXSIZE 1024

//参数Key定义
enum GC_KEY_INDEX_E
{
	GC_KEYIDX_MSGNAME,
	GC_KEYIDX_CALLID,
	GC_KEYIDX_SENDER,
	GC_KEYIDX_PURPOSE,
	GC_KEYIDX_APPLICATION,
	GC_KEYIDX_CAUSE,
	GC_KEYIDX_RESULT,
	GC_KEYIDX_STATUS,
	GC_KEYIDX_KEEPALIVE,
	GC_KEYIDX_INFOTYPE,
	GC_KEYIDX_INFO,

	GC_KEYIDX_COUNT
};

//参数值枚举定义
//Purpose
enum PURPOSE_INDEX_E
{
	PURPOSEIDX_CREATE,
	PURPOSEIDX_JOIN,
	PURPOSEIDX_REQUEST,
};
//Application
enum APPLICATION_INDEX_E
{
	APPLICATIONIDX_SURVEIL,
	APPLICATIONIDX_TALK,
	APPLICATIONIDX_COMMAND,
	APPLICATIONIDX_CONFERENCE,

	APPLICATIONIDX_COUNT
};

//Status
enum STATUS_INDEX_E
{
	STATUSIDX_IDLE,
	STATUSIDX_SURVEIL,
	STATUSIDX_TALK,
	STATUSIDX_COMMAND,
	STATUSIDX_CONFERENCE,

	STATUSIDX_COUNT
};

//Cause
enum CAUSE_INDEX_E
{
	CAUSEIDX_APPLICATION,
	CAUSEIDX_GENERAL,
};
//////////////////////////////////////////////////////////////////////////
LPCTSTR EnumMsgNameToStr(int iMsgName);
int StrMsgNameToEnum(LPCTSTR szMsgName);
LPCTSTR EnumPurposToStr(int iPurpose);
int StrPurposeToEnum(LPCTSTR szPurpose);
LPCTSTR EnumApplicationToStr(int iApp);
int StrApplicationToEnum(LPCTSTR szApp);
LPCTSTR EnumStatusToStr(int iStatus);
int StrStatusToEnum(LPCTSTR szStatus);
LPCTSTR EnumReqTypeToStr(int iType);
int StrReqTypeToEnum(LPCTSTR szReqType);

//////////////////////////////////////////////////////////////////////////
class GenericCallMsgParser
{
public:
	char m_Buf[GENERALCALL_MSG_MAXSIZE];
	const char* m_szValues[GC_KEYIDX_COUNT];
public:
	GenericCallMsgParser(LPCTSTR szMessage=NULL,int* pRet=NULL);
	~GenericCallMsgParser();
	int Parse(LPCTSTR szMessage);
	int MSGName();
	DWORD CallID();
	LPCTSTR Sender();
	int Purpose();
	int Application();
	int Cause();
	int  Result();
	int Status();
	int KeepAlive();
	int InfoType();
	LPCTSTR szInfo();

};

class GenericCallMsgGenerator
{
	char m_Buf[GENERALCALL_MSG_MAXSIZE];
public:
	GenericCallMsgGenerator();
	~GenericCallMsgGenerator();
	operator LPCTSTR(){return (LPCTSTR)this;}
public:
	LPCTSTR FormatCallMsg(LPCTSTR szCallID,LPCTSTR szSender,LPCTSTR szPurpose,LPCTSTR szApplication,LPCTSTR szInfoType,LPCTSTR szInfo);
	LPCTSTR FormatCancelMsg(LPCTSTR szCallID,LPCTSTR szSender);
	LPCTSTR FormatAcceptMsg(LPCTSTR szCallID,LPCTSTR szSender);
	LPCTSTR FormatRejectMsg(LPCTSTR szCallID,LPCTSTR szSender,LPCTSTR szCause);
	LPCTSTR FormatStopMsg(LPCTSTR szCallID,LPCTSTR szSender);
	LPCTSTR FormatExitMsg(LPCTSTR szCallID,LPCTSTR szSender,LPCTSTR szResult);

	LPCTSTR FormatStatusCheckMsg(LPCTSTR szSender,LPCTSTR szStatus,LPCTSTR szKeepAlive);
	LPCTSTR FormatStatusCheckAckMsg(LPCTSTR szSender,LPCTSTR szStatus,LPCTSTR szKeepAlive);
};


};//end namespace SWApi

#endif 
