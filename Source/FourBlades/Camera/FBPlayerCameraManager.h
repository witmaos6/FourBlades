// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "FBPlayerCameraManager.generated.h"

#define FB_CAMERA_DEFAULT_FOV (80.0f)
#define FB_CAMERA_DEFAULT_PITCH_MIN (-89.0f)
#define FB_CAMERA_DEFAULT_PITCH_MAX (89.0f)
/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	AFBPlayerCameraManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
