// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FBSlotNameManager.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFBSlotNameManager : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FOURBLADES_API IFBSlotNameManager // To do: FBSaveGame으로 래핑할 때 여기서 관리하는 것들은 이전한다.
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetSlotName(const FString& InSlotName) = 0;

	virtual bool ValidSlotName() const = 0;

	virtual FString GetSlotName() const = 0;

	virtual FPrimaryAssetId GetUserFacingExperienceId() const = 0;
};
