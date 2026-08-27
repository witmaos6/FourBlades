// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService/BTService_CheckSpawnDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "AI/FBAIKey.h"
#include "FourBlades.h"

UBTService_CheckSpawnDistance::UBTService_CheckSpawnDistance()
{
	NodeName = TEXT("Check Spawn Distance");

	Interval = 1.0f;
	RandomDeviation = 0.0f;

	bCallTickOnSearchStart = false;
	bRestartTimerOnEachActivation = true;

	MaxChaseDistance = 2000.0f;
}

void UBTService_CheckSpawnDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("BlackboardComponent is null in CheckSpawnDistance"));
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("AIController is null in CheckSpawnDistance"));
		return;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("ControlledPawn is null in CheckSpawnDistance"));
		return;
	}

	FVector SpawnLocation = BlackboardComp->GetValueAsVector(BBKey_SpawnedLocation);
	if (SpawnLocation.IsZero())
	{
		BlackboardComp->SetValueAsBool(BBKey_ShouldReturnToSpawn, false);
		return;
	}

	float CurrentDistance = CalculateDistanceFromSpawn(ControlledPawn, SpawnLocation);

	bool bShouldReturn = CurrentDistance > MaxChaseDistance;

	BlackboardComp->SetValueAsBool(BBKey_ShouldReturnToSpawn, bShouldReturn);
}

float UBTService_CheckSpawnDistance::CalculateDistanceFromSpawn(const AActor* OwnerActor, const FVector& SpawnLocation) const
{
	if (!OwnerActor)
	{
		return 0.0f;
	}

	FVector CurrentLocation = OwnerActor->GetActorLocation();
	return FVector::Dist(CurrentLocation, SpawnLocation);
}
