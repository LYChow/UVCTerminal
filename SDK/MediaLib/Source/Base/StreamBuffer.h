#ifndef __STREAMBUFFER__H__
#define __STREAMBUFFER__H__
#include "IBase.h"
_BEGIN_NAMESPACE_PROJ
struct StreamBuffer
{
public:
	StreamBuffer(int size)
	{
		_pBuffer=new char[size];
		_size=size;
		_pTail=_pBuffer;
		_pHead=_pTail;
		_bRef=0;
	}
	StreamBuffer(const StreamBuffer& sb)
	{
		_pBuffer=sb._pBuffer;
		_size=sb._size;
		_pTail=sb._pTail;
		_pHead=sb._pHead;
		_bRef=1;
	}
	StreamBuffer(const char* pBuf,int size=0)
	{
		_pBuffer=(char*)pBuf;
		_size=size;
		_pHead=_pBuffer;
		_pTail=_pBuffer+_size;
		_bRef=1;
	}

	~StreamBuffer()
	{
		if(!_bRef)
		{
			delete _pBuffer;
		}
		_pBuffer=0;
		_size=0;
		_pHead=0;
		_pTail=0;
	}
	void Attach(const char* pBuf,int size)
	{
		if(_bRef==0 && _pBuffer) return;
		_pBuffer=(char*)pBuf;
		_size=size;
		_pHead=_pBuffer;
		_pTail=_pBuffer+_size;
		_bRef=1;
	}
	void Reset()
	{
		ZeroMemory(_pBuffer, _size);
		_pTail=_pBuffer;
		_pHead=_pTail;
	}
	int GetHeadSize(){return int(_pTail-_pHead);}
	template<typename T>
	T& GetHead()
	{
		return *((T*)_pHead);
	}
	char* GetHeadPtr(){return _pHead;}
	LPCTSTR GetTailPtr(){return _pTail;}
	template<typename T>
	T& GetPushBuffer()
	{
		return *((T*)GetPushBufferPtr(sizeof(T)));
	}
	char* GetPushBufferPtr(int size)
	{
		PackBuffer();
		if (int(_pTail-_pBuffer)+size>_size)
		{
			return 0;
		}
		char* p=_pTail;
		ZeroMemory(p,size);
		_pTail+=size;
		return p;

	}
	void RemoveHead(int size)
	{
		int sizeData=int(_pTail-_pHead);
		if (size>sizeData)
		{
			size=sizeData;
		}

		ASSERT(int(_pTail-_pHead)>size);

		_pHead+=size;
	}

	int PushData(const char* pData,int size)
	{
		PackBuffer();
		int sizeAvl=_size-int(_pTail-_pBuffer);
		if(size>sizeAvl) size=sizeAvl;
		memcpy(_pTail,pData,size);
		_pTail+=size;
		return size;
	}
	int PushSource(IDataSource* pSource)
	{
		PackBuffer();
		int sizeAvl=_size-int(_pTail-_pBuffer);
		int size=pSource->ReadData(_pTail,sizeAvl);
		//		TRACEINFO("INFO-PushSource:real recv data,size=%d\n",size);
		_pTail+=size;
		return size;
	}

	template<typename T>
	T* PopVal()
	{
		//RemoveHead(sizeof(T));
		T* p=(T*)_pHead;
		_pHead+=sizeof(T);
		return p;
	}
	template<typename T>
	void PopVal(T& val)
	{
		val=*((T*)_pHead);
		_pHead+=sizeof(T);
	}

	char* PopString()
	{
		char* p=_pHead;
		_pHead+=strlen(p)+1;
		return p;
	}
	int PopString(char* p)
	{
		int size=strlen(_pHead)+1;
		memcpy(p,_pHead,size);
		_pHead+=size;
		return size;
	}

	template<typename T>
	int PushVal(T val)
	{
		T t=val;
		return PushData((char*)&t,sizeof(T));
	}
	int PushString(LPCTSTR str)
	{
		return PushData(str,strlen(str)+1);
	}


	//private:
	void PackBuffer()
	{
		if(_pHead!=_pBuffer)
		{
			int size=int(_pTail-_pHead);
			if(size)
			{
				memmove(_pBuffer,_pHead,size);
			}
			_pHead=_pBuffer;
			_pTail=_pHead+size;
			*(_pBuffer+size)=0;
		}
	}
protected:
	char* _pBuffer;
	int _size;
	char* _pTail;
	char* _pHead;

	int _bRef;

};

_END_NAMESPACE

#endif
