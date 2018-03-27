// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// Main.cpp - Entry point of entire engine.
//==========================================================
#include <Windows.h>
#include <SDL.h>
#include "Renderer.h"
#include "Log.h"
#include "Platform.h"
#include "ThirdParty/Clara/clara.hpp"

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
	//process command line args
	uint32 WindowWidth = 640;
	uint32 WindowHeight = 480;
	bool bEnableD3DDebugLayer = false;
	bool bUseWARPAdapter = false;
	auto cli
		= clara::Opt(WindowWidth, "WindowWidth")
			["-WindowWidth"]["-WindowWidth="]
			("Application window width")
		| clara::Opt(WindowHeight, "WindowHeight")
			["-WindowHeight"]["-WindowHeight="]
			("Application window height")
		| clara::Opt(bEnableD3DDebugLayer, "d3ddebug")
			["-d3ddebug"]
			("Enable d3ddebug layer")
		| clara::Opt(bUseWARPAdapter, "WARP")
			["-WARP"]
			("Use WARP adapter");
	char* argv = new char[nCmdShow];
	wcstombs(argv, pCmdLine, nCmdShow);
	const std::string argvstring(argv);
	//auto result = cli.parse(clara::Args(nCmdShow, argvstring.c_str()));
	delete argv;
	// Init renderer (default d3d12)
	D3D12Renderer.Create("Megascops Engine", 100, 100, WindowWidth, WindowHeight, bEnableD3DDebugLayer, bUseWARPAdapter);

	// Game loop
	while (HandleEvents())
	{
		// Update

		// Render
		D3D12Renderer.Render();
	}

	D3D12Renderer.Destroy();
	SDL_Quit();
}