// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FBDetectionUtilityLibrary.generated.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
	ED_Forward,
	ED_Right,
	ED_Left,
	ED_Back
};
/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBDetectionUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static bool InDistanceFromActor(AActor* FromActor, AActor* TargetActor, const float Distance);

	static bool IsTargetInSectorFromActor(AActor* FromActor, AActor* TargetActor, const float Angle, EDirection InDirection = EDirection::ED_Forward);
	// To do: 함수 이름을 다음으로 변경 할 수 있음 IsTargetWithinViewAngle

	static bool IsTargetInSectorFromActor(const FVector& FromLocation, const FRotator& FromRotation, AActor* TargetActor, const float Angle);
	// 이 함수를 사용하는 경우에는 FromRotaiton에서 이미 방향을 계산하기 때문에 지금은 EDirection을 넣지 않음

private:
	static FVector GetActorDirectionVector(AActor* Actor, EDirection InDirection);
};
