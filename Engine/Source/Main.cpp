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
	//extract argc and argv from this winmain nonsense
	int32 argc;
	WCHAR** argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	//convert argv to UTF-8
	char** utf8Argv = new char *[argc];

	for (int i = 0; i < argc; ++i) 
	{
		int bufSize = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, NULL, 0, NULL, NULL);

		utf8Argv[i] = new char[bufSize];

		WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, utf8Argv[i], bufSize, NULL, NULL);
	}

	//process command line args
	uint32 WindowWidth = 640;
	uint32 WindowHeight = 480;
	bool bEnableD3DDebugLayer = false;
	bool bUseWARPAdapter = false;
	auto cli =
		  clara::Opt(bEnableD3DDebugLayer)
			["--d3ddebug"]
			("Enable d3ddebug layer")
		| clara::Opt(bUseWARPAdapter)
			["--WARP"]
			("Use WARP adapter")
		| clara::Opt(WindowWidth, "WindowWidth")
			["--WinWidth"]
			("Window width")
		| clara::Opt(WindowHeight, "WindowHeight")
			["--WinHeight"]
			("Window height");

	auto result = cli.parse(clara::Args(argc, utf8Argv));
	if (!result)
	{
		MEGALOGLN("[Clara failure] " << result.errorMessage().c_str());
	}
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