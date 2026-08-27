// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GameStateComponent/PlayerInfoCachedComponent.h"
#include "EngineUtils.h"
#include "Character/FBCharacter.h"
#include "GameModes/FBGameState.h"

UPlayerInfoCachedComponent::UPlayerInfoCachedComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UPlayerInfoCachedComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AFBGameState* GameState = Cast<AFBGameState>(GetWorld()->GetGameState()))
	{
		GameState->OnPlayerSpawned.BindUObject(this, &UPlayerInfoCachedComponent::AddUniquePlayerCharacters);
	}
}

void UPlayerInfoCachedComponent::AddUniquePlayerCharacters(AFBCharacter* Character)
{
	if (IsValid(Character))
	{
		PlayerCharacters.AddUnique(Character);
	}
}
