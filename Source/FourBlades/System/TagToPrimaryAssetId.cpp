// Fill out your copyright notice in the Description page of Project Settings.


#include "System/TagToPrimaryAssetId.h"
#include "Library/GameplayTagLevelUtilities.h"
#include "FourBlades.h"

/**
 * TagToPrimaryAssetId
 */
FPrimaryAssetId UTagToPrimaryAssetId::GetPrimaryAssetId() const
{
	if (!TaggedAssetList.IsEmpty())
	{
		const FGameplayTag FirstTag = TaggedAssetList[0].MappingTag;
		if (FirstTag.IsValid())
		{
			FGameplayTag Level1Tag = UGameplayTagLevelUtilities::GetTagAtLevel(FirstTag, 1);
			if (Level1Tag.IsValid())
			{
				return FPrimaryAssetId(FPrimaryAssetType(Level1Tag.GetTagName()), GetFName());
			}
		}
	}
	return FPrimaryAssetId(FPrimaryAssetType("TagToPrimaryAssetId"), GetFName());
}

#if WITH_EDITOR
void UTagToPrimaryAssetId::PreSave(const ITargetPlatform* TargetPlatform)
{
	Super::PreSave(TargetPlatform);

	if (!ValidateTagConsistencyForSave())
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("Cannot save %s due to tag consistency errors"), *GetName());

		// 저장을 막고 싶다면 (주의: 강제적)
		// throw std::runtime_error("Tag consistency validation failed");
	}
}

bool UTagToPrimaryAssetId::ValidateTagConsistencyForSave()
{
    if (TaggedAssetList.IsEmpty())
    {
        return true;
    }

    const FGameplayTag& FirstTag = TaggedAssetList[0].MappingTag;
    if (!FirstTag.IsValid())
    {
        return false;
    }

    FGameplayTag ExpectedLevel1Tag = UGameplayTagLevelUtilities::GetTagAtLevel(FirstTag, 1);
    if (!ExpectedLevel1Tag.IsValid())
    {
        return false;
    }

    for (int32 i = 1; i < TaggedAssetList.Num(); i++)
    {
        if (!TaggedAssetList[i].MappingTag.IsValid())
        {
            return false;
        }

        FGameplayTag CurrentLevel1Tag = UGameplayTagLevelUtilities::GetTagAtLevel(TaggedAssetList[i].MappingTag, 1);
        if (CurrentLevel1Tag != ExpectedLevel1Tag)
        {
            return false;
        }
    }
    return true;
}
#endif
