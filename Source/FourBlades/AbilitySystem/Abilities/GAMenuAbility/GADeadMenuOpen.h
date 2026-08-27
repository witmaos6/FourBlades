// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GAMenuBase.h"
#include "GADeadMenuOpen.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGADeadMenuOpen : public UGAMenuBase
{
	GENERATED_BODY()
public:
	UGADeadMenuOpen(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void CallEndAbility();
};
