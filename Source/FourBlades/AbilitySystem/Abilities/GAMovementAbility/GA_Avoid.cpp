// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAMovementAbility/GA_Avoid.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment/AbilityFragment_AnimMontage.h"
#include "Player/FBPlayerState.h"
#include "AbilitySystem/FBAbilitySet.h"

UGA_Avoid::UGA_Avoid()
{
	AbilityTags.AddTag(FBTAG_CHARACTER_ABILITY_AVOID);

	RequiredTagsAnyOne.AddTag(FBTAG_CHARACTER_STATE_NORMAL);
	RequiredTagsAnyOne.AddTag(FBTAG_CHARACTER_STATE_ATTACK);

	ActivationOwnedTags.AddTag(FBTAG_CHARACTER_STATE_AVOID);

	CancelAbilitiesWithTag.AddTag(FBTAG_CHARACTER_ABILITY_SLASH);
	CancelAbilitiesWithTag.AddTag(FBTAG_CHARACTER_ABILITY_SMASH);
}

void UGA_Avoid::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (const UFBAbilityFragmentManager* FragmentManager = GetAbilityFragmentManager(Spec))
	{
		if (UAbilityFragment_AnimMontage* AnimMontageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AnimMontage>())
		{
			AvoidMontage = AnimMontageFragment->AnimMontage;
		}
	}
}

void UGA_Avoid::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	SetRotationAvatarToLastMoveDirection();

	const float AnimPlayRate = 1.0f;

	UAbilityTask_PlayMontageAndWait* PlayAvoidTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Avoid"), AvoidMontage, AnimPlayRate);
	PlayAvoidTask->OnCompleted.AddDynamic(this, &UGA_Avoid::OnCompleteCallback);
	PlayAvoidTask->OnInterrupted.AddDynamic(this, &UGA_Avoid::OnInterruptedCallback);
	PlayAvoidTask->ReadyForActivation();

	// 기획에 따라 무적 판정 들어갈 수 있음
}