// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FBGameplayAbility.h"
#include "GA_HitLaunch.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_HitLaunch : public UFBGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_HitLaunch();

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
