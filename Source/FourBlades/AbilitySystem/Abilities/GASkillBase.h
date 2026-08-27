// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FBGameplayAbility.h"
#include "AbilitySystem/FBGameplayEffectContext.h"
#include "GASkillBase.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGASkillBase : public UFBGameplayAbility
{
	GENERATED_BODY()

public:
	UGASkillBase();

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = GameplayCue, meta = (Categories = "GameplayCue"))
	FGameplayTag TargetDataGameplayCueTag;

	// TArray<float> 버프 등 추가 데미지 로직 들어갈 수 있음
};
