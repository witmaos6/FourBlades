// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Enemy/EnemyData/FBEnemyAbilitySet.h"
#include "BTTaskNode_TurnToTarget.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UBTTaskNode_TurnToTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskNode_TurnToTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "ConditionKey")
	FBlackboardKeySelector PlayerPawnKey;

	UPROPERTY(EditAnywhere, Category = "Condition")
	EAbilityConditionType AbilityConditionType;

	float ValidRadius;
};
