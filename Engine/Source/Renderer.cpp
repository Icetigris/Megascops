// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// Renderer.cpp - D3D12 renderer that's going to get refactored away soon enough
//==========================================================
#include <SDL.h>
#include "Renderer.h"
#include "Log.h"
#include "D3D12Adapter.h"
#include "D3D12Device.h"

HWND WindowHandle; //used in swapchain

uint32 Renderer::FrameIndex = 0;
//Window-related globals
extern uint32 WinWidth = 640;
extern uint32 WinHeight = 480;
extern bool bEnableD3DDebug = false;
extern bool bCreateWARPAdapter = false;
extern float AspectRatio = 1.0f;
void Renderer::Create(const char* Title, int32 PositionX, int32 PositionY, int32 Width, int32 Height, bool bEnableD3DDebug, bool bCreateWARPAdapter)
{
	// Init SDL window
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		MEGALOGLN("SDL_Init Error: " << SDL_GetError());
		SDL_Quit();
	}
	SDLWin = SDL_CreateWindow(Title, PositionX, PositionY, Width, Height, /*Flags=*/SDL_WINDOW_SHOWN);

	if (SDLWin == nullptr)
	{
		MEGALOGLN("SDL_CreateWindow Error: " << SDL_GetError());
		SDL_Quit();
	}

	// Get HWND
	SDL_SysWMinfo windowinfo;
	SDL_VERSION(&windowinfo.version);
	SDL_GetWindowWMInfo(SDLWin, &windowinfo);
	WindowHandle = windowinfo.info.win.window;


	WinWidth = Width;
	WinHeight = Height;
	AspectRatio = (float)WinWidth / (float)WinHeight;

	// might need like a factory or something for multigpu
	Adapter = new D3D12Adapter(*this);
	Adapter->Initialize(WindowHandle);
}

void Renderer::Render()
{
	Adapter->Present();
}

void Renderer::Destroy()
{
	delete Adapter;
	SDL_DestroyWindow(SDLWin);
}