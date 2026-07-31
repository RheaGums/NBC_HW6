// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW6GameState.h"

#include "Engine/World.h"
#include "HW6PlayerController.h"

void AHW6GameState::MulticastBroadcastMessage_Implementation(
	const FString& Message
)
{
	AHW6PlayerController* LocalPlayerController =
		Cast<AHW6PlayerController>(GetWorld()->GetFirstPlayerController());

	if (
		IsValid(LocalPlayerController)
		&& LocalPlayerController->IsLocalController()
	)
	{
		LocalPlayerController->DisplayMessage(Message);
	}
}
