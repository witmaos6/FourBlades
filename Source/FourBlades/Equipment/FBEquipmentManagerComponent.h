// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "AbilitySystem/FBAbilitySet.h"
#include "FBEquipmentManagerComponent.generated.h"

/*
* AppliedEquipmentEntry
*/
USTRUCT(BlueprintType)
struct FFBAppliedEquipmentEntry
{
	GENERATED_BODY()
	
	UPROPERTY()
	TSubclassOf<class UFBEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<class UFBEquipmentInstance> Instance = nullptr;

	UPROPERTY()
	FFBAbilitySet_GrantedHandles GrantedHandles;
};

/*
* EquipmentList
*/
USTRUCT(BlueprintType)
struct FFBEquipmentList
{
	GENERATED_BODY()
	
	FFBEquipmentList(UActorComponent* InOwnerComponent = nullptr) : OwnerComponent(InOwnerComponent) {}

	UFBEquipmentInstance* AddEntry(TSubclassOf<UFBEquipmentDefinition> EquipmentDefinition);

	void RemoveEntry(UFBEquipmentInstance* Instance);

	UFBAbilitySystemComponent* GetAbilitySystemComponent() const;

	UPROPERTY()
	TArray<FFBAppliedEquipmentEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

/*
 * EquipmentManagerComponent
 */
UCLASS()
class FOURBLADES_API UFBEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	UFBEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFBEquipmentInstance* EquipItem(TSubclassOf<UFBEquipmentDefinition> EquipmentDefinition);

	void UnequipItem(UFBEquipmentInstance* ItemInstance);

	UFUNCTION(BlueprintCallable)
	TArray<UFBEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UFBEquipmentInstance> InstanceType) const;

	UPROPERTY()
	FFBEquipmentList EquipmentList;
};
