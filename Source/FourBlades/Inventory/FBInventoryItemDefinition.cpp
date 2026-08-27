// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/FBInventoryItemDefinition.h"

UFBInventoryItemDefinition::UFBInventoryItemDefinition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const UFBInventoryItemFragment* UFBInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UFBInventoryItemFragment> FragmentClass) const
{
    if (FragmentClass)
    {
        for (UFBInventoryItemFragment* Fragment : Fragments)
        {
            if (Fragment && Fragment->IsA(FragmentClass))
            {
                return Fragment;
            }
        }
    }
    return nullptr;
}
