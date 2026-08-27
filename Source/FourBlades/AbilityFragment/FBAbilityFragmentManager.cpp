// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityFragment/FBAbilityFragmentManager.h"

UFBAbilityFragmentManager::UFBAbilityFragmentManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const UFBAbilityFragment* UFBAbilityFragmentManager::FindFragmentByClass(TSubclassOf<UFBAbilityFragment> FragmentClass) const
{
    if (FragmentClass)
    {
        for (UFBAbilityFragment* Fragment : Fragments)
        {
            if (Fragment && Fragment->IsA(FragmentClass))
            {
                return Fragment;
            }
        }
    }
    return nullptr;
}
