// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FBAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FBAIKey.h"
#include "FourBlades.h"

AFBAIController::AFBAIController()
{
	
}

void AFBAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTree)
	{
		RunAI();
	}
}

void AFBAIController::SetBehaviorTree(UBehaviorTree* InBehaviorTree)
{
	BehaviorTree = InBehaviorTree;
}

void AFBAIController::RunAI()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	if (UseBlackboard(BlackboardData, BlackboardPtr))
	{
		if (GetPawn())
		{
			Blackboard->SetValueAsVector(BBKey_SpawnedLocation, GetPawn()->GetActorLocation());
		}

		if (BehaviorTree)
		{
			bool RunResult = RunBehaviorTree(BehaviorTree);
			ensure(RunResult);
		}
	}
}

void AFBAIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StopTree(EBTStopMode::Forced);
	}
}

void AFBAIController::ContinueAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StartLogic();
	}
}

void AFBAIController::ReStartAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->RestartLogic();
	}
}
