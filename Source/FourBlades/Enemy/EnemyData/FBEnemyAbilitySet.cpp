// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyData/FBEnemyAbilitySet.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "FourBlades.h"

UFBEnemyAbilitySet::UFBEnemyAbilitySet()
{
}

UFBAbilityFragmentManager* UFBEnemyAbilitySet::GetAbilityFragmentManager(const FGameplayTag& InAbilityTag)
{
	if (!InAbilityTag.IsValid())
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("AbilityTag is invalid"));
		return nullptr;
	}
	if (AbilityList.Num() == 0)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("AbilityList is null"));
		return nullptr;
	}
	for (const FFBConditionAbilities& ConditionAbilities : AbilityList)
	{
		for (const FFBWeightedAbilityDefinition& AbilityDefinition : ConditionAbilities.WeightedDefinitions)
		{
			if (AbilityDefinition.Definition.AbilityTag == InAbilityTag)
			{
				return AbilityDefinition.Definition.FragmentManager;
			}
		}
	}
	return nullptr;
}

const float UFBEnemyAbilitySet::GetMinRange(EAbilityConditionType InConditionType) const
{
	for (const FFBConditionAbilities& ConditionAbilities : AbilityList)
	{
		if (ConditionAbilities.ConditionType == InConditionType)
			return ConditionAbilities.MinRagne;
	}
	return -1.0f;
}

const float UFBEnemyAbilitySet::GetMaxRange(EAbilityConditionType InConditionType) const
{
	for (const FFBConditionAbilities& ConditionAbilities : AbilityList)
	{
		if (ConditionAbilities.ConditionType == InConditionType)
			return ConditionAbilities.MaxRange;
	}
	return -1.0f;
}

const float UFBEnemyAbilitySet::GetValidRadius(EAbilityConditionType InConditionType) const
{
	for (const FFBConditionAbilities& ConditionAbilities : AbilityList)
	{
		if (ConditionAbilities.ConditionType == InConditionType)
			return ConditionAbilities.ValidRadius;
	}
	return -1.0f;
}

FGameplayTag UFBEnemyAbilitySet::GetAbilityTagWithWeight(EAbilityConditionType InConditionType) const
{
	for (const FFBConditionAbilities& ConditionAbilities : AbilityList)
	{
		if (ConditionAbilities.ConditionType == InConditionType)
		{
			return ConditionAbilities.SelectRandomAbilityTag();
		}
	}
	return FGameplayTag::EmptyTag;
}

UFBAbilityFragmentManager* UFBEnemyAbilitySet::GetReactAbilityFragmentManager(const FGameplayTag& InAbilityTag)
{
	for (const FFBAbilityDefinition& HitReactAbility : HitReactAbilities)
	{
		if (HitReactAbility.AbilityTag == InAbilityTag)
		{
			return HitReactAbility.FragmentManager;
		}
	}
	return nullptr;
}
