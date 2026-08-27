// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/FBEnemyAbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "FourBlades.h"

UFBEnemyAbilitySystemComponent::UFBEnemyAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UFBEnemyAbilitySystemComponent::EnforceSingleStateTag()
{
	const FGameplayTag EnemyStateTag = FGameplayTag::RequestGameplayTag(FName("Enemy.State"));
	const UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
	const FGameplayTagContainer EnemyStateTags = TagManager.RequestGameplayTagChildren(EnemyStateTag);

	for (const FGameplayTag& StateTag : EnemyStateTags)
	{
		RegisterGameplayTagEvent(StateTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UFBEnemyAbilitySystemComponent::OnEnemyStateTagChanged);
	}
}

void UFBEnemyAbilitySystemComponent::OnEnemyStateTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (Tag != FBTAG_ENEMY_STATE_IDLE)
	{
		if (NewCount > 0)
		{
			FGameplayTagContainer TagsToRemove;
			const FGameplayTag EnemyStateTag = FGameplayTag::RequestGameplayTag(FName("Enemy.State"));

			for (const FGameplayTag& ActiveTag : GetOwnedGameplayTags())
			{
				if (ActiveTag.MatchesTag(EnemyStateTag) && ActiveTag != Tag)
				{
					TagsToRemove.AddTag(ActiveTag);
				}
			}

			for (const FGameplayTag& TagToRemove : TagsToRemove)
			{
				int32 RemoveCount = GetGameplayTagCount(TagToRemove);
				RemoveLooseGameplayTag(TagToRemove, RemoveCount);
			}
		}
		else if (NewCount == 0)
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::SetEnemyStateIdle);
		}
	}
}

void UFBEnemyAbilitySystemComponent::SetEnemyStateIdle()
{
	const FGameplayTagContainer& TagContainer = GetOwnedGameplayTags();
	const FGameplayTag EnemyStateTag = FGameplayTag::RequestGameplayTag(FName("Enemy.State"));
	FGameplayTagContainer EnemyStateTags;

	for (FGameplayTag OwnedTag : TagContainer)
	{
		if (OwnedTag.MatchesTag(EnemyStateTag))
		{
			EnemyStateTags.AddTag(OwnedTag);
		}
	}

	if (EnemyStateTags.IsEmpty())
	{
		FGameplayTag CharacterStateNormalTag = FBTAG_ENEMY_STATE_IDLE;
		AddLooseGameplayTag(CharacterStateNormalTag);
	}
}
