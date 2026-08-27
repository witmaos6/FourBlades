// Fill out your copyright notice in the Description page of Project Settings.


#include "Library/GameplayTagLevelUtilities.h"
#include "GameplayTagsManager.h"

FGameplayTag UGameplayTagLevelUtilities::GetTagAtLevel(const FGameplayTag& InputTag, int32 Level)
{
    if (!InputTag.IsValid() || Level < 1)
    {
        return FGameplayTag::EmptyTag;
    }

    FString TagString = InputTag.ToString();
    TArray<FString> TagParts;
    TagString.ParseIntoArray(TagParts, TEXT("."));

    // 요청된 레벨이 태그의 깊이를 초과하면 빈 태그 반환
    if (Level > TagParts.Num())
    {
        return FGameplayTag::EmptyTag;
    }

    // 지정된 레벨까지의 태그 문자열 구성
    FString ResultTagString = TagParts[0];
    for (int32 i = 1; i < Level; i++)
    {
        ResultTagString += TEXT(".") + TagParts[i];
    }

    // GameplayTag로 변환하여 반환
    return FGameplayTag::RequestGameplayTag(FName(*ResultTagString));
}

int32 UGameplayTagLevelUtilities::GetTagDepth(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid())
    {
        return 0;
    }

    FString TagString = InputTag.ToString();
    TArray<FString> TagParts;
    TagString.ParseIntoArray(TagParts, TEXT("."));

    return TagParts.Num();
}

TArray<FGameplayTag> UGameplayTagLevelUtilities::GetTagsUpToLevel(const FGameplayTag& InputTag, int32 MaxLevel)
{
    TArray<FGameplayTag> ResultTags;

    if (!InputTag.IsValid() || MaxLevel < 1)
    {
        return ResultTags;
    }

    FString TagString = InputTag.ToString();
    TArray<FString> TagParts;
    TagString.ParseIntoArray(TagParts, TEXT("."));

    // MaxLevel을 실제 태그 깊이로 제한
    int32 ActualMaxLevel = FMath::Min(MaxLevel, TagParts.Num());

    // 1단계부터 MaxLevel까지 태그 생성
    FString CurrentTagString = TagParts[0];
    ResultTags.Add(FGameplayTag::RequestGameplayTag(FName(*CurrentTagString)));

    for (int32 i = 1; i < ActualMaxLevel; i++)
    {
        CurrentTagString += TEXT(".") + TagParts[i];
        ResultTags.Add(FGameplayTag::RequestGameplayTag(FName(*CurrentTagString)));
    }

    return ResultTags;
}

TArray<FGameplayTag> UGameplayTagLevelUtilities::GetAllTagLevels(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid())
    {
        return TArray<FGameplayTag>();
    }

    int32 MaxDepth = GetTagDepth(InputTag);
    return GetTagsUpToLevel(InputTag, MaxDepth);
}

FString UGameplayTagLevelUtilities::GetTagNameAtLevel(const FGameplayTag& InputTag, int32 Level)
{
    if (!InputTag.IsValid() || Level < 1)
    {
        return FString();
    }

    FString TagString = InputTag.ToString();
    TArray<FString> TagParts;
    TagString.ParseIntoArray(TagParts, TEXT("."));

    if (Level > TagParts.Num())
    {
        return FString();
    }

    return TagParts[Level - 1]; // Level은 1부터 시작하므로 -1
}

bool UGameplayTagLevelUtilities::AreTagsEqualAtLevel(const FGameplayTag& TagA, const FGameplayTag& TagB, int32 Level)
{
    FGameplayTag LevelTagA = GetTagAtLevel(TagA, Level);
    FGameplayTag LevelTagB = GetTagAtLevel(TagB, Level);

    return LevelTagA.IsValid() && LevelTagB.IsValid() && LevelTagA.MatchesTagExact(LevelTagB);
}

// 사용 예제 함수들
void ExampleUsage()
{
    // 예제 태그 생성 (실제로는 GameplayTag 에셋에서 정의되어야 함)
    FGameplayTag ExampleTag = FGameplayTag::RequestGameplayTag(FName("CharacterStat.Health.Level"));

    // 1. 특정 레벨의 태그 가져오기
    FGameplayTag Level1Tag = UGameplayTagLevelUtilities::GetTagAtLevel(ExampleTag, 1); // "CharacterStat"
    FGameplayTag Level2Tag = UGameplayTagLevelUtilities::GetTagAtLevel(ExampleTag, 2); // "CharacterStat.Health"

    // 2. 태그 깊이 확인
    int32 Depth = UGameplayTagLevelUtilities::GetTagDepth(ExampleTag); // 3

    // 3. 지정된 레벨까지의 모든 태그 가져오기
    TArray<FGameplayTag> TagsUpToLevel2 = UGameplayTagLevelUtilities::GetTagsUpToLevel(ExampleTag, 2);
    // 결과: ["CharacterStat", "CharacterStat.Health"]

    // 4. 모든 레벨의 태그 가져오기
    TArray<FGameplayTag> AllLevels = UGameplayTagLevelUtilities::GetAllTagLevels(ExampleTag);
    // 결과: ["CharacterStat", "CharacterStat.Health", "CharacterStat.Health.Level"]

    // 5. 특정 레벨의 태그 이름만 가져오기
    FString Level2Name = UGameplayTagLevelUtilities::GetTagNameAtLevel(ExampleTag, 2); // "Health"

    // 6. 디버그 로그
    UE_LOG(LogTemp, Warning, TEXT("Level 1 Tag: %s"), *Level1Tag.ToString());
    UE_LOG(LogTemp, Warning, TEXT("Level 2 Tag: %s"), *Level2Tag.ToString());
    UE_LOG(LogTemp, Warning, TEXT("Tag Depth: %d"), Depth);
    UE_LOG(LogTemp, Warning, TEXT("Level 2 Name: %s"), *Level2Name);
}
