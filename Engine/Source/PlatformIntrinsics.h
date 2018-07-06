// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// PlatformIntrinsics.h - Wrapper for compiler intrinsic functions. 
//==============================================================================================================
#pragma once

#include "Platform.h"

// Windows/Visual C++ compiler
#ifdef MSVC
#include <intrin.h>
#endif

// Intrinsics supported on """all architectures""" (i.e. x86, AMD64, and ARM)

// Find first 1 in a bit field (scan from LSB (index 0) to MSB (index 31) and return index of the first 1 we found)
#ifdef MSVC
#pragma intrinsic(_BitScanForward)
// also gives you the first set bit
uint32 CountTrailingZeros(uint32 Value)
{
	if (Value == 0)
	{
		return 32;
	}
	unsigned long BitIndex;
	_BitScanForward(&BitIndex, Value);
	return BitIndex;
	}

#endif
// clang: __builtin_ctz

// Find last 1 in a bit field (scan from MSB (index 31) to LSB (index 0) and return index of the first 1 we found)
#ifdef MSVC
#pragma intrinsic(_BitScanReverse)
//also gives you the last set bit
uint32 CountLeadingZeros(uint32 Value)
{
	if (Value == 0)
	{
		return 32;
	}
	unsigned long BitIndex;
	_BitScanReverse(&BitIndex, Value);
	return BitIndex;
}
#endif
//clang: __builtin_clz

// x64 intrinsics
#ifdef MSVC
#pragma intrinsic(__popcnt)
// "Population count" i.e. the number of 1s in a bit field
uint32 PopCnt(uint32 Value)
{
	return __popcnt(Value);
}
#endif
//clang: __builtin_popcount