// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// CommandLine.cpp - Functions for dealing with command line input.
//==========================================================
#include <Windows.h>
#include "Renderer.h"
#include "Log.h"
#include "CommandLine.h"
#include "ThirdParty/Clara/clara.hpp"

extern void HandleCommandLineInput()
{
	//extract argc and argv from this winmain nonsense
	int32 argc;
	WCHAR** argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	//convert argv to UTF-8
	char** utf8Argv = new char *[argc];

	for (int32 i = 0; i < argc; ++i) 
	{
		int32 bufSize = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, NULL, 0, NULL, NULL);

		utf8Argv[i] = new char[bufSize];

		WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, utf8Argv[i], bufSize, NULL, NULL);
	}

	//process command line args
	auto cli =
		  clara::Opt(bEnableD3DDebug)
			["--d3ddebug"]
			("Enable d3ddebug layer")
		| clara::Opt(bCreateWARPAdapter)
			["--WARP"]
			("Use WARP adapter")
		| clara::Opt(WinPosX, "WindowPositionX")
			["--WinPosX"]
			("Window X position")
		| clara::Opt(WinPosY, "WindowPositionY")
			["--WinPosY"]
			("Window Y position")
		| clara::Opt(WinWidth, "WindowWidth")
			["--WinWidth"]
			("Window width")
		| clara::Opt(WinHeight, "WindowHeight")
			["--WinHeight"]
			("Window height");

	auto result = cli.parse(clara::Args(argc, utf8Argv));
	if (!result)
	{
		MEGALOGLN("[Clara failure] " << result.errorMessage().c_str());
	}
}