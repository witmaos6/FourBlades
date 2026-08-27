// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GASkillBase.h"
#include "GA_ModeChange.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_ModeChange : public UGASkillBase
{
	GENERATED_BODY()
public:
	UGA_ModeChange();

public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> ChangedMontage;

	UPROPERTY()
	TObjectPtr<UAnimMontage> ResetMontage;

	UPROPERTY(EditDefaultsOnly, Category = "InputConfig")
	FName ActivateConfig;

	UPROPERTY(EditDefaultsOnly, Category = "InputConfig")
	FName DeactivateConfig;

public:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
