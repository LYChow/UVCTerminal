#ifndef __CONFIG__H__
#define __CONFIG__H__

//------------------pub header
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <map>

#pragma warning(disable:4995 4996 4200 4800)

using namespace std;

//---------------base os type define
#if defined(WIN32)
#define __ENV_WIN
#include "__PCHWIN.h"
#else
#define __ENV_IOS
#include "__PCHIOS.h"
#endif //end target

//-------------public type define
#include "BasePub.h"
#include "debug.h"


#include "OSApi.h"
#include "StreamBuffer.h"
#include "Thread.h"
#include "Timer.h"


#endif
