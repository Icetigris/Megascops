// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// GameInstance.cpp - Interface thing for updating game-specific data
//==========================================================
#include "GameInstance.h"
#include <SDL.h>
#include "D3D12Adapter.h"
#include "D3D12Device.h"

uint64 GameInstance::FrameNumber = 0;
double GameInstance::DeltaTime = 1 / 30.0;
double GameInstance::CurrentTime = 0.0;
double GameInstance::PreviousTime = 0.0;

//turgle - to be moved/changed
Renderer GameInstance::D3D12Renderer;

//turgle - to be moved/changed
Triangle triangle;

GameInstance::GameInstance()
{
	Time& t = Time::GetInstance();
	D3D12Renderer.Create("Megascops Engine"); //turgle
	triangle.Initialize(D3D12Renderer.Adapter->RootDevice->d3dDevice, /*NodeMask=*/0); //turgle
}

// GameInstance::~GameInstance()
// {
// 	D3D12Renderer.Destroy();
// }

bool HandleEvents(SDL_Event& SDLEvent)
{
	while (SDL_PollEvent(&SDLEvent))
	{
		// Handle events
		switch (SDLEvent.type)
		{
		case SDL_QUIT:
			return false;
			break;
		case SDL_KEYUP:
			// single key press
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

void GameInstance::Tick()
{
	SDL_Event SDLEvent;
	while(HandleEvents(SDLEvent))
	{
		PreviousTime = CurrentTime;
		CurrentTime = Time::GetTimestampInSeconds();

		DeltaTime = CurrentTime - PreviousTime;

		//query runtime database for objects that need updating
		//or have table of things that need updating?
		//update anything the database gives back

		triangle.Update(SDLEvent);
		D3D12Renderer.Render(triangle);
	}

	// Clean up before quitting.
	D3D12Renderer.Destroy();
	SDL_Quit();
}