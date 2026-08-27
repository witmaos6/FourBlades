// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Cosmetics/FBCosmeticAnimationTypes.h"
#include "Equipment/FBEquipmentInstance.h"
#include "FBWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBWeaponInstance : public UFBEquipmentInstance
{
	GENERATED_BODY()
public:
	UFBWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Animation)
	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FFBAnimLayerSelectionSet EquippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FFBAnimLayerSelectionSet UnequippedAnimSet;
};
