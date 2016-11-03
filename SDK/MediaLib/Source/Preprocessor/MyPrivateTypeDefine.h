#ifndef _MYPRIVATE_TYPE_DEFINE_H_
#define _MYPRIVATE_TYPE_DEFINE_H_

#define PREFIX_SINOWAVE	0x53

template <class T>
inline void SafeDeleteArray(T *&p)
{
	if (NULL != p) 
	{
		delete [] p;
		p = NULL;
	}
}

template <class T>
inline void SafeDelete(T *&p)
{
	if (NULL != p) 
	{
		delete p;
		p = NULL;
	}
}

template <class T>
inline void SafeRelease(T *&p)
{
	if (NULL != p) 
	{
		p->Release();
		p = NULL;
	}
}

#endif//_MYPRIVATE_TYPE_DEFINE_H_