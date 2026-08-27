// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ClearDetectedPlayer.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UBTTask_ClearDetectedPlayer : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ClearDetectedPlayer();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
