// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GASkillBase.h"
#include "GA_Guard.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_Guard : public UGASkillBase
{
	GENERATED_BODY()
public:
	UGA_Guard();

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void BindGuard();

	float GuardDecreaseApplyDamage(float ApplyDamage);

protected:
	void OnGuardSuccess();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> GuardMontage;

	UPROPERTY(VisibleDefaultsOnly, Category = "Montage")
	FName GuardBeginSectionName;

	UPROPERTY(VisibleDefaultsOnly, Category = "Montage")
	FName GuardLoopBeginSectionName;

	UPROPERTY(VisibleDefaultsOnly, Category = "Montage")
	FName GuardLoopEndSectionName;

	UPROPERTY(VisibleDefaultsOnly, Category = "Montage")
	FName ParrySectionName;

	UPROPERTY(VisibleDefaultsOnly, Category = "Montage")
	FName HitSectionName;

	UPROPERTY(EditDefaultsOnly, Category = "Counter")
	TSubclassOf<UGameplayEffect> ParryTagEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Counter")
	TSubclassOf<UGameplayEffect> RemoveParryTagEffectClass;

protected:
	UPROPERTY(EditDefaultsOnly, Category = GameplayCue, meta = (Categories = "GameplayCue"))
	FGameplayTag GuardSoundCueTag;

protected:

	void JumpToParry();

	void ActivateCounterIMC();

	void RestoreIMCFromCounter();

public:
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void ChangeNormalGuard();
	
protected:
	UPROPERTY()
	uint8 bAllowReGuard : 1;

	UPROPERTY()
	uint8 bCounterIMCActive : 1;

	uint8 bChangeNormalGuard : 1;

public:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void AllowReGuard();

	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	void AbilityReset();

	void UnbindDecreaseApplyDamage(const FGameplayAbilityActorInfo* ActorInfo);
};
