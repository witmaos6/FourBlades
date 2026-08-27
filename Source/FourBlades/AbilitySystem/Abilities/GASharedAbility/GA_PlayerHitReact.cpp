// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GASharedAbility/GA_PlayerHitReact.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment/AbilityFragment_AnimMontage.h"
#include "ActorComponents/ReactData.h"
#include "GameFramework/Character.h"

UGA_PlayerHitReact::UGA_PlayerHitReact()
{
}

void UGA_PlayerHitReact::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (const UFBAbilityFragmentManager* FragmentManager = GetAbilityFragmentManager(Spec))
	{
		if (UAbilityFragment_AnimMontage* AnimMontageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AnimMontage>())
		{
			ReactMontage = AnimMontageFragment->AnimMontage;
		}
	}
}

void UGA_PlayerHitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	float AnimPlayRate = 1.0f;
	if (TriggerEventData && TriggerEventData->OptionalObject)
	{
		if (const UReactData* ReactData = Cast<UReactData>(TriggerEventData->OptionalObject))
		{
			if (ReactData->ReactDurationTime > 0.0f)
			{
				AnimPlayRate = (ReactMontage->GetPlayLength() / ReactData->ReactDurationTime);
			}
			
		}
	}

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("React"), ReactMontage, AnimPlayRate);

	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_PlayerHitReact::OnCompleteCallback);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_PlayerHitReact::OnInterruptedCallback);
	PlayAttackTask->ReadyForActivation();
}