// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW6PlayerState.h"

#include "Net/UnrealNetwork.h"

AHW6PlayerState::AHW6PlayerState()
	: CurrentAttempts(0)
	, MaxAttempts(3)
{
}

void AHW6PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHW6PlayerState, CurrentAttempts);
}

int32 AHW6PlayerState::GetCurrentAttempts() const
{
	return CurrentAttempts;
}

int32 AHW6PlayerState::GetMaxAttempts() const
{
	return MaxAttempts;
}

bool AHW6PlayerState::HasAttemptsLeft() const
{
	return CurrentAttempts < MaxAttempts;
}

bool AHW6PlayerState::AddAttempt()
{
	if (!HasAuthority() || !HasAttemptsLeft())
	{
		return false;
	}

	++CurrentAttempts;

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[Server] %s used an attempt: %d/%d"),
		*GetPlayerName(),
		CurrentAttempts,
		MaxAttempts
	);

	return true;
}

void AHW6PlayerState::ResetAttempts()
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentAttempts = 0;
}

void AHW6PlayerState::OnRep_CurrentAttempts()
{
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[Client] %s received attempts: %d/%d"),
		*GetPlayerName(),
		CurrentAttempts,
		MaxAttempts
	);
}
