// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "FBGameplayCueManager.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()
public:
	UFBGameplayCueManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UFBGameplayCueManager* Get();

	void RefreshGameplayCuePrimaryAsset();
};
