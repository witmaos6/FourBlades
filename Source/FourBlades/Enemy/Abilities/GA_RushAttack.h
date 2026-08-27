// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Abilities/FBEnemyGameplayAbility.h"
#include "GA_RushAttack.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_RushAttack : public UFBEnemyGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_RushAttack();

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "DataAsset")
	TObjectPtr<UAnimMontage> RushMontage;

	UPROPERTY()
	TObjectPtr<class UAbilityFragment_AttackDamages> AttackDamages;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffect;

public:
	void AnimNotifyAttackHitCheck();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "TargetActor")
	TSubclassOf<class ATargetActor_Sweep> TargetActorClass;

	UFUNCTION()
	void OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

public:
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);
};
