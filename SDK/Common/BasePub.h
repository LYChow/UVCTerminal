#ifndef __PCHPUBDEF__H__
#define __PCHPUBDEF__H__

typedef string String;


/*
#ifndef STRNUM_MAX_SIZE
#define STRNUM_MAX_SIZE 64
#endif
*/



template<int STRNUM_MAX_SIZE=64>
struct ConvStrBase
{
	char buf[STRNUM_MAX_SIZE];
	ConvStrBase()
	{
		buf[0]=0;
	}
	ConvStrBase(LPCTSTR fmt,DWORD v)
	{
		buf[0]=0;
		sprintf(buf,fmt,v);
	}
	ConvStrBase(DWORD v){sprintf(buf,"%u",v);}
	ConvStrBase(int v){sprintf(buf,"%d",v);}
	ConvStrBase(void* v){sprintf(buf,"%p",v);}
	operator LPCTSTR(){return buf;}
};

typedef ConvStrBase<> ConvStr;

template<int _SIZE=32>
struct StrBuf
{
	char buf[_SIZE];
	StrBuf(const char* p)
	{
		int len=strlen(p)+1;
		if(len<=_SIZE)
		{
			memcpy(buf,p,len);
		}
	}
	operator LPCTSTR(){return buf;}
};
typedef StrBuf<32> StrBuf32;
typedef StrBuf<64> StrBuf64;
typedef StrBuf<512> StrBuf512;


#endif
