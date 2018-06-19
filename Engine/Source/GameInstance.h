// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// GameInstance.h - Interface thing for updating game-specific data
//==========================================================
#pragma once
#include "Platform.h"
#include "Time.h"
#include "Renderer.h" //turgle - once the database is hooked up, shouldn't need to pass things to the renderer directly
#include "Triangle.h" //turgle - this will come from a database later

class GameInstance
{
public:
	static uint64 FrameNumber;
	static double DeltaTime;

	static void Tick();
	static GameInstance& GetInstance()
	{
		return GameInstance();
	}

private:
	static double CurrentTime;
	static double PreviousTime;
	GameInstance();
	//~GameInstance();

	//turgle - to be moved/changed
	static Renderer D3D12Renderer;
};