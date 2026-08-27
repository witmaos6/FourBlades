// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_ValidRushDistance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/Interface/FBEnemyDataInterface.h"
#include "AI/FBAIKey.h"
#include "FourBlades.h"

UBTDecorator_ValidRushDistance::UBTDecorator_ValidRushDistance()
{
	NodeName = TEXT("ValudRushDistance");

	AbilityConditionType = EAbilityConditionType::MeleeForwardAttack;
}

bool UBTDecorator_ValidRushDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return false;
	}

	IFBEnemyDataInterface* AIPawn = Cast<IFBEnemyDataInterface>(ControllingPawn);
	if (AIPawn == nullptr)
	{
		return false;
	}

	const float MinRange = AIPawn->GetAbilityActivateMinRange(AbilityConditionType);
	const float MaxRange = AIPawn->GetAbilityActivateMaxRange(AbilityConditionType);

	float Distance = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(PlayerPawnKey.SelectedKeyName);

	if (Distance >= MinRange && Distance <= MaxRange)
	{
		return bResult;
	}
	return false;
}
