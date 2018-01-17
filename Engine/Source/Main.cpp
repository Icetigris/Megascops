#include <Windows.h>
#include <SDL.h>
#include "Log.h"

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
			MEGALOG("binch");
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

SDL_Renderer *SDLRenderer;
void InitRenderer(SDL_Window* Window)//, GraphicsAPI RequestedAPI)
{
	SDLRenderer = SDL_CreateRenderer(SDLWin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (SDLRenderer == nullptr)
	{
		SDL_DestroyWindow(SDLWin);
		MEGALOG("SDL_CreateRenderer Error: " << SDL_GetError() << std::endl);
		SDL_Quit();
	}
}

void CleanupAndQuit()
{
	SDL_DestroyRenderer(SDLRenderer);
	SDL_DestroyWindow(SDLWin);
	SDL_Quit();
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Init SDL subsystems we're going to use.
	InitSDL(SDL_INIT_VIDEO);

	// Init SDL window
	InitWindow("Megascops Engine", 100, 100, 640, 480, SDL_WINDOW_SHOWN);

	// Init renderer (default d3d12)
	InitRenderer(SDLWin);//, RenderAPI)

	// Game loop
	while (HandleEvents())
	{
		// Update

		// Render
		SDL_RenderClear(SDLRenderer);
		SDL_RenderPresent(SDLRenderer);
	}

	CleanupAndQuit();
}