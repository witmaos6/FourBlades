// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GASkillBase.h"
#include "GA_Rush.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_Rush : public UGASkillBase
{
	GENERATED_BODY()

public:
	UGA_Rush();

protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY()
	TObjectPtr<class UAbilityFragment_AttackDamages> AttackDamages;

public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "TargetActor")
	TSubclassOf<class ATargetActor_Sweep> TargetActorClass;

public:
	void AnimNotifyAttackHitCheck();

	UFUNCTION()
	void OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};
