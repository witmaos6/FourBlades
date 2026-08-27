#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "FBSaveGameTypes.generated.h"

/*
* Delegates
*/
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterLevelUp, const uint8 InOldLevel, const uint8 InNewLevel);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterStatChanged, const FFBStatSaveData& ChangedStatSaveData);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterXPChanged, const uint32 InOldXP, const uint32 InNewXP);

/*
* UserPlayMode
*/
UENUM(BlueprintType)
enum class EFBPlayMode : uint8 // PlayMode의 순서를 신경써야 한다.
{ 
	CombatMode,
	// Story,
};

/*
* FBCharacterJob
*/
UENUM(BlueprintType)
enum class EFBCharacterJob : uint8 // 캐릭터의 체력, 공격력 등의 계수들을 Table로부터 가져오기 위해서 enum class 사용
{
	Hwando = 0,
	Battleax = 1,
	Lance = 2,
	Dualblade = 3,
};

/*
* StatSaveData
*/
USTRUCT()
struct FFBStatSaveData
{
	GENERATED_BODY()
public:
	FFBStatSaveData()
	{
		RemainingLevelUpStatPoints = 0;
		TotalLevelUpStatPoints = 0;
		AllocatedLevelUpStats.Add(FBTAG_CHARACTERSTAT_ATTACKPOWER_ALLOCATEDPOINT, 0);
		AllocatedLevelUpStats.Add(FBTAG_CHARACTERSTAT_DEFENSE_ALLOCATEDPOINT, 0);
		AllocatedLevelUpStats.Add(FBTAG_CHARACTERSTAT_HEALTH_ALLOCATEDPOINT, 0);
	}
	UPROPERTY(SaveGame)
	uint32 TotalLevelUpStatPoints;

	UPROPERTY(SaveGame)
	uint32 RemainingLevelUpStatPoints;

	UPROPERTY(SaveGame)
	TMap<FGameplayTag, uint32> AllocatedLevelUpStats;
};

/*
* CharacterProfile
*/
USTRUCT()
struct FFBCharacterProfile
{
	GENERATED_BODY()

	FFBCharacterProfile()
	{
		CharacterJob = EFBCharacterJob::Hwando;
		CharacterLevel = 1;
		CharacterXP = 0;
	}

	UPROPERTY(SaveGame)
	EFBCharacterJob CharacterJob;

	UPROPERTY(SaveGame)
	uint8 CharacterLevel;

	UPROPERTY(SaveGame)
	uint32 CharacterXP;

	UPROPERTY(SaveGame)
	FFBStatSaveData CharacterStat;
};

/*
* SavePoint
*/
USTRUCT()
struct FFBSavePoint
{
	GENERATED_BODY()

	FFBSavePoint()
	{
		Location = FVector::ZeroVector;
		Rotator = FRotator::ZeroRotator;
	}

	UPROPERTY(SaveGame)
	FVector Location;

	UPROPERTY(SaveGame)
	FRotator Rotator;
};
