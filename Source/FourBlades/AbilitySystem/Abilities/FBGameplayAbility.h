// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FBGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EFBAbilityActivationPolicy : uint8
{
	OnInputTriggered, // Pressed / Released
	WhileInputActive,
	OnSpawn,
};

/**
 *
 */
UCLASS()
class FOURBLADES_API UFBGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UFBGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FB|AbilityActivation")
	EFBAbilityActivationPolicy ActivationPolicy;

public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
	FGameplayTagContainer RequiredTagsAnyOne;

protected:
	void SetRotationAvatarToLastMoveDirection();

	void SetMovementMode(EMovementMode InMovementMode);

	FTransform GetOwnerSocketTransform(const FName SocketName);

	APlayerState* GetOwnerPlayerState();

	class UFBAbilityFragmentManager* GetAbilityFragmentManager(const FGameplayTag InAbilityTag);

	const class UFBAbilityFragmentManager* GetAbilityFragmentManager(const FGameplayAbilitySpec& Spec);

	void ApplyTemporaryTimeDilation(float TimeScale, float DurationInRealSeconds);

public:
	void ResetGlobalTimeDilation();

protected:
	UFUNCTION()
	virtual void OnCompleteCallback();

	UFUNCTION()
	virtual void OnBlendOut();

	UFUNCTION()
	virtual void OnInterruptedCallback();

	void ApplyTagEffect(TSubclassOf<UGameplayEffect>& EffectClass);
};
