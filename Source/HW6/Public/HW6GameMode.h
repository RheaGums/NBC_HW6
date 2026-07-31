// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HW6GameMode.generated.h"

class AHW6PlayerController;

UCLASS()
class HW6_API AHW6GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHW6GameMode();

	void ProcessPlayerInput(AHW6PlayerController* SubmittingPlayer, const FString& Input);

protected:
	virtual void BeginPlay() override;

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

private:
	static constexpr int32 SecretNumberLength = 3;
	static constexpr int32 MinSecretDigit = 1;
	static constexpr int32 MaxSecretDigit = 9;

	TArray<int32> SecretNumbers;
};
