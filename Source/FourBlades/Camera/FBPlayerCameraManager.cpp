// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/FBPlayerCameraManager.h"

AFBPlayerCameraManager::AFBPlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DefaultFOV = FB_CAMERA_DEFAULT_FOV;
	ViewPitchMin = FB_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = FB_CAMERA_DEFAULT_PITCH_MAX;
}
