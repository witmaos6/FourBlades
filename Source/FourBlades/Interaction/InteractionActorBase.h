// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/FBInteractionInterface.h"
#include "InteractionActorBase.generated.h"

UCLASS()
class FOURBLADES_API AInteractionActorBase : public AActor, public IFBInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractionActorBase();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySet")
	TObjectPtr<class UFBAbilitySet> AbilitySet;

	virtual class UFBAbilitySet* GetAbilitySet() const override;
};
