// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GASkillBase.h"
#include "AbilitySystem/AbilitySystemData.h"
#include "GA_ContinuousAttack.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_ContinuousAttack : public UGASkillBase
{
	GENERATED_BODY()
public:
	UGA_ContinuousAttack();

public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	FName GetNextSection();

public:
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void CheckClash();

	void SuccessClash();

	void ApplyInstigatorClash();

protected:
	void JumpToSection();

protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY()
	TObjectPtr<class UAbilityFragment_AttackDamages> AttackDamages;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	EContinuousDirection ContinuousDirection;

	uint8 CurrentCombo = 0;

	uint8 MaxComboCount = 3;

	bool bInput = false;

	UPROPERTY(EditDefaultsOnly, Category = "Clash")
	TSubclassOf<UGameplayEffect> ClashSuccessEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Clash")
	TSubclassOf<UGameplayEffect> ClashEffect; // 몬스터가 부여하는 ClashEffect와 동일

	UPROPERTY(EditDefaultsOnly, Category = "Clash")
	TSubclassOf<UGameplayEffect> ResistKnockback;

	UPROPERTY(EditDefaultsOnly, Category = GameplayCue, meta = (Categories = "GameplayCue"))
	FGameplayTag ClashSuccessGameplayCue;

	bool bClashSuccess = false;

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

protected:
	void AbilityReset();
};
