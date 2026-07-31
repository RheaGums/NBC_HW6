// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW6PlayerController.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HW6GameMode.h"

void AHW6PlayerController::SubmitGuess(const FString& Input)
{
	if (!IsLocalController())
	{
		return;
	}

	ServerSubmitGuess(Input);
}

void AHW6PlayerController::ServerSubmitGuess_Implementation(const FString& Input)
{
	AHW6GameMode* GameMode = Cast<AHW6GameMode>(GetWorld()->GetAuthGameMode());

	if (!IsValid(GameMode))
	{
		ClientReceiveMessage(TEXT("서버 GameMode를 찾을 수 없습니다."));
		return;
	}

	GameMode->ProcessPlayerInput(this, Input);
}

void AHW6PlayerController::ClientReceiveMessage_Implementation(const FString& Message)
{
	UE_LOG(LogTemp, Display, TEXT("[Client] %s"), *Message);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Cyan,
			Message
		);
	}
}
