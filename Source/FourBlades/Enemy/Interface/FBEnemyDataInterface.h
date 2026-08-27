// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enemy/EnemyData/EnemyPawnData.h"
#include "Enemy/EnemyData/FBEnemyAbilitySet.h"
#include "FBEnemyDataInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFBEnemyDataInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FOURBLADES_API IFBEnemyDataInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual class UFBAbilityFragmentManager* GetAbilityFragmentManager(const FGameplayTag& InAbilityTag) = 0;

	virtual const float GetAbilityActivateMinRange(EAbilityConditionType InConditionType) = 0;

	virtual const float GetAbilityActivateMaxRange(EAbilityConditionType InConditionType) = 0;

	virtual const float GetAbilityActivateValidRadius(EAbilityConditionType InConditionType) = 0;

	virtual FGameplayTag GetAbilityTagWithWeight(EAbilityConditionType InConditionType) = 0;

public:
	virtual class UFBAbilityFragmentManager* GetReactAbilityFragmentManager(const FGameplayTag& InAbilityTag) = 0;
};
