// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService/BTService_SetDistanceToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/FBAIKey.h"
#include "FourBlades.h"

UBTService_SetDistanceToTarget::UBTService_SetDistanceToTarget()
{
	NodeName = TEXT("SetDistanceToTarget");
	Interval = 0.2f;
	RandomDeviation = 0.0f;
}

void UBTService_SetDistanceToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
		return;

	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKey_AlivePlayer));
	if (Target == nullptr)
	{
		Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKey_DetectPlayer));
		if (Target == nullptr)
		{
			return;
		}
	}

	const float Distance = ControllingPawn->GetDistanceTo(Target);

	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(BBKey_DistanceToTarget, Distance);
}
