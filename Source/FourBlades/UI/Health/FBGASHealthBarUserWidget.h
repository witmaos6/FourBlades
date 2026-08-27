// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/FBGASUserWidget.h"
#include "GameplayEffectTypes.h"
#include "FBGASHealthBarUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBGASHealthBarUserWidget : public UFBGASUserWidget
{
	GENERATED_BODY()
protected:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

	virtual void OnHealthChanged(const FOnAttributeChangeData& ChangeData);

	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);

	void UpdateHealthBar();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextHealth;

	float CurrentHealth = 0.0f;

	float CurrentMaxHealth = 1.0f;
};
