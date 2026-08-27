// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FBInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFBInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class UFBAbilitySet;

/**
 * 
 */
class FOURBLADES_API IFBInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UFBAbilitySet* GetAbilitySet() const = 0;
};
