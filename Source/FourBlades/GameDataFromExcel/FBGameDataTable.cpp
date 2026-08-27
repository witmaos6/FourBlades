// Fill out your copyright notice in the Description page of Project Settings.


#include "GameDataFromExcel/FBGameDataTable.h"
#include "Library/GameplayTagLevelUtilities.h"

UFBGameDataTable::UFBGameDataTable()
{
}

FPrimaryAssetId UFBGameDataTable::GetPrimaryAssetId() const
{
	if (DataTableTag.IsValid())
	{
		FGameplayTag Level1Tag = UGameplayTagLevelUtilities::GetTagAtLevel(DataTableTag, 1);
		if (Level1Tag.IsValid())
		{
			return FPrimaryAssetId(FPrimaryAssetType(Level1Tag.GetTagName()), GetFName());
		}
	}

	return FPrimaryAssetId(FPrimaryAssetType("TagToPrimaryAssetId"), GetFName());
}
