// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// Renderer.cpp - D3D12 renderer that's going to get refactored away soon enough
//==========================================================
#include "Renderer.h"
#include "Log.h"
#include "D3D12Adapter.h"
#include "D3D12Device.h"

HWND WindowHandle; //used in swapchain

uint32 Renderer::FrameIndex = 0;
void Renderer::Create(SDL_Window* SDLWin)
{
	SDL_SysWMinfo windowinfo;
	SDL_VERSION(&windowinfo.version);
	SDL_GetWindowWMInfo(SDLWin, &windowinfo);
	WindowHandle = windowinfo.info.win.window;

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
}