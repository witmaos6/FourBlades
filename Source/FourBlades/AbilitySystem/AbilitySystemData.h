// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbilitySystemData.generated.h"

UENUM()
enum class EContinuousDirection
{
	NONE,
	Left,
	Right
};

USTRUCT(BlueprintType)
struct FFBContinuousCombo
{
	GENERATED_BODY()

	FFBContinuousCombo() : PrevDirection(EContinuousDirection::NONE), LeftComboIndex(0), RightComboIndex(0) {}

	FFBContinuousCombo(EContinuousDirection InDirection, uint8 InLeftComboIndex, uint8 InRightComboIndex) 
		: PrevDirection(InDirection), LeftComboIndex(InLeftComboIndex), RightComboIndex(InRightComboIndex) {}

	EContinuousDirection PrevDirection;

	uint8 LeftComboIndex;

	uint8 RightComboIndex;
};

/**
 * 
 */
UCLASS()
class FOURBLADES_API UAbilitySystemData : public UObject
{
	GENERATED_BODY()
public:
	UAbilitySystemData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private: // Slash, Smash
	uint8 SlashComboIndex;

public:
	void SetSlashComboIndex(uint8 Index) { SlashComboIndex = Index; }

	uint8 GetSlashComboIndex() { return SlashComboIndex; }

private: // Continuous Combo Attack
	FFBContinuousCombo ContinuousCombo;

	FTimerHandle ContinuousComboTimeoutHandle;

public:
	void SetContinuousCombo(EContinuousDirection InDirection, uint8 InCombo)
	{
		ContinuousCombo.PrevDirection = InDirection;
		if (InDirection == EContinuousDirection::Left)
		{
			ContinuousCombo.LeftComboIndex = InCombo;
		}
		else
		{
			ContinuousCombo.RightComboIndex = InCombo;
		}
	}

	EContinuousDirection GetPrevDirection() { return ContinuousCombo.PrevDirection; }

	uint8 GetDirectionCombo(EContinuousDirection InDirection)
	{
		if (InDirection == EContinuousDirection::Left)
			return ContinuousCombo.LeftComboIndex;

		if (InDirection == EContinuousDirection::Right)
			return ContinuousCombo.RightComboIndex;

		return 255;
	}

	uint8 GetReverseDirectionCombo(EContinuousDirection InDirection)
	{
		if (InDirection == EContinuousDirection::Left)
			return ContinuousCombo.RightComboIndex;

		if (InDirection == EContinuousDirection::Right)
			return ContinuousCombo.LeftComboIndex;

		return 255;
	}

	void StartContinuousComboTimeout(float ComboTimeoutDuration = 1.0f);

	void CancelContinuousComboTimeout();

	void ResetContinuousCombo();

private: // Guard Counter
	FName SwitchingInputConfig;

public:
	void SetSwitchingInputConfig(FName InputConfig) { SwitchingInputConfig = InputConfig; }

	FName GetSwitchingInputConfig() const { return SwitchingInputConfig; }

public:
	virtual void BeginDestroy() override;
};
