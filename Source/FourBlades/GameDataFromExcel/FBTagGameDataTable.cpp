// Fill out your copyright notice in the Description page of Project Settings.


#include "GameDataFromExcel/FBTagGameDataTable.h"
#include "Library/GameplayTagLevelUtilities.h"

UFBTagGameDataTable::UFBTagGameDataTable()
{
}

FPrimaryAssetId UFBTagGameDataTable::GetPrimaryAssetId() const
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
