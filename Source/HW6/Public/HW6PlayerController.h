// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HW6PlayerController.generated.h"

UCLASS()
class HW6_API AHW6PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void SubmitGuess(const FString& Input);

	UFUNCTION(Server, Reliable)
	void ServerSubmitGuess(const FString& Input);

	UFUNCTION(Client, Reliable)
	void ClientReceiveMessage(const FString& Message);
};
