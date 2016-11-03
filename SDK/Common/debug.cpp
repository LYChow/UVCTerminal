#include "__PCH.h"
#include "debug.h"

#ifdef __ENV_IOS
//#define OutputDebugString(str) printf("%s",str)
extern void OutputDebugString(const char* str);
#endif

void  _DebugTrace(const char* lpszFormat, ...)
{

	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	char szBuffer[4096];

	nBuf = vsnprintf(szBuffer, 4096, lpszFormat, args);
	szBuffer[4095]=0;

	// was there an error? was the expanded string too long?
	ASSERT(nBuf >= 0);
	szBuffer[nBuf]=0;
	OutputDebugString(szBuffer);
	va_end(args);
}

