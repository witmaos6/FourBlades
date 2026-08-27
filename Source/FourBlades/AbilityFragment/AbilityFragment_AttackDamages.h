// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment/FBSkillDamageValue.h"
#include "AbilityFragment_AttackDamages.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UAbilityFragment_AttackDamages : public UFBAbilityFragment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "GameplayEffect|Damage")
	TArray<FFBSkillDamageValue> SkillDamageValues;

	UPROPERTY(EditDefaultsOnly, Category = "LaunchInfo")
	TArray<FFBLaunchInfo> LaunchInfos;
};
