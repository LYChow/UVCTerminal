#ifndef __ISWUA__H__
#define __ISWUA__H__
#include "IDataObject.h"
namespace SWApi
{
    //	using namespace MediaLib;

enum LoginRet
{
	LoginConnectFail = -4,
	LoginGetRegInfoFail,
	LoginGetRegInfoLoginedElsewhere,
	LoginRegNodeFail,
	LoginSucess,
	LoginConnecting,
	LoginGetRegInfo,
	LoginRegNode,
};


enum CallType
{
	CallType_None =0,
	CallType_VideoTalk,//建立视频对讲
	CallType_VideoConfInvite,//邀请参加视频会议,
	CallType_VideoConfJoin,//申请加入视频会议,
};


enum CallDirection
{
	CallDir_OutGoing=0,
	CallDir_InComming,
};

enum CallState
{
	CallState_Invalid=-1,
	CallState_Idle,
	CallState_Setup,
	CallState_Alert,
	CallState_Connect,
	CallState_Release,
};

enum CallMsgType
{
	CallMsg_Setup = 0, //incomming recv, peer request create call
	CallMsg_Cancel,		//incomming .., peer canceled a call
	CallMsg_Accept,		//outgoing .., peer accept a call
	CallMsg_Reject,		//outgoing .., peer reject a call
	CallMsg_Release,	//incomming .., peer release a call
	CallMsg_ReleaseComplete, //outgoing .., peer release a call complete
	CallMsg_Alert = 8,	//outgoing ..,peer alerting
};

enum CallResult
{
	CallResult_Success,
	CallResult_Canceled,
	CallResult_Busy,
	CallResult_Offline,
	CallResult_Reject,
	CallResult_Missed,
	CallResult_LossConnection,
	CallResult_ErrorKnown,
};

struct ISWCall:public IObject
{
//property
	virtual int GetType()=0;
	virtual int GetDirection()=0;
	virtual int GetState()=0;
	virtual struct ITerminal* GetPeer()=0;
	virtual LPCTSTR GetCallInfo()=0;
	virtual int GetCallResult()=0;
	virtual time_t GetStartTime()=0;
	virtual int GetDuration()=0;
//operation
	virtual int Setup(long lParam=0)=0;
	virtual int Cancel()=0;
	virtual int Accept()=0;
	virtual int Reject(int cause,LPCTSTR szCause=0)=0;
	virtual int ReleaseCall(int ret=0)=0;
};

enum TermProp
{
	TermPropName = 0,
	TermPropAlise,
	TermPropFriendlyName,
	TermPropDepartment,
	TermPropDomain,
	TermPropIPAddress,
	TermPropPhone,
	TermPropChairman,
	TermPropConfInfo,
	TermPropBandInfo,
	TermPropDispOrder,
	TermPropType,
	TermPropLevel,
	TermPropGrant,
	TermPropState,
};

enum TermStateFlag
{
	TermStateOnLine=0,
	TermStateInConf,
	TermStateAudioRecv,
	TermStateVideoRecv,
	TermStateAudioSend,
	TermStateVideoSend,
};

struct ITerminal:public IObject
{
	virtual LPCTSTR Name()=0;
	virtual LPCTSTR Aliase()=0;

	virtual LPCTSTR FriendlyName()=0;
	virtual void SetFriendlyName(LPCTSTR szName)=0;

