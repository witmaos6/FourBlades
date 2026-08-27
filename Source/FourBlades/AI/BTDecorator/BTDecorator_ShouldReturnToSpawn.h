// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_ShouldReturnToSpawn.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UBTDecorator_ShouldReturnToSpawn : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_ShouldReturnToSpawn();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
