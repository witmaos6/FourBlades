// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FBInventoryManagerComponent.generated.h"

/*
* InventoryEntry
*/
USTRUCT(BlueprintType)
struct FFBInventoryEntry
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<class UFBInventoryItemInstance> Instance = nullptr;
};

/*
* InventoryList
*/
USTRUCT(BlueprintType)
struct FFBInventoryList
{
	GENERATED_BODY()
	
	FFBInventoryList(UActorComponent* InOwnerComponent = nullptr) : OwnerComponent(InOwnerComponent) {}

	UFBInventoryItemInstance* AddEntry(TSubclassOf<UFBInventoryItemDefinition> ItemDef);

	UPROPERTY()
	TArray<FFBInventoryEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

/*
* InventoryManagerComponent
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FOURBLADES_API UFBInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UFBInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = Inventory)
	UFBInventoryItemInstance* AddItemDefinition(TSubclassOf<UFBInventoryItemDefinition> ItemDef);

	UPROPERTY()
	FFBInventoryList InventoryList;
};
