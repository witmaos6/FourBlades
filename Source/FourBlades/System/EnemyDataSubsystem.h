// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Engine/AssetManager.h"
#include "EnemyDataSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UEnemyDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UEnemyDataSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	void InitializeEnemyDataTable();

	void InitializeEnemyStatFromLevel();

	void InitializeEnemyPawnData();

	void InitializeEnemyAbilities();

	void LoadAndProcessTagMappingAssets(const FString& AssetType, TMap<FGameplayTag, FPrimaryAssetId>& TargetMap);

private: // EnemyDataTable
	UPROPERTY()
	TObjectPtr<class UGameplayTagDataTable> EnemyDataTable;
	// 추후에 DataTable을 여러개로 분리할 시에는 조정이 필요하다. 현재는 하나만 사용하기 때문에 문제 없다.
public:
	uint8 GetEnemyLevel(const FGameplayTag& EnemyKeyTag);

	FGameplayTag GetEnemyPawnDataKey(const FGameplayTag& EnemyKeyTag);

	FGameplayTag GetEnemyAbilitiesDataKey(const FGameplayTag& EnemyKeyTag);

	uint32 GetEnemyXP(const FGameplayTag& EnemyKeyTag);

private: // EnemyStat From Level
	UPROPERTY()
	TObjectPtr<UDataTable> EnemyStatFromLevel;
public:
	struct FFBEnemyStatFromLevelRow* GetEnemyStatFromLevelRow(uint8 Level);

private:
	UPROPERTY()
	TMap<FGameplayTag, FPrimaryAssetId> TagToEnemyPawnData;

	UPROPERTY()
	TMap<FGameplayTag, FPrimaryAssetId> TagToEnemyAbilities;

public:
	void GetEnemyPawnDatas(const FGameplayTagContainer& EnemyPawnTags, TArray<FPrimaryAssetId>& Array);

	void GetEnemyAbilities(const FGameplayTagContainer& EnemyAbilitiesTags, TArray<FPrimaryAssetId>& Array);

	FPrimaryAssetId GetEnemyPawnDataAssetId(const FGameplayTag& Tag);

	FPrimaryAssetId GetEnemyPawnDataAssetIdFromEnemyKey(const FGameplayTag& EnemyKeyTag);

	FPrimaryAssetId GetEnemyAbilityDataAssetId(const FGameplayTag& Tag);

	FPrimaryAssetId GetEnemyAbilitiesDataAssetIdFromEnemyKey(const FGameplayTag& EnemyKeyTag);
};