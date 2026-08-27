// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/FBActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "FBCharacterStatMenu.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBCharacterStatMenu : public UFBActivatableWidget
{
	GENERATED_BODY()
public:
	UFBCharacterStatMenu(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	void UpdateCharacterStatUI();

protected:
	UPROPERTY()
	TWeakObjectPtr<class UFBCharacterStatComponent> CharacterStatComponent;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> FinalAttackPowerStat;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> FinalDefenseStat;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> FinalMaxHealthStat;

private:
	void StatTextUpdate(class UTextBlock* StatText, const FGameplayTag& StatTag);

protected:
	virtual void NativeDestruct() override;
};
