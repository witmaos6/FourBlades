// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FBUserFacingExperience.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBUserFacingExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "FBExperienceDefinition"))
	FPrimaryAssetId ExperienceID;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	class UCommonSession_HostSessionRequest* CreateHostingRequest() const;
};
