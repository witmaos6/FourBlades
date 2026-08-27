// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/FBInventoryItemInstance.h"
#include "FBInventoryItemDefinition.h"

UFBInventoryItemInstance::UFBInventoryItemInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const UFBInventoryItemFragment* UFBInventoryItemInstance::FindFragmentByClass(TSubclassOf<UFBInventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UFBInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
