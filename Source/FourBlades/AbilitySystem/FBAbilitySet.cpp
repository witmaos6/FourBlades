// Fill out your copyright notice in the Description page of Project Settings.


#include "FBAbilitySet.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/FBGameplayAbility.h"
#include "FourBlades.h"

/*
 * AbilitySet
 */
UFBAbilitySet::UFBAbilitySet(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UFBAbilitySet::GiveToAbilitySystem(UFBAbilitySystemComponent* FBASC, FFBAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject)
{
	check(FBASC);

	if (!FBASC->IsOwnerActorAuthoritative())
		return;

	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FFBTaggedAbility_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];
		if (!IsValid(AbilityToGrant.Ability))
		{
			continue;
		}

		UFBGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UFBGameplayAbility>(); // InstancedPerActor를 사용하게 될 경우 변경될 수 있음

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		if (AbilityToGrant.FragmentManager)
		{
			TSharedPtr<FGameplayEventData> EventDataPtr = MakeShared<FGameplayEventData>();
			EventDataPtr->OptionalObject = (const UObject*)AbilityToGrant.FragmentManager.Get();
			AbilitySpec.GameplayEventData = EventDataPtr;
		}		

		const FGameplayAbilitySpecHandle AbilitySpecHandle = FBASC->GiveAbility(AbilitySpec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
}

UFBAbilityFragmentManager* UFBAbilitySet::GetAbilityFragmentManager(const FGameplayTag& InAbilityTag)
{
	if (!InAbilityTag.IsValid())
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("AbilityTag is invalid"));
		return nullptr;
	}
	
	for (const FFBTaggedAbility_GameplayAbility& TaggedAbility : GrantedGameplayAbilities)
	{
		if (TaggedAbility.AbilityTag == InAbilityTag)
		{
			return TaggedAbility.FragmentManager;
		}
	}
	return nullptr;
}

/*
* AbilitySet_GrantedHandles
*/
void FFBAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FFBAbilitySet_GrantedHandles::TakeFromAbilitySystem(UFBAbilitySystemComponent* FBASC)
{
	if (!FBASC->IsOwnerActorAuthoritative())
		return;

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			FBASC->ClearAbility(Handle);
		}
	}
}
