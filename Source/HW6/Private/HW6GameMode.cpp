// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW6GameMode.h"

#include "HW6GameState.h"
#include "HW6PlayerController.h"
#include "HW6PlayerState.h"

AHW6GameMode::AHW6GameMode()
{
	GameStateClass = AHW6GameState::StaticClass();
	PlayerControllerClass = AHW6PlayerController::StaticClass();
	PlayerStateClass = AHW6PlayerState::StaticClass();
}

void AHW6GameMode::BeginPlay()
{
	Super::BeginPlay();

	GenerateRandomNumbers();
}

void AHW6GameMode::GenerateRandomNumbers()
{
	if (!HasAuthority())
	{
		return;
	}

	SecretNumbers.Reset();

	while (SecretNumbers.Num() < SecretNumberLength)
	{
		const int32 Candidate = FMath::RandRange(MinSecretDigit, MaxSecretDigit);

		if (!SecretNumbers.Contains(Candidate))
		{
			SecretNumbers.Add(Candidate);
		}
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[Server] Secret Number: %d%d%d"),
		SecretNumbers[0],
		SecretNumbers[1],
		SecretNumbers[2]
	);
}

bool AHW6GameMode::ValidateInput(
	const FString& Input,
	FString& OutValidatedInput,
	FString& OutErrorMessage
) const
{
	OutValidatedInput.Reset();
	OutErrorMessage.Reset();

	const FString NormalizedInput = Input.TrimStartAndEnd();

	if (NormalizedInput.Len() != SecretNumberLength)
	{
		OutErrorMessage = TEXT("3자리 숫자를 입력해주세요.");
		return false;
	}

	TArray<TCHAR> SeenDigits;
	SeenDigits.Reserve(SecretNumberLength);

	for (int32 Index = 0; Index < NormalizedInput.Len(); ++Index)
	{
		const TCHAR Digit = NormalizedInput[Index];

		if (!FChar::IsDigit(Digit))
		{
			OutErrorMessage = TEXT("숫자만 입력해주세요.");
			return false;
		}

		if (Digit == TEXT('0'))
		{
			OutErrorMessage = TEXT("1부터 9까지의 숫자를 입력해주세요.");
			return false;
		}

		if (SeenDigits.Contains(Digit))
		{
			OutErrorMessage = TEXT("중복되지 않은 숫자를 입력해주세요.");
			return false;
		}

		SeenDigits.Add(Digit);
	}

	OutValidatedInput = NormalizedInput;
	return true;
}

FString AHW6GameMode::CheckAnswer(
	const FString& ValidatedInput,
	int32& OutStrikeCount,
	int32& OutBallCount
) const
{
	OutStrikeCount = 0;
	OutBallCount = 0;

	if (
		SecretNumbers.Num() != SecretNumberLength
		|| ValidatedInput.Len() != SecretNumberLength
	)
	{
		UE_LOG(LogTemp, Error, TEXT("[Server] CheckAnswer received invalid data."));
		return TEXT("INVALID");
	}

	for (int32 Index = 0; Index < SecretNumberLength; ++Index)
	{
		const int32 GuessDigit = ValidatedInput[Index] - TEXT('0');

		if (SecretNumbers[Index] == GuessDigit)
		{
			++OutStrikeCount;
		}
		else if (SecretNumbers.Contains(GuessDigit))
		{
			++OutBallCount;
		}
	}

	if (OutStrikeCount == 0 && OutBallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(
		TEXT("%dS%dB"),
		OutStrikeCount,
		OutBallCount
	);
}
