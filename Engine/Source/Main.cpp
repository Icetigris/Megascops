// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// Main.cpp - Entry point of entire engine.
//==========================================================
#include <Windows.h>
#include <SDL.h>
#include "GameInstance.h"
#include "Renderer.h"
#include "Log.h"
#include "Platform.h"
#include "CommandLine.h"

bool HandleEvents()
{
	SDL_Event SDLEvent;
	while(SDL_PollEvent(&SDLEvent))
	{
		// Handle events
		switch (SDLEvent.type)
		{
		case SDL_QUIT:
			return false;
			break;
		case SDL_KEYUP:
			// single key press
			//log some shit to the console
			MEGALOGLN("binch");
			break;
		case SDL_KEYDOWN:
			//hold key
			break;
		case SDL_MOUSEBUTTONUP:
			//single click
			break;
		case SDL_MOUSEBUTTONDOWN:
			//hold mouse button
			break;
		default:
			break;
		}
	}

	return true;
}

//this will be moved
DependencyNode Root;
Renderer D3D12Renderer(Root);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	HandleCommandLineInput();
	GameInstance& Game = GameInstance::GetInstance();
	// Init renderer (default d3d12)
	D3D12Renderer.Create("Megascops Engine");

	// Game loop
	while (HandleEvents())
	{
		// Update
		GameInstance::Update();

		// Render
		D3D12Renderer.Render();
	}

	D3D12Renderer.Destroy();
	SDL_Quit();
}