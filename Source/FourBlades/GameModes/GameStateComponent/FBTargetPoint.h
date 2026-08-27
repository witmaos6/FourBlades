// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FBTargetPoint.generated.h"

UCLASS()
class FOURBLADES_API AFBTargetPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFBTargetPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void SpawnedEnemyDestroyBind(ACharacter* Character);

	UFUNCTION()
	void SpawnedEnemyOnDestroy(AActor* EnemyCharacter);

	void RequestRespawn();
};
