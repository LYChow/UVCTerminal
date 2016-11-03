#ifndef __SENDCOMMAND__H__
#define __SENDCOMMAND__H__


namespace SWApi{

struct VC3Terminal;
#define TERMINAL VC3Terminal

//typedef  VC3Terminal TERMINAL;

/*
Mcu临时提交:
1. 增加刷全部在线终端列表功能, 原刷列表接口扩展filter=GETALLHOST
2. 增加获取某个终端授权列表功能CMD_GETTERMGRANTLIST(接替指挥用)


获取某终端可操作的终端列表及权限值（终端发给MCU）	
CMD_GETTERMGRANTLIST	1028	0	被查询的终端名


获取某终端可操作的终端列表及权限值异步返回结果	
CMD_GETTERMGRANTLIST_RESPONSE_ASYNC	1029
lParam:

HIWORD：总条目数
LOWORD：本页起始编号

sParam: 
条目很多时会分页返回，每页1000个条目	被查询的终端名 HostName GrantValue HostName GrantValue …
*/

#define SZDEF_UPDATELIST_FILTER_GETALLHOST "GETALLHOST"



#define CMD_SENDSHORTMESSAGE				100             //'短消息
#define CMD_COMMON_CONTROL_WITH_PTZTOKEN	101				//0= szParam为云台控制命令
															//1=szParam为扩展面板命令



#define CMD_CMDMODE_CALL					103                 //指挥模式的呼叫消息
#define CMD_CMDMODE_ACCEPT_CALL				104          //指挥模式的接受呼叫消息
#define CMD_CMDMODE_REJECT_CALL				105          //指挥模式的拒绝呼叫消息
#define CMD_CMDMODE_LEAVE					106                //指挥模式的呼叫消息,sparam=[SenderName]+空格+[NewChairmanName]
#define CMD_CMDMODE_ACCEPT_LEAVE			107         //指挥模式的接受呼叫消息,sparam=[NewChairmanName]
#define CMD_CMDMODE_REJECT_LEAVE			108         //指挥模式的拒绝呼叫消息,sparam=[NewChairmanName]
#define CMD_CMDMODE_STOP					109                 //指挥模式的停止指挥消息
#define CMD_CMDMODE_EXIT					110                 //指挥模式的退出被指挥状态消息
#define CMD_CMDMODE_PULSE				    111                //指挥模式的心跳脉冲消息
#define CMD_CMDMODE_HAND					112                 //指挥模式的举手申请发言消息
#define CMD_CMDMODE_ACCEPT_HAND				113          //指挥模式的接受发言申请消息
#define CMD_CMDMODE_REJECT_HAND				114          //指挥模式的拒绝发言申请消息
#define CMD_CMDMODE_PULSE_RESPONSE          115       //指挥模式的心跳脉冲回应消息
#define CMD_CMDMODE_ASKJOINCOMMAND          116       //指挥模式下申请加入指挥组消息
#define CMD_CMDMODE_REJECT_ASK				117           //指挥模式下拒绝加入指挥组的申请
#define CMD_CMDMODE_CANCEL_ASK_JOIN         118      //指挥模式下取消加入申请消息
#define CMD_CMDMODE_STOP_ASK_JOIN			119        //指挥模式下停止加入申请消息
#define CMD_CMDMODE_RANGE_UPPER				119
#define CMD_CMDMODE_CANCEL_HAND				120

//talk msg
#define CMD_INTERTALK_START					121              //'开始对讲请求消息
#define CMD_INTERTALK_ACCEPT_START			122       //'接受开始对讲请求消息
#define CMD_INTERTALK_REJECT_START			123      // '拒绝开始对讲请求消息
#define CMD_INTERTALK_STOP					124              // '结束对讲消息
#define CMD_INTERTALK_BACKSTOP				125          // '反向结束对讲消息
#define CMD_INTERTALK_PULSE					126             // '对讲模式下的心跳脉冲消息
#define CMD_INTERTALK_PULSE_RESPONSE		127    // '对讲模式下的心跳脉冲消息
#define CMD_INTERTALK_REPORT				128		//对讲上报
#define CMD_INTERTALK_REJECT_REPORT			129
#define CMD_INTERTALK_CANCEL_REPORT         130
#define CMD_INTERTALK_END					131

#define CMD_LOUDSPEECH_CALL					143              //语音喊话模式的呼叫指令,sparam=[SenderName]+空格+[SenderIP]
#define CMD_LOUDSPEECH_ACCEPT_CALL          144       //语音喊话模式的接受呼叫指令
#define CMD_LOUDSPEECH_REJECT_CALL          145       //语音喊话模式的拒绝呼叫指令
#define CMD_LOUDSPEECH_STOP					149              //语音喊话模式的停止喊话模式指令
#define CMD_LOUDSPEECH_EXIT					150              //语音喊话模式的退出被指挥状态指令
#define CMD_LOUDSPEECH_PULSE				151             //语音喊话模式的心跳脉冲指令
#define CMD_LOUDSPEECH_PULSE_RESPONSE       155    //语音喊话模式的心跳脉冲回应指令

#define CMD_CONMODE_RANGE_LOWER				203
#define CMD_CONMODE_CALL					203                 //会议模式的呼叫消息
#define CMD_CONMODE_ACCEPT_CALL				204          //会议模式的接受呼叫消息
#define CMD_CONMODE_REJECT_CALL				205          //会议模式的拒绝呼叫消息
#define CMD_CONMODE_LEAVE					206                //会议模式的呼叫消息,sparam=[SenderName]+空格+[NewChairmanName]
#define CMD_CONMODE_ACCEPT_LEAVE			207         //会议模式的接受呼叫消息,sparam=[NewChairmanName]
#define CMD_CONMODE_REJECT_LEAVE			208         //会议模式的拒绝呼叫消息,sparam=[NewChairmanName]
#define CMD_CONMODE_STOP					209                 //会议模式的停止指挥消息
#define CMD_CONMODE_EXIT					210                 //会议模式的退出被指挥状态消息
#define CMD_CONMODE_PULSE					211                //会议模式的心跳脉冲消息
#define CMD_CONMODE_HAND					212                 //会议模式的举手申请发言消息
#define CMD_CONMODE_ACCEPT_HAND				213          //会议模式的接受发言申请消息
#define CMD_CONMODE_REJECT_HAND				214          //会议模式的拒绝发言申请消息
#define CMD_CONMODE_PULSE_RESPONSE          215       //会议模式的心跳脉冲回应消息
#define CMD_CONMODE_ASKJOINCONFERENCE       216    //会议模式下申请加入会议消息
#define CMD_CONMODE_REJECT_ASK				217           //会议模式下拒绝加入会议的申请
#define CMD_CONMODE_CANCEL_ASK_JOIN         218      //会议模式下取消加入申请消息
#define CMD_CONMODE_STOP_ASK_JOIN			219        //会议模式下停止加入申请消息
#define CMD_CONMODE_CANCEL_HAND				220
//#define CMD_CONMODE_RANGE_UPPER				219

#define CMD_ATTENDLIST						300		//           '参会者列表
#define CMD_GENERAL_CALLMSG					301		//新呼叫协议

//{{{ 双流消息
#define CMD_DATASTREAMSWITCH				302			//'通知参会终端打开或关闭数据流：1=打开，0=关闭
#define CMD_DATACONFERENCESWITCH			303			//'通知参会终端数据会议开始或停止：1=开始，0=停止
//}}
#define CMD_REPORT_CAPBILITY				304			//'报告能力 ：0 = 正常，1 = 双流数据能力
#define CMD_CLEARECHO						305			//通知对方消除回声
#define CMD_CMDGROUP_NOTIFY					306   //lParam=0,sparam=type,action,creator,groupname



struct MSG_TERMOPENAV
{
	String csMsg;
	MSG_TERMOPENAV(bool bOpen,int type,struct TERMINAL* pTerm);
	operator LPCTSTR(){return csMsg.c_str();}
};

struct TERMOPENAV_MSG
{
	bool bOpen;
	int type;
	struct TERMINAL* pTerm;
	struct TERMINAL* pSendTerm;
	TERMOPENAV_MSG(LPCTSTR szMsg);
};

struct MSG_TERMOPENAV_RET
{
	String csMsg;
	MSG_TERMOPENAV_RET(int ret,int type,struct TERMINAL* pTerm);
	operator LPCTSTR(){return csMsg.c_str();}

};

struct TERMOPENAV_MSG_RET
{
	int ret;
	int type;
	struct TERMINAL* pTerm;
	struct TERMINAL* pSendTerm;
	TERMOPENAV_MSG_RET(LPCTSTR szMsg);
};

enum TERMNOTIFY_E
{
	TERMNOTIFY_CONFCREATE,					//会议信息:发送者,操作{Add,Del,Rename},主席,会议名称[新名称|原名称],预约时间,
	TERMNOTIFY_OPEN_AV,						// sender,open,avtype{av,v,a},source_termname,ip
	TERMNOTIFY_OPEN_AV_RET,					// sender,ret{ok,err{}},avtype,source_termname,receiver
	TERMNOTIFY_OFFLINEFILE,					// sender,filename,filepath,comment
	TERMNOTIFY_RMTCTL,						// ctlcode,sender,msg
	TERMNOTIFY_RMTCTL_RESP,					// ctlcode,sender,ret msg
	TERMNOTIFY_SELVIDEOSTREAM,				// select video stream =0,1,2,3
	TERMNOTIFY_CONFAPPINFO,					// app user defined string
};

#define CMD_TERM_NOTIFY						307  //lParam=TERMNOTIFY_E

#define CMD_RECORDSERVER_CALL				501          //  '呼叫录像服务器指令,sparam=[GroupName]+空格+[SenderName]+空格+[SenderIP]
#define CMD_RECORDSERVER_BROAD				502          // '会议状态下广播转移指令,sparam=[GroupName]+空格+[SenderName]+空格+[SenderIP]+空格+[TransName]+空格+[TransIP]
#define CMD_RECORDSERVER_STOP				503        //    '停止录像服务器指令,sparam=[GroupName]+空格+[SenderName]+空格+[SenderIP]
#define CMD_RECORDSERVER_ACCEPT_CALL		504  //   '录像服务器接受呼叫指令,sparam=[chairmanName]+空格+[SenderName]
#define CMD_RECORDSERVER_REJECT_CALL		505  //   '录像服务器拒绝呼叫指令,sparam=[chairmanName]+空格+[SenderName]
#define CMD_RECORDSERVER_PULSE				506        //   '发给录像服务器的心跳脉冲指令,sparam=[SenderName]
#define CMD_RECORDSERVER_PULSE_RESPONSE		507 // '录像服务器发出的心跳脉冲回应指令,sparam=[chairmanName]+空格+[SenderName]
#define CMD_RECORDSERVER_EXIT				508           // '录像服务器退出指令

//指挥会议状态
//sParam=CONFSTATE_INDEX[会议名称,会议持续时间,参会人数,主席,转移终端,双流发送者,数据会议主持人]
#define CMD_CONFSTATUS						600				

//数据会议透传消息
#define CMD_DATACONFMSG						610				//
#define CMD_TERMCONFIG						611				//termname|name=value;..name=value



#define CMD_TRANSMIT_FILE					623		//简化方式传输文件（基于SendCommand携带内容的文件传输）
#define CMD_TRANSMIT_FILE_RECEIVED			624		//简化方式传输文件的回执
#define CMD_TRANSMITFILE_NOTIFYTERM			905		//简化方式传输文件向应用层发通知
#define CMD_TRANSMITFILE_NOTIFYMCU			1022	//简化方式传输文件向MCU发通知

#define CMD_VOIPMSG							800
#define CMD_VOIPCMD							801
#define CMD_SIPPHONEMSG						810

#define CMD_EXTENDPANEL_INDICATOR			815				//终端扩展面板专用"指示/通知/消息"命令
#define CMD_EXTENDPANEL_RESPONSE			816				//终端扩展面板专用"响应/应答"命令
#define CMD_INSERTKEYFRAME					821				//

//#include "H323_VC3_Define.h"				//[851,861]

#define CMD_FORMATTED_REPORT				902
#define CMD_FORMATTED_REPORT_RESPONSE		903

#define CMD_CUSTOM_AFEU_KILLED_JUDGE_RESULT 1023
const int CMD_NOTIFY_REFRESHTERMINALLIST = 906;		//通知终端需要刷新终端列表
#define BITSTATE_HIDEMODE			0x00000002	//终端隐身状态（0-未隐身；1-隐身）


#define CMD_CLEARALLSENDFROMME				1001          //'MCU命令，在MCU上清除所有本终端作为发送者的登记
#define CMD_SETTERMTYPEONMCU				1002            //'MCU命令，更改本终端在MCU上登记的终端类型
#define CMD_SETTERMLEVELONMCU				1003           //'MCU命令，更改本终端在MCU上登记的终端级别
#define CMD_REGISTERASCOMMANDER				1004         //'MCU命令，将自己在MCU上注册为指挥者或会议主席
#define CMD_SETCMDOFCONFMEMBERS				1018         //MCU命令,向MCU发送参会列表

#define CMD_SETATTRIBONMCU	1013			//通用的设置在MCU上的自身相关属性
											//SW_VC3_SendCommand( , "MCU", CMD_SETATTRIBONMCU, 0, "<DetachedCommandGrant=1>"
/*
MCU通知终端清除一个发送者
	lParam: 1=video;2=audio,3=data
	sParam: szSender+"\r\n"+szCauseCode
		szCauseCode: 0=For Conference
		

*/											
#define CMD_REMOVESENDER					1026		

#define CMD_GETTERMGRANTLIST				1028
#define CMD_GETTERMGRANTLIST_RESPONSE_ASYNC 1029

#define CMD_SYSINNER_ALARMDATA_FROMSERVER		65537
#define CMD_SYSINNER_ALARMDATA_BACKTOSERVER		65538
#define CMD_SYSINNER_CALLREADYCMD				65539
#define CMD_SYSTEMSCAN_REQUEST					65540
#define CMD_SYSTEMSCAN_RESPONSE					65541
#define CMD_SYSINNER_LONGSPARAM_FRAGMENT		65542

														
enum CONFSTATE_INDEX
{
	CSI_CNAME,
	CSI_CTIME,
	CSI_ATCNT,
	CSI_CHRM,
	CSI_TSTERM,
	CSI_DSSENDER,
	CSI_DCCHRM,
	CSI_SPECIAL,
	CSI_CMD_PAUSED,
	CSI_CONF_APPSTATUS,

	CSI_END
};

LPCTSTR GetVC3CmdName(int cmd);

//CMD_TERM_NOTIFY-0
int FormatConfInfoNotifyMsg(String& csMsg,int op,LPCTSTR szOldName,LPCTSTR szNewName,time_t start,time_t duration);

int ParseConfInfoMsg( LPCTSTR szMsg,String& csChair,int& op,String& csOldName,String& csNew,time_t& start,time_t duration);


int FormatTermConfigMsg(char* szMsg,LPCTSTR szFrom,DWORD dwVersion,struct TIMEOUTLEN* tmout,LPCTSTR szIpAddr,LPCTSTR szOwner,int level,DWORD dwCapbility);
int ParseTermConfigMsg(LPCTSTR szMsg,char* szFrom,DWORD& dwVersion,struct TIMEOUTLEN* tmout,char* szIpAddr,char* szOwner,int& level,DWORD& dwCapbility);


};//end namespace SWApi

#endif

