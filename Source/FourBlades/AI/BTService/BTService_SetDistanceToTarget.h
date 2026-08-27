// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SetDistanceToTarget.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UBTService_SetDistanceToTarget : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_SetDistanceToTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
