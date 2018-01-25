// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// Log.h - EntryDebug logging.
//==========================================================

#ifdef _MSC_VER
#include <debugapi.h>
#include <iostream>
#include <sstream>

#define MEGALOG( s ) \
{\
	std::wostringstream os_; \
	os_ << s; \
	OutputDebugStringW( os_.str().c_str() ); \
}
#else
#define MEGALOG( s ) \
{ \
	std::cout << s;\
}
#endif