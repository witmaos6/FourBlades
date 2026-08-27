// Fill out your copyright notice in the Description page of Project Settings.


#include "FBExperienceDefinition.h"
#include "GameFeatureAction.h"

UFBExperienceDefinition::UFBExperienceDefinition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

#if WITH_EDITORONLY_DATA
void UFBExperienceDefinition::UpdateAssetBundleData()
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
