// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// Renderer.h - D3D12 renderer that's going to get refactored away soon enough
//==============================================================================================================
#pragma once
#include "Platform.h"
#include "SDL_syswm.h"
#include "DependencyNode.h"

const uint32 WinWidth = 640;
const uint32 WinHeight = 480;
const float AspectRatio = (float)WinWidth / (float)WinHeight;
class D3D12Adapter;
class D3D12Device;

// 2 for double buffering, 3 for triple buffering
static const uint32 FrameBufferCount = 2;
class Renderer : public DependencyNode
{
public:
	Renderer(DependencyNode& Root)
		: DependencyNode(Root, "Renderer")
	{}
	~Renderer()	{}
	void Create(SDL_Window* SDLWin);
	void Render();
	void Destroy();

	D3D12Adapter* Adapter;
	static uint32 FrameIndex;
};