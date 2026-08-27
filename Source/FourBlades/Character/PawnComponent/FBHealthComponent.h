// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "Delegates/Delegate.h"
#include "FBHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FFBHealthAttributeChanged, UFBHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FFBMaxHealthAttributeChanged, UFBHealthComponent*, HealthComponent, float, OldValue, float, NewValue);

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()
public:
	UFBHealthComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FFBHealthAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FFBMaxHealthAttributeChanged OnMaxHealthChanged;

public:
	UPROPERTY()
	TObjectPtr<class UFBAbilitySystemComponent> AbilitySystemComponent;

	void InitializeWithAbilitySystem(UFBAbilitySystemComponent* InASC);

	void UninitializeWithAbilitySystem();

	void HandleHealthChanged(const struct FOnAttributeChangeData& ChangeData);

	void HandleMaxHealthChanged(const struct FOnAttributeChangeData& ChangeData);

public:
	UPROPERTY()
	TObjectPtr<const class UFBHealthSet> HealthSet;

	UFUNCTION(BlueprintPure, Category = "FB|Health")
	static UFBHealthComponent* FindHealthComponent(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "FB|Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "FB|Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "FB|Health")
	float GetHealthNormalized() const;
};
