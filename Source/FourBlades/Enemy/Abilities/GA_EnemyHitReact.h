// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Abilities/FBEnemyGameplayAbility.h"
#include "GA_EnemyHitReact.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_EnemyHitReact : public UFBEnemyGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_EnemyHitReact();

public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> ReactMontage;

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
