// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW6GameMode.h"

#include "Engine/World.h"
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

void AHW6GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!HasAuthority() || !IsValid(NewPlayer))
	{
		return;
	}

	AHW6PlayerState* NewPlayerState =
		NewPlayer->GetPlayerState<AHW6PlayerState>();

	if (!IsValid(NewPlayerState))
	{
		return;
	}

	const FString NewPlayerName = FString::Printf(
		TEXT("Player %d"),
		NextPlayerNumber
	);
	++NextPlayerNumber;

	NewPlayerState->SetPlayerName(NewPlayerName);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[Server] %s joined the game."),
		*NewPlayerName
	);
}

void AHW6GameMode::ProcessPlayerInput(
	AHW6PlayerController* SubmittingPlayer,
	const FString& Input
)
{
	if (!HasAuthority() || !IsValid(SubmittingPlayer))
	{
		return;
	}

	if (!bRoundActive)
	{
		SubmittingPlayer->ClientReceiveMessage(
			TEXT("라운드가 종료되었습니다. 다음 라운드를 기다려주세요.")
		);
		return;
	}

	AHW6PlayerState* SubmittingPlayerState =
		SubmittingPlayer->GetPlayerState<AHW6PlayerState>();

	if (!IsValid(SubmittingPlayerState))
	{
		SubmittingPlayer->ClientReceiveMessage(TEXT("PlayerState를 찾을 수 없습니다."));
		return;
	}

	if (!SubmittingPlayerState->HasAttemptsLeft())
	{
		SubmittingPlayer->ClientReceiveMessage(TEXT("시도 기회를 모두 사용했습니다."));
		return;
	}

	FString ValidatedInput;
	FString ErrorMessage;

	if (!ValidateInput(Input, ValidatedInput, ErrorMessage))
	{
		SubmittingPlayer->ClientReceiveMessage(ErrorMessage);
		return;
	}

	if (!SubmittingPlayerState->AddAttempt())
	{
		SubmittingPlayer->ClientReceiveMessage(TEXT("시도 횟수를 증가시킬 수 없습니다."));
		return;
	}

	int32 StrikeCount = 0;
	int32 BallCount = 0;
	const FString Result = CheckAnswer(
		ValidatedInput,
		StrikeCount,
		BallCount
	);

	const FString ResultMessage = FString::Printf(
		TEXT("%s: %s -> %s [%d/%d]"),
		*SubmittingPlayerState->GetPlayerName(),
		*ValidatedInput,
		*Result,
		SubmittingPlayerState->GetCurrentAttempts(),
		SubmittingPlayerState->GetMaxAttempts()
	);

	UE_LOG(LogTemp, Log, TEXT("[Server] %s"), *ResultMessage);

	AHW6GameState* HW6GameState =
		GetWorld()->GetGameState<AHW6GameState>();

	if (IsValid(HW6GameState))
	{
		HW6GameState->MulticastBroadcastMessage(ResultMessage);
	}

	if (StrikeCount == SecretNumberLength)
	{
		EndRound(
			FString::Printf(
				TEXT("%s 승리! 3초 후 새 라운드가 시작됩니다."),
				*SubmittingPlayerState->GetPlayerName()
			)
		);
		return;
	}

	if (AreAllPlayersOutOfAttempts())
	{
		EndRound(TEXT("무승부! 3초 후 새 라운드가 시작됩니다."));
	}
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

bool AHW6GameMode::AreAllPlayersOutOfAttempts() const
{
	const AHW6GameState* HW6GameState =
		GetWorld()->GetGameState<AHW6GameState>();

	if (!IsValid(HW6GameState))
	{
		return false;
	}

	bool bFoundPlayer = false;

	for (const APlayerState* PlayerState : HW6GameState->PlayerArray)
	{
		const AHW6PlayerState* HW6PlayerState =
			Cast<AHW6PlayerState>(PlayerState);

		if (!IsValid(HW6PlayerState))
		{
			continue;
		}

		bFoundPlayer = true;

		if (HW6PlayerState->HasAttemptsLeft())
		{
			return false;
		}
	}

	return bFoundPlayer;
}

void AHW6GameMode::EndRound(const FString& ResultMessage)
{
	if (!HasAuthority() || !bRoundActive)
	{
		return;
	}

	bRoundActive = false;

	AHW6GameState* HW6GameState =
		GetWorld()->GetGameState<AHW6GameState>();

	if (IsValid(HW6GameState))
	{
		HW6GameState->MulticastShowRoundResult(ResultMessage);
	}

	GetWorldTimerManager().SetTimer(
		ResetGameTimerHandle,
		this,
		&AHW6GameMode::ResetGame,
		RoundResetDelay,
		false
	);
}

void AHW6GameMode::ResetGame()
{
	if (!HasAuthority())
	{
		return;
	}

	AHW6GameState* HW6GameState =
		GetWorld()->GetGameState<AHW6GameState>();

	if (IsValid(HW6GameState))
	{
		for (APlayerState* PlayerState : HW6GameState->PlayerArray)
		{
			AHW6PlayerState* HW6PlayerState =
				Cast<AHW6PlayerState>(PlayerState);

			if (IsValid(HW6PlayerState))
			{
				HW6PlayerState->ResetAttempts();
			}
		}
	}

	GenerateRandomNumbers();
	bRoundActive = true;

	if (IsValid(HW6GameState))
	{
		HW6GameState->MulticastHideRoundResult();
		HW6GameState->MulticastBroadcastMessage(
			TEXT("새 라운드가 시작되었습니다!")
		);
	}
}
