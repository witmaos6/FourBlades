// Fill out your copyright notice in the Description page of Project Settings.


#include "System/EnemyDataSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/FBAssetManager.h"
#include "TagToPrimaryAssetId.h"
#include "GameDataFromExcel/FBTagGameDataTable.h"
#include "GameDataFromExcel/GameplayTagDataTable.h"
#include "GameDataFromExcel/FBGameDataTable.h"
#include "GameDataFromExcel/FBEnemyDataRow.h"
#include "GameDataFromExcel/FBEnemyStatFromLevelRow.h"
#include "FourBlades.h"

UEnemyDataSubsystem::UEnemyDataSubsystem()
{

}

void UEnemyDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

    InitializeEnemyDataTable();

    InitializeEnemyStatFromLevel();

	InitializeEnemyPawnData();

	InitializeEnemyAbilities();
}

void UEnemyDataSubsystem::InitializeEnemyDataTable()
{
    TArray<FPrimaryAssetId> TagMappingAssets;
    UKismetSystemLibrary::GetPrimaryAssetIdList(FPrimaryAssetType(FName("EnemyKey")), TagMappingAssets);

    for (const FPrimaryAssetId& AssetId : TagMappingAssets)
    {
        UFBTagGameDataTable* TagMappingAsset = UFBAssetManager::Get().LoadPrimaryAssetSync<UFBTagGameDataTable>(AssetId);
        if (TagMappingAsset)
        {
            if (TagMappingAsset->DataTable)
            {
                EnemyDataTable = TagMappingAsset->DataTable.GetDefaultObject();
                FB_FUNC_LOG(LogTemp, Log, TEXT("Successfully processed data table: %s"), *TagMappingAsset->GetName());
                break;
                // 현재는 DataTable을 하나만 사용하기 때문에 하나가 등록되면 중지한다.
            }
        }
        else
        {
            FB_FUNC_LOG(LogTemp, Error, TEXT("Failed to load data table: %s"), *AssetId.ToString());
        }
    }
}

void UEnemyDataSubsystem::InitializeEnemyStatFromLevel()
{
    TArray<FPrimaryAssetId> TagMappingAssets;
    UKismetSystemLibrary::GetPrimaryAssetIdList(FPrimaryAssetType(FName("EnemyStatLevel")), TagMappingAssets);

    for (const FPrimaryAssetId& AssetId : TagMappingAssets)
    {
        UFBGameDataTable* TagMappingAsset = UFBAssetManager::Get().LoadPrimaryAssetSync<UFBGameDataTable>(AssetId);
        if (TagMappingAsset)
        {
            if (TagMappingAsset->DataTable)
            {
                EnemyStatFromLevel = TagMappingAsset->DataTable;
                FB_FUNC_LOG(LogTemp, Log, TEXT("Successfully processed data table: %s"), *TagMappingAsset->GetName());
                break;
                // 현재는 DataTable을 하나만 사용하기 때문에 하나가 등록되면 중지한다.
            }
        }
        else
        {
            FB_FUNC_LOG(LogTemp, Error, TEXT("Failed to load data table: %s"), *AssetId.ToString());
        }
    }
}

void UEnemyDataSubsystem::InitializeEnemyPawnData()
{
    FString AssetType = TEXT("EnemyPawn");
	LoadAndProcessTagMappingAssets(AssetType, TagToEnemyPawnData);
}

void UEnemyDataSubsystem::InitializeEnemyAbilities()
{
    FString AssetType = TEXT("EnemyAbilities");
	LoadAndProcessTagMappingAssets(AssetType, TagToEnemyAbilities);
}

void UEnemyDataSubsystem::LoadAndProcessTagMappingAssets(const FString& AssetType, TMap<FGameplayTag, FPrimaryAssetId>& TargetMap)
{
    TArray<FPrimaryAssetId> TagMappingAssets;
    UKismetSystemLibrary::GetPrimaryAssetIdList(FPrimaryAssetType(FName(*AssetType)), TagMappingAssets);

    for (const FPrimaryAssetId& AssetId : TagMappingAssets)
    {
        UTagToPrimaryAssetId* TagMappingAsset = UFBAssetManager::Get().LoadPrimaryAssetSync<UTagToPrimaryAssetId>(AssetId);
        if (TagMappingAsset)
        {
            for (const FTaggedPrimaryAssetId& TaggedAsset : TagMappingAsset->TaggedAssetList)
            {
                if (!TaggedAsset.MappingTag.IsValid())
                {
                    FB_FUNC_LOG(LogTemp, Warning, TEXT("Invalid MappingTag found in %s"), *TagMappingAsset->GetName());
                    continue;
                }
                TargetMap.Add(TaggedAsset.MappingTag, TaggedAsset.PrimaryAssetId);

                FB_FUNC_LOG(LogTemp, Log, TEXT("Added %s mapping: %s -> %s"), *AssetType, *TaggedAsset.MappingTag.ToString(), *TaggedAsset.PrimaryAssetId.ToString());
            }
            FB_FUNC_LOG(LogTemp, Log, TEXT("Successfully processed %s asset: %s"), *AssetType, *TagMappingAsset->GetName());
        }
        else
        {
            FB_FUNC_LOG(LogTemp, Error, TEXT("Failed to load %s asset: %s"), *AssetType, *AssetId.ToString());
        }
    }
}

