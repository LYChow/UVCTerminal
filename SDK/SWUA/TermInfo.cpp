#include "__PCH.h"
#include "TermInfo.h"
#include "SWUA.h"
//#include "SWApi.h"
namespace SWApi{
static const char* szNull="";

#define DELETESTR(p) if((p)&&(p)!=szNull){ delete p;p=0;}

#define ISNullStr(p) ((p)==0 || *(p)==0 || ((*p)=='-' && *((p)+1)==0))

#define RealStr(p) (ISNullStr(p)?szNull:p)


VC3Terminal::VC3Terminal()
{
	ZeroMemory(&dwNetVersion,sizeof(*this)-_MMBOFFSET(dwNetVersion));
}

VC3Terminal::~VC3Terminal()
{
	delete pAuxDevInfo;
	ZeroMemory(&dwNetVersion,sizeof(*this)-_MMBOFFSET(dwNetVersion));
}



bool TerminalInfo::ParseNext()
{

	ZeroMemory(szHostName,sizeof(*this)-sizeof(StreamBuffer));

	if(*PopVal<int>()==0) return 0;
	const char* szName=0;
	char szIpAddr[16];
	char szMultiIpAddr[16];
	char szAudioIpAddr[16];
	char szVideoIpAddr[16];
	
	szName=PopString();
	PopString(szIpAddr);
	PopString(szMultiIpAddr);
	PopString(szAudioIpAddr);
	PopString(szVideoIpAddr);
	PopString(szUserData1);
	PopString(szUserData2);
	PopString(szMessage);
	IpAddr=szIpAddr;
	IpAddrMulti=szMultiIpAddr;
	IpAddrVideo=szVideoIpAddr;
	IpAddrAudio=szAudioIpAddr;


	LPCTSTR szOtherInfoEx=PopString();

	int n=sscanf(szOtherInfoEx,SZOTHERINFOEX_SSCANF_FMT,
		szChairman,szDomain,&dwVersion,&dwAppVersion,szBandInfo,szBandOwner
		);
	
	PopVal(nTermType);

	LPCTSTR szOtherInfo=PopString();
	VC3AUXDEVICE* pAuxDevInfo=&auxDev;

	n=sscanf( szOtherInfo, SZOTHERINFO_SSCANF_FMT, \
		szDepartment, \
		(int*)&pAuxDevInfo->VideoDivType,(int*)&pAuxDevInfo->DEVNum,(int*)&pAuxDevInfo->CurDevNo, \
		pAuxDevInfo->DevName[0], (int*)&pAuxDevInfo->YTFlag[0], \
		pAuxDevInfo->DevName[1], (int*)&pAuxDevInfo->YTFlag[1], \
		pAuxDevInfo->DevName[2], (int*)&pAuxDevInfo->YTFlag[2], \
		pAuxDevInfo->DevName[3], (int*)&pAuxDevInfo->YTFlag[3], \
		pAuxDevInfo->DevName[4], (int*)&pAuxDevInfo->YTFlag[4], \
		pAuxDevInfo->DevName[5], (int*)&pAuxDevInfo->YTFlag[5], \
		pAuxDevInfo->DevName[6], (int*)&pAuxDevInfo->YTFlag[6], \
		pAuxDevInfo->DevName[7], (int*)&pAuxDevInfo->YTFlag[7], \
		pAuxDevInfo->DevName[8], (int*)&pAuxDevInfo->YTFlag[8], \
		pAuxDevInfo->DevName[9], (int*)&pAuxDevInfo->YTFlag[9], \
		pAuxDevInfo->DevName[10], (int*)&pAuxDevInfo->YTFlag[10], \
		pAuxDevInfo->DevName[11], (int*)&pAuxDevInfo->YTFlag[11], \
		pAuxDevInfo->DevName[12], (int*)&pAuxDevInfo->YTFlag[12], \
		pAuxDevInfo->DevName[13], (int*)&pAuxDevInfo->YTFlag[13], \
		pAuxDevInfo->DevName[14], (int*)&pAuxDevInfo->YTFlag[14], \
		pAuxDevInfo->DevName[15], (int*)&pAuxDevInfo->YTFlag[15], \
		&nTermLevel,(int*)&pAuxDevInfo->CodecType,(int*)&IsChairman,szConfInfo,
		(int*)&IsOuter,(int*)&dwGrant,(int*)&lDispOrder,&geoX,&geoY, \
		&IsStaticSender,szAliase,&nSubType,szPhoneNumber, \
		(int*)&pAuxDevInfo->YTAddr[0],(int*)&pAuxDevInfo->YTAddr[1],(int*)&pAuxDevInfo->YTAddr[2],(int*)&pAuxDevInfo->YTAddr[3],szOrder,szHostName,&dwGlobalFlag,&dwGlobalId);
	return 1;

}

DWORD TermInfoFlagToProp(DWORD dwLow,DWORD dwHigh)
{
	DWORD dwFlag=0;
	if(TermInfo_Name&dwLow){dwFlag|=1<<TermPropName;}
	if(TermInfo_NameAliase&dwLow){dwFlag|=1<<TermPropAlise;}
	if(TermInfo_PhoneNumber&dwLow){dwFlag|=1<<TermPropPhone;}
	if(TermInfo_Domain&dwLow){dwFlag|=1<<TermPropDomain;}
	if(TermInfo_Department&dwLow){dwFlag|=1<<TermPropDepartment;}
	if(TermInfo_IpAddr&dwLow){dwFlag|=1<<TermPropIPAddress;}
//	if(TermInfo_MultiAddr&dwLow){dwFlag|=1<<TermProp;}
///	if(TermInfo_AudioIpAddr&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_VideoIpAddr&dwLow){dwFlag|=1<<TermProp;}
	if(TermInfo_Chairman&dwLow){dwFlag|=1<<TermPropChairman;}
//	if(TermInfo_Message&dwLow){dwFlag|=1<<TermProp;}
	if(TermInfo_ConfInfo&dwLow){dwFlag|=1<<TermPropConfInfo;}
	if(TermInfo_BandInfo&dwLow){dwFlag|=1<<TermPropBandInfo;}
//	if(TermInfo_BandOwner&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_UserData1&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_UserData2&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_NetVersion&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_AppVersion&dwLow){dwFlag|=1<<TermProp;}
	if(TermInfo_TermType&dwLow){dwFlag|=1<<TermPropType;}
//	if(TermInfo_TermSubType&dwLow){dwFlag|=1<<TermProp;}
	if(TermInfo_TermLevel&dwLow){dwFlag|=1<<TermPropLevel;}
	if(TermInfo_Grant&dwLow){dwFlag|=1<<TermPropGrant;}
	if(TermInfo_DispOrder&dwLow){dwFlag|=1<<TermPropDispOrder;}
//	if(TermInfo_Order&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_GlobalFlag&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_GlobalId&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_AuxDevInfo&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_GeoX&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_GeoY&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_IsChairman&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_IsOuter&dwLow){dwFlag|=1<<TermProp;}
//	if(TermInfo_IsStaticSender&dwLow){dwFlag|=1<<TermProp;}
	return dwFlag;
}

#define AsignTermStr(mmb,p) if(pTerm->str##mmb!=RealStr(p)){pTerm->str##mmb=RealStr(p);dwUpdateLow|=TermInfo_##mmb;}
#define AsignTermVal(mmb,v) if(pTerm->mmb!=v){pTerm->mmb=v;dwUpdateLow|=TermInfo_##mmb;}


DWORD TerminalInfo::UpdateTerminal( VC3Terminal* pTerm,DWORD& dwUpdateHigh )
{
	DWORD dwUpdateLow=0;
	if (pTerm==NULL)
	{
		return 0;
	}
	AsignTermStr(NameAliase,szAliase);
	AsignTermStr(PhoneNumber,szPhoneNumber);
	AsignTermStr(Domain,szDomain);
	AsignTermStr(Department,szDepartment);
	AsignTermVal(IpAddr,IpAddr);
	AsignTermVal(IpAddrMulti,IpAddrMulti);
	AsignTermVal(IpAddrVideo,IpAddrVideo);
	AsignTermVal(IpAddrAudio,IpAddrAudio);
/*
	AsignTermStr(MultiAddr,szMultiIpAddr);
	AsignTermStr(AudioIpAddr,szAudioIpAddr);
	AsignTermStr(VideoIpAddr,szVideoIpAddr);
*/
	AsignTermStr(Message,szMessage);
	AsignTermStr(ConfInfo,szConfInfo);
	AsignTermStr(BandInfo,szBandInfo);
	AsignTermStr(BandOwner,szBandOwner);
	AsignTermStr(UserData1,szUserData1);
	AsignTermStr(UserData2,szUserData2);

	int Order=-1;
	if(szOrder[0]!='-') Order=atoi(szOrder);
	if (pTerm->pAuxDevInfo==NULL)
	{
		pTerm->pAuxDevInfo=new VC3AUXDEVICE();
	}

	if(pTerm->dwNetVersion!=dwVersion){        pTerm->dwNetVersion=dwVersion; dwUpdateLow|=TermInfo_NetVersion;}     
	if(pTerm->dwAppVersion!=dwAppVersion)
	{     pTerm->dwAppVersion=dwAppVersion;dwUpdateLow|=TermInfo_AppVersion;}
	if(pTerm->nTermType!=nTermType){           pTerm->nTermType=nTermType;dwUpdateLow|=TermInfo_TermType;}          
	if(pTerm->nTermSubType!=nSubType){         pTerm->nTermSubType=nSubType;dwUpdateLow|=TermInfo_TermSubType;}        
	if(pTerm->nTermLevel!=nTermLevel){         pTerm->nTermLevel=nTermLevel;dwUpdateLow|=TermInfo_TermLevel;}        
	if(pTerm->dwGrant!=dwGrant){               pTerm->dwGrant=dwGrant;dwUpdateLow|=TermInfo_Grant;}              
	if(pTerm->lDispOrder!=lDispOrder){         pTerm->lDispOrder=lDispOrder;dwUpdateLow|=TermInfo_DispOrder;}        
	if(pTerm->Order!=Order){                   pTerm->Order=Order;dwUpdateLow|=TermInfo_Order;}                  
	if(pTerm->dwGlobalFlag!=dwGlobalFlag){     pTerm->dwGlobalFlag=dwGlobalFlag;dwUpdateLow|=TermInfo_GlobalFlag;}    
	if(pTerm->dwGlobalId!=dwGlobalId){           pTerm->dwGlobalId=dwGlobalId;dwUpdateLow|=TermInfo_GlobalId;}          
	if(memcmp(pTerm->pAuxDevInfo,&auxDev,sizeof(auxDev)))
	{           *pTerm->pAuxDevInfo=auxDev;dwUpdateLow|=TermInfo_AuxDevInfo;}          
	if(pTerm->geoX!=geoX){                     pTerm->geoX=geoX;dwUpdateLow|=TermInfo_GeoX;}                    
	if(pTerm->geoY!=geoY){                     pTerm->geoY=geoY;dwUpdateLow|=TermInfo_GeoY;}                    
	if(pTerm->IsChairman!=(bool)IsChairman){         pTerm->IsChairman=(bool)IsChairman;dwUpdateLow|=TermInfo_IsChairman;}        
	if(pTerm->IsOuter!=(bool)IsOuter){               pTerm->IsOuter=(bool)IsOuter;dwUpdateLow|=TermInfo_IsOuter;}              
	if(pTerm->IsStaticSender!=(bool)IsStaticSender){ pTerm->IsStaticSender=(bool)IsStaticSender;dwUpdateLow|=TermInfo_IsStaticSender;}
	if(pTerm->bOnline=1){                     pTerm->bOnline=1;}
	dwUpdateHigh|=TermInfo_UpdateFlag;
	return dwUpdateLow;
}


TIMEOUTLEN* GetTimeoutConfig( int talk,int con,int hand,int leave,int report,int join )
{
	static BaseArray<TIMEOUTLEN> _tmConfig;
	TIMEOUTLEN tl(talk,con,hand,leave,report,join);
	int i=0;
	for (i=0;i<_tmConfig.GetSize();i++)
	{
		if (memcmp(&tl,&_tmConfig[i],sizeof(TIMEOUTLEN))==0)
		{
			return &_tmConfig[i];
		}

	}
	i=_tmConfig.Add(tl);
	return &_tmConfig[i];

}


};//end namespace SWApi