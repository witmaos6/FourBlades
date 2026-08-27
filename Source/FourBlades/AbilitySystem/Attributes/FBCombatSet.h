// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/FBAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "FBCombatSet.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBCombatSet : public UFBAttributeSet
{
	GENERATED_BODY()
public:
	UFBCombatSet();

	ATTRIBUTE_ACCESSORS(UFBCombatSet, AttackPower);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FGameplayAttributeData AttackPower;

	ATTRIBUTE_ACCESSORS(UFBCombatSet, ResistLaunch);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FGameplayAttributeData ResistLaunch;

private:
	void AttackPowerClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

	void ResistLaunchClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
