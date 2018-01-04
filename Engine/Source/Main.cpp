#include <iostream>
#include <SDL.h>

void WinMain()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
	}

	SDL_Window* SDLWin = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (SDLWin == nullptr) 
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
	}

	SDL_Renderer *SDLRenderer = SDL_CreateRenderer(SDLWin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (SDLRenderer == nullptr) 
	{
		SDL_DestroyWindow(SDLWin);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
	}

	SDL_RenderClear(SDLRenderer);
	SDL_RenderPresent(SDLRenderer);
	SDL_Delay(10000);

	SDL_DestroyRenderer(SDLRenderer);
	SDL_DestroyWindow(SDLWin);
	SDL_Quit();
}