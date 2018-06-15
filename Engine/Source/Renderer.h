// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// Renderer.h - D3D12 renderer that's going to get refactored away soon enough
//==============================================================================================================
#pragma once
#include "Platform.h"
#include "SDL_syswm.h"

//Window-related globals
extern uint32 WinPosX;
extern uint32 WinPosY;
extern uint32 WinWidth;
extern uint32 WinHeight;
extern bool GEnableD3DDebug;
extern bool GCreateWARPAdapter;
extern bool GIsFullscreen;
extern float AspectRatio;

class D3D12Adapter;
class D3D12Device;

// 2 for double buffering, 3 for triple buffering
static const uint32 FrameBufferCount = 2;
class Renderer
{
public:
	Renderer()
	{}
	~Renderer()	{}
	void Create(const char* Title);
	void Render();
	void Destroy();

	SDL_Window* SDLWin;
	D3D12Adapter* Adapter;
	static uint32 FrameIndex;
};