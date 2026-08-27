// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "TagToPrimaryAssetId.generated.h"

/*
* TaggedPrimaryAssetId
*/
USTRUCT(BlueprintType)
struct FTaggedPrimaryAssetId
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayTag")
	FGameplayTag MappingTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PrimaryAssetId")
	FPrimaryAssetId PrimaryAssetId;
};

/**
 * TagToPrimaryAssetId
 */
UCLASS()
class FOURBLADES_API UTagToPrimaryAssetId : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AssetList")
	TArray<FTaggedPrimaryAssetId> TaggedAssetList;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

#if WITH_EDITOR
    virtual void PreSave(const class ITargetPlatform* TargetPlatform) override;

private:
	bool ValidateTagConsistencyForSave();
#endif
};
