// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LaunchData.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API ULaunchData : public UObject
{
	GENERATED_BODY()
public:
	FVector Direction = FVector::ZeroVector;
	float Distance = 0.0f;
	float Duration = 0.3f;
};
