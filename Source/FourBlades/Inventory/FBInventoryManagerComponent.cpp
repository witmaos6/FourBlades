// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/FBInventoryManagerComponent.h"
#include "FBInventoryItemDefinition.h"
#include "FBInventoryItemInstance.h"

/*
* InventoryManagerComponent
*/
UFBInventoryManagerComponent::UFBInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
}

UFBInventoryItemInstance* UFBInventoryManagerComponent::AddItemDefinition(TSubclassOf<UFBInventoryItemDefinition> ItemDef)
{
	UFBInventoryItemInstance* Result = nullptr;
	if (ItemDef)
	{
		Result = InventoryList.AddEntry(ItemDef);
	}
	return Result;
}

/*
* InventoryList
*/
UFBInventoryItemInstance* FFBInventoryList::AddEntry(TSubclassOf<UFBInventoryItemDefinition> ItemDef)
{
	UFBInventoryItemInstance* Result = nullptr;
	check(ItemDef);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FFBInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UFBInventoryItemInstance>(OwningActor);
	NewEntry.Instance->ItemDef = ItemDef;

	Result = NewEntry.Instance;
	return Result;
}
