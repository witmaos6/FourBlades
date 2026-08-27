// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "Abilities/FBGameplayAbility.h"
#include "Animation/FBAnimInstance.h"
#include "AbilitySystem/AbilitySystemData.h"
#include "GameplayTagsManager.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "AbilitySystem/Attributes/FBGuardSet.h"
#include "Log/FBLogChannels.h"

UFBAbilitySystemComponent::UFBAbilitySystemComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemData = CreateDefaultSubobject<UAbilitySystemData>(TEXT("AbilitySystemData"));
}

void UFBAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bHasNewPawnAvatar)
	{
		if (UFBAnimInstance* AnimInstance = Cast<UFBAnimInstance>(ActorInfo->GetAnimInstance()))
		{
			AnimInstance->InitializeWithAbilitySystem(this); // AnimInstance에서 AbilitySystemComponent가 등록이 되지 않는 경우가 발생할 수 있어 다시한번 호출한다.
		}
	}
}

void UFBAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UFBAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UFBAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UFBGameplayAbility* FBAbilityCDO = CastChecked<UFBGameplayAbility>(AbilitySpec->Ability);

				if (FBAbilityCDO->ActivationPolicy == EFBAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UFBGameplayAbility* FBAbilityCDO = CastChecked<UFBGameplayAbility>(AbilitySpec->Ability);

					if (FBAbilityCDO->ActivationPolicy == EFBAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;
				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UFBAbilitySystemComponent::EnforceSingleStateTag()
{
	const FGameplayTag CharacterStateTag = FGameplayTag::RequestGameplayTag(FName("Character.State"));
	const UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
	const FGameplayTagContainer CharacterStateTags = TagManager.RequestGameplayTagChildren(CharacterStateTag);

	for (const FGameplayTag& StateTag : CharacterStateTags)
	{
		RegisterGameplayTagEvent(StateTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UFBAbilitySystemComponent::OnCharacterStateTagChanged);
	}
}

void UFBAbilitySystemComponent::OnCharacterStateTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (Tag != FBTAG_CHARACTER_STATE_NORMAL)
	{
		if (NewCount > 0)
		{	
			FGameplayTagContainer TagsToRemove;
			const FGameplayTag CharacterStateTag = FGameplayTag::RequestGameplayTag(FName("Character.State"));

			for (const FGameplayTag& ActiveTag : GetOwnedGameplayTags())
			{
				if (ActiveTag.MatchesTag(CharacterStateTag) && ActiveTag != Tag)
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
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::SetCharacterStateNormal);
		}
	}
}

void UFBAbilitySystemComponent::SetCharacterStateNormal()
{
	const FGameplayTagContainer& TagContainer = GetOwnedGameplayTags();
	const FGameplayTag CharacterStateTag = FGameplayTag::RequestGameplayTag(FName("Character.State"));
	FGameplayTagContainer CharacterStateTags;

	for (FGameplayTag OwnedTag : TagContainer)
	{
		if (OwnedTag.MatchesTag(CharacterStateTag))
		{
			CharacterStateTags.AddTag(OwnedTag);
		}
	}

	if (CharacterStateTags.IsEmpty())
	{
		FGameplayTag CharacterStateNormalTag = FBTAG_CHARACTER_STATE_NORMAL;
		AddLooseGameplayTag(CharacterStateNormalTag);
	}
}

void UFBAbilitySystemComponent::StartGuardGaugeRecovery()
{
	if (!GuardGaugeRecoveryTimer.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(GuardGaugeRecoveryTimer, this, &UFBAbilitySystemComponent::RecoverGuardGauge, GuardRecoveryInterval, true);
	}
}

void UFBAbilitySystemComponent::StopGuardGaugeRecovery()
{
	if (GuardGaugeRecoveryTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(GuardGaugeRecoveryTimer);
	}
}

void UFBAbilitySystemComponent::RecoverGuardGauge()
{
	UFBGuardSet* GuardSet = const_cast<UFBGuardSet*>(GetSet<UFBGuardSet>());
	if (!GuardSet) return;

	float Current = GuardSet->GetGuardGauge();
	float Max = GuardSet->GetMaxGuardGauge();

	if (Current >= Max)
	{
		StopGuardGaugeRecovery();
		return;
	}

	GuardSet->SetGuardGauge(FMath::Min(Current + GuardRecoveryRate, Max));
}
