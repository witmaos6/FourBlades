// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilitySystem/FBGameplayEffectContext.h"
#include "FBGameplayEffectApplicator.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFBGameplayEffectApplicator : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FOURBLADES_API IFBGameplayEffectApplicator
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void InitDamageExecutionParams(FFBDamageExecutionParams&& InDamageExecutionParams) = 0;

	virtual void InitTargetPayload(FFBTargetPayload&& InTargetPayload) = 0;
};
