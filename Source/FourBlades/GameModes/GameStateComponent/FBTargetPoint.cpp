// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GameStateComponent/FBTargetPoint.h"
#include "GameModes/FBGameState.h"
#include "GameModes/GameStateComponent/EnemySpawnManagerComponent.h"
#include "GameFramework/Character.h"

// Sets default values
AFBTargetPoint::AFBTargetPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void AFBTargetPoint::BeginPlay()
{
	Super::BeginPlay();
}

void AFBTargetPoint::SpawnedEnemyDestroyBind(ACharacter* Character)
{
	if (Character)
	{
		Character->OnDestroyed.AddDynamic(this, &AFBTargetPoint::SpawnedEnemyOnDestroy);
	}
}

void AFBTargetPoint::SpawnedEnemyOnDestroy(AActor* EnemyCharacter)
{
	FTimerHandle RequestTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(RequestTimerHandle, this, &AFBTargetPoint::RequestRespawn, 5.0f, false);
}

void AFBTargetPoint::RequestRespawn()
{
	if (AFBGameState* GameState = Cast<AFBGameState>(GetWorld()->GetGameState()))
	{
		if (UEnemySpawnManagerComponent* SpawnManagerComponent = GameState->FindComponentByClass<UEnemySpawnManagerComponent>())
		{
			SpawnManagerComponent->RequestRespawn(this);
		}
	}
}
