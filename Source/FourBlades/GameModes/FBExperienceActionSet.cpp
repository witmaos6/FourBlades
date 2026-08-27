// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/FBExperienceActionSet.h"
#include "GameFeatureAction.h"

UFBExperienceActionSet::UFBExperienceActionSet()
{
}

#if WITH_EDITORONLY_DATA
void UFBExperienceActionSet::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (UGameFeatureAction* Action : Actions)
	{
		if (Action)
		{
			Action->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif