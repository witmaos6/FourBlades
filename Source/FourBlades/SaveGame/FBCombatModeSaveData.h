// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGame/FBSaveGame.h"
#include "SaveGame/FBSaveGameTypes.h"
#include "Interface/FBCharacterProfileInterface.h"
#include "Interface/FBSavePointInterface.h"
#include "FBCombatModeSaveData.generated.h"

/**
 * CombatModeSaveData
 */
UCLASS()
class FOURBLADES_API UFBCombatModeSaveData : public UFBSaveGame, public IFBCharacterProfileInterface, public IFBSavePointInterface
{
	GENERATED_BODY()
public:
	UFBCombatModeSaveData();

public:
	/*UPROPERTY() // 필요하면 사용
	TArray<FString> RequiredGameFeaturePlugins;*/

	FOnCharacterLevelUp OnCharacterLevelUp;

	FOnCharacterStatChanged OnCharacterStatChanged;

	FOnCharacterXPChanged OnCharacterXPChanged;

	virtual FOnCharacterLevelUp& GetLevelUpDelegate() override { return OnCharacterLevelUp; }

	virtual FOnCharacterXPChanged& GetXPChangeDelegate() override { return OnCharacterXPChanged; }

public:
	UPROPERTY(SaveGame)
	FFBCharacterProfile CharacterProfile;

	virtual const EFBCharacterJob GetCharacterJob() const override { return CharacterProfile.CharacterJob; }

	virtual const uint8 GetCharacterLevel() const override { return CharacterProfile.CharacterLevel; }

	virtual const uint32 GetCurrentXP() const override { return CharacterProfile.CharacterXP; }

	virtual uint32 GetXPRequiredForNextLevel() const override { return 10; }

	void AddXP(const uint32 Amount) override;

	virtual bool PossibleLevelUp() const override;

	virtual void LevelUp() override;

public: // StatSaveData
	virtual FOnCharacterStatChanged& GetCharacterStatChanged() override { return OnCharacterStatChanged; }

	virtual const uint32 GetRemainStatPoints() const override { return CharacterProfile.CharacterStat.RemainingLevelUpStatPoints; }

	virtual const uint32 GetTotalLevelUpStatPoints() const override { return CharacterProfile.CharacterStat.TotalLevelUpStatPoints; }

	virtual const FFBStatSaveData& GetStatSaveData() const override { return CharacterProfile.CharacterStat; }

	virtual void UpdateStatSaveData(const FFBStatSaveData& InStatSaveData) override;

public:
	UPROPERTY(SaveGame)
	FFBSavePoint SavePoint;

	virtual void SetSavePoint(const FVector& SaveLocation, const FRotator& SaveRotation) override;

	virtual const FFBSavePoint& GetSavePoint() const override;
};
