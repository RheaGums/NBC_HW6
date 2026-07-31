// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HW6GameMode.generated.h"

UCLASS()
class HW6_API AHW6GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHW6GameMode();

protected:
	virtual void BeginPlay() override;

	void GenerateRandomNumbers();
	bool ValidateInput(
		const FString& Input,
		FString& OutValidatedInput,
		FString& OutErrorMessage
	) const;

private:
	static constexpr int32 SecretNumberLength = 3;
	static constexpr int32 MinSecretDigit = 1;
	static constexpr int32 MaxSecretDigit = 9;

	TArray<int32> SecretNumbers;
};
