// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_FindPatrolLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/AI/FBAICharacterInterface.h"
#include "AI/FBAIKey.h"

UBTTask_FindPatrolLocation::UBTTask_FindPatrolLocation()
{
}

EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	IFBAICharacterInterface* FBAICharacterInterface = Cast<IFBAICharacterInterface>(ControllingPawn);
	if (FBAICharacterInterface == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FVector SpawnedPos = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKey_SpawnedLocation);
	FNavLocation NextPatrolPos;
	if (NavSystem->GetRandomPointInNavigableRadius(SpawnedPos, FBAICharacterInterface->GetPatrolRadius(), NextPatrolPos))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKey_PatrolLocation, NextPatrolPos.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
