// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameplayTagLevelUtilities.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGameplayTagLevelUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    /**
     * 특정 레벨의 태그를 가져옵니다 (1단계부터 시작)
     * 예: CharacterStat.Health.Level에서 Level=2면 "CharacterStat.Health" 반환
     */
    UFUNCTION(BlueprintCallable, Category = "GameplayTag Utilities", meta = (CallInEditor = "true"))
    static FGameplayTag GetTagAtLevel(const FGameplayTag& InputTag, int32 Level);

    /**
     * 태그의 총 레벨(깊이)을 반환합니다
     * 예: CharacterStat.Health.Level = 3레벨
     */
    UFUNCTION(BlueprintCallable, Category = "GameplayTag Utilities", meta = (CallInEditor = "true"))
    static int32 GetTagDepth(const FGameplayTag& InputTag);

    /**
     * 1단계부터 지정된 레벨까지의 모든 태그를 배열로 반환
     * 예: CharacterStat.Health.Level, MaxLevel=2 -> [CharacterStat, CharacterStat.Health]
     */
    UFUNCTION(BlueprintCallable, Category = "GameplayTag Utilities", meta = (CallInEditor = "true"))
    static TArray<FGameplayTag> GetTagsUpToLevel(const FGameplayTag& InputTag, int32 MaxLevel);

    /**
     * 모든 레벨의 태그를 순서대로 반환 (1단계부터 최대 레벨까지)
     */
    UFUNCTION(BlueprintCallable, Category = "GameplayTag Utilities", meta = (CallInEditor = "true"))
    static TArray<FGameplayTag> GetAllTagLevels(const FGameplayTag& InputTag);

    /**
     * 특정 레벨의 태그 이름(마지막 부분)만 반환
     * 예: CharacterStat.Health.Level에서 Level=2면 "Health" 반환
     */
    UFUNCTION(BlueprintCallable, Category = "GameplayTag Utilities", meta = (CallInEditor = "true"))
    static FString GetTagNameAtLevel(const FGameplayTag& InputTag, int32 Level);

    /**
     * 두 태그가 지정된 레벨까지 동일한지 확인
     * 예: CharacterStat.Health.Level과 CharacterStat.Health.Max를 Level=2로 비교하면 true
     */
    UFUNCTION(BlueprintCallable, Category = "GameplayTag Utilities", meta = (CallInEditor = "true"))
    static bool AreTagsEqualAtLevel(const FGameplayTag& TagA, const FGameplayTag& TagB, int32 Level);
};
