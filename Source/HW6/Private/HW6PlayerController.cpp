// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW6PlayerController.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HW6GameMode.h"
#include "HW6GameState.h"
#include "HW6MainWidget.h"
#include "HW6PlayerState.h"

void AHW6PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	if (!MainWidgetClass)
	{
		MainWidgetClass = LoadClass<UHW6MainWidget>(
			nullptr,
			TEXT("/Game/UI/WBP_MainWidget.WBP_MainWidget_C")
		);
	}

	if (!MainWidgetClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("WBP_MainWidget을 /Game/UI 경로에서 찾을 수 없습니다.")
		);
		return;
	}

	MainWidget = CreateWidget<UHW6MainWidget>(this, MainWidgetClass);

	if (!IsValid(MainWidget))
	{
		return;
	}

	MainWidget->AddToViewport();
	RefreshAttemptsDisplay();
	RefreshTurnDisplay();

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(MainWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(
		EMouseLockMode::DoNotLock
	);
	SetInputMode(InputMode);

	bShowMouseCursor = true;
}

void AHW6PlayerController::SubmitGuess(const FString& Input)
{
	if (!IsLocalController())
	{
		return;
	}

	const FString NormalizedInput = Input.TrimStartAndEnd();

	if (
		NormalizedInput.Equals(TEXT("/chat"), ESearchCase::IgnoreCase)
		|| NormalizedInput.StartsWith(
			TEXT("/chat "),
			ESearchCase::IgnoreCase
		)
	)
	{
		ServerSubmitChatMessage(
			NormalizedInput.Mid(5).TrimStartAndEnd()
		);
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

void AHW6PlayerController::ServerSubmitChatMessage_Implementation(
	const FString& Message
)
{
	FString SanitizedMessage = Message.TrimStartAndEnd();
	SanitizedMessage.ReplaceInline(TEXT("\r"), TEXT(" "));
	SanitizedMessage.ReplaceInline(TEXT("\n"), TEXT(" "));

	if (SanitizedMessage.IsEmpty())
	{
		ClientReceiveMessage(TEXT("채팅 내용을 입력해주세요."));
		return;
	}

	constexpr int32 MaxChatMessageLength = 100;

	if (SanitizedMessage.Len() > MaxChatMessageLength)
	{
		ClientReceiveMessage(TEXT("채팅은 100자 이하로 입력해주세요."));
		return;
	}

	const AHW6PlayerState* HW6PlayerState =
		GetPlayerState<AHW6PlayerState>();

	if (!IsValid(HW6PlayerState))
	{
		ClientReceiveMessage(TEXT("PlayerState를 찾을 수 없습니다."));
		return;
	}

	AHW6GameState* HW6GameState =
		GetWorld()->GetGameState<AHW6GameState>();

	if (!IsValid(HW6GameState))
	{
		ClientReceiveMessage(TEXT("서버 GameState를 찾을 수 없습니다."));
		return;
	}

	const FString ChatMessage = FString::Printf(
		TEXT("[Chat] %s: %s"),
		*HW6PlayerState->GetPlayerName(),
		*SanitizedMessage
	);

	UE_LOG(LogTemp, Log, TEXT("[Server] %s"), *ChatMessage);
	HW6GameState->MulticastBroadcastMessage(ChatMessage);
}

void AHW6PlayerController::ClientReceiveMessage_Implementation(const FString& Message)
{
	DisplayMessage(Message);
}

void AHW6PlayerController::DisplayMessage(const FString& Message)
{
	UE_LOG(LogTemp, Display, TEXT("[Client] %s"), *Message);

	if (IsValid(MainWidget))
	{
		MainWidget->SetMessage(Message);
		RefreshAttemptsDisplay();
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Cyan,
			Message
		);
	}
}

void AHW6PlayerController::RefreshAttemptsDisplay()
{
	if (!IsLocalController() || !IsValid(MainWidget))
	{
		return;
	}

	const AHW6PlayerState* HW6PlayerState =
		GetPlayerState<AHW6PlayerState>();

	if (!IsValid(HW6PlayerState))
	{
		MainWidget->SetAttempts(0, 3);
		return;
	}

	MainWidget->SetAttempts(
		HW6PlayerState->GetCurrentAttempts(),
		HW6PlayerState->GetMaxAttempts()
	);
}

void AHW6PlayerController::RefreshTurnDisplay()
{
	if (!IsLocalController() || !IsValid(MainWidget))
	{
		return;
	}

	const AHW6GameState* HW6GameState =
		GetWorld()->GetGameState<AHW6GameState>();

	if (!IsValid(HW6GameState))
	{
		MainWidget->SetTurnState(TEXT(""), 0, false);
		return;
	}

	const AHW6PlayerState* CurrentTurnPlayer =
		HW6GameState->GetCurrentTurnPlayer();
	const AHW6PlayerState* LocalPlayerState =
		GetPlayerState<AHW6PlayerState>();

	const bool bIsLocalPlayersTurn =
		IsValid(CurrentTurnPlayer)
		&& CurrentTurnPlayer == LocalPlayerState;

	MainWidget->SetTurnState(
		IsValid(CurrentTurnPlayer)
			? CurrentTurnPlayer->GetPlayerName()
			: TEXT(""),
		HW6GameState->GetRemainingTurnSeconds(),
		bIsLocalPlayersTurn
	);
}

void AHW6PlayerController::ShowRoundResult(const FString& ResultMessage)
{
	if (IsValid(MainWidget))
	{
		MainWidget->ShowRoundResult(ResultMessage);
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Yellow,
			ResultMessage
		);
	}
}

void AHW6PlayerController::HideRoundResult()
{
	if (IsValid(MainWidget))
	{
		MainWidget->HideRoundResult();
	}
}
