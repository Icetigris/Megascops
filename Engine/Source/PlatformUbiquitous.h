// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// PlatformUbiquitous.h - #defines for plain old data types that are (nearly) ubiquitous across platforms.
//==============================================================================================================
#pragma once

#include <stdint.h>

struct UbiquitousPlatformTypes
{
	// Integer types

	// Unsigned
	typedef uint8_t  uint8;  //1 byte
	typedef uint16_t uint16; //2 bytes
	typedef uint32_t uint32; //4 bytes
	typedef uint64_t uint64; //8 bytes

	// Signed
	typedef int8_t  int8;   //1 byte
	typedef int16_t int16;  //2 bytes
	typedef int32_t int32;  //4 bytes
	typedef int64_t int64;  //8 bytes

	// Other stuff

	// Null types
	typedef int32 TYPE_OF_NULL;
	typedef decltype(nullptr) TYPE_OF_NULLPTR;
};