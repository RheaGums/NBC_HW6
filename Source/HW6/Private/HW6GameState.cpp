// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW6GameState.h"

#include "Engine/World.h"
#include "HW6PlayerController.h"
#include "HW6PlayerState.h"
#include "Net/UnrealNetwork.h"

void AHW6GameState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHW6GameState, CurrentTurnPlayer);
	DOREPLIFETIME(AHW6GameState, RemainingTurnSeconds);
}

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

void AHW6GameState::SetTurnState(
	AHW6PlayerState* InCurrentTurnPlayer,
	const int32 InRemainingTurnSeconds
)
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentTurnPlayer = InCurrentTurnPlayer;
	RemainingTurnSeconds = FMath::Max(0, InRemainingTurnSeconds);
	NotifyTurnStateChanged();
}

void AHW6GameState::SetRemainingTurnSeconds(
	const int32 InRemainingTurnSeconds
)
{
	if (!HasAuthority())
	{
		return;
	}

	RemainingTurnSeconds = FMath::Max(0, InRemainingTurnSeconds);
	NotifyTurnStateChanged();
}

AHW6PlayerState* AHW6GameState::GetCurrentTurnPlayer() const
{
	return CurrentTurnPlayer;
}

int32 AHW6GameState::GetRemainingTurnSeconds() const
{
	return RemainingTurnSeconds;
}

void AHW6GameState::OnRep_TurnState()
{
	NotifyTurnStateChanged();
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

void AHW6GameState::NotifyTurnStateChanged()
{
	AHW6PlayerController* LocalPlayerController =
		GetLocalHW6PlayerController();

	if (IsValid(LocalPlayerController))
	{
		LocalPlayerController->RefreshTurnDisplay();
	}
}
