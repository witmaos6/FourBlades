// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Abilities/FBEnemyGameplayAbility.h"
#include "Enemy/EnemyData/FBEnemyAbilitySet.h"
#include "GA_EnemyAttack.generated.h"


/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_EnemyAttack : public UFBEnemyGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_EnemyAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	FName GetNextSection();

protected:
	void JumpToSection();

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EAbilityConditionType AbilityConditionType;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "DataAsset")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY()
	TObjectPtr<class UAbilityFragment_AttackDamages> AttackDamages;

	uint8 CurrentCombo = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffect;

public:
	void AnimNotifyAttackHitCheck();

	void AnimNotifyNextCombo();

	UPROPERTY()
	TWeakObjectPtr<AActor> CombatTargetActor;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "TargetActor")
	TSubclassOf<class ATargetActor_Sweep> TargetActorClass;

	UFUNCTION()
	void OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

public:
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);

public:
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
