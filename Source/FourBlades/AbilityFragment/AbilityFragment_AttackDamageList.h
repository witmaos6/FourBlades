// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment/FBSkillDamageValue.h"
#include "AbilityFragment_AttackDamageList.generated.h"

USTRUCT()
struct FSkillDamageList
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FFBSkillDamageValue> SkillDamageValues;

	UPROPERTY(EditDefaultsOnly)
	TArray<FFBLaunchInfo> LaunchInfos;
};
/**
 * 
 */
UCLASS()
class FOURBLADES_API UAbilityFragment_AttackDamageList : public UFBAbilityFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FSkillDamageList> SkillDamageValueList;

	const FSkillDamageList& GetSkillDamageValueList(uint8 Index);
};
