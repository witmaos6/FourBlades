// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Abilities/FBEnemyGameplayAbility.h"
#include "GA_SpawnActor.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_SpawnActor : public UFBEnemyGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_SpawnActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "DataAsset")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY()
	TObjectPtr<class UAbilityFragment_AttackDamages> AttackDamages;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor> SpawnActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName SpawnSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffect;

public:
	void AnimNotifyAttackHitCheck();

public:
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);
};
