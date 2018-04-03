// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// CommandLine.h - Functions for dealing with command line input.
//==============================================================================================================
#pragma once
#include "Platform.h"

struct CommandLine
{
	int32 argc;
	char** argv;

};

extern void HandleCommandLineInput();