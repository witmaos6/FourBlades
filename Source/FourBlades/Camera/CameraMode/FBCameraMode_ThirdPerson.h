// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/FBCameraMode.h"
#include "FBCameraMode_ThirdPerson.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class FOURBLADES_API UFBCameraMode_ThirdPerson : public UFBCameraMode
{
	GENERATED_BODY()
public:
	UFBCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void UpdateView(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	TObjectPtr<const class UCurveVector> TargetOffsetCurve;
};
