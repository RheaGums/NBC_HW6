// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HW6MainWidget.generated.h"

class UButton;
class UEditableTextBox;
class UTextBlock;

UCLASS()
class HW6_API UHW6MainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMessage(const FString& Message);
	void SetAttempts(int32 CurrentAttempts, int32 MaxAttempts);

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void HandleSubmitClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> GuessInputTextBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SubmitButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MessageTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AttemptsTextBlock;
};
