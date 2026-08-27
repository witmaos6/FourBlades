// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_ProbabilityCheck.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UBTDecorator_ProbabilityCheck : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_ProbabilityCheck();
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere, Category = "Random", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float Probability;
};
