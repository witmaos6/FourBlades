// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FBGameplayAbility.h"
#include "FBEnemyGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBEnemyGameplayAbility : public UFBGameplayAbility
{
	GENERATED_BODY()
public:
	UFBEnemyGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	AActor* GetCombatTargetActor() const;

	APawn* GetCombatTargetPawn() const;
};
