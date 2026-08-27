// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTaskNode_TurnToTarget.h"
#include "AI/FBAIKey.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Library/FBDetectionUtilityLibrary.h"
#include "Enemy/Interface/FBEnemyDataInterface.h"

UBTTaskNode_TurnToTarget::UBTTaskNode_TurnToTarget()
{
	NodeName = TEXT("Turn");
	bNotifyTick = true;
	bTickIntervals = true;

	ValidRadius = 10.f;

	AbilityConditionType = EAbilityConditionType::MeleeForwardAttack;
}

EBTNodeResult::Type UBTTaskNode_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerPawnKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if (IFBEnemyDataInterface* AICharacterInterface = Cast<IFBEnemyDataInterface>(ControllingPawn))
	{
		ValidRadius = AICharacterInterface->GetAbilityActivateValidRadius(AbilityConditionType);
	}

	return EBTNodeResult::InProgress;
}

void UBTTaskNode_TurnToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return;
	}

	APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerPawnKey.SelectedKeyName));
	if (TargetPawn == nullptr)
	{
		return;
	}

	const float TurnSpeed = 5.f; // To do: 멤버 변수로 변경할 수도 있음
	FVector LookVector = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRotation = FRotationMatrix::MakeFromX(LookVector).Rotator();
	ControllingPawn->SetActorRotation(FMath::RInterpTo(ControllingPawn->GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), TurnSpeed));

	bool bValidRadius = UFBDetectionUtilityLibrary::IsTargetInSectorFromActor(ControllingPawn, TargetPawn, ValidRadius);
	if (bValidRadius)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