	virtual LPCTSTR Department()=0;
	virtual LPCTSTR Domain()=0;
	virtual LPCTSTR IPAddress()=0;
	virtual LPCTSTR PhoneNumber()=0;
	virtual ITerminal* Chairman()=0;
	virtual LPCTSTR ConfInfo()=0;
	virtual LPCTSTR BandInfo()=0;
	virtual int DispOrder()=0;
	virtual DWORD Type()=0;
	virtual DWORD Level()=0;
	virtual DWORD Grant()=0;
	virtual DWORD State()=0;
	virtual DWORD GetNetId()=0;
	virtual void* UserPtr()=0;
	virtual void SetUserPtr(void* pUserPtr)=0;

};


enum UAMessage
{
	UAMsg_NetOpenMedia,//iParam=MediaType{1=video,2=audio},lParam=(const char*)PeerAddr
	UAMsg_NetCloseMedia,//iParam=MediaType{1=video,2=audio},lParam=(const char*)PeerAddr
	UAMsg_NetVideoNeedKeyFrame,//iParam=undefine,lParam=(const char*)PeerAddr
	UAMsg_TerminalMediaOpen,//iParam=MediaType,lParam=(ITerminal*)pTerm
	UAMsg_TerminalMediaClose,//iParam=MediaType,lParam=(ITerminal*)pTerm
};


typedef ITerminal* PTERM;

struct ISWUA;
struct ISWUAEventHandler
{
	virtual void OnUALogin(ISWUA* pua,int ret){};
	virtual void OnUAStateChange(ISWUA* pua,int state){};
	virtual void OnUACall(ISWUA* pua,ISWCall* pCall,int msg,long info){};
	virtual void OnUARecvShortMessage(ISWUA* pua,ITerminal* pTerm,LPCTSTR szMessage,LPCTSTR szInfo){};
	virtual void OnUARecvTerminalCommand(ISWUA* pua,ITerminal* pTerm,int lParam,LPCTSTR szParam){};
	virtual void OnUATerminalInfoUpdate(ISWUA* pua,ITerminal* pTerm,DWORD dwUpdateFlag){};
	virtual void OnUAMessage(ISWUA* pua,int msg,int iParam,long lParam){};

};


enum UAState
{
	UA_Idle,//空闲
	UA_Login,//正在登录
	UA_Working,
	UA_ReConnect,//
	UA_Logout,
};


struct IPTZControl:public IObject
{
	virtual void SetControlTerminal(ITerminal* pTerm)=0;
	virtual ITerminal* GetControlTerminal()=0;
	virtual int Move(int dir)=0;//0=stop,1=left,2=up,3=right,4=down
	virtual int Move3D(int pan,int tile,int zoom)=0;
	virtual int SavePreset(int iPreset)=0;
	virtual int LoadPreset(int iPreset)=0;
	//analog:0=大小,1=光圈,3=聚集
	virtual int AdjustAnalog(int iAnalog,int step)=0;
	//switch:0=灯光,1=雨刷,2=电源
	virtual int SetSwitch(int iSwitch,bool bOpen)=0;
	virtual int SendControlCode(const char* pCode,int size)=0;
//	virtual int SetDeviceIndex(int i);
//	virtual int GetDeviceIndex();

};


struct ISWUA:public IObject
{
	//comunication
public:
	virtual int Login(LPCTSTR szServer,LPCTSTR szUsername,LPCTSTR szPassword)=0;
	virtual int CancelLogin()=0;
	virtual int Logout()=0;
	virtual ITerminal* GetSelf()=0;
	virtual ITerminal* AddTerminal(LPCTSTR szTermName,LPCTSTR szFriendlyName="")=0;
	virtual ITerminal* GetTerminalByName(LPCTSTR szTermName)=0;
	virtual int GetTerminalCount()=0;
	virtual ITerminal* GetTerminal(int i)=0;
	virtual int UpdateTerminalInfo(int nCount,ITerminal* pTerms[]=0)=0;
	virtual ISWCall* CreateCall(ITerminal* pTerm,int type)=0;

	//获取所有呼叫,包括正则进行中的和完成的呼叫
	virtual int GetCallCount()=0;
	virtual ISWCall* GetCall(int i)=0;
	//设置最大的呼叫历史保存条数(包括正则进行的),超过最大条数自动删除最早的呼叫
	virtual int SetMaxCallCount(int nMax=200)=0;

	virtual int SendShortMessage(ITerminal* pTerm,LPCTSTR szMessage,LPCTSTR szInfo="")=0;
	virtual int BroadShortMessage(int nCount,ITerminal* pTerms[],LPCTSTR szMessage,LPCTSTR szInfo="")=0;

	virtual int SendCommand(ITerminal* pTerm,DWORD dwCommandCode,int lParam,LPCTSTR szParam="")=0;
	virtual IPTZControl* GetPTZControl(ITerminal* pTerm)=0;
	virtual int SendTermVideo(ITerminal* pTermTo,int iChannel=0)=0;
	virtual int SendTermAudio(ITerminal* pTermTo,int iChannel=0)=0;
	virtual DWORD GetNetMediaId()=0;

};

};//end namespace


#endif
