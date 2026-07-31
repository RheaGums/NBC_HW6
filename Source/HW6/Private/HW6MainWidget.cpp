// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW6MainWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "HW6PlayerController.h"

void UHW6MainWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(SubmitButton))
	{
		SubmitButton->OnClicked.AddDynamic(
			this,
			&UHW6MainWidget::HandleSubmitClicked
		);
	}

	HideRoundResult();
}

void UHW6MainWidget::HandleSubmitClicked()
{
	AHW6PlayerController* PlayerController =
		Cast<AHW6PlayerController>(GetOwningPlayer());

	if (!IsValid(PlayerController) || !IsValid(GuessInputTextBox))
	{
		return;
	}

	const FString Input = GuessInputTextBox->GetText().ToString();
	PlayerController->SubmitGuess(Input);

	GuessInputTextBox->SetText(FText::GetEmpty());
	GuessInputTextBox->SetKeyboardFocus();
}

void UHW6MainWidget::SetMessage(const FString& Message)
{
	if (IsValid(MessageTextBlock))
	{
		MessageTextBlock->SetText(FText::FromString(Message));
	}
}

void UHW6MainWidget::SetAttempts(
	const int32 CurrentAttempts,
	const int32 MaxAttempts
)
{
	if (!IsValid(AttemptsTextBlock))
	{
		return;
	}

	const FText AttemptsText = FText::Format(
		NSLOCTEXT("HW6", "AttemptsFormat", "시도 횟수: {0}/{1}"),
		FText::AsNumber(CurrentAttempts),
		FText::AsNumber(MaxAttempts)
	);

	AttemptsTextBlock->SetText(AttemptsText);
}

void UHW6MainWidget::ShowRoundResult(const FString& ResultMessage)
{
	if (IsValid(ResultTextBlock))
	{
		ResultTextBlock->SetText(FText::FromString(ResultMessage));
	}

	if (IsValid(ResultOverlay))
	{
		ResultOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void UHW6MainWidget::HideRoundResult()
{
	if (IsValid(ResultOverlay))
	{
		ResultOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}
