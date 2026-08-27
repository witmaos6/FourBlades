// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GAMovementBase.h"
#include "GA_Sprint.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_Sprint : public UGAMovementBase
{
	GENERATED_BODY()
public:
	UGA_Sprint();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "CancelStateTags", Meta = (Categories = "Character.State"))
	FGameplayTagContainer CancelStateTags;

	void OnCancelTagChanged(const FGameplayTag Tag, int32 NewCount);

private:
	FTimerHandle StopCheckTimer;

	TMap<FGameplayTag, FDelegateHandle> CancelStateTagEventHandles;

	void CheckIfStopped();

public:
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	float OriginSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float IncreasingSpeed;

public:
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void AbilityReset();
};
