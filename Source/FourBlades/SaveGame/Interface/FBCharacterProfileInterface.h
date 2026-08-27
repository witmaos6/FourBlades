// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveGame/FBSaveGameTypes.h"
#include "FBCharacterProfileInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFBCharacterProfileInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FOURBLADES_API IFBCharacterProfileInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual const EFBCharacterJob GetCharacterJob() const = 0;

	virtual const uint8 GetCharacterLevel() const = 0;

	virtual const uint32 GetCurrentXP() const = 0;

	virtual uint32 GetXPRequiredForNextLevel() const = 0;

	virtual void AddXP(const uint32 Amount) = 0;

	virtual bool PossibleLevelUp() const = 0;

	virtual void LevelUp() = 0;

	virtual FOnCharacterXPChanged& GetXPChangeDelegate() = 0;

	virtual FOnCharacterLevelUp& GetLevelUpDelegate() = 0;

// StatSaveData
	virtual FOnCharacterStatChanged& GetCharacterStatChanged() = 0;

	virtual const uint32 GetRemainStatPoints() const = 0;

	virtual const uint32 GetTotalLevelUpStatPoints() const = 0;

	virtual const FFBStatSaveData& GetStatSaveData() const = 0;

	virtual void UpdateStatSaveData(const FFBStatSaveData& InStatSaveData) = 0;
};
