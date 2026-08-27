// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FBAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UFBAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

public:
	UPROPERTY()
	TObjectPtr<class UAbilitySystemData> AbilitySystemData;

public:
	void EnforceSingleStateTag();

protected:
	void OnCharacterStateTagChanged(const FGameplayTag Tag, int32 NewCount);

	void SetCharacterStateNormal();

// 가드 게이지 회복 로직으로 임시 기능이다.
public:
	void StartGuardGaugeRecovery();

	void StopGuardGaugeRecovery();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Guard")
	float GuardRecoveryRate = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Guard")
	float GuardRecoveryInterval = 1.0f;

	FTimerHandle GuardGaugeRecoveryTimer;

	void RecoverGuardGauge();

// 가드 게이지 회복 로직으로 임시 기능이다.
};
