// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/FBGASUserWidget.h"
#include "GameplayTagContainer.h"
#include "FBCoolDownWidget.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBCoolDownWidget : public UFBGASUserWidget
{
	GENERATED_BODY()
public:
	UFBCoolDownWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

protected:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

	UPROPERTY(EditDefaultsOnly, Category = "CoolDownTag", Meta = (Categories = "AbilityCoolDown"))
	FGameplayTag CoolDownTag;

	FDelegateHandle CoolDownDelegate;

	FTimerHandle CooldownUpdateTimer;

	float CachedTotalTime;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> CoolDownProgressBar;

	void OnCoolDownTagChanged(const FGameplayTag Tag, int32 NewCount);

	void StartCooldownDisplay();

	void UpdateCoolDown();

	void StopCooldownDisplay();

	float GetCoolDownTotalTime() const;

	float GetCooldownRemainingTime() const;

public:
	virtual void NativeDestruct() override;
};
