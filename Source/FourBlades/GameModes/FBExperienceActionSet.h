// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FBExperienceActionSet.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UFBExperienceActionSet();

#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif

	UPROPERTY(EditAnywhere, Category = "ActionsToPerform")
	TArray<TObjectPtr<class UGameFeatureAction>> Actions;
};
