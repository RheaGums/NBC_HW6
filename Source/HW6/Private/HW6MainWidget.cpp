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
		if (!SubmitButton->OnClicked.IsAlreadyBound(
			this,
			&UHW6MainWidget::HandleSubmitClicked
		))
		{
			SubmitButton->OnClicked.AddDynamic(
				this,
				&UHW6MainWidget::HandleSubmitClicked
			);
		}
	}

	if (IsValid(GuessInputTextBox))
	{
		if (!GuessInputTextBox->OnTextCommitted.IsAlreadyBound(
			this,
			&UHW6MainWidget::HandleInputTextCommitted
		))
		{
			GuessInputTextBox->OnTextCommitted.AddDynamic(
				this,
				&UHW6MainWidget::HandleInputTextCommitted
			);
		}

		GuessInputTextBox->SetHintText(
			NSLOCTEXT(
				"HW6",
				"InputHint",
				"숫자 3자리 또는 /chat 내용"
			)
		);
	}

	HideRoundResult();
}

void UHW6MainWidget::HandleSubmitClicked()
{
	SubmitCurrentInput();
}

void UHW6MainWidget::HandleInputTextCommitted(
	const FText& Text,
	const ETextCommit::Type CommitMethod
)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		SubmitCurrentInput();
	}
}

void UHW6MainWidget::SubmitCurrentInput()
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

void UHW6MainWidget::SetTurnState(
	const FString& CurrentTurnPlayerName,
	const int32 RemainingSeconds,
	const bool bIsLocalPlayersTurn
)
{
	if (IsValid(CurrentTurnTextBlock))
	{
		const FText TurnText = CurrentTurnPlayerName.IsEmpty()
			? NSLOCTEXT("HW6", "WaitingForTurn", "현재 턴: 대기 중")
			: FText::Format(
				bIsLocalPlayersTurn
					? NSLOCTEXT("HW6", "MyTurnFormat", "현재 턴: {0} (내 차례)")
					: NSLOCTEXT("HW6", "OtherTurnFormat", "현재 턴: {0}"),
				FText::FromString(CurrentTurnPlayerName)
			);

		CurrentTurnTextBlock->SetText(TurnText);
	}

	if (IsValid(TurnTimerTextBlock))
	{
		const FText TimerText = FText::Format(
			NSLOCTEXT("HW6", "TurnTimerFormat", "남은 시간: {0}초"),
			FText::AsNumber(RemainingSeconds)
		);

		TurnTimerTextBlock->SetText(TimerText);
	}

	if (IsValid(GuessInputTextBox))
	{
		GuessInputTextBox->SetIsEnabled(true);
	}

	if (IsValid(SubmitButton))
	{
		SubmitButton->SetIsEnabled(true);
	}
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
