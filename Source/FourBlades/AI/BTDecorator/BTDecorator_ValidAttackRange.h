// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Enemy/EnemyData/FBEnemyAbilitySet.h"
#include "BTDecorator_ValidAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UBTDecorator_ValidAttackRange : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_ValidAttackRange();

protected:
	UPROPERTY(EditAnywhere, Category = "Condition")
	EAbilityConditionType AbilityConditionType;

	UPROPERTY(EditAnywhere, Category = "ConditionKey")
	FBlackboardKeySelector PlayerPawnKey;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
