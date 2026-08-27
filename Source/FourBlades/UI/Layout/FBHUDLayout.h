// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/FBActivatableWidget.h"
#include "FBHUDLayout.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBHUDLayout : public UFBActivatableWidget
{
	GENERATED_BODY()

public:
	UFBHUDLayout(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
