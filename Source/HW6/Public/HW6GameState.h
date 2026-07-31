// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HW6GameState.generated.h"

class AHW6PlayerController;

UCLASS()
class HW6_API AHW6GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastBroadcastMessage(const FString& Message);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowRoundResult(const FString& ResultMessage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHideRoundResult();

private:
	AHW6PlayerController* GetLocalHW6PlayerController() const;
};
