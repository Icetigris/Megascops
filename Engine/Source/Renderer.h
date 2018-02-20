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
class D3D12Adapter;
class D3D12Device;

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
};