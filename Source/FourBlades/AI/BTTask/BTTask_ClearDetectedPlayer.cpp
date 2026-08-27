// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_ClearDetectedPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/FBAIKey.h"
#include "FourBlades.h"

UBTTask_ClearDetectedPlayer::UBTTask_ClearDetectedPlayer()
{
	NodeName = TEXT("Clear Detected Player");

	bNotifyTick = false;
	bNotifyTaskFinished = true;

	bCreateNodeInstance = false;
}

EBTNodeResult::Type UBTTask_ClearDetectedPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("BlackboardComponent is null in ClearDetectedPlayer"));
		return EBTNodeResult::Failed;
	}

	BlackboardComp->ClearValue(BBKey_DetectPlayer);
	BlackboardComp->SetValueAsBool(BBKey_ShouldReturnToSpawn, false);

	UObject* VerifyValue = BlackboardComp->GetValueAsObject(BBKey_DetectPlayer);
	if (VerifyValue == nullptr)
	{
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
