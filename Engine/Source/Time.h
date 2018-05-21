// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// Time.h - Time includes and functions.
//==============================================================================================================
#pragma once
class Time
{
public:
	// Platform abstraction:
	//Gets the high-resolution timestamp for Right Now
	static double GetTimestampInSeconds();
	static Time& GetInstance()
	{
		return Time();
	}
private:
	Time();
	static double SecondsPerCycle;
};