uint8 UEnemyDataSubsystem::GetEnemyLevel(const FGameplayTag& EnemyKeyTag)
{
    if (FFBEnemyDataRow* EnemyDataRow = EnemyDataTable->FindRowByGameplayTag<FFBEnemyDataRow>(EnemyKeyTag))
    {
        return EnemyDataRow->EnemyLevel;
    }
    return -1;
}

FGameplayTag UEnemyDataSubsystem::GetEnemyPawnDataKey(const FGameplayTag& EnemyKeyTag)
{
    if (FFBEnemyDataRow* EnemyDataRow = EnemyDataTable->FindRowByGameplayTag<FFBEnemyDataRow>(EnemyKeyTag))
    {
        FGameplayTag EnemyPawnDataTag = FGameplayTag::RequestGameplayTag(EnemyDataRow->EnemyPawnDataKey);
        if(EnemyPawnDataTag.IsValid())
            return EnemyPawnDataTag;
    }
    return FGameplayTag::EmptyTag;
}

FGameplayTag UEnemyDataSubsystem::GetEnemyAbilitiesDataKey(const FGameplayTag& EnemyKeyTag)
{
    if (FFBEnemyDataRow* EnemyDataRow = EnemyDataTable->FindRowByGameplayTag<FFBEnemyDataRow>(EnemyKeyTag))
    {
        FGameplayTag EnemyAbilitiesDataTag = FGameplayTag::RequestGameplayTag(EnemyDataRow->AbilitiesKey);
        if (EnemyAbilitiesDataTag.IsValid())
            return EnemyAbilitiesDataTag;
    }
    return FGameplayTag::EmptyTag;
}

uint32 UEnemyDataSubsystem::GetEnemyXP(const FGameplayTag& EnemyKeyTag)
{
    if (FFBEnemyDataRow* EnemyDataRow = EnemyDataTable->FindRowByGameplayTag<FFBEnemyDataRow>(EnemyKeyTag))
    {
        return EnemyDataRow->XPReward;
    }
    return 0;
}

FFBEnemyStatFromLevelRow* UEnemyDataSubsystem::GetEnemyStatFromLevelRow(uint8 Level)
{
    FString LevelString = FString::Printf(TEXT("%u"), Level);
    FName LevelName = FName(LevelString);

    if (FFBEnemyStatFromLevelRow* EnemyStatRow = EnemyStatFromLevel->FindRow<FFBEnemyStatFromLevelRow>(LevelName, FString::Printf(TEXT("GetEnemyStatFromLevelRow"))))
    {
        return EnemyStatRow;
    }
    return nullptr;
}

void UEnemyDataSubsystem::GetEnemyPawnDatas(const FGameplayTagContainer& EnemyPawnTags, TArray<FPrimaryAssetId>& Array)
{
    for (const FGameplayTag& Tag : EnemyPawnTags)
    {
        if (Tag.IsValid())
        {
            if (TagToEnemyPawnData.Contains(Tag))
            {
                Array.AddUnique(TagToEnemyPawnData[Tag]);
            }
        }
    }
}

void UEnemyDataSubsystem::GetEnemyAbilities(const FGameplayTagContainer& EnemyAbilitiesTags, TArray<FPrimaryAssetId>& Array)
{
    for (const FGameplayTag& Tag : EnemyAbilitiesTags)
    {
        if (Tag.IsValid())
        {
            if (TagToEnemyAbilities.Contains(Tag))
            {
                Array.AddUnique(TagToEnemyAbilities[Tag]);
            }
        }
    }
}

FPrimaryAssetId UEnemyDataSubsystem::GetEnemyPawnDataAssetId(const FGameplayTag& Tag)
{
    if (Tag.IsValid())
    {
        if (TagToEnemyPawnData.Contains(Tag))
        {
            return TagToEnemyPawnData[Tag];
        }
    }
    return FPrimaryAssetId();
}

FPrimaryAssetId UEnemyDataSubsystem::GetEnemyPawnDataAssetIdFromEnemyKey(const FGameplayTag& EnemyKeyTag)
{
    FGameplayTag EnemyPawnDataKey = GetEnemyPawnDataKey(EnemyKeyTag);
    if (EnemyPawnDataKey.IsValid())
    {
        FPrimaryAssetId AssetId = GetEnemyPawnDataAssetId(EnemyPawnDataKey);
        if (AssetId.IsValid())
            return AssetId;
    }
    return FPrimaryAssetId();
}

FPrimaryAssetId UEnemyDataSubsystem::GetEnemyAbilityDataAssetId(const FGameplayTag& Tag)
{
    if (Tag.IsValid())
    {
        if (TagToEnemyAbilities.Contains(Tag))
        {
            return TagToEnemyAbilities[Tag];
        }
    }
    return FPrimaryAssetId();
}

FPrimaryAssetId UEnemyDataSubsystem::GetEnemyAbilitiesDataAssetIdFromEnemyKey(const FGameplayTag& EnemyKeyTag)
{
    FGameplayTag EnemyAbilitiesKey = GetEnemyAbilitiesDataKey(EnemyKeyTag);
    if (EnemyAbilitiesKey.IsValid())
    {
        FPrimaryAssetId AssetId = GetEnemyAbilityDataAssetId(EnemyAbilitiesKey);
        if (AssetId.IsValid())
            return AssetId;
    }
    return FPrimaryAssetId();
}
