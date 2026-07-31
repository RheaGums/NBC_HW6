// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW6GameState.h"

#include "Engine/World.h"
#include "HW6PlayerController.h"

void AHW6GameState::MulticastBroadcastMessage_Implementation(
	const FString& Message
)
{
	AHW6PlayerController* LocalPlayerController =
		GetLocalHW6PlayerController();

	if (IsValid(LocalPlayerController))
	{
		LocalPlayerController->DisplayMessage(Message);
	}
}

void AHW6GameState::MulticastShowRoundResult_Implementation(
	const FString& ResultMessage
)
{
	AHW6PlayerController* LocalPlayerController =
		GetLocalHW6PlayerController();

	if (IsValid(LocalPlayerController))
	{
		LocalPlayerController->ShowRoundResult(ResultMessage);
	}
}

void AHW6GameState::MulticastHideRoundResult_Implementation()
{
	AHW6PlayerController* LocalPlayerController =
		GetLocalHW6PlayerController();

	if (IsValid(LocalPlayerController))
	{
		LocalPlayerController->HideRoundResult();
	}
}

AHW6PlayerController* AHW6GameState::GetLocalHW6PlayerController() const
{
	AHW6PlayerController* LocalPlayerController =
		Cast<AHW6PlayerController>(GetWorld()->GetFirstPlayerController());

	if (
		!IsValid(LocalPlayerController)
		|| !LocalPlayerController->IsLocalController()
	)
	{
		return nullptr;
	}

	return LocalPlayerController;
}
