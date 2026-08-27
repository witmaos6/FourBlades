// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "FBGameDataTable.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBGameDataTable : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UFBGameDataTable();

	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	FGameplayTag DataTableTag;

	UPROPERTY(EditDefaultsOnly, Category = "DataTable")
	TObjectPtr<UDataTable> DataTable;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
