// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Library/FBDetectionUtilityLibrary.h"
#include "Enemy/EnemyData/FBEnemyAbilitySet.h"
#include "BTDecorator_ValidDirection.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UBTDecorator_ValidDirection : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_ValidDirection();

protected:
	UPROPERTY(EditAnywhere, Category = "Condition")
	EAbilityConditionType AbilityConditionType;

	UPROPERTY(EditAnywhere, Category = "ConditionKey")
	FBlackboardKeySelector PlayerPawnKey;

	UPROPERTY(EditAnywhere, Category = "Direction")
	EDirection CheckDirection;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
