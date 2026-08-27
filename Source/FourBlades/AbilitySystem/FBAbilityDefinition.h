#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FBAbilityDefinition.generated.h"

USTRUCT(BlueprintType)
struct FFBAbilityDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "AbilityTag")
	FGameplayTag AbilityTag;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<class UGameplayAbility> Ability;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "AbilityFragments")
	TObjectPtr<class UFBAbilityFragmentManager> FragmentManager;
};
