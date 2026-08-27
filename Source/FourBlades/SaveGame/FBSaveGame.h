// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FBSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBSaveGame : public USaveGame
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "SaveGame")
	FPrimaryAssetId SaveUserFacingExperienceId;

public:
	void SetUserFacingExperienceId(const FPrimaryAssetId InSaveUserFacingExperienceId) {SaveUserFacingExperienceId = InSaveUserFacingExperienceId;}

	FPrimaryAssetId GetUserFacingExperienceId() const { return SaveUserFacingExperienceId; }

private:
	UPROPERTY()
	FString SlotName;

public:
	void SetSlotName(const FString& InSlotName) { SlotName = InSlotName; }

	bool ValidSlotName() const { return !SlotName.IsEmpty(); }

	const FString& GetSlotName() const { return SlotName; }
};
