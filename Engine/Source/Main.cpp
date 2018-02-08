// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// Main.cpp - Entry point of entire engine.
//==========================================================
#include <Windows.h>
#include <SDL.h>
#include "Renderer.h"
#include "Log.h"
#include "PlatformUbiquitous.h"

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
	if (SDL_Init(InitFlags) != 0)
	{
		MEGALOG("SDL_Init Error: " << SDL_GetError() << std::endl);
		SDL_Quit();
	}
}

SDL_Window* SDLWin;
void InitWindow(const char* Title, int PositionX, int PositionY, int Width, int Height, int Flags)
{
	SDLWin = SDL_CreateWindow(Title, PositionX, PositionY, Width, Height, Flags);

	if (SDLWin == nullptr)
	{
		MEGALOG("SDL_CreateWindow Error: " << SDL_GetError() << std::endl);
		SDL_Quit();
	}
}

void CleanupAndQuit()
{
	SDL_DestroyWindow(SDLWin);
	SDL_Quit();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Init SDL subsystems we're going to use.
	InitSDL(SDL_INIT_VIDEO);

	// Init SDL window
	InitWindow("Megascops Engine", 100, 100, 640, 480, SDL_WINDOW_SHOWN);

	// Init renderer (default d3d12)
	CreateRenderer(SDLWin);//, RenderAPI)

	// Game loop
	while (HandleEvents())
	{
		// Update

		// Render
		Render();
	}

	CleanupAndQuit();
}