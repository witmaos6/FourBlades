// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_PlayerInfoRecieve.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UBTService_PlayerInfoRecieve : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_PlayerInfoRecieve();

	UPROPERTY(EditAnywhere, Category = "PlayerPawn")
	FBlackboardKeySelector PlayerTargetKey;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
