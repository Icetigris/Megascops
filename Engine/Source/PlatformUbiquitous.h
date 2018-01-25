// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// PlatformUbiquitous.h - #defines for plain old data types that are (nearly) ubiquitous across platforms.
//==============================================================================================================
#pragma once

struct UbiquitousPlatformTypes
{
	// Integer types

	// Unsigned
	typedef unsigned char      uint8;  //1 byte
	typedef unsigned short int uint16; //2 bytes
	typedef unsigned int       uint32; //4 bytes
	typedef unsigned long long uint64; //8 bytes

	// Signed
	typedef signed char        int8;   //1 byte
	typedef signed short int   int16;  //2 bytes
	typedef signed int         int32;  //4 bytes
	typedef signed long long   int64;  //8 bytes

	// Other stuff

	// Null types
	typedef int32 TYPE_OF_NULL;
	typedef decltype(nullptr) TYPE_OF_NULLPTR;
};