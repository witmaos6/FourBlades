// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GASkillBase.h"
#include "GA_Smash.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_Smash : public UGASkillBase
{
	GENERATED_BODY()
	
public:
	UGA_Smash();

public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	FName GetNextSection();

public:
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	void JumpToSection();

protected:
	UPROPERTY()
	TObjectPtr<class UAbilityFragment_AnimMontageList> AnimMontageListFragment;

	UPROPERTY()
	TObjectPtr<class UAbilityFragment_AttackDamageList> AttackDamageListFragment;

	uint8 ComboAttackDataIndex;

	uint8 CurrentCombo = 0;

	uint8 MaxComboCount = 0;

	bool bInput = false;

public:
	bool bAllowAttack = true; // AnimNotify에서 수정

	void AnimNotifyAttackHitCheck();

	void AnimNotifyAllowNextCombo();

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
