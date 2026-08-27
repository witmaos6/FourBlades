// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FBExperienceDefinition.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFBExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TObjectPtr<class UFBPawnData> DefaultPawnData;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<TObjectPtr<class UFBExperienceActionSet>> ActionSets;

	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	TArray<TObjectPtr<class UGameFeatureAction>> Actions;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TArray<TObjectPtr<class UFBGameDataTable>> GameData;
};