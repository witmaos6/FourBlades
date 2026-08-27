// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "FBStatPointAllocatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBStatPointAllocatorWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UFBStatPointAllocatorWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeOnInitialized() override;

private:
	void BindButtonEvents();

protected:
	virtual void NativeConstruct() override;

	void LevelUp(uint8 OldLevel, uint8 CurrentLevel);

private:
	void InitializeTempData();

	void UpdateAllUI();

	void UpdateStatDisplay();

	void UpdateButtonStates();

	bool CanDeallocatePoint(const FGameplayTag& StatTag);

	int32 GetFinalAllocatedPoints(const FGameplayTag& StatTag);

	bool HasPendingChanges() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TextBlock", meta = (BindWidget))
	TObjectPtr<class UTextBlock> TempAttackAllocatedPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TextBlock", meta = (BindWidget))
	TObjectPtr<class UTextBlock> TempDefenseAllocatedPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TextBlock", meta = (BindWidget))
	TObjectPtr<class UTextBlock> TempHealthAllocatedPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TextBlock", meta = (BindWidget))
	TObjectPtr<class UTextBlock> RemainStatPoint;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Button", meta = (BindWidget))
	TObjectPtr<class UButton> AttackPowerPointUpButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Button", meta = (BindWidget))
	TObjectPtr<class UButton> AttackPowerPointDownButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Button", meta = (BindWidget))
	TObjectPtr<class UButton> DefensePointUpButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Button", meta = (BindWidget))
	TObjectPtr<class UButton> DefensePointDownButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Button", meta = (BindWidget))
	TObjectPtr<class UButton> HealthPointUpButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Button", meta = (BindWidget))
	TObjectPtr<class UButton> HealthPointDownButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Button", meta = (BindWidget))
	TObjectPtr<class UButton> ApplyButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Button", meta = (BindWidget))
	TObjectPtr<class UButton> CancelButton;

protected:
	UFUNCTION()
	void OnAttackUpClicked();

	UFUNCTION()
	void OnAttackDownClicked();

	UFUNCTION()
	void OnDefenseUpClicked();

	UFUNCTION()
	void OnDefenseDownClicked();

	UFUNCTION()
	void OnHealthUpClicked();

	UFUNCTION()
	void OnHealthDownClicked();

	void AllocatePoint(const FGameplayTag& StatTag);

	void DeallocatePoint(const FGameplayTag& StatTag);

private:
	UPROPERTY()
	TObjectPtr<class UFBSaveGame> SaveData;

	TWeakInterfacePtr<class IFBCharacterProfileInterface> CharacterProfileInterface;

private:
	UPROPERTY()
	TMap<FGameplayTag, int32> BaseAllocatedPoints;

	UPROPERTY()
	TMap<FGameplayTag, int32> ChangeDelta;

	UPROPERTY()
	uint32 TempRemainingPoints;

protected:
	UFUNCTION()
	void OnApplyClicked();

	UFUNCTION()
	void OnCancelClicked();

private:
	void ApplyTempDataToSaveGame();

protected:
	virtual void NativeDestruct() override;
};
