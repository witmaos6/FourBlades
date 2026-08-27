// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FBGameplayAbility.h"
#include "GAMenuBase.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGAMenuBase : public UFBGameplayAbility
{
	GENERATED_BODY()
public:
	UGAMenuBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftClassPtr<class UCommonActivatableWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "LayerTag", Meta = (Categories = "UI"))
	FGameplayTag LayerTag;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UCommonActivatableWidget> Widget;

protected:
	void OnWidgetDeactivated();
};
