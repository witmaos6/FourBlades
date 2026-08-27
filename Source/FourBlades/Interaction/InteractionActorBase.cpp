// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/InteractionActorBase.h"
#include "AbilitySystem/FBAbilitySet.h"

// Sets default values
AInteractionActorBase::AInteractionActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

UFBAbilitySet* AInteractionActorBase::GetAbilitySet() const
{
	check(AbilitySet);
	return AbilitySet;
}
