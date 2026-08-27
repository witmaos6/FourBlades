// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "Enemy/EnemyData/FBEnemyAbilitySet.h"
#include "BTTask_FBAIMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UBTTask_FBAIMoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()
public:
	UBTTask_FBAIMoveTo();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "ConditionKey")
	FBlackboardKeySelector StopMovementConditionKey;

	UPROPERTY(EditAnywhere, Category = "Condition")
	EAbilityConditionType AbilityConditionType;

	UPROPERTY(EditAnywhere, Category = "Task")
	uint8 bTaskInterruption : 1;

private:
	bool ShouldAbortMovement(UBehaviorTreeComponent& OwnerComp) const;
};
