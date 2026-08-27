// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FBEquipmentInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FOURBLADES_API UFBEquipmentInstance : public UObject
{
	GENERATED_BODY()
public:
	UFBEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnEquipped();

	virtual void OnUnequipped();

	void SpawnEquipmentActors(const TArray<struct FFBEquipmentActorToSpawn>& ActorsToSpawn);

	void DestroyEquipmentActors();

	UPROPERTY()
	TObjectPtr<UObject> Instigator;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedActors;

public: // Blueprint 호출 함수
	UFUNCTION(BlueprintImplementableEvent, Category = Equipment, meta = (DisplayName = "OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = Equipment, meta = (DisplayName = "OnUnequipped"))
	void K2_OnUnequipped();

	UFUNCTION(BlueprintPure, Category = Equipment)
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

public: // interface
	UFUNCTION(BlueprintPure, Category = Equipment)
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category = Equipment, meta = (DeterminesOutputType = PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;
};
