// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "FBCameraMode.h"
#include "FBCameraComponent.generated.h"

template <class TClass> class TSubclassOf;

DECLARE_DELEGATE_RetVal(TSubclassOf<UFBCameraMode>, FFBCameraModeDelegate);
/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
public:
	UFBCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UFBCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UFBCameraComponent>() : nullptr); }

	AActor* GetTargetActor() const { return GetOwner(); }

public:
	virtual void OnRegister() final;

	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) final;

public:
	void UpdateCameraModes();

public:
	UPROPERTY()
	TObjectPtr<class UFBCameraModeStack> CameraModeStack;

	FFBCameraModeDelegate DetermineCameraModeDelegate;
};
