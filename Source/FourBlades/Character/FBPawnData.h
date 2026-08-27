// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FBPawnData.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UFBPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FB|Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FB|Camera")
	TSubclassOf<class UFBCameraMode> DefaultCameraMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FB|InputConfig")
	TObjectPtr<class UFBInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FB|Abilities")
	TArray<TObjectPtr<class UFBAbilitySet>> AbilitySets; // PawnData에서는 공용으로 사용하는 어빌리티를 주로 넣는다.
};
