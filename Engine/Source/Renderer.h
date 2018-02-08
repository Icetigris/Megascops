// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// Renderer.h - D3D12 renderer that's going to get refactored away soon enough
//==============================================================================================================
#pragma once
#include "PlatformUbiquitous.h"
#include "SDL_syswm.h"

const uint32 WinWidth = 640;
const uint32 WinHeight = 480;

void CreateRenderer(SDL_Window* SDLWin);
void Render();
void DestroyRenderer();