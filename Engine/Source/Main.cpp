// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// Main.cpp - Entry point of entire engine.
//==========================================================
#include <Windows.h>
#include "GameInstance.h"
#include "CommandLine.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	HandleCommandLineInput();
	GameInstance& Game = GameInstance::GetInstance();

	// Game loop
	GameInstance::Tick();
}