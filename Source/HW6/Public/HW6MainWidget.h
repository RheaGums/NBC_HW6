// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "HW6MainWidget.generated.h"

class UButton;
class UOverlay;
class UTextBlock;

UCLASS()
class HW6_API UHW6MainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMessage(const FString& Message);
	void SetAttempts(int32 CurrentAttempts, int32 MaxAttempts);
	void SetTurnState(
		const FString& CurrentTurnPlayerName,
		int32 RemainingSeconds,
		bool bIsLocalPlayersTurn
	);
	void ShowRoundResult(const FString& ResultMessage);
	void HideRoundResult();

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void HandleSubmitClicked();

	UFUNCTION()
	void HandleInputTextCommitted(
		const FText& Text,
		ETextCommit::Type CommitMethod
	);

	void SubmitCurrentInput();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> GuessInputTextBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SubmitButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MessageTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AttemptsTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentTurnTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TurnTimerTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> ResultOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultTextBlock;
};
