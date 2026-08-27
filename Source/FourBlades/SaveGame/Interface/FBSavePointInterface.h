// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveGame/FBSaveGameTypes.h"
#include "FBSavePointInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFBSavePointInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FOURBLADES_API IFBSavePointInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetSavePoint(const FVector& SaveLocation, const FRotator& SaveRotation) = 0;

	virtual const FFBSavePoint& GetSavePoint() const = 0;
};
