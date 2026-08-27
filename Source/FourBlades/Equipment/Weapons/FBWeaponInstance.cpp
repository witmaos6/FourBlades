// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Weapons/FBWeaponInstance.h"

UFBWeaponInstance::UFBWeaponInstance(const FObjectInitializer& ObjectInitializer)
{
}

TSubclassOf<UAnimInstance> UFBWeaponInstance::PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const
{
    const FFBAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UnequippedAnimSet);
    return SetToQuery.SelectBestLayer(CosmeticTags);
}
