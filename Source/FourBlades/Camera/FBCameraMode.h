// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FBCameraMode.generated.h"

/**
 * 
 */

/*
* CameraModeView
*/
struct FFBCameraModeView
{
	FFBCameraModeView();

	void Blend(const FFBCameraModeView& Other, float OtherWeight);

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

/*
* [0,1]을 BlendFunction에 맞게 재매핑을 위한 타입
*/
UENUM(BlueprintType)
enum class EFBCameraModeBlendFunction : uint8
{
	Linear,
	EaseIn, // 지수함수와 같은 개형이다.
	EaseOut, // 로그함수와 같은 개형이다.
	EaseInOut, //In과 Out을 적절히 섞은 개형이다.
	COUNT
};

/*
* CameraMode
*/
UCLASS(Abstract, NotBlueprintable)
class FOURBLADES_API UFBCameraMode : public UObject
{
	GENERATED_BODY()
public:
	UFBCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void UpdateCameraMode(float DeltaTime);

	virtual void UpdateView(float DeltaTime);

	void UpdateBlending(float DeltaTime);

	class UFBCameraComponent* GetFBCameraComponent() const;
	AActor* GetTargetActor() const;
	FVector GetPivotLocation() const;
	FRotator GetPivotRotation() const;

public:
	FFBCameraModeView View;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170.0", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.0", UIMax = "89.0", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.0", UIMax = "89.0", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	float BlendAlpha;

	float BlendWeight;

	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	EFBCameraModeBlendFunction BlendFunction;
};

/*
* CameraModeStack
*/
UCLASS()
class UFBCameraModeStack : public UObject
{
	GENERATED_BODY()
public:
	UFBCameraModeStack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFBCameraMode* GetCameraModeInstance(TSubclassOf<UFBCameraMode>& CameraModeClass);

	void PushCameraMode(TSubclassOf<UFBCameraMode>& CameraModeClass);

	void EvaluateStack(float DeltaTime, FFBCameraModeView& OutCameraModeView);

	void UpdateStack(float DeltaTime);

	void BlendStack(FFBCameraModeView& OutCameraModeView) const;

public:
	UPROPERTY()
	TArray<TObjectPtr<UFBCameraMode>> CameraModeInstances;

	UPROPERTY()
	TArray<TObjectPtr<UFBCameraMode>> CameraModeStack;
};
