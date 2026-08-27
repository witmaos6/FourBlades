// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_ShouldReturnToSpawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AI/FBAIKey.h"
#include "FourBlades.h"

UBTDecorator_ShouldReturnToSpawn::UBTDecorator_ShouldReturnToSpawn()
{
	NodeName = TEXT("Should Return To Spawn");

	FlowAbortMode = EBTFlowAbortMode::Self;
}

bool UBTDecorator_ShouldReturnToSpawn::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("BlackboardComponent is null in ShouldReturnToSpawn"));
		return false;
	}

	bool bShouldReturn = BlackboardComp->GetValueAsBool(BBKey_ShouldReturnToSpawn);
	return bShouldReturn;
}