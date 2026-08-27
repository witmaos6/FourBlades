// Fill out your copyright notice in the Description page of Project Settings.


#include "Cosmetics/FBCosmeticAnimationTypes.h"

/*
* AnimLayerSelectionSet // AnimBody와 헷갈리지 않게 주의
*/
TSubclassOf<UAnimInstance> FFBAnimLayerSelectionSet::SelectBestLayer(const FGameplayTagContainer& CosmeticTags) const
{
    for (const FFBAnimLayerSelectionEntry& Rule : LayerRules)
    {
        if ((Rule.Layer != nullptr) && CosmeticTags.HasAll(Rule.RequiredTags))
        {
            return Rule.Layer;
        }
    }
    return DefaultLayer;
}

/*
* AnimBodyStyleSelectionSet
*/
USkeletalMesh* FFBAnimBodyStyleSelectionSet::SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const
{
    for (const FFBAnimBodyStyleSelectionEntry& Rule : MeshRules)
    {
        if ((Rule.Mesh) && CosmeticTags.HasAll(Rule.RequiredTags))
        {
            return Rule.Mesh;
        }
    }
    return DefaultMesh;
}