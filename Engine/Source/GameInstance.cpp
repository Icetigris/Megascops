// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// GameInstance.cpp - Interface thing for updating game-specific data
//==========================================================
#include "GameInstance.h"

uint64 GameInstance::FrameNumber = 0;
double GameInstance::DeltaTime = 1 / 30.0;
double GameInstance::CurrentTime = 0.0;
double GameInstance::PreviousTime = 0.0;

void GameInstance::Update()
{
	PreviousTime = CurrentTime;
	CurrentTime = Time::GetTimestampInSeconds();

	DeltaTime = CurrentTime - PreviousTime;

	//query runtime database for objects that need updating
	//or have table of things that need updating?
	//update anything the database gives back
}