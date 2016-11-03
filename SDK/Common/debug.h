#ifndef __DEBUG__H__
#define __DEBUG__H__

#define _DECL_ENUMSTR(ename) LPCTSTR ename(int i)
#define _BEGIN_IMPL_ENUMSTR(ename) \
	LPCTSTR ename(int i){ \
	static char szUnknown[32]; \
	const char* SZUNKNOWN=#ename "_Unknown[%d]"; \
	const char* szNames[]={ \

#define _END_IMPL_ENUMSTR \
};if(i<0 || i>=(sizeof(szNames)/sizeof(char*))) {sprintf(szUnknown,SZUNKNOWN,i); return szUnknown;} \
	return szNames[i];};\


#define _BEGIN_IMPL_ENUMSTRFROM(ename,first) \
LPCTSTR ename(int i){ \
static char szUnknown[32]; \
static int iFirst=first; \
const char* SZUNKNOWN=#ename "_Unknown[%d]"; \
const char* szNames[]={ \

#define _END_IMPL_ENUMSTRFROM \
};i-=iFirst;if(i<0 || i>=(sizeof(szNames)/sizeof(char*))) {sprintf(szUnknown,SZUNKNOWN,i); return szUnknown;} \
return szNames[i];};\



#ifdef TRACE
#undef TRACE
#endif
void  _DebugTrace(const char* lpszFormat, ...);
#define TRACE _DebugTrace
#define TRACEINFO	TRACE
#define TRACEERR	_DebugTrace

#define TRACETIMER  //TRACE

#define TRACESOCKET //TRACE

#define TRACETHREAD //TRACE

#define TRACEMEDIA //TRACE

#define TRACEMSG TRACE
#define TRACEDEV TRACE

#endif
