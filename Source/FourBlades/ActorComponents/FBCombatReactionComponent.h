// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "AbilitySystem/FBGameplayEffectContext.h"
#include "GameplayTagContainer.h"
#include "FBCombatReactionComponent.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBCombatReactionComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UFBCombatReactionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	void RetryBind();

	bool HealthSetBind();
private:
	FTimerHandle RetryBindTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "React")
	uint8 FlinchMaxCount;

	uint8 FlinchCount;

	UPROPERTY(EditDefaultsOnly, Category = "React")
	TSubclassOf<class UGameplayEffect> ResistFlinchLaunch;

	UPROPERTY(EditDefaultsOnly, Category = "React")
	TSubclassOf<class UGameplayEffect> ResistKnockbackLaunch;

	FActiveGameplayEffectHandle ResistEffectHandle;

	FDelegateHandle ResistRemovedDelegateHandle;

public:
	void OnReaction(AActor* CurrentInstigator, AActor* CurrentEffectCauser, const struct FFBTargetPayload& TargetPayload);

	void CheckReaction(UAbilitySystemComponent* OwnerASC, const struct FFBTargetPayload& TargetPayload);

	void ActivateReaction(FGameplayTag ActivateReaction, float ReactDurationTime = 1.0f);

	void ApplyResistEffect(UAbilitySystemComponent* OwnerASC, TSubclassOf<class UGameplayEffect> ResistEffect);

	void OnResistEffectRemoved(const FGameplayEffectRemovalInfo& RemovedEffect);

	FVector GetLaunchDirection(AActor* EffectCauser, const FFBTargetPayload& TargetPayload);

	void Resurrection();
};
