// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0601	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifdef LIBRARY_EXPORTS
#    define LIBRARY_API __declspec(dllexport)
#else
#    define LIBRARY_API __declspec(dllimport)
//#    define LIBRARY_API 
#endif

#include <cassert>

#define MAX_BUFFER_TO_SEND	( 512 )	// UUID should be 128 bytes long

//#define ALLOW_CONSOLE_PRINT
#ifdef _DEBUG
	#define ALLOW_CONSOLE_PRINT
#endif
#ifdef ALLOW_CONSOLE_PRINT
	#include <iostream>
	#define DEBUG_SSL_PRINT(codetoprint)	{ codetoprint }
	#define DEBUG_LIC_PRINT(codetoprint)	{ codetoprint }
	#define DEBUG_FP_PRINT(codetoprint)		{ codetoprint }
	#define DEBUG_CONF_PRINT(codetoprint)	{ codetoprint }
	#define DEBUG_GRACE_PRINT(codetoprint)	{ codetoprint }
	#define DEBUG_LOG_PRINT(codetoprint)	{ codetoprint }
#else
	#define DEBUG_SSL_PRINT(x)		{;}
	#define DEBUG_LIC_PRINT(x)		{;}
	#define DEBUG_FP_PRINT(x)		{;}
	#define DEBUG_CONF_PRINT(x)		{;}
	#define DEBUG_GRACE_PRINT(x)	{;}
	#define DEBUG_LOG_PRINT(x)		{;}
#endif

//#include <iostream>
//#define DEBUG_GRACE_PRINT(codetoprint)	{ codetoprint }


#include <windows.h>
#include "SimpleList.h"
#include "Logging.h"
