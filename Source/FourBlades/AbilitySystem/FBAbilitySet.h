// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayAbilitySpec.h"
#include "FBAbilitySet.generated.h"

/*
* FFBTaggedAbility_GameplayAbility
*/
USTRUCT(BlueprintType)
struct FFBTaggedAbility_GameplayAbility
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UFBGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1; // 어빌리티의 허용 조건, 임시로 Level로 설정

	UPROPERTY(EditDefaultsOnly, Category = "AbilityTag", Meta = (Categories = "Character.Ability"))
	FGameplayTag AbilityTag;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "AbilityFragments")
	TObjectPtr<class UFBAbilityFragmentManager>  FragmentManager;
};

/*
* AbilitySet_GrantedHandles
*/
USTRUCT(BlueprintType)
struct FFBAbilitySet_GrantedHandles
{
	GENERATED_BODY()
	
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);

	void TakeFromAbilitySystem(class UFBAbilitySystemComponent* FBASC);

protected:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};

/**
 * AbilitySet
 */
UCLASS()
class FOURBLADES_API UFBAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UFBAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<FFBTaggedAbility_GameplayAbility> GrantedGameplayAbilities;

public:
	void GiveToAbilitySystem(class UFBAbilitySystemComponent* FBASC, FFBAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr);

	class UFBAbilityFragmentManager* GetAbilityFragmentManager(const FGameplayTag& InAbilityTag);
};
