// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_ValidDirection.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/Interface/FBEnemyDataInterface.h"
#include "AI/FBAIKey.h"
#include "Library/FBDetectionUtilityLibrary.h"
#include "Character/FBCharacter.h"
#include "FourBlades.h"

UBTDecorator_ValidDirection::UBTDecorator_ValidDirection()
{
	NodeName = TEXT("ValidDirection");

	AbilityConditionType = EAbilityConditionType::MeleeForwardAttack;
	CheckDirection = EDirection::ED_Forward;
}

bool UBTDecorator_ValidDirection::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return false;
	}

	bool ValidDirection = false;

	AFBCharacter* Target = Cast<AFBCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerPawnKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return false;
	}

	IFBEnemyDataInterface* AIPawn = Cast<IFBEnemyDataInterface>(ControllingPawn);
	if (AIPawn)
	{
		ValidDirection = UFBDetectionUtilityLibrary::IsTargetInSectorFromActor(ControllingPawn, Target, AIPawn->GetAbilityActivateValidRadius(AbilityConditionType), CheckDirection);
	}
	
	return bResult && ValidDirection;
}
