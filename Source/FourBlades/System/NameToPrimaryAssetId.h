// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NameToPrimaryAssetId.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UNameToPrimaryAssetId : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Name")
	FName KeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PrimaryAssetId")
	FPrimaryAssetId PrimaryAssetId;
};
