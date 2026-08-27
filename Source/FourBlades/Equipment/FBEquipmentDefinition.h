// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FBEquipmentDefinition.generated.h"

/*
* EquipmentActorToSpawn
*/
USTRUCT()
struct FFBEquipmentActorToSpawn
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FTransform AttachTransform;
};

/*
 * EquipmentDefinition 
 */
UCLASS(Blueprintable)
class FOURBLADES_API UFBEquipmentDefinition : public UObject
{
	GENERATED_BODY()
public:
	UFBEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TSubclassOf<class UFBEquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<FFBEquipmentActorToSpawn> ActorsToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<TObjectPtr<class UFBAbilitySet>> AbilitySetsToGrant;
};
