// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// Main.cpp - Entry point of entire engine.
//==========================================================
#include <Windows.h>
#include <SDL.h>
#include "Renderer.h"
#include "Log.h"
#include "Platform.h"

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

void InitSDL(Uint32 InitFlags)
{

}

SDL_Window* SDLWin;
void InitWindow(const char* Title, int PositionX, int PositionY, int Width, int Height, int Flags)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		MEGALOGLN("SDL_Init Error: " << SDL_GetError());
		SDL_Quit();
	}
	SDLWin = SDL_CreateWindow(Title, PositionX, PositionY, Width, Height, Flags);

	if (SDLWin == nullptr)
	{
		MEGALOGLN("SDL_CreateWindow Error: " << SDL_GetError());
		SDL_Quit();
	}
}

//this will be moved
DependencyNode Root;
Renderer D3D12Renderer(Root);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Init SDL window
	InitWindow("Megascops Engine", 100, 100, 640, 480, SDL_WINDOW_SHOWN);

	// Init renderer (default d3d12)
	D3D12Renderer.Create(SDLWin);//, RenderAPI)

	// Game loop
	while (HandleEvents())
	{
		// Update

		// Render
		D3D12Renderer.Render();
	}

	D3D12Renderer.Destroy();
	SDL_DestroyWindow(SDLWin);
	SDL_Quit();
}