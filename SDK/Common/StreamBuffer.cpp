#include "__PCH.h"
#include "StreamBuffer.h"
char* ScanString(char* string,int nIndex,char* outArr[],int* nParsed,LPCTSTR szSeprator)
{
#define ISSEP(ch) (strchr(szSeprator,(ch))!=NULL)
	int i=0;
	char* p=string;

	//
	while (*p && ISSEP(*p))//(*p==' ' || *p=='\t'))
	{
		p++;
	}
	if (*p==0)
	{
		return NULL;
	}

	char* pCur=p;
	while (*p)
	{
		//		while (*p && *p!=' ' && *p!='\t') p++;
		while (*p && !ISSEP(*p)) p++;
		if(*p)
		{
			*p=0;
			p++;
			//		while(*p && (*p==' ' || *p=='\t')) p++;
			while(*p && ISSEP(*p)) p++;
		}
		if (outArr)
		{
			outArr[i]=pCur;
		}
		if(i==nIndex)
		{
			if(nParsed)
			{
				*nParsed=i+1;
			}
			return pCur;
		}
		i++;
		pCur=p;
	}
	if (nParsed)
	{
		*nParsed=i;
	}
	return NULL;
}

int ScanString(char* string,char* outArr[],int nCount,LPCTSTR szSeprator)
{
	int n=0;
	ScanString(string,nCount,outArr,&n,szSeprator);
	return n;
}

int SplitStr(char* str,char* arr[],int nCount,char ch)
{

	char* p=str;
	p=str;
	char* pNext=p;
	int i=0;
	arr[0]=0;
	nCount--;
	while(i<nCount && *p)
	{
		arr[i]=p;
		pNext=strchr(p,ch);
		i++;
		if (pNext==NULL)
		{
			break;
		}
		*pNext=0;
		pNext++;
		p=pNext;
	}
	arr[i]=0;
	return i;
}

