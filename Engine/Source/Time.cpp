// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// Time.cpp - Time includes and functions.
//==============================================================================================================
#include "Platform.h"
#include "Time.h"
#include <assert.h>

// Platform-specific functionality restricted to file-global functions in here so nobody else tries to use them.

// Windows time functions:
#ifdef WIN32
#include <Windows.h>
// The frequency of the performance counter is fixed at system boot and is consistent across all processors.
// Therefore, the frequency need only be queried upon application initialization, and the result can be cached.
double GetSecondsPerCycle()
{
	LARGE_INTEGER Frequency;
	assert(QueryPerformanceFrequency(&Frequency));

	return 1.0 / Frequency.QuadPart;
}

double GetTimestamp(double SecondsPerCycle)
{
	LARGE_INTEGER Cycles;
	QueryPerformanceCounter(&Cycles);

	return Cycles.QuadPart * SecondsPerCycle;
}
#endif

// Other platforms:



// Time singleton:
double Time::SecondsPerCycle = 0;
Time::Time()
{
	SecondsPerCycle = GetSecondsPerCycle();
}

double Time::GetTimestampInSeconds()
{
	return GetTimestamp(SecondsPerCycle);
}
