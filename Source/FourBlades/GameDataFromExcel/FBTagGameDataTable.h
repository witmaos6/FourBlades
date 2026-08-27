// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "FBTagGameDataTable.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBTagGameDataTable : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UFBTagGameDataTable();
	
	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	FGameplayTag DataTableTag;

	UPROPERTY(EditDefaultsOnly, Category = "DataTable")
	TSubclassOf<class UGameplayTagDataTable> DataTable;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
