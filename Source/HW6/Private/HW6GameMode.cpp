// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW6GameMode.h"

#include "HW6GameState.h"
#include "HW6PlayerController.h"
#include "HW6PlayerState.h"

AHW6GameMode::AHW6GameMode()
{
	GameStateClass = AHW6GameState::StaticClass();
	PlayerControllerClass = AHW6PlayerController::StaticClass();
	PlayerStateClass = AHW6PlayerState::StaticClass();
}
