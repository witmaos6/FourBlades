// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_ValidAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/Interface/FBEnemyDataInterface.h"
#include "AI/FBAIKey.h"
#include "Library/FBDetectionUtilityLibrary.h"
#include "Character/FBCharacter.h"
#include "FourBlades.h"

UBTDecorator_ValidAttackRange::UBTDecorator_ValidAttackRange()
{
	NodeName = TEXT("ValidRange");
	AbilityConditionType = EAbilityConditionType::MeleeForwardAttack;
}

bool UBTDecorator_ValidAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return false;
	}

	bool ValidDistance = false;

	AFBCharacter* Target = Cast<AFBCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerPawnKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return false;
	}

	IFBEnemyDataInterface* AIPawn = Cast<IFBEnemyDataInterface>(ControllingPawn);
	if (AIPawn)
	{
		ValidDistance = UFBDetectionUtilityLibrary::InDistanceFromActor(ControllingPawn, Target, AIPawn->GetAbilityActivateMinRange(AbilityConditionType));
	}

	return bResult && ValidDistance;
}
