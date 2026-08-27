// Fill out your copyright notice in the Description page of Project Settings.


#include "Library/FBDetectionUtilityLibrary.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "FourBlades.h"

bool UFBDetectionUtilityLibrary::InDistanceFromActor(AActor* FromActor, AActor* TargetActor, const float Distance)
{
	if (Distance < 0.0f)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Distance is less than 0"));
		return false;
	}
	if (IsValid(FromActor) && IsValid(TargetActor))
	{
		ACharacter* FromCharacter = Cast<ACharacter>(FromActor);
		ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
		if (FromCharacter && TargetCharacter)
		{
			float FromRadius = FromCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
			float TargetRaidus = TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
			float DistanceToTarget = FromActor->GetDistanceTo(TargetActor) - (FromRadius + TargetRaidus);
			return (DistanceToTarget <= Distance);
			// To do: Character의 CapsuleComponent의 반지름까지 고려한 것이므로 함수를 분리할 수도 있음
		}
		float DistanceToTarget = FromActor->GetDistanceTo(TargetActor);
		return (DistanceToTarget <= Distance);
	}
	if (!IsValid(FromActor))
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("FromActor is nullptr"));
	}
	if (!IsValid(TargetActor))
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("TargetActor is nullptr"));
	}
	return false;
}

bool UFBDetectionUtilityLibrary::IsTargetInSectorFromActor(AActor* FromActor, AActor* TargetActor, const float Angle, EDirection InDirection)
{
	if (Angle < 0.0f)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Angle is less than 0"));
		return false;
	}
	if (IsValid(FromActor) && IsValid(TargetActor))
	{
		FVector TargetLocation = TargetActor->GetActorLocation();
		FVector FromActorLocation = FromActor->GetActorLocation();
		FVector FromActorDirectionVector = UFBDetectionUtilityLibrary::GetActorDirectionVector(FromActor, InDirection);

		FVector DirectionToTarget = (TargetLocation - FromActorLocation).GetSafeNormal2D();
		float DotProduct = FVector::DotProduct(FromActorDirectionVector, DirectionToTarget);
		float CosineThreshold = FMath::Cos(FMath::DegreesToRadians(Angle));

		return (DotProduct >= CosineThreshold);
	}
	if (!IsValid(FromActor))
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("FromActor is nullptr"));
	}
	if (!IsValid(TargetActor))
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("TargetActor is nullptr"));
	}
	return false;
}

bool UFBDetectionUtilityLibrary::IsTargetInSectorFromActor(const FVector& FromLocation, const FRotator& FromRotation, AActor* TargetActor, const float Angle)
{
	if (Angle < 0.0f)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Angle is less than 0"));
		return false;
	}
	if (IsValid(TargetActor))
	{
		FVector TargetLocation = TargetActor->GetActorLocation();
		FVector DirectionToTarget = (TargetLocation - FromLocation).GetSafeNormal();

		FVector FromActorDirectionVector = FromRotation.Vector();
		float DotProduct = FVector::DotProduct(FromActorDirectionVector, DirectionToTarget);
		float CosineThreshold = FMath::Cos(FMath::DegreesToRadians(Angle));

		return (DotProduct >= CosineThreshold);
	}
	if (!IsValid(TargetActor))
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("TargetActor is nullptr"));
	}
	return false;
}

FVector UFBDetectionUtilityLibrary::GetActorDirectionVector(AActor* Actor, EDirection InDirection)
{
	check(Actor);

	if (InDirection == EDirection::ED_Forward)
	{
		return Actor->GetActorForwardVector();
	}
	else if (InDirection == EDirection::ED_Right)
	{
		return Actor->GetActorRightVector();
	}
	else if (InDirection == EDirection::ED_Back)
	{
		return -1 * Actor->GetActorForwardVector();
	}
	else if (InDirection == EDirection::ED_Left)
	{
		return -1 * Actor->GetActorRightVector();
	}

	FB_FUNC_LOG(LogTemp, Warning, TEXT("EDirection Warning"));
	return FVector::ZeroVector;
}
