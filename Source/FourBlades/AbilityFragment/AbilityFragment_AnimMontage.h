// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment_AnimMontage.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UAbilityFragment_AnimMontage : public UFBAbilityFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AnimMontage;
};
