// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FBCosmeticAnimationTypes.generated.h"

/*
* AnimLayerSelectionEntry // AnimBody와 헷갈리지 않게 주의
*/
USTRUCT(BlueprintType)
struct FFBAnimLayerSelectionEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> Layer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RequiredTags;
};

/*
* AnimLayerSelectionSet // AnimBody와 헷갈리지 않게 주의
*/
USTRUCT(BlueprintType)
struct FFBAnimLayerSelectionSet
{
	GENERATED_BODY()

	TSubclassOf<UAnimInstance> SelectBestLayer(const FGameplayTagContainer& CosmeticTags) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FFBAnimLayerSelectionEntry> LayerRules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> DefaultLayer;
};

/*
* AnimBodyStyleSelectionEntry // AnimBody 대신 Mesh같은 단어를 쓰는 것이 좋아 보인다.
*/
USTRUCT(BlueprintType)
struct FFBAnimBodyStyleSelectionEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Cosmetic"))
	FGameplayTagContainer RequiredTags;
};

/*
 * AnimBodyStyleSelectionSet 
 */
USTRUCT(BlueprintType)
struct FFBAnimBodyStyleSelectionSet
{
	GENERATED_BODY()
	
	USkeletalMesh* SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FFBAnimBodyStyleSelectionEntry> MeshRules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> DefaultMesh = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPhysicsAsset> ForcedPhysicsAsset = nullptr;
};
