// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Enemy/EnemyData/FBEnemyAbilitySet.h"
#include "BTTask_ActivateAbilityByTag.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UBTTask_ActivateAbilityByTag : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_ActivateAbilityByTag();

	UPROPERTY(EditAnywhere, Category = "Condition")
	EAbilityConditionType AbilityConditionType;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
