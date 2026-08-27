// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GASkillAbility/GA_ModeChange.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment/AbilityFragment_AnimMontageList.h"
#include "Character/FBCharacter.h"
#include "Character/PawnComponent/FBCharacterComponent.h"

UGA_ModeChange::UGA_ModeChange()
{

}

void UGA_ModeChange::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (const UFBAbilityFragmentManager* FragmentManager = GetAbilityFragmentManager(Spec))
	{
		if (UAbilityFragment_AnimMontageList* AnimMontageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AnimMontageList>())
		{
			if (AnimMontageFragment->AnimMontageList.IsValidIndex(0))
			{
				ChangedMontage = AnimMontageFragment->AnimMontageList[0];
			}
			if (AnimMontageFragment->AnimMontageList.IsValidIndex(1))
			{
				ResetMontage = AnimMontageFragment->AnimMontageList[1];
			}
		}
	}
}

void UGA_ModeChange::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (AFBCharacter* Character = Cast<AFBCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UFBCharacterComponent* CharacterComponent = Character->FindComponentByClass<UFBCharacterComponent>())
		{
			CharacterComponent->SwitchInputConfig(DeactivateConfig, ActivateConfig);
		}
	}

	float AnimPlayRate = 1.0f;
	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Attack"), ChangedMontage, AnimPlayRate);
	PlayAttackTask->ReadyForActivation();
}

void UGA_ModeChange::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	float AnimPlayRate = 1.0f;
	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Attack"), ResetMontage, AnimPlayRate);
	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_ModeChange::OnCompleteCallback);
	PlayAttackTask->ReadyForActivation();
}

void UGA_ModeChange::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AFBCharacter* Character = Cast<AFBCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UFBCharacterComponent* CharacterComponent = Character->FindComponentByClass<UFBCharacterComponent>())
		{
			CharacterComponent->SwitchInputConfig(ActivateConfig, DeactivateConfig);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
