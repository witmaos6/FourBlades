// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FBCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EFBCustomMovementMode : uint8
{
	MOVE_Launch = 0
};
/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UFBCharacterMovementComponent();

	void StartLaunch(FVector Direction, float Distance, float Duration);

protected:
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

private:
	void PhysLaunch(float DeltaTime);

	FVector LaunchDirection;

	float LaunchTargetDistance;

	float LaunchSpeed;

	float LaunchAccumulatedDistance;
};
