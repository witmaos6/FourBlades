// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySystem/FBAbilityDefinition.h"
#include "FBEnemyAbilitySet.generated.h"

UENUM()
enum class EAbilityConditionType
{
	NONE,
	MeleeForwardAttack,
	MeleeRightAttack,
	MeleeLeftAttack,
	MeleeBackAttack,

	RangedAttack,

	Avoid, // 방향 고려 할 수도 있음
};

/*
 FFBWeightedAbilityDefinition
*/
USTRUCT(BlueprintType)
struct FFBWeightedAbilityDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FFBAbilityDefinition Definition;

	UPROPERTY(EditDefaultsOnly, Category = "Selection", meta = (ClampMin = 0, ClampMax = 100))
	float SelectionWeight = 1.0f;
};

/*
 FFBConditionToAbilities
*/
USTRUCT(BlueprintType)
struct FFBConditionAbilities
{
	GENERATED_BODY()

	FFBConditionAbilities() : ConditionType(EAbilityConditionType::NONE), MinRagne(0.0f), MaxRange(0.0f), ValidRadius(0.0f) {}

	UPROPERTY(EditDefaultsOnly, Category = "BTType")
	EAbilityConditionType ConditionType;

	UPROPERTY(EditDefaultsOnly, Category = "BTType")
	float MinRagne;

	UPROPERTY(EditDefaultsOnly, Category = "BTType")
	float MaxRange;

	UPROPERTY(EditDefaultsOnly, Category = "BTType")
	float ValidRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FFBWeightedAbilityDefinition> WeightedDefinitions;

	FGameplayTag SelectRandomAbilityTag() const
	{
		if (WeightedDefinitions.Num() == 0)
			return FGameplayTag::EmptyTag;

		float TotalWeight = 0.f;
		for (const FFBWeightedAbilityDefinition& Definition : WeightedDefinitions)
		{
			TotalWeight += Definition.SelectionWeight;
		}

		float Random = FMath::FRandRange(0.0f, TotalWeight);
		float CurrentWeight = 0.f;

		for (const FFBWeightedAbilityDefinition& Weighted : WeightedDefinitions)
		{
			CurrentWeight += Weighted.SelectionWeight;
			if (Random <= CurrentWeight)
			{
				return Weighted.Definition.AbilityTag;
			}
		}
		return WeightedDefinitions.Last().Definition.AbilityTag;
	}
};

/**
 * UFBEnemyAbilitySet
 */
UCLASS()
class FOURBLADES_API UFBEnemyAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UFBEnemyAbilitySet();

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FFBConditionAbilities> AbilityList;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FFBAbilityDefinition> HitReactAbilities;

	class UFBAbilityFragmentManager* GetAbilityFragmentManager(const FGameplayTag& InAbilityTag);

	const float GetMinRange(EAbilityConditionType InConditionType) const;

	const float GetMaxRange(EAbilityConditionType InConditionType) const;

	const float GetValidRadius(EAbilityConditionType InConditionType) const;

	FGameplayTag GetAbilityTagWithWeight(EAbilityConditionType InConditionType) const;

	class UFBAbilityFragmentManager* GetReactAbilityFragmentManager(const FGameplayTag& InAbilityTag);
};
