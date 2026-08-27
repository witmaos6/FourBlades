// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GAInteractionBase.h"
#include "GA_SavingCurrentPoint.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGA_SavingCurrentPoint : public UGAInteractionBase
{
	GENERATED_BODY()
public:
	UGA_SavingCurrentPoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void AsyncSaveGameCompleted(bool bSuccess);

	void ShowSaveResultWidget(bool bSuccess);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UUserWidget> SaveSuccessWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UUserWidget> SaveFailedWidget;
};
