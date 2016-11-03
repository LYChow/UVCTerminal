#include "__PCH.h"

#include "MediaLib.h"

namespace MediaLib{

struct IMP* OpenProcessor(IMP* pParent,int iIn,int iOut)
{
	int sIn,sOut;//StreamType:S,C,M
	int mIn,mOut;//Media:A,V
	IMP* pMPIn=NULL;
	IMP* pMPOut=NULL;
#if 0
	pMPIn=FindSpliterIfNotCreate(pParent,iIn);

	switch(sIn)
	{
	case 0://S
		pMPIn->AddOutput(iOut);
		switch(sOut)
		{
		case 0://S
			pMPOut=GetMixerIfNotCreate(pParent,iOut);
			pMPOut->AddInput(iIn);
			break;
		case 1://C
			pMPOut=GetComposerIfNotCreate(pParent,iOut);
			pMPOut->AddInput(iIn);
			break;
		case 2://M
			pMPOut=GetMuxerIfNotCreate(pParent,iOut);
			pMPOut->AddInput(iIn);
			break;
		}
		break;
	case 1://C

		switch(sOut)
		{
		case 0://S
			{
				IMP* pDeCompMP=pMPIn->AddDeComposer(iIn,iOut);

			}
			break;
		case 1://C
			break;
		case 2://M
				
			break;
		}
		break;
	case 2://M
		switch(sOut)
		{
		case 0://S
			break;
		case 1://C
			break;
		case 2://M
			break;
		}

		break;
	}
#endif
	return 0;

}
int MediaProcessor::Create()
{
	int i,j;
	int nIn,nOut;
	int sIn,sOut;//StreamType:S,C,M
	int mIn,mOut;//Media:A,V

	for (i=0;i<nIn;i++)
	{
		for (j=0;j<nOut;j++)
		{

			IMP* pMP=OpenProcessor(this,i,j);
/*
			switch(sIn)
			{
			case 0://S
				break;
			case 1://C
				break;
			case 2://M
				break;
			}
*/
		}
		
	}
	
	return 0;
}


int MediaProcessor::CreateStreamePath(int iIn,int iOut)
{

	return 0;
};


//////////////////////////////////////////////////////////////////////////

	MediaLib::MediaLib()
	{
		_pObserver=0;
	}

	IMediaProcessor* MediaLib::CreateMediaProcessor( int nInCount,int nOutCount,int fmtIn/*=0*/,int fmtOut/*=0*/ )
	{
		return new MediaProcessor(nInCount,nOutCount,fmtIn,fmtOut);
	}

	IMediaDevice* MediaLib::GetDevice( MediaDeviceType DevType,int iDev )
	{
		void* pDev=0;
		switch (DevType)
		{
		case MediaDeviceVideoCap:
			pDev=_vcaps[iDev];
			break;
		case MediaDeviceVideoWindow:
			pDev=_vwnds[iDev];
			break;
		}
		return (IMediaDevice*)pDev;
	}

	IMediaLib* CreateMediaLib(void* pParam)
	{
		static MediaLib* p=0;
		if (p==0)
		{
			p=new MediaLib();
		}
		return p;
	}
//////////////////////////////////////////////////////////////////////////



};//end namespace

