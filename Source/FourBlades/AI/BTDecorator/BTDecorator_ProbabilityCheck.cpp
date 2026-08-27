// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_ProbabilityCheck.h"
#include "FourBlades.h"

UBTDecorator_ProbabilityCheck::UBTDecorator_ProbabilityCheck()
{
	NodeName = TEXT("ValidProbability");
	Probability = 0.5f;
}

bool UBTDecorator_ProbabilityCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const float RandomValue = FMath::RandRange(0.0f, 1.0f);
	return RandomValue <= Probability;
}
