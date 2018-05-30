// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// GameInstance.h - Interface thing for updating game-specific data
//==========================================================
#pragma once
#include "Platform.h"
#include "Time.h"

class GameInstance
{
public:
	static uint64 FrameNumber;
	static double DeltaTime;

	static void Update();
	static GameInstance& GetInstance()
	{
		return GameInstance();
	}

private:
	static double CurrentTime;
	static double PreviousTime;
	GameInstance()
	{
		Time& t = Time::GetInstance();
	}
};