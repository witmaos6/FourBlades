// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/FBCombatModeSaveData.h"
#include "Kismet/GameplayStatics.h"
#include "FourBlades.h"

UFBCombatModeSaveData::UFBCombatModeSaveData()
{
	SaveUserFacingExperienceId = FPrimaryAssetId(FPrimaryAssetType("FBUserFacingExperience"), FName("DA_ActionCore_ActionGym"));
	// PlayMode를 사용할 때 시작하는 맵과 Experience가 정해져있고 이것이 UserFacingExperinece에 정의되어 있기 때문에 이를 사용한다.
}

void UFBCombatModeSaveData::AddXP(const uint32 Amount)
{
	uint32 CurrXP = CharacterProfile.CharacterXP;
	CharacterProfile.CharacterXP += Amount;

	if (OnCharacterXPChanged.IsBound())
	{
		OnCharacterXPChanged.Broadcast(CurrXP, CharacterProfile.CharacterXP);
	}
}

bool UFBCombatModeSaveData::PossibleLevelUp() const
{
	return (CharacterProfile.CharacterXP >= 10 && CharacterProfile.CharacterLevel < 10); // 임시로 10마다 레벨 업 // 만렙은 10로 임시 설정
}

void UFBCombatModeSaveData::LevelUp() // 현재 만렙은 10
{
	if (CharacterProfile.CharacterXP >= 10)
	{
		uint32 CurrXP = CharacterProfile.CharacterXP;
		uint8 CurrentLevel = CharacterProfile.CharacterLevel;
		uint8 IncreaseLevel = (CharacterProfile.CharacterXP / 10);
		uint8 NextLevel = FMath::Min(CurrentLevel + IncreaseLevel, 10);
		if (CurrentLevel == NextLevel)
			return;

		CharacterProfile.CharacterLevel = NextLevel;
		FB_FUNC_LOG(LogTemp, Log, TEXT("Current CharacterLevel: %u"), CharacterProfile.CharacterLevel);
		
		uint32 IncreaseStatPoint = NextLevel - CurrentLevel;
		CharacterProfile.CharacterStat.RemainingLevelUpStatPoints += IncreaseStatPoint;
		CharacterProfile.CharacterStat.TotalLevelUpStatPoints += IncreaseStatPoint;
		FB_FUNC_LOG(LogTemp, Log, TEXT("Current CharacterStatPoint: %u"), CharacterProfile.CharacterStat.RemainingLevelUpStatPoints);

		CharacterProfile.CharacterXP -= (10 * IncreaseLevel);

		if (OnCharacterLevelUp.IsBound() && CurrentLevel < NextLevel)
		{
			OnCharacterLevelUp.Broadcast(CurrentLevel, NextLevel);
		}
		if (OnCharacterXPChanged.IsBound())
		{
			OnCharacterXPChanged.Broadcast(CurrXP, CharacterProfile.CharacterXP);
		}
	}
}

void UFBCombatModeSaveData::UpdateStatSaveData(const FFBStatSaveData& InStatSaveData)
{
	CharacterProfile.CharacterStat = InStatSaveData;
	if (OnCharacterStatChanged.IsBound())
	{
		OnCharacterStatChanged.Broadcast(InStatSaveData);
	}
}

void UFBCombatModeSaveData::SetSavePoint(const FVector& SaveLocation, const FRotator& SaveRotation)
{
	SavePoint.Location = SaveLocation;
	SavePoint.Rotator = SaveRotation;
}

const FFBSavePoint& UFBCombatModeSaveData::GetSavePoint() const
{
	return SavePoint;
}
