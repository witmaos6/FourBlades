// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipAbility/FBGameplayAbility_FromEquipment.h"
#include "FBGameplayAbility_MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBGameplayAbility_MeleeWeapon : public UFBGameplayAbility_FromEquipment
{
	GENERATED_BODY()
public:
	UFBGameplayAbility_MeleeWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
