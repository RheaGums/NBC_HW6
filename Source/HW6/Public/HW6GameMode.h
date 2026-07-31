// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "HW6GameMode.generated.h"

class AHW6PlayerController;
class AHW6PlayerState;

UCLASS()
class HW6_API AHW6GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHW6GameMode();

	void ProcessPlayerInput(AHW6PlayerController* SubmittingPlayer, const FString& Input);

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void GenerateRandomNumbers();
	bool ValidateInput(
		const FString& Input,
		FString& OutValidatedInput,
		FString& OutErrorMessage
	) const;
	FString CheckAnswer(
		const FString& ValidatedInput,
		int32& OutStrikeCount,
		int32& OutBallCount
	) const;
	bool AreAllPlayersOutOfAttempts() const;
	void StartTurn(AHW6PlayerState* TurnPlayer);
	void AdvanceTurn();
	void HandleTurnTimerTick();
	void HandleTurnTimeout();
	void StopTurnTimer();
	void EndRound(const FString& ResultMessage);
	void ResetGame();

private:
	static constexpr int32 SecretNumberLength = 3;
	static constexpr int32 MinSecretDigit = 1;
	static constexpr int32 MaxSecretDigit = 9;
	static constexpr int32 TurnDurationSeconds = 10;
	static constexpr float RoundResetDelay = 3.0f;

	TArray<int32> SecretNumbers;
	TWeakObjectPtr<AHW6PlayerState> CurrentTurnPlayer;
	bool bRoundActive = true;
	bool bSubmittedThisTurn = false;
	int32 RemainingTurnSeconds = 0;
	int32 NextPlayerNumber = 1;
	FTimerHandle TurnTimerHandle;
	FTimerHandle ResetGameTimerHandle;
};
