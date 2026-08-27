// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_FBAIMoveTo.h"
#include "AIController.h"
#include "Enemy/Interface/FBEnemyDataInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FourBlades.h"

UBTTask_FBAIMoveTo::UBTTask_FBAIMoveTo()
{
	NodeName = TEXT("Move To Valid Distance");

	bNotifyTick = true;
	bTaskInterruption = true;
	AbilityConditionType = EAbilityConditionType::MeleeForwardAttack;
}

EBTNodeResult::Type UBTTask_FBAIMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bNotifyTick = bTaskInterruption;
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Controlling Pawn is nullptr"));
		return Super::ExecuteTask(OwnerComp, NodeMemory);
	}

	IFBEnemyDataInterface* FBAICharacterInterface = Cast<IFBEnemyDataInterface>(ControllingPawn);
	if (FBAICharacterInterface)
	{
		AcceptableRadius = FBAICharacterInterface->GetAbilityActivateMinRange(AbilityConditionType);
		return Super::ExecuteTask(OwnerComp, NodeMemory);
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_FBAIMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (ShouldAbortMovement(OwnerComp))
	{
		AAIController* AIController = OwnerComp.GetAIOwner();
		if (AIController)
		{
			AIController->StopMovement();
		}

		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
	}
}

bool UBTTask_FBAIMoveTo::ShouldAbortMovement(UBehaviorTreeComponent& OwnerComp) const
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return false;
	}

	bool bShouldReturn = BlackboardComp->GetValueAsBool(StopMovementConditionKey.SelectedKeyName);

	return bShouldReturn;
}
