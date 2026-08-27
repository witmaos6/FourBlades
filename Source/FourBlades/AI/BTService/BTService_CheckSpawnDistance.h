// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckSpawnDistance.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UBTService_CheckSpawnDistance : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_CheckSpawnDistance();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chase Settings", meta = (ClampMin = "0.0"))
	float MaxChaseDistance = 2000.0f;

private:
	float CalculateDistanceFromSpawn(const AActor* OwnerActor, const FVector& SpawnLocation) const;
};
