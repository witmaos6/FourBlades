// Fill out your copyright notice in the Description page of Project Settings.


#include "FBGameState.h"
#include "FBExperienceManagerComponent.h"
#include "Character/FBCharacter.h"
#include "GameModes/GameStateComponent/EnemySpawnManagerComponent.h"
#include "FourBlades.h"

AFBGameState::AFBGameState()
{
	ExperienceManagerComponent = CreateDefaultSubobject<UFBExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AFBGameState::PlayerPossessedFinish(AFBCharacter* NewCharacter)
{
	if (OnPlayerSpawned.IsBound())
	{
		OnPlayerSpawned.Execute(NewCharacter);
	}
}
