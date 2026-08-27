// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FBGameplayAbility.h"
#include "GA_PlayerHitReact.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_PlayerHitReact : public UFBGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_PlayerHitReact();

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> ReactMontage;

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
