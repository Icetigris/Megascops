// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// Renderer.cpp - D3D12 renderer
//==========================================================
#include <SDL.h>
#include "Renderer.h"
#include "Log.h"
#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include "Triangle.h" //turgle

HWND WindowHandle; //used in swapchain

uint32 Renderer::FrameIndex = 0;

//Window-related globals
extern uint32 WinPosX = 100;
extern uint32 WinPosY = 100;
extern uint32 WinWidth = 640;
extern uint32 WinHeight = 480;
extern bool GEnableD3DDebug = false;
extern bool GCreateWARPAdapter = false;
extern bool GIsFullscreen = false;
extern float AspectRatio = 1.0f;

void Renderer::Create(const char* Title)
{
	// Init SDL window
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		MEGALOGLN("SDL_Init Error: " << SDL_GetError());
		SDL_Quit();
	}
	SDLWin = SDL_CreateWindow(Title, WinPosX, WinPosY, WinWidth, WinHeight, /*Flags=*/SDL_WINDOW_SHOWN);

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

	AspectRatio = (float)WinWidth / (float)WinHeight;

	// might need like a factory or something for multigpu
	Adapter = new D3D12Adapter(*this);
	Adapter->Initialize(WindowHandle);
}

void Renderer::Render(Triangle triangle)
{
	Adapter->ChildDevice->Draw(triangle.ConstantBuffer, triangle.VertexBuffer); //MULTIGPUTODO: for each device //turgle - pass render components in a less hacky way later
	Adapter->Present();
}

void Renderer::Destroy()
{
	delete Adapter;
	SDL_DestroyWindow(SDLWin);
}