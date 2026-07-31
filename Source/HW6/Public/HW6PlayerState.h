// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HW6PlayerState.generated.h"

UCLASS()
class HW6_API AHW6PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AHW6PlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	int32 GetCurrentAttempts() const;
	int32 GetMaxAttempts() const;
	bool HasAttemptsLeft() const;

	bool AddAttempt();
	void ResetAttempts();

protected:
	UFUNCTION()
	void OnRep_CurrentAttempts();

	UPROPERTY(ReplicatedUsing = OnRep_CurrentAttempts, VisibleAnywhere, BlueprintReadOnly, Category = "Number Baseball")
	int32 CurrentAttempts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Number Baseball", meta = (ClampMin = "1"))
	int32 MaxAttempts;
};
