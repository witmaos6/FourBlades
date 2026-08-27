// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterComponents/FBCharacterMovementComponent.h"
#include "FourBlades.h"

UFBCharacterMovementComponent::UFBCharacterMovementComponent()
{
	LaunchDirection = FVector::ZeroVector;
	LaunchTargetDistance = 0.f;
	LaunchSpeed = 0.f;
	LaunchAccumulatedDistance = 0.f;
}

void UFBCharacterMovementComponent::StartLaunch(FVector Direction, float Distance, float Duration)
{
	if (Direction.IsZero() || Distance <= 0.f || Duration <= 0.0f)
		return;

	LaunchDirection = Direction.GetSafeNormal2D();
	LaunchTargetDistance = Distance;
	LaunchSpeed = Distance / Duration;
	LaunchAccumulatedDistance = 0.0f;

	SetMovementMode(MOVE_Custom, (uint8)EFBCustomMovementMode::MOVE_Launch);
}

void UFBCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	if (CustomMovementMode == (uint8)EFBCustomMovementMode::MOVE_Launch)
	{
		PhysLaunch(DeltaTime);
		return;
	}
	Super::PhysCustom(DeltaTime, Iterations);
}

void UFBCharacterMovementComponent::PhysLaunch(float DeltaTime)
{
	float RemainDistance = LaunchTargetDistance - LaunchAccumulatedDistance;
	if (RemainDistance <= 0.0f)
	{
		SetMovementMode(MOVE_Walking);
		return;
	}

	float MoveDistance = FMath::Min(LaunchSpeed * DeltaTime, RemainDistance);
	Velocity = LaunchDirection * (MoveDistance / DeltaTime);

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	CalcVelocity(DeltaTime, 0.0f, false, 0.0f);
	
	FHitResult Hit;
	SafeMoveUpdatedComponent(Velocity * DeltaTime, UpdatedComponent->GetComponentQuat(), true, Hit);

	float ActualMovedDistance = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
	LaunchAccumulatedDistance += ActualMovedDistance;

	if (Hit.bBlockingHit)
	{
		SetMovementMode(MOVE_Walking);
		return;
	}

	if (LaunchAccumulatedDistance >= LaunchTargetDistance)
	{
		SetMovementMode(MOVE_Walking);
	}
}


