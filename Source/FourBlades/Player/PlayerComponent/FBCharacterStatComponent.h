// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerStateComponent.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "GameFramework/SaveGame.h"
#include "AttributeSet.h"
#include "FBCharacterStatComponent.generated.h"

/*
* StatModifierEntry
*/
USTRUCT(BlueprintType)
struct FFBStatModifierEntry
{
	GENERATED_BODY()

	FFBStatModifierEntry() : Value(0.0f) {}

	FFBStatModifierEntry(const FGameplayTag& InStatIdTag, float InValue)
		: StatIdTag(InStatIdTag), Value(InValue) {
	}

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag StatIdTag;

	UPROPERTY(BlueprintReadWrite)
	float Value;
};

/*
* CharacterStatEntry
*/
USTRUCT(BlueprintType)
struct FFBCharacterStatEntry
{
	GENERATED_BODY()

	FFBCharacterStatEntry() {}

	FFBCharacterStatEntry(const FGameplayTag& InParentTag);

	bool IsValidSourceTag(const FGameplayTag& SourceTag) const;

	bool SetStatValue(const FFBStatModifierEntry& InModifierEntry);

	const float GetTotalStat() const;

private:
	UPROPERTY()
	TMap<FGameplayTag, float> StatValues;

	UPROPERTY()
	FGameplayTag ParentStatTag;

	UPROPERTY()
	FGameplayTagContainer AllowedSourceTags;
	/*UPROPERTY(EditDefaultsOnly, Category = Tag) // 직업 계수 정보를 불러오기 위한 태그다. 아직은 사용하지 않는다.
	FGameplayTag JobCoefficientTag;*/
};

/*
* CharacterStatTable
*/
USTRUCT()
struct FFBCharacterStatTable
{
	GENERATED_BODY()

	FFBCharacterStatTable()
	{
		AllowedSourceTags.AddTag(FBTAG_CHARACTERSTAT_ATTACKPOWER);
		AllowedSourceTags.AddTag(FBTAG_CHARACTERSTAT_HEALTH);
		AllowedSourceTags.AddTag(FBTAG_CHARACTERSTAT_DEFENSE);
	}

	void SetStatEntry(const FFBStatModifierEntry& InModifierEntry);

	const float GetTotalStat(const FGameplayTag& StatTag);

	const float GetLastTotalStat(const FGameplayTag& StatTag);

private:
	UPROPERTY()
	TMap<FGameplayTag, FFBCharacterStatEntry> CharacterStatEntries;

	UPROPERTY()
	TMap<FGameplayTag, float> LastTotalStat;

	UPROPERTY()
	FGameplayTagContainer AllowedSourceTags;

	bool IsValidEntryTag(const FGameplayTag& StatTag) const;

	bool IsValidSourceTag(const FGameplayTag& StatTag) const;
};

DECLARE_MULTICAST_DELEGATE(FOnUpdateCharacterStat);

/**
 * 
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class FOURBLADES_API UFBCharacterStatComponent : public UPlayerStateComponent
{
	GENERATED_BODY()
public:
	UFBCharacterStatComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	UPROPERTY()
	TObjectPtr<class UFBSaveGame> SaveData; // SaveData대신 인터페이스를 캐싱해야 할 수도 있다.
	
protected:
	virtual void OnRegister() override;

	void InitializeAttributeMapping();

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	FFBCharacterStatTable CharacterStatTable;

	UPROPERTY()
	TMap<FGameplayTag, FGameplayAttribute> StatTagToAttributeMap;

	UPROPERTY(EditDefaultsOnly, Category = Tag)
	FGameplayTag CharacterStatByLevelTag;

	void InitializeStats();

private:
	FGameplayTagContainer GetRequireTags();

	void InitializeCharacterStat();

	void InitializeAttributeSet();

	void UpdateCharacterStat(const struct FFBStatSaveData& InStatSaveData);

	void UpdateAllocatedStatPoint(const struct FFBStatSaveData& InStatSaveData);

	void UpdateAttributeSet();

public:
	FOnUpdateCharacterStat OnUpdateCharacterStat;
	// To do: FBCharacterStat의 변동이 있을 때마다 콜백함수를 통해 AttributeSet 업데이트

public:
	const float GetLastTotalStat(const FGameplayTag& StatTag);

public:
	void AddXP(const uint32 Amount); // XP를 업데이트하는 부분을 FBCharacterXPComponent로 이전하거나 PlayerState로 이전하는 것이 좋을수도 있다.

	void LevelUp();

	void Resurrection();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};