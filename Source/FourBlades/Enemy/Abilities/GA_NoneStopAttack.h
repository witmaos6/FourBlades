// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Abilities/FBEnemyGameplayAbility.h"
#include "Enemy/EnemyData/FBEnemyAbilitySet.h"
#include "GA_NoneStopAttack.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_NoneStopAttack : public UFBEnemyGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_NoneStopAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EAbilityConditionType AbilityConditionType;

	UPROPERTY(EditDefaultsOnly, Category = "DataAsset")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY()
	TObjectPtr<class UAbilityFragment_AttackDamages> AttackDamages;

	uint8 CurrentAttack = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> ClashEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> ResistLaunchEffect; // To do: 추후에 FBResistLaunchEffect로 변경

	FActiveGameplayEffectHandle ResistLaunchEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Clash")
	TSubclassOf<UGameplayEffect> ClashSuccessEffect; // 플레이어가 부여하는 ClashSuccessEffect와 동일하다.

	UPROPERTY(EditDefaultsOnly, Category = "Clash")
	TSubclassOf<UGameplayEffect> DecreaseResistLaunchEffect;

protected:
	void CurrentAttackCheck();

public:
	void AnimNotifyAttackHitCheck();

	void AnimNotifyClashWindow();

protected:
	float ValidDistance;

	float ValidAngle;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "TargetActor")
	TSubclassOf<class ATargetActor_Sweep> TargetActorClass;

	UFUNCTION()
	void OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	bool IsClash();

	void SelfLaunch();

public:
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);

public:
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
