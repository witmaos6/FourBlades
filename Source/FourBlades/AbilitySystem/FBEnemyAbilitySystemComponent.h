// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FBEnemyAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBEnemyAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UFBEnemyAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void EnforceSingleStateTag();

protected:
	void OnEnemyStateTagChanged(const FGameplayTag Tag, int32 NewCount);

	void SetEnemyStateIdle();
};
