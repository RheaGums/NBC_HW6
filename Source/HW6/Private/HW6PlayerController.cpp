// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW6PlayerController.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HW6GameMode.h"
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
