// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HW6GameState.generated.h"

class AHW6PlayerController;
class AHW6PlayerState;

UCLASS()
class HW6_API AHW6GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastBroadcastMessage(const FString& Message);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowRoundResult(const FString& ResultMessage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHideRoundResult();

	void SetTurnState(AHW6PlayerState* InCurrentTurnPlayer, int32 InRemainingTurnSeconds);
	void SetRemainingTurnSeconds(int32 InRemainingTurnSeconds);

	AHW6PlayerState* GetCurrentTurnPlayer() const;
	int32 GetRemainingTurnSeconds() const;

protected:
	UFUNCTION()
	void OnRep_TurnState();

	UPROPERTY(ReplicatedUsing = OnRep_TurnState)
	TObjectPtr<AHW6PlayerState> CurrentTurnPlayer;

	UPROPERTY(ReplicatedUsing = OnRep_TurnState)
	int32 RemainingTurnSeconds = 0;

private:
	AHW6PlayerController* GetLocalHW6PlayerController() const;
	void NotifyTurnStateChanged();
};
